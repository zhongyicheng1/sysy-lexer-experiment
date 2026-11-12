# Lab2 语法分析器 AI 使用记录

## 1. 使用说明

本记录依据实验期间的实际讨论、Git 提交和终端测试结果整理。对话内容为关键过程的重构摘要，不是逐字聊天导出。AI 用于解释框架、给出文法设计建议、辅助检查代码与测试结果；小组成员负责确认实验要求、阅读框架接口、执行命令、审查修改并完成最终验收。

AI 参与过的代码部分在下文中明确列出，没有将 AI 辅助代码表述为纯人工编写。

## 2. 实验目标确认

**学生问题**

> Lab1 已经识别了 `int`、常量和运算符，Lab2 为什么还要写这些规则？这次实验主要完成什么？

**AI 建议**

AI 说明 Lab1 的输出是平铺的 Token 序列，Lab2 要把 Token 按 SysY 文法组织成 AST。例如，Lab1 将 `a = b + 1` 识别为 `IDENT ASSIGN IDENT PLUS INT_CONST`，Lab2 需要创建以赋值为根、加法为右子树的表达式节点。AI 建议主要阅读：

- `frontend/parser/yacc.y`
- `frontend/ast/expr.h`
- `frontend/ast/stmt.h`
- `frontend/ast/decl.h`
- `interfaces/frontend/ast/ast_defs.h`
- `frontend/ast/visitor/printer/`

**人工确认与处理**

小组对照实验 PDF、PPT 和旧版框架确认：本实验核心是补全 `yacc.y` 的产生式及归约动作，构造 AST；完整符号表、作用域和类型检查留到后续语义分析实验。

## 3. 两人分工讨论

**学生问题**

> 如何把 `yacc.y` 的任务分成两部分，并减少两个人同时修改同一位置造成的冲突？

**AI 建议**

AI 建议按非终结符职责划分：

- 成员 1：表达式优先级、赋值、字面量、基础类型和数组下标。
- 成员 2：变量声明、初始化、函数参数、块语句和控制流。

双方约定沿用框架已有的 `%nterm` 类型，不自行创建重复 AST 节点；成员 1 保证表达式规则返回 `ExprNode*`，成员 2 使用这些表达式构造声明和语句节点。

**人工确认与处理**

小组采用该边界，并分别在 Git 分支上完成任务。成员 2 的提交 `9abaf8b` 直接建立在成员 1 的提交 `b389d5b` 之上，因此最终可以线性结合，不需要手工解决两套文法的冲突。

## 4. 表达式文法设计

**学生问题**

> 怎样保证 `a + b * c`、`a || b && c` 和 `a = b = c` 的优先级与结合性正确？

**AI 建议**

AI 建议不要用一个统一的 `EXPR -> EXPR op EXPR` 规则，而是按优先级拆成：

```text
ASSIGN_EXPR
LOGICAL_OR_EXPR
LOGICAL_AND_EXPR
EQUALITY_EXPR
RELATIONAL_EXPR
ADDSUB_EXPR
MULDIV_EXPR
UNARY_EXPR
BASIC_EXPR
```

二元算术和逻辑规则使用左递归，使同级运算左结合；赋值右侧使用 `NOCOMMA_EXPR`，从而允许 `a = (b = c)`。归约动作复用框架的 `BinaryExpr`、`UnaryExpr` 和 `Operator` 枚举。

**人工确认与处理**

成员 1 阅读 `expr.h` 与 `ast_defs.h` 后确认构造函数和枚举名称，再将规则写入 `yacc.y`。实现包括：

- `=`, `||`, `&&`
- `==`, `!=`, `<`, `<=`, `>`, `>=`
- `+`, `-`, `*`, `/`, `%`
- 一元 `+`, `-`, `!`
- `INT_CONST`, `LL_CONST`, `FLOAT_CONST`
- `intType`, `floatType`, `voidType`
- 多维数组下标列表

该部分有 AI 参与设计和代码修改，成员 1 负责核对框架接口、运行构建并审查 AST 输出。

## 5. Bison 冲突检查

**学生问题**

> 成员 1 完成后，Bison 报 92 个 shift/reduce 和 104 个 reduce/reduce 冲突，是表达式文法错误吗？

**AI 建议**

