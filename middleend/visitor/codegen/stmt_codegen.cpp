#include <middleend/visitor/codegen/ast_codegen.h>

namespace ME
{
    bool ASTCodeGen::terminated() const
    {
        if (curBlock->insts.empty()) return false;
        auto op = curBlock->insts.back()->opcode;
        return op == Operator::RET || op == Operator::BR_COND || op == Operator::BR_UNCOND;
    }

    size_t ASTCodeGen::castReg(DataType from, DataType to, size_t reg)
    {
        if (from == to) return reg;
        for (auto* inst : createTypeConvertInst(from, to, reg)) insert(inst);
        return getMaxReg();
    }

    void ASTCodeGen::visit(FE::AST::ExprStmt& node, Module* m)
    {
        if (node.expr) apply(*this, *node.expr, m);
    }

    void ASTCodeGen::visit(FE::AST::FuncDeclStmt& node, Module* m)
    {
        auto ret = convert(node.retType);
        auto* def = new FuncDefInst(ret, node.entry->getName());
        auto* func = new Function(def);
        m->functions.push_back(func);
        enterFunc(func);
        enterBlock(createBlock());
        name2reg.enterScope();
        reg2attr.clear();
        paramPtrTab.clear();
        lval2ptr.clear();

        if (node.params)
        {
            size_t index = 0;
            for (auto* param : *node.params)
                def->argRegs.emplace_back(convert(param->attr.val.value.type), getRegOperand(index++));
            if (index) func->setMaxReg(index - 1);
            index = 0;
            for (auto* param : *node.params)
            {
                auto type = def->argRegs[index].first;
                FE::AST::VarAttr attr(param->type, false, 0);
                size_t reg = index;
                if (type == DataType::PTR)
                {
                    paramPtrTab[reg] = true;
                    for (size_t i = 1; i < param->dims->size(); ++i)
                        attr.arrayDims.push_back((*param->dims)[i]->attr.val.getInt());
                }
                else
                {
                    reg = getNewRegId();
                    insert(createAllocaInst(type, reg));
                    insert(createStoreInst(type, index, getRegOperand(reg)));
                }
                reg2attr[reg] = attr;
                name2reg.addSymbol(param->entry, reg);
                ++index;
            }
        }

        if (node.body) apply(*this, *node.body, m);
        for (auto& [label, block] : func->blocks)
        {
            enterBlock(block);
            if (terminated()) continue;
            if (ret == DataType::VOID) insert(createRetInst());
            else if (ret == DataType::F32) insert(createRetInst(0.0f));
            else insert(createRetInst(0));
        }
        name2reg.exitScope();
        exitBlock();
        exitFunc();
    }

    void ASTCodeGen::visit(FE::AST::VarDeclStmt& node, Module* m)
    {
        if (node.decl) apply(*this, *node.decl, m);
    }

    void ASTCodeGen::visit(FE::AST::BlockStmt& node, Module* m)
    {
        name2reg.enterScope();
        if (node.stmts)
            for (auto* stmt : *node.stmts)
            {
                if (terminated()) break;
                if (stmt) apply(*this, *stmt, m);
            }
        name2reg.exitScope();
    }

    void ASTCodeGen::visit(FE::AST::ReturnStmt& node, Module* m)
    {
        if (!node.retExpr)
        {
            insert(createRetInst());
            return;
        }
        apply(*this, *node.retExpr, m);
        auto type = curFunc->funcDef->retType;
        insert(createRetInst(type, castReg(convert(node.retExpr->attr.val.value.type), type, getMaxReg())));
    }

    void ASTCodeGen::visit(FE::AST::WhileStmt& node, Module* m)
    {
        auto* cond = createBlock();
        auto* body = createBlock();
        auto* end = createBlock();
        insert(createBranchInst(cond->blockId));
        enterBlock(cond);
        apply(*this, *node.cond, m);
        auto test = castReg(convert(node.cond->attr.val.value.type), DataType::I1, getMaxReg());
        insert(createBranchInst(test, body->blockId, end->blockId));
        enterBlock(body);
        loopTargets.emplace_back(cond->blockId, end->blockId);
        if (node.body) apply(*this, *node.body, m);
        loopTargets.pop_back();
        if (!terminated()) insert(createBranchInst(cond->blockId));
        enterBlock(end);
    }

    void ASTCodeGen::visit(FE::AST::IfStmt& node, Module* m)
    {
        apply(*this, *node.cond, m);
        auto test = castReg(convert(node.cond->attr.val.value.type), DataType::I1, getMaxReg());
        auto* yes = createBlock();
        auto* no = node.elseStmt ? createBlock() : nullptr;
        auto* end = createBlock();
        insert(createBranchInst(test, yes->blockId, no ? no->blockId : end->blockId));
        enterBlock(yes);
        if (node.thenStmt) apply(*this, *node.thenStmt, m);
        if (!terminated()) insert(createBranchInst(end->blockId));
        if (no)
        {
            enterBlock(no);
            apply(*this, *node.elseStmt, m);
            if (!terminated()) insert(createBranchInst(end->blockId));
        }
        enterBlock(end);
    }

    void ASTCodeGen::visit(FE::AST::BreakStmt&, Module*)
    {
        insert(createBranchInst(loopTargets.back().second));
    }

    void ASTCodeGen::visit(FE::AST::ContinueStmt&, Module*)
    {
        insert(createBranchInst(loopTargets.back().first));
    }

    void ASTCodeGen::visit(FE::AST::ForStmt& node, Module* m)
    {
        name2reg.enterScope();
        if (node.init) apply(*this, *node.init, m);
        auto* cond = createBlock();
        auto* body = createBlock();
        auto* step = createBlock();
        auto* end = createBlock();
        insert(createBranchInst(cond->blockId));
        enterBlock(cond);
        if (node.cond)
        {
            apply(*this, *node.cond, m);
            auto test = castReg(convert(node.cond->attr.val.value.type), DataType::I1, getMaxReg());
            insert(createBranchInst(test, body->blockId, end->blockId));
        }
        else insert(createBranchInst(body->blockId));

        enterBlock(body);
        loopTargets.emplace_back(step->blockId, end->blockId);
        if (node.body) apply(*this, *node.body, m);
        loopTargets.pop_back();
        if (!terminated()) insert(createBranchInst(step->blockId));
        enterBlock(step);
        if (node.step) apply(*this, *node.step, m);
        insert(createBranchInst(cond->blockId));
        enterBlock(end);
        name2reg.exitScope();
    }
}  // namespace ME
