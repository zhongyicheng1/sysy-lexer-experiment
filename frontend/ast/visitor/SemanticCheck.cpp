#include "SemanticCheck.h"

#include <cstddef>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {

const Type kIntType{BaseType::INT};
const Type kBoolType{BaseType::BOOL};
const Type kVoidType{BaseType::VOID};
const Type kFloatType{BaseType::FLOAT};
const Type kIntArrayType{BaseType::INT, std::vector<int>{0}};

bool isComparison(BinaryOp op) {
    return op == BinaryOp::Lt || op == BinaryOp::Gt ||
           op == BinaryOp::Le || op == BinaryOp::Ge ||
           op == BinaryOp::Eq || op == BinaryOp::Ne;
}

bool isLogical(BinaryOp op) {
    return op == BinaryOp::And || op == BinaryOp::Or;
}

}  // namespace

SemanticCheck::SemanticCheck() {
    reset();
}

void SemanticCheck::reset() {
    symTable = SymbolTable{};
    errors.clear();
    hasMainFunc = false;
    currentReturnType_ = Type{};
    currentFunctionName_.clear();
    inFunction_ = false;
    registerBuiltins();
}

void SemanticCheck::registerBuiltins() {
    addBuiltin("getint", kIntType, {});
    addBuiltin("getch", kIntType, {});
    addBuiltin("getarray", kIntType, {kIntArrayType});
    addBuiltin("putint", kVoidType, {kIntType});
    addBuiltin("putch", kVoidType, {kIntType});
    addBuiltin("putarray", kVoidType, {kIntType, kIntArrayType});
    addBuiltin("starttime", kVoidType, {});
    addBuiltin("stoptime", kVoidType, {});
}

void SemanticCheck::addBuiltin(const std::string& name,
                              const Type& returnType,
                              std::vector<Type> paramTypes) {
    auto entry =
        std::make_shared<FuncEntry>(name, returnType, std::move(paramTypes));
    (void)symTable.addSymbol(std::move(entry));
}

bool SemanticCheck::check(ASTNode* root) {
    reset();

    if (root == nullptr) {
        reportError(0, "MissingMain", "Missing main function");
        return false;
    }

    if (auto* program = dynamic_cast<Program*>(root)) {
        program->accept(*this);
    } else {
        root->accept(*this);
        if (!hasMainFunc) {
            reportError(root->line, "MissingMain", "Missing main function");
        }
    }

    return errors.empty();
}

void SemanticCheck::reportError(int line, const std::string& category,
                                const std::string& msg) {
    errors.push_back("[Line " + std::to_string(line) + "][" + category +
                     "] " + msg);
}

void SemanticCheck::declareFunctions(Program& program) {
    for (const auto& function : program.functions) {
        if (!function) {
            continue;
        }
        if (!registerFunction(*function)) {
            reportError(function->line, "Redefinition",
                        "Function '" + function->name + "' is redefined");
        }
    }
}

bool SemanticCheck::registerFunction(const FuncDecl& function) {
    std::vector<Type> paramTypes;
    paramTypes.reserve(function.params.size());
    for (const auto& param : function.params) {
        if (param) {
            paramTypes.push_back(param->type);
        }
    }

    return symTable.addSymbol(std::make_shared<FuncEntry>(
        function.name, function.returnType, std::move(paramTypes)));
}

void SemanticCheck::visit(Program& node) {
    // Function signatures are hoisted so forward calls and recursion resolve.
    // All body checks still happen in post-order below.
    declareFunctions(node);

    for (auto& global : node.globals) {
        if (global) {
            global->accept(*this);
        }
    }
    for (auto& function : node.functions) {
        if (function) {
            function->accept(*this);
        }
    }

    if (!hasMainFunc) {
        reportError(node.line, "MissingMain", "Missing main function");
    }
}

void SemanticCheck::visit(FuncDecl& node) {
    if (node.name == "main") {
        hasMainFunc = true;
    }

    // Normally already installed by visit(Program).  This fallback also makes
    // a standalone FuncDecl visit self-contained.
    if (!symTable.getSymbol(node.name)) {
        if (!registerFunction(node)) {
            reportError(node.line, "Redefinition",
                        "Function '" + node.name + "' is redefined");
        }
    }

    const Type previousReturnType = currentReturnType_;
    const std::string previousFunctionName = currentFunctionName_;
    const bool previousInFunction = inFunction_;

    currentReturnType_ = node.returnType;
    currentFunctionName_ = node.name;
    inFunction_ = true;

    symTable.enterScope();
    for (const auto& param : node.params) {
        if (!param) {
            continue;
        }
        auto entry = std::make_shared<VarEntry>(param->name, param->type,
                                                false, 0);
        if (!symTable.addSymbol(entry)) {
            reportError(param->line, "Redefinition",
                        "Parameter '" + param->name + "' is redefined");
        }
    }

    if (node.body) {
        node.body->accept(*this);
    }
    symTable.exitScope();

    currentReturnType_ = previousReturnType;
    currentFunctionName_ = previousFunctionName;
    inFunction_ = previousInFunction;

    if (node.name == "main") {
        checkMainSignature(node);
    }
}