AI 建议运行 `bison -v` 查看 `yacc.output`，不要只根据冲突数量猜测。状态报告显示当时存在以下空产生式：

```text
VAR_DECL_STMT  -> ε
IF_STMT        -> ε
VAR_DECLARATOR -> ε
INITIALIZER    -> ε
```

其中 `VAR_DECL_STMT` 和 `IF_STMT` 又是 `STMT` 的分支，导致分析器在许多 Token 前都可以把多个非终结符归约为空，从而产生大量冲突。

**人工确认与处理**

小组保留成员 1 的表达式层次，等待成员 2 补全声明和控制流。成员 2 完成后重新执行构建，所有 `shift/reduce`、`reduce/reduce`、`empty rule` 和 `useless` 警告均消失，证明早期冲突来自尚未完成的空规则。

## 6. 声明和控制流检查

**学生问题**

> 成员 2 的分支是否保留了成员 1 的内容，声明、初始化和控制流规则是否覆盖实验要求？

**AI 建议**

AI 建议先检查提交图和文件差异，再合并。检查结果为：

```text
d6001b2  Lab1 词法分析
   |
b389d5b  成员 1 表达式文法
   |
9abaf8b  成员 2 声明、函数与控制流
```

成员 2 增加了变量声明、初始化列表、函数参数、块语句、`if/else`、`while`、`return` 和 `break` 等规则，没有删除成员 1 的表达式实现。

**人工确认与处理**

小组审查 `git diff b389d5b..9abaf8b`，确认成员 2 的提交只有 `frontend/parser/yacc.y`，并且建立在成员 1 的提交上。随后将当前分支快进到成员 2 的提交。

## 7. 多维数组参数漏洞检查

**学生问题**

> 已有规则支持 `int a[]` 和 `int a[10]`，是否足以覆盖指导文法和数组测试？

**AI 建议**

AI 对照指导文法指出，函数参数还应覆盖首维省略后继续有维度的形式，例如 `int a[][10]`，以及一般多维形式 `int a[10][20]`。建议复用 `ARRAY_DIMENSION_EXPR_LIST`，不要为每个维度数量单独增加规则。

**人工确认与处理**

小组增加两种参数规则：

```text
TYPE IDENT '[' ']' ARRAY_DIMENSION_EXPR_LIST
TYPE IDENT ARRAY_DIMENSION_EXPR_LIST
```

首维省略时用值为 `-1` 的 `LiteralExpr` 表示未知首维。该修正由 AI 辅助提出并修改，小组通过额外样例验证 AST：

```c
int sum(int a[][10], int i, int j)
{
    return a[i][j];
}
```

输出正确显示 `sum(int a[][10], int i, int j)`，并保留 `a[i][j]` 的两个下标子节点。

## 8. 最终测试与人工验收

小组在 WSL 中执行：

```bash
make clean-lexer
make 2>&1 | tee build-lab2.log

grep -E \
  "shift/reduce|reduce/reduce|empty rule|useless|error:" \
  build-lab2.log
```

结果：项目成功链接为 `bin/compiler`，`grep` 无输出。

随后运行三个框架 Parser 样例：

```bash
./bin/compiler -parser -o lab2-simple.parser testcase/parser/simple.sy
./bin/compiler -parser -o lab2-witharray.parser testcase/parser/witharray.sy
./bin/compiler -parser -o lab2-withfloat.parser testcase/parser/withfloat.sy
```

三个输出分别与预期文件执行 `diff --strip-trailing-cr -u`，均无差异。小组还运行了多维数组参数补充测试，成功输出完整 AST。

## 9. 小组成员人工实现与修改明细

### 9.1 成员 1 完成的工作

成员 1 负责表达式体系及相关基础节点，主要人工工作包括：

