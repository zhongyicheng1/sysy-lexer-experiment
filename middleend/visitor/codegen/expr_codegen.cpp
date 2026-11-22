#include <middleend/visitor/codegen/ast_codegen.h>

namespace ME
{
    void ASTCodeGen::visit(FE::AST::LeftValExpr& node, Module* m)
    {
        size_t reg = name2reg.getReg(node.entry);
        bool global = reg == static_cast<size_t>(-1);
        const auto& attr = global ? glbSymbols.at(node.entry) : reg2attr.at(reg);
        Operand* ptr = global ? static_cast<Operand*>(getGlobalOperand(node.entry->getName()))
                              : static_cast<Operand*>(getRegOperand(reg));
        bool parameter = !global && paramPtrTab.count(reg);
        auto type = convert(attr.type);
        std::vector<Operand*> indices;
        if (!parameter && !attr.arrayDims.empty()) indices.push_back(getImmeI32Operand(0));
        if (node.indices)
            for (auto* index : *node.indices)
            {
                apply(*this, *index, m);
                auto indexReg = castReg(convert(index->attr.val.value.type), DataType::I32, getMaxReg());
                indices.push_back(getRegOperand(indexReg));
            }
        auto resultType = convert(node.attr.val.value.type);
        if (resultType == DataType::PTR && (!parameter || !indices.empty()))
            indices.push_back(getImmeI32Operand(0));
        if (!indices.empty())
        {
            auto address = getNewRegId();
            insert(createGEP_I32Inst(type, ptr, attr.arrayDims, indices, address));
            ptr = getRegOperand(address);
        }
        lval2ptr[&node] = ptr;
        if (resultType != DataType::PTR) insert(createLoadInst(type, ptr, getNewRegId()));
        else if (indices.empty())
            insert(createGEP_I32Inst(type, ptr, attr.arrayDims, {getImmeI32Operand(0)}, getNewRegId()));
    }

    void ASTCodeGen::visit(FE::AST::LiteralExpr& node, Module* m)
    {
        (void)m;

        size_t reg = getNewRegId();
        switch (node.literal.type->getBaseType())
        {
            case FE::AST::Type_t::INT:
            case FE::AST::Type_t::LL:  // treat as I32
            {
                int             val  = node.literal.getInt();
                ArithmeticInst* inst = createArithmeticI32Inst_ImmeAll(Operator::ADD, val, 0, reg);  // reg = val + 0
                insert(inst);
                break;
            }
            case FE::AST::Type_t::FLOAT:
            {
                float           val  = node.literal.getFloat();
                ArithmeticInst* inst = createArithmeticF32Inst_ImmeAll(Operator::FADD, val, 0, reg);  // reg = val + 0
                insert(inst);
                break;
            }
            default: ERROR("Unsupported literal type");
        }
    }

    void ASTCodeGen::visit(FE::AST::UnaryExpr& node, Module* m)
    {
        handleUnaryCalc(*node.expr, node.op, curBlock, m);
    }

    void ASTCodeGen::handleAssign(FE::AST::LeftValExpr& lhs, FE::AST::ExprNode& rhs, Module* m)
    {
        apply(*this, lhs, m);
        auto* address = lval2ptr.at(&lhs);
        apply(*this, rhs, m);
        auto type = convert(lhs.attr.val.value.type);
        auto value = castReg(convert(rhs.attr.val.value.type), type, getMaxReg());
        insert(createStoreInst(type, value, address));
        insert(createLoadInst(type, address, getNewRegId()));
    }

    size_t ASTCodeGen::logical(FE::AST::ExprNode& lhs, FE::AST::ExprNode& rhs, bool isAnd, Module* m)
    {
        auto slot = getNewRegId();
        curFunc->getBlock(0)->insertFront(createAllocaInst(DataType::I1, slot));
        apply(*this, lhs, m);
        auto test = castReg(convert(lhs.attr.val.value.type), DataType::I1, getMaxReg());
        insert(createStoreInst(DataType::I1, test, getRegOperand(slot)));
        auto* next = createBlock();
        auto* end = createBlock();
        insert(createBranchInst(test, isAnd ? next->blockId : end->blockId,
            isAnd ? end->blockId : next->blockId));
        enterBlock(next);
        apply(*this, rhs, m);
        auto value = castReg(convert(rhs.attr.val.value.type), DataType::I1, getMaxReg());
        insert(createStoreInst(DataType::I1, value, getRegOperand(slot)));
        insert(createBranchInst(end->blockId));
        enterBlock(end);
        auto result = getNewRegId();
        insert(createLoadInst(DataType::I1, getRegOperand(slot), result));
        return result;
    }

    void ASTCodeGen::handleLogicalAnd(
        FE::AST::BinaryExpr&, FE::AST::ExprNode& lhs, FE::AST::ExprNode& rhs, Module* m)
    {
        logical(lhs, rhs, true, m);
    }

    void ASTCodeGen::handleLogicalOr(
        FE::AST::BinaryExpr&, FE::AST::ExprNode& lhs, FE::AST::ExprNode& rhs, Module* m)
    {
        logical(lhs, rhs, false, m);
    }

    void ASTCodeGen::visit(FE::AST::BinaryExpr& node, Module* m)
    {
        using Op = FE::AST::Operator;
        switch (node.op)
        {
            case Op::ASSIGN: handleAssign(static_cast<FE::AST::LeftValExpr&>(*node.lhs), *node.rhs, m); break;
            case Op::AND: handleLogicalAnd(node, *node.lhs, *node.rhs, m); break;
            case Op::OR: handleLogicalOr(node, *node.lhs, *node.rhs, m); break;
            default: handleBinaryCalc(*node.lhs, *node.rhs, node.op, curBlock, m); break;
        }
    }

    void ASTCodeGen::visit(FE::AST::CallExpr& node, Module* m)
    {
        auto* decl = funcDecls.at(node.func);
        CallInst::argList args;
        if (node.args)
            for (size_t i = 0; i < node.args->size(); ++i)
            {
                auto* arg = (*node.args)[i];
                auto* param = (*decl->params)[i];
                auto type = param->dims && !param->dims->empty() ? DataType::PTR : convert(param->type);
                apply(*this, *arg, m);
                args.emplace_back(type, getRegOperand(castReg(convert(arg->attr.val.value.type), type, getMaxReg())));
            }
        auto type = convert(decl->retType);
        if (type == DataType::VOID) insert(createCallInst(type, node.func->getName(), args));
        else insert(createCallInst(type, node.func->getName(), args, getNewRegId()));
    }

    void ASTCodeGen::visit(FE::AST::CommaExpr& node, Module* m)
    {
        for (auto* expr : *node.exprs) apply(*this, *expr, m);
    }
}  // namespace ME