void SemanticCheck::checkMainSignature(const FuncDecl& function) {
    if (!(function.returnType == kIntType)) {
        reportError(function.line, "MainReturnType",
                    "Function 'main' must return int");
    }
}

void SemanticCheck::visit(VarDecl& node) {
    // The initializer is checked before the name is inserted, preventing a
    // declaration from referring to itself.
    if (node.initializer) {
        node.initializer->accept(*this);
    }

    int initialValue = 0;
    if (node.initializer) {
        const NodeAttr& init = node.initializer->attr;
        if (!init.hasError) {
            if (init.type.isVoid()) {
                reportError(node.line, "VoidExpression",
                            "Void value used as initializer");
            } else if (!isAssignable(node.type, init.type)) {
                reportError(node.line, "TypeMismatch",
                            "Cannot initialize variable '" + node.name +
                                "' of type " + typeToString(node.type) +
                                " with " + typeToString(init.type));
            } else if (node.isConst && !init.isConst) {
                reportError(node.line, "ConstInitializer",
                            "Initializer of const variable '" + node.name +
                                "' is not a constant expression");
            }
        }
        if (init.isConst) {
            initialValue = init.constValue;
        }
    } else if (node.isConst) {
        reportError(node.line, "ConstInitializer",
                    "Const variable '" + node.name +
                        "' requires an initializer");
    }

    auto entry = std::make_shared<VarEntry>(node.name, node.type, node.isConst,
                                            initialValue);
    if (!symTable.addSymbol(entry)) {
        reportError(node.line, "Redefinition",
                    "Variable '" + node.name + "' is redefined");
    }
}

void SemanticCheck::visit(CompoundStmt& node) {
    symTable.enterScope();
    for (auto& statement : node.statements) {
        if (statement) {
            statement->accept(*this);
        }
    }
    symTable.exitScope();
}

void SemanticCheck::visit(IfStmt& node) {
    if (node.condition) {
        node.condition->accept(*this);
    }
    if (node.thenBranch) {
        node.thenBranch->accept(*this);
    }
    if (node.elseBranch) {
        node.elseBranch->accept(*this);
    }
    checkCondition(node.condition.get(), node.line, "if condition");
}

void SemanticCheck::visit(WhileStmt& node) {
    if (node.condition) {
        node.condition->accept(*this);
    }
    if (node.body) {
        node.body->accept(*this);
    }
    checkCondition(node.condition.get(), node.line, "while condition");
}

void SemanticCheck::checkCondition(const Expr* condition, int line,
                                   const std::string& context) {
    if (!condition) {
        reportError(line, "InvalidCondition", "Missing " + context);
        return;
    }
    const NodeAttr& attr = condition->attr;
    if (attr.hasError) {
        return;
    }
    if (attr.type.isVoid()) {
        reportError(line, "VoidExpression",
                    "Void value used in " + context);
        return;
    }
    if (!attr.type.isScalar() ||
        !(attr.type.isInt() || attr.type.isBool())) {
        reportError(line, "InvalidCondition",
                    "Condition must be int or bool, got " +
                        typeToString(attr.type));
    }
}

void SemanticCheck::visit(ReturnStmt& node) {
    if (node.value) {
        node.value->accept(*this);
    }

    if (!inFunction_) {
        reportError(node.line, "InvalidReturn",
                    "Return statement outside a function");
        return;
    }

    if (currentReturnType_.isVoid()) {
        if (node.value) {
            reportError(node.line, "ReturnType",
                        "Void function '" + currentFunctionName_ +
                            "' cannot return a value");
        }
        return;
    }

    if (!node.value) {
        reportError(node.line, "ReturnType",
                    "Function '" + currentFunctionName_ +
                        "' must return a value");
        return;
    }

    const NodeAttr& value = node.value->attr;
    if (value.hasError) {
        return;
    }
    if (value.type.isVoid()) {
        reportError(node.line, "VoidExpression",
                    "Void value returned from function '" +
                        currentFunctionName_ + "'");
    } else if (!isAssignable(currentReturnType_, value.type)) {
        reportError(node.line, "ReturnType",
                    "Function '" + currentFunctionName_ + "' returns " +
                        typeToString(currentReturnType_) +
                        " but value has type " + typeToString(value.type));
    }
}