1. 阅读 `expr.h` 和 `ast_defs.h`，逐项核对 `BinaryExpr`、`UnaryExpr`、`LiteralExpr` 的构造参数，以及 `Operator` 枚举的实际名称。
2. 按运算优先级组织 `LOGICAL_OR_EXPR`、`LOGICAL_AND_EXPR`、`EQUALITY_EXPR`、`RELATIONAL_EXPR`、`ADDSUB_EXPR` 和 `MULDIV_EXPR`，并检查同级运算的左结合结构。
3. 将赋值规则限制为左侧必须是 `LEFT_VAL_EXPR`，避免普通表达式出现在赋值号左边。
4. 补充 `INT_CONST`、`LL_CONST`、`FLOAT_CONST` 到 `LiteralExpr` 的转换，并将 `INT`、`FLOAT`、`VOID` 映射到框架提供的类型对象。
5. 实现数组下标列表的累积，使 `a[i][j]` 能在 `LeftValExpr::indices` 中保存多个子表达式。
6. 在 WSL 中执行构建、生成 `yacc.output`、定位早期冲突，并提交成员 1 分支代码。

成员 1 没有修改 AST 类的公开接口，而是按框架已有构造函数完成归约动作，减少了对后续语义分析和代码生成模块的影响。

### 9.2 成员 2 完成的工作

成员 2 负责声明、函数和控制流部分，主要人工工作包括：

1. 实现普通变量、数组变量、带初始化变量和多个声明符组成的变量声明语句。
2. 实现普通初始化表达式、空初始化列表及嵌套初始化列表，使二维数组初始化能够形成多层 `InitializerList`。
3. 实现空参数、单参数和多参数组成的函数参数列表，并构造 `ParamDeclarator`。
4. 将 `WHILE_STMT`、`RETURN_STMT`、`BREAK_STMT` 和 `BLOCK_STMT` 接入统一的 `STMT` 入口。
5. 实现有 `else` 和无 `else` 的 `IF_STMT`，使用 `THEN` 与 `ELSE` 优先级处理悬空 `else`。
6. 在成员 1 提交基础上继续开发，提交 `9abaf8b`，避免重新实现或覆盖表达式文法。

### 9.3 小组共同修改与验证

双方结合代码后共同完成以下工作：

1. 使用提交图确认成员 2 的分支以成员 1 提交为父提交，再采用线性方式结合代码。
2. 对照指导文法检查函数数组参数，补充 `int a[][10]` 和一般多维参数形式。
3. 保留首维未知的数组参数表示方法，用值为 `-1` 的 `LiteralExpr` 记录省略的第一维。
4. 清理空产生式造成的 Bison 冲突，并确认最终构建没有 `shift/reduce` 或 `reduce/reduce` 警告。
5. 分别运行 `simple`、`witharray`、`withfloat` 三个官方测试，并使用 `diff` 与预期 AST 输出逐行比较。
6. 编写额外多维数组参数样例，人工检查函数签名、数组访问节点和嵌套初始化列表的打印结构。

小组最终接受的不是未经检查的 AI 输出，而是经过接口核对、代码审查、版本差异检查和可重复测试验证后的实现。

## 10. AI 辅助范围汇总

| 内容 | AI 参与方式 | 人工工作 |
| --- | --- | --- |
| 实验要求梳理 | 解释 Lab1 与 Lab2 边界，整理文件职责 | 对照 PDF、PPT 和仓库确认 |
| 两人分工 | 建议按表达式与语句声明划分 | 小组确定接口并分支开发 |
| 成员 1 表达式规则 | 提供文法结构建议并参与代码修改 | 阅读 AST 接口、执行构建、检查输出 |
| Bison 冲突分析 | 根据 `yacc.output` 判断空规则冲突来源 | 运行命令并提供完整状态报告 |
| 成员 2 代码 | AI 未代替成员 2 编写原提交 | 成员 2 完成并提交 `9abaf8b` |
| 分支结合 | 比较提交历史和差异，检查覆盖风险 | 小组确认提交并运行最终测试 |
| 多维参数修正 | 发现覆盖不足并参与修改 | 小组使用额外样例验收 |
| 最终结论 | 根据实际输出检查要求覆盖情况 | 小组保留测试证据并准备答辩 |

## 11. 对 AI 输出的审查

小组没有直接以 AI 回答代替实验理解。所有建议均通过以下方式复核：

1. 对照框架中的 AST 构造函数和枚举类型。
2. 使用 `bison -v` 检查冲突状态，而非只看是否能编译。
3. 使用三个官方 Parser 样例与预期输出逐行比较。
4. 设计多维数组参数补充样例验证文法覆盖。
5. 通过 Git 差异确认提交只包含手写源文件，不包含生成文件和测试产物。

最终代码由小组成员负责理解、讲解和维护。
