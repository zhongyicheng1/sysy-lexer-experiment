#include <middleend/visitor/codegen/ast_codegen.h>
#include <debug.h>
#include <functional>

namespace ME
{
    void ASTCodeGen::visit(FE::AST::Initializer& node, Module* m)
    {
        (void)m;
        ERROR("Initializer should not appear here, at line %d", node.line_num);
    }
    void ASTCodeGen::visit(FE::AST::InitializerList& node, Module* m)
    {
        (void)m;
        ERROR("InitializerList should not appear here, at line %d", node.line_num);
    }
    void ASTCodeGen::visit(FE::AST::VarDeclarator& node, Module* m)
    {
        (void)m;
        ERROR("VarDeclarator should not appear here, at line %d", node.line_num);
    }
    void ASTCodeGen::visit(FE::AST::ParamDeclarator& node, Module* m)
    {
        (void)m;
        ERROR("ParamDeclarator should not appear here, at line %d", node.line_num);
    }

    void ASTCodeGen::visit(FE::AST::VarDeclaration& node, Module* m)
    {
        // TODO(Lab 3-2): 生成变量声明 IR（alloca、数组零初始化、可选初始化表达式）
        if (!node.decls) return;
        auto type = convert(node.type);
        for (auto* declarator : *node.decls)
        {
            auto& lhs = static_cast<FE::AST::LeftValExpr&>(*declarator->lval);
            FE::AST::VarAttr attr(node.type, node.isConstDecl, 0);
            if (lhs.indices)
                for (auto* dim : *lhs.indices) attr.arrayDims.push_back(dim->attr.val.getInt());
            auto reg = getNewRegId();
            auto* address = getRegOperand(reg);
            const auto& dims = attr.arrayDims;
            curFunc->getBlock(0)->insertFront(createAllocaInst(type, reg, dims));
            reg2attr[reg] = attr;
            name2reg.addSymbol(lhs.entry, reg);
            if (!declarator->init) continue;
            std::vector<size_t> stride(dims.size() + 1, 1);
            for (size_t i = dims.size(); i > 0; --i) stride[i - 1] = stride[i] * dims[i - 1];
            auto element = [&](size_t index) -> Operand* {
                if (dims.empty()) return address;
                std::vector<Operand*> indices{getImmeI32Operand(0)};
                for (size_t i = 1; i < stride.size(); ++i)
                {
                    indices.push_back(getImmeI32Operand(index / stride[i]));
                    index %= stride[i];
                }
                auto result = getNewRegId();
                insert(createGEP_I32Inst(type, address, dims, indices, result));
                return getRegOperand(result);
            };
            if (!dims.empty())
            {
                CallInst::argList args{{DataType::PTR, address}, {DataType::I8, getImmeI32Operand(0)},
                    {DataType::I32, getImmeI32Operand(stride.front() * 4)}, {DataType::I1, getImmeI32Operand(0)}};
                insert(createCallInst(DataType::VOID, "llvm.memset.p0.i32", args));
            }
            std::function<void(FE::AST::InitDecl*, size_t, size_t)> emitInit;
            emitInit = [&](FE::AST::InitDecl* init, size_t depth, size_t start) {
                if (auto* scalar = dynamic_cast<FE::AST::Initializer*>(init))
                {
                    apply(*this, *scalar->init_val, m);
                    auto value = castReg(convert(scalar->init_val->attr.val.value.type), type, getMaxReg());
                    insert(createStoreInst(type, value, element(start)));
                    return;
                }
                auto* list = static_cast<FE::AST::InitializerList*>(init);
                size_t cursor = start;
                if (list->init_list)
                    for (auto* child : *list->init_list)
                    {
                        if (dynamic_cast<FE::AST::InitializerList*>(child) && depth < dims.size())
                        {
                            auto width = stride[depth + 1];
                            cursor = start + ((cursor - start + width - 1) / width) * width;
                            emitInit(child, depth + 1, cursor);
                            cursor += width;
                        }
                        else emitInit(child, depth, cursor++);
                    }
            };
            emitInit(declarator->init, 0, 0);
            if (dims.empty())
                if (auto* list = dynamic_cast<FE::AST::InitializerList*>(declarator->init);
                    list && (!list->init_list || list->init_list->empty()))
                {
                    Operand* zero = type == DataType::F32 ? static_cast<Operand*>(getImmeF32Operand(0))
                                                           : static_cast<Operand*>(getImmeI32Operand(0));
                    insert(createStoreInst(type, zero, address));
                }
        }
    }
}  // namespace ME