void SemanticCheck::visit(ExprStmt& node) {
    if (node.expr) {
        node.expr->accept(*this);
    }
}

void SemanticCheck::visit(IntLiteral& node) {
    node.attr = NodeAttr{};
    node.attr.type = kIntType;
    node.attr.isConst = true;
    node.attr.constValue = node.value;
}

void SemanticCheck::visit(BoolLiteral& node) {
    node.attr = NodeAttr{};
    node.attr.type = kBoolType;
    node.attr.isConst = true;
    node.attr.constValue = node.value ? 1 : 0;
}

void SemanticCheck::visit(VarRef& node) {
    node.attr = NodeAttr{};

    const auto symbol = symTable.getSymbol(node.name);
    if (!symbol) {
        reportError(node.line, "UndeclaredVariable",
                    "Undeclared variable '" + node.name + "'");
        node.attr.type = kVoidType;
        node.attr.hasError = true;
        return;
    }

    const auto variable = std::dynamic_pointer_cast<VarEntry>(symbol);
    if (!variable) {
        reportError(node.line, "InvalidUse",
                    "Function '" + node.name + "' is not a variable");
        node.attr.type = kVoidType;
        node.attr.hasError = true;
        return;
    }

    node.attr.type = variable->type;
    node.attr.isConst = variable->isConst;
    node.attr.constValue = variable->initValue;
}

void SemanticCheck::visit(UnaryExpr& node) {
    if (node.operand) {
        node.operand->accept(*this);
    }

    node.attr = NodeAttr{};

    if (!node.operand) {
        reportError(node.line, "InvalidExpression",
                    "Unary expression is missing its operand");
        node.attr.type = kVoidType;
        node.attr.hasError = true;
        return;
    }

    const NodeAttr& operand = node.operand->attr;
    if (operand.hasError) {
        node.attr.type = kVoidType;
        node.attr.hasError = true;
        return;
    }
    if (operand.type.isVoid()) {
        reportError(node.line, "VoidExpression",
                    "Void value used in expression");
        node.attr.type = kVoidType;
        node.attr.hasError = true;
        return;
    }

    if (node.op == UnaryOp::Not) {
        if (!isNumeric(operand.type)) {
            reportError(node.line, "InvalidOperand",
                        "Operand of '!' must be a scalar");
            node.attr.type = kVoidType;
            node.attr.hasError = true;
            return;
        }
        node.attr.type = kBoolType;
    } else {
        if (!isNumeric(operand.type)) {
            reportError(node.line, "InvalidOperand",
                        "Operand of unary '" +
                            std::string(unaryOpSymbol(node.op)) +
                            "' must be numeric");
            node.attr.type = kVoidType;
            node.attr.hasError = true;
            return;
        }
        node.attr.type = operand.type.isFloat() ? kFloatType : kIntType;
    }

    node.attr.isConst = operand.isConst;
    if (node.attr.isConst && !node.attr.type.isFloat()) {
        node.attr.constValue =
            evaluateUnary(node.op, operand.constValue);
    }
}

