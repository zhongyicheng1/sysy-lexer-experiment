#include <middleend/visitor/codegen/ast_codegen.h>
#include <functional>

namespace ME
{
    namespace
    {
        FE::AST::Type* tensorElem(FE::AST::Type* t)
        {
            if (t && t->getTypeGroup() == FE::AST::TypeGroup::TENSOR)
                return static_cast<FE::AST::TensorType*>(t)->elem;
            return t;
        }
    }  // namespace

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
        bool tensorValue = node.attr.val.value.type &&
                           node.attr.val.value.type->getTypeGroup() == FE::AST::TypeGroup::TENSOR;
        if (tensorValue)
        {
            tensorVals[&node] = ptr;
            return;
        }
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
        if (isTensorNode(*node.expr))
        {
            handleTensorUnary(node, *node.expr, m);
            return;
        }
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
        bool lhsTensor = isTensorNode(*node.lhs);
        bool rhsTensor = isTensorNode(*node.rhs);

        if (node.op == Op::ASSIGN)
        {
            auto* lhs = dynamic_cast<FE::AST::LeftValExpr*>(node.lhs);
            if (lhs && isTensorNode(*lhs))
            {
                handleTensorAssign(*lhs, *node.rhs, m);
                tensorVals[&node] = lval2ptr.at(lhs);
            }
            else handleAssign(*lhs, *node.rhs, m);
            return;
        }
        if (lhsTensor || rhsTensor)
        {
            if (node.op == Op::MATMUL) handleTensorMatmul(node, *node.lhs, *node.rhs, m);
            else handleTensorBinary(node, *node.lhs, *node.rhs, m);
            return;
        }
        switch (node.op)
        {
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
                bool  tensorParam =
                    param->type && param->type->getTypeGroup() == FE::AST::TypeGroup::TENSOR;
                if (tensorParam)
                {
                    apply(*this, *arg, m);
                    auto it = tensorVals.find(arg);
                    ASSERT(it != tensorVals.end() && "tensor argument has no storage");
                    args.emplace_back(DataType::PTR, it->second);
                    continue;
                }
                auto type = param->dims && !param->dims->empty() ? DataType::PTR : convert(param->type);
                apply(*this, *arg, m);
                args.emplace_back(type, getRegOperand(castReg(convert(arg->attr.val.value.type), type, getMaxReg())));
            }
        bool tensorRet = decl->retType && decl->retType->getTypeGroup() == FE::AST::TypeGroup::TENSOR;
        auto type      = tensorRet ? DataType::PTR : convert(decl->retType);
        if (type == DataType::VOID) insert(createCallInst(type, node.func->getName(), args));
        else
        {
            insert(createCallInst(type, node.func->getName(), args, getNewRegId()));
            if (tensorRet) tensorVals[&node] = getRegOperand(getMaxReg());
        }
    }

    void ASTCodeGen::visit(FE::AST::CommaExpr& node, Module* m)
    {
        for (auto* expr : *node.exprs) apply(*this, *expr, m);
    }

    // ===================== 张量代码生成 =====================

    static size_t tensorSize(const std::vector<int>& shape)
    {
        size_t n = 1;
        for (int d : shape) n *= static_cast<size_t>(d);
        return n;
    }

    bool ASTCodeGen::isTensorNode(FE::AST::ExprNode& node)
    {
        return node.attr.val.value.type &&
               node.attr.val.value.type->getTypeGroup() == FE::AST::TypeGroup::TENSOR;
    }

    Operand* ASTCodeGen::newTensorTemp(const std::vector<int>& shape, DataType elem)
    {
        size_t reg = getNewRegId();
        curFunc->getBlock(0)->insertFront(createAllocaInst(elem, reg, shape));
        return getRegOperand(reg);
    }

    Operand* ASTCodeGen::tensorElemPtr(
        Operand* base, DataType elem, size_t total, size_t idxReg, bool parameter)
    {
        (void)parameter;  // 基地址总是指向第 0 个元素，统一用 [0, idx] 寻址
        size_t res = getNewRegId();
        insert(createGEP_I32Inst(
            elem, base, {static_cast<int>(total)}, {getImmeI32Operand(0), getRegOperand(idxReg)}, res));
        return getRegOperand(res);
    }

    void ASTCodeGen::emitCountedLoop(size_t count, const std::function<void(size_t)>& body)
    {
        size_t slot = getNewRegId();
        curFunc->getBlock(0)->insertFront(createAllocaInst(DataType::I32, slot));
        insert(createStoreInst(DataType::I32, getImmeI32Operand(0), getRegOperand(slot)));

        auto* cond = createBlock();
        auto* bodyB = createBlock();
        auto* end = createBlock();
        insert(createBranchInst(cond->blockId));

        enterBlock(cond);
        size_t iReg = getNewRegId();
        insert(createLoadInst(DataType::I32, getRegOperand(slot), iReg));
        size_t cmp = getNewRegId();
        insert(createIcmpInst_ImmeRight(ICmpOp::SLT, iReg, static_cast<int>(count), cmp));
        insert(createBranchInst(cmp, bodyB->blockId, end->blockId));

        enterBlock(bodyB);
        body(iReg);
        if (!terminated())
        {
            size_t v = getNewRegId();
            insert(createLoadInst(DataType::I32, getRegOperand(slot), v));
            size_t nv = getNewRegId();
            insert(createArithmeticI32Inst_ImmeLeft(Operator::ADD, 1, v, nv));
            insert(createStoreInst(DataType::I32, nv, getRegOperand(slot)));
            insert(createBranchInst(cond->blockId));
        }
        enterBlock(end);
    }

    void ASTCodeGen::emitTensorCopy(Operand* dst, bool dstParam, Operand* src, bool srcParam, DataType elem,
        const std::vector<int>& shape)
    {
        size_t total = tensorSize(shape);
        emitCountedLoop(total, [&](size_t i) {
            Operand* s = tensorElemPtr(src, elem, total, i, srcParam);
            Operand* d = tensorElemPtr(dst, elem, total, i, dstParam);
            size_t   v = getNewRegId();
            insert(createLoadInst(elem, s, v));
            insert(createStoreInst(elem, v, d));
        });
    }

    void ASTCodeGen::handleTensorAssign(FE::AST::LeftValExpr& lhs, FE::AST::ExprNode& rhs, Module* m)
    {
        apply(*this, lhs, m);
        Operand* dst = lval2ptr.at(&lhs);

        apply(*this, rhs, m);
        auto* src = tensorVals.at(&rhs);

        auto elem = convert(tensorElem(lhs.attr.val.value.type));
        emitTensorCopy(dst, false, src, false, elem, lhs.tensorDims);
    }

    void ASTCodeGen::handleTensorUnary(FE::AST::UnaryExpr& node, FE::AST::ExprNode& operand, Module* m)
    {
        apply(*this, operand, m);
        Operand* src  = tensorVals.at(&operand);
        auto     elem = convert(tensorElem(node.attr.val.value.type));
        auto     shape = node.tensorDims;
        size_t   total = tensorSize(shape);
        Operand* dst   = newTensorTemp(shape, elem);

        emitCountedLoop(total, [&](size_t i) {
            Operand* s = tensorElemPtr(src, elem, total, i, false);
            size_t   v = getNewRegId();
            insert(createLoadInst(elem, s, v));
            size_t r = v;
            if (node.op == FE::AST::Operator::SUB)
            {
                r = getNewRegId();
                if (elem == DataType::F32)
                    insert(createArithmeticF32Inst_ImmeLeft(Operator::FSUB, 0.0f, v, r));
                else
                    insert(createArithmeticI32Inst_ImmeLeft(Operator::SUB, 0, v, r));
            }
            insert(createStoreInst(elem, r, tensorElemPtr(dst, elem, total, i, false)));
        });

        tensorVals[&node] = dst;
    }

    void ASTCodeGen::handleTensorBinary(
        FE::AST::BinaryExpr& node, FE::AST::ExprNode& lhs, FE::AST::ExprNode& rhs, Module* m)
    {
        bool lhsTensor = isTensorNode(lhs);
        bool rhsTensor = isTensorNode(rhs);

        apply(*this, lhs, m);
        Operand* lhsPtr = lhsTensor ? tensorVals.at(&lhs) : nullptr;
        size_t   lhsReg = lhsTensor ? 0 : getMaxReg();

        apply(*this, rhs, m);
        Operand* rhsPtr = rhsTensor ? tensorVals.at(&rhs) : nullptr;
        size_t   rhsReg = rhsTensor ? 0 : getMaxReg();

        auto   elem  = convert(tensorElem(node.attr.val.value.type));
        auto   shape = node.tensorDims;
        size_t total = tensorSize(shape);
        Operand* dst = newTensorTemp(shape, elem);

        bool isFloat = elem == DataType::F32;
        FE::AST::Operator astOp = node.op;
        Operator op = Operator::OTHER;
        if (astOp == FE::AST::Operator::ADD) op = isFloat ? Operator::FADD : Operator::ADD;
        else if (astOp == FE::AST::Operator::SUB) op = isFloat ? Operator::FSUB : Operator::SUB;
        else if (astOp == FE::AST::Operator::MUL) op = isFloat ? Operator::FMUL : Operator::MUL;
        else if (astOp == FE::AST::Operator::DIV) op = isFloat ? Operator::FDIV : Operator::DIV;
        else if (astOp == FE::AST::Operator::MOD) op = Operator::MOD;

        emitCountedLoop(total, [&](size_t i) {
            size_t   lv = lhsReg;
            size_t   rv = rhsReg;
            Operand* lp = nullptr;
            Operand* rp = nullptr;
            if (lhsTensor)
            {
                lp = tensorElemPtr(lhsPtr, elem, total, i, false);
                lv = getNewRegId();
                insert(createLoadInst(elem, lp, lv));
            }
            if (rhsTensor)
            {
                rp = tensorElemPtr(rhsPtr, elem, total, i, false);
                rv = getNewRegId();
                insert(createLoadInst(elem, rp, rv));
            }
            size_t r = getNewRegId();
            if (isFloat) insert(createArithmeticF32Inst(op, lv, rv, r));
            else insert(createArithmeticI32Inst(op, lv, rv, r));
            insert(createStoreInst(elem, r, tensorElemPtr(dst, elem, total, i, false)));
        });

        tensorVals[&node] = dst;
    }

    void ASTCodeGen::handleTensorMatmul(
        FE::AST::BinaryExpr& node, FE::AST::ExprNode& lhs, FE::AST::ExprNode& rhs, Module* m)
    {
        apply(*this, lhs, m);
        Operand* aPtr = tensorVals.at(&lhs);
        apply(*this, rhs, m);
        Operand* bPtr = tensorVals.at(&rhs);

        const auto& A = lhs.tensorDims;
        const auto& B = rhs.tensorDims;
        size_t M = A[0], N = A[1], P = B[1];

        auto     elem  = convert(tensorElem(node.attr.val.value.type));
        bool     isF   = elem == DataType::F32;
        Operand* cPtr  = newTensorTemp({static_cast<int>(M), static_cast<int>(P)}, elem);

        size_t acc = getNewRegId();
        curFunc->getBlock(0)->insertFront(createAllocaInst(elem, acc));

        emitCountedLoop(M, [&](size_t i) {
            emitCountedLoop(P, [&](size_t j) {
                if (isF)
                    insert(createStoreInst(elem, getImmeF32Operand(0.0f), getRegOperand(acc)));
                else
                    insert(createStoreInst(elem, getImmeI32Operand(0), getRegOperand(acc)));

                emitCountedLoop(N, [&](size_t k) {
                    size_t iN = getNewRegId();
                    insert(createArithmeticI32Inst_ImmeLeft(Operator::MUL, static_cast<int>(N), i, iN));
                    size_t idxA = getNewRegId();
                    insert(createArithmeticI32Inst(Operator::ADD, iN, k, idxA));
                    size_t kP = getNewRegId();
                    insert(createArithmeticI32Inst_ImmeLeft(Operator::MUL, static_cast<int>(P), k, kP));
                    size_t idxB = getNewRegId();
                    insert(createArithmeticI32Inst(Operator::ADD, kP, j, idxB));

                    Operand* ap = tensorElemPtr(aPtr, elem, tensorSize(A), idxA, false);
                    Operand* bp = tensorElemPtr(bPtr, elem, tensorSize(B), idxB, false);
                    size_t   av = getNewRegId();
                    size_t   bv = getNewRegId();
                    insert(createLoadInst(elem, ap, av));
                    insert(createLoadInst(elem, bp, bv));
                    size_t prod = getNewRegId();
                    if (isF) insert(createArithmeticF32Inst(Operator::FMUL, av, bv, prod));
                    else insert(createArithmeticI32Inst(Operator::MUL, av, bv, prod));
                    size_t cur = getNewRegId();
                    insert(createLoadInst(elem, getRegOperand(acc), cur));
                    size_t sum = getNewRegId();
                    if (isF) insert(createArithmeticF32Inst(Operator::FADD, cur, prod, sum));
                    else insert(createArithmeticI32Inst(Operator::ADD, cur, prod, sum));
                    insert(createStoreInst(elem, sum, getRegOperand(acc)));
                });

                size_t iP = getNewRegId();
                insert(createArithmeticI32Inst_ImmeLeft(Operator::MUL, static_cast<int>(P), i, iP));
                size_t idxC = getNewRegId();
                insert(createArithmeticI32Inst(Operator::ADD, iP, j, idxC));
                size_t v = getNewRegId();
                insert(createLoadInst(elem, getRegOperand(acc), v));
                insert(createStoreInst(elem, v, tensorElemPtr(cPtr, elem, M * P, idxC, false)));
            });
        });

        tensorVals[&node] = cPtr;
    }
}  // namespace ME