void SemanticCheck::visit(BinaryExpr& node) {
    if (node.lhs) {
        node.lhs->accept(*this);
    }
    if (node.rhs) {
        node.rhs->accept(*this);
    }

    node.attr = NodeAttr{};

    if (!node.lhs || !node.rhs) {
        reportError(node.line, "InvalidExpression",
                    "Binary expression is missing an operand");
        node.attr.type = kVoidType;
        node.attr.hasError = true;
        return;
    }

    const NodeAttr& lhs = node.lhs->attr;
    const NodeAttr& rhs = node.rhs->attr;

    if ((lhs.type.isVoid() && !lhs.hasError) ||
        (rhs.type.isVoid() && !rhs.hasError)) {
        reportError(node.line, "VoidExpression",
                    "Void value used in expression");
        node.attr.type = kVoidType;
        node.attr.hasError = true;
        return;
    }
    if (lhs.hasError || rhs.hasError) {
        node.attr.type = kVoidType;
        node.attr.hasError = true;
        return;
    }

    if (!isNumeric(lhs.type) || !isNumeric(rhs.type)) {
        reportError(node.line, "InvalidOperand",
                    "Operator '" + std::string(binaryOpSymbol(node.op)) +
                        "' requires scalar operands, got " +
                        typeToString(lhs.type) + " and " +
                        typeToString(rhs.type));
        node.attr.type = kVoidType;
        node.attr.hasError = true;
        return;
    }

    if (node.op == BinaryOp::Mod &&
        (!isIntegral(lhs.type) || !isIntegral(rhs.type))) {
        reportError(node.line, "InvalidOperand",
                    "Operator '%' requires integer operands, got " +
                        typeToString(lhs.type) + " and " +
                        typeToString(rhs.type));
        node.attr.type = kVoidType;
        node.attr.hasError = true;
        return;
    }

    if (isLogical(node.op) || isComparison(node.op)) {
        node.attr.type = kBoolType;
    } else if (node.op == BinaryOp::Mod) {
        node.attr.type = kIntType;
    } else if (lhs.type.isFloat() || rhs.type.isFloat()) {
        node.attr.type = kFloatType;
    } else {
        node.attr.type = kIntType;
    }

    if ((node.op == BinaryOp::Div || node.op == BinaryOp::Mod) &&
        lhs.isConst && rhs.isConst && rhs.constValue == 0) {
        reportError(node.line, "DivisionByZero",
                    "Division by zero in constant expression");
        node.attr.hasError = true;
        return;
    }

    node.attr.isConst = lhs.isConst && rhs.isConst;
    if (node.attr.isConst && !node.attr.type.isFloat()) {
        node.attr.constValue =
            evaluateBinary(node.op, lhs.constValue, rhs.constValue);
    }
}

void SemanticCheck::visit(FuncCall& node) {
    // Arguments are complete subexpressions and must be checked before the
    // call itself (post-order).
    for (auto& arg : node.args) {
        if (arg) {
            arg->accept(*this);
        }
    }

    node.attr = NodeAttr{};

    const auto symbol = symTable.getSymbol(node.callee);
    const auto function = std::dynamic_pointer_cast<FuncEntry>(symbol);
    if (!function) {
        reportError(node.line, "UndeclaredFunction",
                    "Undeclared function '" + node.callee + "'");
        node.attr.type = kVoidType;
        node.attr.hasError = true;
        return;
    }

    bool hasError = false;
    if (node.args.size() != function->paramTypes.size()) {
        reportError(node.line, "ArgumentCount",
                    "Function '" + node.callee + "' expects " +
                        std::to_string(function->paramTypes.size()) +
                        " argument(s) but got " +
                        std::to_string(node.args.size()));
        hasError = true;
    } else {
        for (std::size_t i = 0; i < node.args.size(); ++i) {
            const auto& arg = node.args[i];
            if (!arg) {
                reportError(node.line, "InvalidArgument",
                            "Missing argument " + std::to_string(i + 1) +
                                " in call to '" + node.callee + "'");
                hasError = true;
                continue;
            }
            const NodeAttr& argAttr = arg->attr;
            if (argAttr.hasError) {
                hasError = true;
                continue;
            }
            if (argAttr.type.isVoid()) {
                reportError(node.line, "VoidExpression",
                            "Void value used as argument " +
                                std::to_string(i + 1) + " of function '" +
                                node.callee + "'");
                hasError = true;
                continue;
            }
            if (!isAssignable(function->paramTypes[i], argAttr.type)) {
                reportError(node.line, "ArgumentType",
                            "Argument " + std::to_string(i + 1) +
                                " of function '" + node.callee + "' has type " +
                                typeToString(argAttr.type) + ", expected " +
                                typeToString(function->paramTypes[i]));
                hasError = true;
            }
        }
    }

    node.attr.type = function->returnType;
    node.attr.isConst = false;
    node.attr.hasError = hasError;
}

void SemanticCheck::visit(AssignExpr& node) {
    if (node.target) {
        node.target->accept(*this);
    }
    if (node.value) {
        node.value->accept(*this);
    }

    node.attr = NodeAttr{};

    if (!node.target || !node.value) {
        reportError(node.line, "InvalidAssignment",
                    "Assignment is missing an operand");
        node.attr.type = kVoidType;
        node.attr.hasError = true;
        return;
    }

    const NodeAttr& target = node.target->attr;
    const NodeAttr& value = node.value->attr;
    if (target.hasError || value.hasError) {
        node.attr.type = target.type;
        node.attr.hasError = true;
        return;
    }

    auto* variableRef = dynamic_cast<VarRef*>(node.target.get());
    if (!variableRef) {
        reportError(node.line, "InvalidAssignment",
                    "Left-hand side of assignment is not a variable");
        node.attr.type = kVoidType;
        node.attr.hasError = true;
        return;
    }

    const auto symbol = symTable.getSymbol(variableRef->name);
    const auto variable = std::dynamic_pointer_cast<VarEntry>(symbol);
    if (!variable) {
        node.attr.type = kVoidType;
        node.attr.hasError = true;
        return;
    }
    if (variable->isConst) {
        reportError(node.line, "ConstAssignment",
                    "Cannot assign to const variable '" +
                        variableRef->name + "'");
        node.attr.type = variable->type;
        node.attr.hasError = true;
        return;
    }
    if (value.type.isVoid()) {
        reportError(node.line, "VoidExpression",
                    "Void value used in assignment");
        node.attr.type = variable->type;
        node.attr.hasError = true;
        return;
    }
    if (!isAssignable(variable->type, value.type)) {
        reportError(node.line, "TypeMismatch",
                    "Cannot assign " + typeToString(value.type) +
                        " to variable '" + variableRef->name +
                        "' of type " + typeToString(variable->type));
        node.attr.type = variable->type;
        node.attr.hasError = true;
        return;
    }

    node.attr.type = variable->type;
    node.attr.isConst = false;
}

bool SemanticCheck::isNumeric(const Type& type) {
    return type.isScalar() &&
           (type.isInt() || type.isBool() || type.isFloat());
}

bool SemanticCheck::isIntegral(const Type& type) {
    return type.isScalar() && (type.isInt() || type.isBool());
}

bool SemanticCheck::isAssignable(const Type& target, const Type& source) {
    if (target.isVoid() || source.isVoid()) {
        return false;
    }

    if (target.isScalar() && source.isScalar()) {
        return isNumeric(target) && isNumeric(source);
    }

    if (target.dimensions.size() != source.dimensions.size()) {
        return false;
    }
    if (target.base != source.base) {
        return false;
    }
    for (std::size_t i = 0; i < target.dimensions.size(); ++i) {
        // Zero denotes an unspecified dimension such as int a[].
        if (target.dimensions[i] != 0 &&
            target.dimensions[i] != source.dimensions[i]) {
            return false;
        }
    }
    return true;
}

std::string SemanticCheck::typeToString(const Type& type) {
    std::string result;
    switch (type.base) {
        case BaseType::INT:
            result = "int";
            break;
        case BaseType::BOOL:
            result = "bool";
            break;
        case BaseType::FLOAT:
            result = "float";
            break;
        case BaseType::VOID:
            result = "void";
            break;
    }
    for (int dimension : type.dimensions) {
        result += "[";
        if (dimension > 0) {
            result += std::to_string(dimension);
        }
        result += "]";
    }
    return result;
}

int SemanticCheck::evaluateUnary(UnaryOp op, int value) {
    switch (op) {
        case UnaryOp::Plus:
            return value;
        case UnaryOp::Minus:
            if (value == std::numeric_limits<int>::min()) {
                return value;
            }
            return -value;
        case UnaryOp::Not:
            return value == 0 ? 1 : 0;
    }
    return 0;
}

int SemanticCheck::evaluateBinary(BinaryOp op, int lhs, int rhs) {
    switch (op) {
        case BinaryOp::Add:
            return static_cast<int>(static_cast<long long>(lhs) + rhs);
        case BinaryOp::Sub:
            return static_cast<int>(static_cast<long long>(lhs) - rhs);
        case BinaryOp::Mul:
            return static_cast<int>(static_cast<long long>(lhs) * rhs);
        case BinaryOp::Div:
            if (rhs == 0) {
                return 0;
            }
            if (lhs == std::numeric_limits<int>::min() && rhs == -1) {
                return lhs;
            }
            return lhs / rhs;
        case BinaryOp::Mod:
            if (rhs == 0) {
                return 0;
            }
            if (lhs == std::numeric_limits<int>::min() && rhs == -1) {
                return 0;
            }
            return lhs % rhs;
        case BinaryOp::Lt:
            return lhs < rhs ? 1 : 0;
        case BinaryOp::Gt:
            return lhs > rhs ? 1 : 0;
        case BinaryOp::Le:
            return lhs <= rhs ? 1 : 0;
        case BinaryOp::Ge:
            return lhs >= rhs ? 1 : 0;
        case BinaryOp::Eq:
            return lhs == rhs ? 1 : 0;
        case BinaryOp::Ne:
            return lhs != rhs ? 1 : 0;
        case BinaryOp::And:
            return (lhs != 0 && rhs != 0) ? 1 : 0;
        case BinaryOp::Or:
            return (lhs != 0 || rhs != 0) ? 1 : 0;
    }
    return 0;
}
