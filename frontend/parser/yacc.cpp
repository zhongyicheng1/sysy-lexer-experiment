// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

// "%code top" blocks.
#line 31 "frontend/parser/yacc.y"

#include <iostream>

#include <frontend/parser/parser.h>
#include <frontend/parser/location.hh>
#include <frontend/parser/scanner.h>
#include <frontend/parser/yacc.h>

using namespace FE;
using namespace FE::AST;

static YaccParser::symbol_type yylex(Scanner& scanner, Parser& parser)
{
    (void)parser;
    return scanner.nextToken();
}

extern size_t errCnt;

#line 59 "frontend/parser/yacc.cpp"

#include "yacc.h"

#ifndef YY_
#if defined YYENABLE_NLS && YYENABLE_NLS
#if ENABLE_NLS
#include <libintl.h>  // FIXME: INFRINGES ON USER NAME SPACE.
#define YY_(msgid) dgettext("bison-runtime", msgid)
#endif
#endif
#ifndef YY_
#define YY_(msgid) msgid
#endif
#endif

// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
#if defined __GNUC__ && !defined __EXCEPTIONS
#define YY_EXCEPTIONS 0
#else
#define YY_EXCEPTIONS 1
#endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
#define YYLLOC_DEFAULT(Current, Rhs, N)                                  \
    do                                                                   \
        if (N)                                                           \
        {                                                                \
            (Current).begin = YYRHSLOC(Rhs, 1).begin;                    \
            (Current).end   = YYRHSLOC(Rhs, N).end;                      \
        }                                                                \
        else { (Current).begin = (Current).end = YYRHSLOC(Rhs, 0).end; } \
    while (false)
#endif

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
#define YYCDEBUG \
    if (yydebug_) (*yycdebug_)

#define YY_SYMBOL_PRINT(Title, Symbol)     \
    do {                                   \
        if (yydebug_)                      \
        {                                  \
            *yycdebug_ << Title << ' ';    \
            yy_print_(*yycdebug_, Symbol); \
            *yycdebug_ << '\n';            \
        }                                  \
    } while (false)

#define YY_REDUCE_PRINT(Rule)                 \
    do {                                      \
        if (yydebug_) yy_reduce_print_(Rule); \
    } while (false)

#define YY_STACK_PRINT()                 \
    do {                                 \
        if (yydebug_) yy_stack_print_(); \
    } while (false)

#else  // !YYDEBUG

#define YYCDEBUG \
    if (false) std::cerr
#define YY_SYMBOL_PRINT(Title, Symbol) YY_USE(Symbol)
#define YY_REDUCE_PRINT(Rule) static_cast<void>(0)
#define YY_STACK_PRINT() static_cast<void>(0)

#endif  // !YYDEBUG

#define yyerrok (yyerrstatus_ = 0)
#define yyclearin (yyla.clear())

#define YYACCEPT goto yyacceptlab
#define YYABORT goto yyabortlab
#define YYERROR goto yyerrorlab
#define YYRECOVERING() (!!yyerrstatus_)

#line 4 "frontend/parser/yacc.y"
namespace FE
{
#line 159 "frontend/parser/yacc.cpp"

    /// Build a parser object.
    YaccParser ::YaccParser(FE::Scanner& scanner_yyarg, FE::Parser& parser_yyarg)
#if YYDEBUG
        : yydebug_(false),
          yycdebug_(&std::cerr),
#else
        :
#endif
          scanner(scanner_yyarg),
          parser(parser_yyarg)
    {}

    YaccParser ::~YaccParser() {}

    YaccParser ::syntax_error::~syntax_error() YY_NOEXCEPT YY_NOTHROW {}

    /*---------.
    | symbol.  |
    `---------*/

    // by_state.
    YaccParser ::by_state::by_state() YY_NOEXCEPT : state(empty_state) {}

    YaccParser ::by_state::by_state(const by_state& that) YY_NOEXCEPT : state(that.state) {}

    void YaccParser ::by_state::clear() YY_NOEXCEPT { state = empty_state; }

    void YaccParser ::by_state::move(by_state& that)
    {
        state = that.state;
        that.clear();
    }

    YaccParser ::by_state::by_state(state_type s) YY_NOEXCEPT : state(s) {}

    YaccParser ::symbol_kind_type YaccParser ::by_state::kind() const YY_NOEXCEPT
    {
        if (state == empty_state)
            return symbol_kind::S_YYEMPTY;
        else
            return YY_CAST(symbol_kind_type, yystos_[+state]);
    }

    YaccParser ::stack_symbol_type::stack_symbol_type() {}

    YaccParser ::stack_symbol_type::stack_symbol_type(YY_RVREF(stack_symbol_type) that)
        : super_type(YY_MOVE(that.state), YY_MOVE(that.location))
    {
        switch (that.kind())
        {
            case symbol_kind::S_ASSIGN_EXPR:           // ASSIGN_EXPR
            case symbol_kind::S_EXPR:                  // EXPR
            case symbol_kind::S_NOCOMMA_EXPR:          // NOCOMMA_EXPR
            case symbol_kind::S_LOGICAL_OR_EXPR:       // LOGICAL_OR_EXPR
            case symbol_kind::S_LOGICAL_AND_EXPR:      // LOGICAL_AND_EXPR
            case symbol_kind::S_EQUALITY_EXPR:         // EQUALITY_EXPR
            case symbol_kind::S_RELATIONAL_EXPR:       // RELATIONAL_EXPR
            case symbol_kind::S_ADDSUB_EXPR:           // ADDSUB_EXPR
            case symbol_kind::S_MULDIV_EXPR:           // MULDIV_EXPR
            case symbol_kind::S_UNARY_EXPR:            // UNARY_EXPR
            case symbol_kind::S_BASIC_EXPR:            // BASIC_EXPR
            case symbol_kind::S_FUNC_CALL_EXPR:        // FUNC_CALL_EXPR
            case symbol_kind::S_ARRAY_DIMENSION_EXPR:  // ARRAY_DIMENSION_EXPR
            case symbol_kind::S_LEFT_VAL_EXPR:         // LEFT_VAL_EXPR
            case symbol_kind::S_LITERAL_EXPR:          // LITERAL_EXPR
                value.YY_MOVE_OR_COPY<FE::AST::ExprNode*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_INITIALIZER:  // INITIALIZER
                value.YY_MOVE_OR_COPY<FE::AST::InitDecl*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_UNARY_OP:  // UNARY_OP
                value.YY_MOVE_OR_COPY<FE::AST::Operator>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_PARAM_DECLARATOR:  // PARAM_DECLARATOR
                value.YY_MOVE_OR_COPY<FE::AST::ParamDeclarator*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_PROGRAM:  // PROGRAM
                value.YY_MOVE_OR_COPY<FE::AST::Root*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_STMT:            // STMT
            case symbol_kind::S_CONTINUE_STMT:   // CONTINUE_STMT
            case symbol_kind::S_EXPR_STMT:       // EXPR_STMT
            case symbol_kind::S_VAR_DECL_STMT:   // VAR_DECL_STMT
            case symbol_kind::S_FUNC_BODY:       // FUNC_BODY
            case symbol_kind::S_FUNC_DECL_STMT:  // FUNC_DECL_STMT
            case symbol_kind::S_FOR_STMT:        // FOR_STMT
            case symbol_kind::S_IF_STMT:         // IF_STMT
            case symbol_kind::S_WHILE_STMT:      // WHILE_STMT
            case symbol_kind::S_RETURN_STMT:     // RETURN_STMT
            case symbol_kind::S_BREAK_STMT:      // BREAK_STMT
            case symbol_kind::S_BLOCK_STMT:      // BLOCK_STMT
                value.YY_MOVE_OR_COPY<FE::AST::StmtNode*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_TYPE:  // TYPE
                value.YY_MOVE_OR_COPY<FE::AST::Type*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_VAR_DECLARATION:  // VAR_DECLARATION
                value.YY_MOVE_OR_COPY<FE::AST::VarDeclaration*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_VAR_DECLARATOR:  // VAR_DECLARATOR
                value.YY_MOVE_OR_COPY<FE::AST::VarDeclarator*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_FLOAT_CONST:  // FLOAT_CONST
                value.YY_MOVE_OR_COPY<float>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_INT_CONST:  // INT_CONST
                value.YY_MOVE_OR_COPY<int>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_LL_CONST:  // LL_CONST
                value.YY_MOVE_OR_COPY<long long>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_STR_CONST:      // STR_CONST
            case symbol_kind::S_ERR_TOKEN:      // ERR_TOKEN
            case symbol_kind::S_SLASH_COMMENT:  // SLASH_COMMENT
            case symbol_kind::S_IDENT:          // IDENT
                value.YY_MOVE_OR_COPY<std::string>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_EXPR_LIST:                  // EXPR_LIST
            case symbol_kind::S_ARRAY_DIMENSION_EXPR_LIST:  // ARRAY_DIMENSION_EXPR_LIST
                value.YY_MOVE_OR_COPY<std::vector<FE::AST::ExprNode*>*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_INITIALIZER_LIST:  // INITIALIZER_LIST
                value.YY_MOVE_OR_COPY<std::vector<FE::AST::InitDecl*>*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_PARAM_DECLARATOR_LIST:  // PARAM_DECLARATOR_LIST
                value.YY_MOVE_OR_COPY<std::vector<FE::AST::ParamDeclarator*>*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_STMT_LIST:  // STMT_LIST
                value.YY_MOVE_OR_COPY<std::vector<FE::AST::StmtNode*>*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_VAR_DECLARATOR_LIST:  // VAR_DECLARATOR_LIST
                value.YY_MOVE_OR_COPY<std::vector<FE::AST::VarDeclarator*>*>(YY_MOVE(that.value));
                break;

            default: break;
        }

#if 201103L <= YY_CPLUSPLUS
        // that is emptied.
        that.state = empty_state;
#endif
    }

    YaccParser ::stack_symbol_type::stack_symbol_type(state_type s, YY_MOVE_REF(symbol_type) that)
        : super_type(s, YY_MOVE(that.location))
    {
        switch (that.kind())
        {
            case symbol_kind::S_ASSIGN_EXPR:           // ASSIGN_EXPR
            case symbol_kind::S_EXPR:                  // EXPR
            case symbol_kind::S_NOCOMMA_EXPR:          // NOCOMMA_EXPR
            case symbol_kind::S_LOGICAL_OR_EXPR:       // LOGICAL_OR_EXPR
            case symbol_kind::S_LOGICAL_AND_EXPR:      // LOGICAL_AND_EXPR
            case symbol_kind::S_EQUALITY_EXPR:         // EQUALITY_EXPR
            case symbol_kind::S_RELATIONAL_EXPR:       // RELATIONAL_EXPR
            case symbol_kind::S_ADDSUB_EXPR:           // ADDSUB_EXPR
            case symbol_kind::S_MULDIV_EXPR:           // MULDIV_EXPR
            case symbol_kind::S_UNARY_EXPR:            // UNARY_EXPR
            case symbol_kind::S_BASIC_EXPR:            // BASIC_EXPR
            case symbol_kind::S_FUNC_CALL_EXPR:        // FUNC_CALL_EXPR
            case symbol_kind::S_ARRAY_DIMENSION_EXPR:  // ARRAY_DIMENSION_EXPR
            case symbol_kind::S_LEFT_VAL_EXPR:         // LEFT_VAL_EXPR
            case symbol_kind::S_LITERAL_EXPR:          // LITERAL_EXPR
                value.move<FE::AST::ExprNode*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_INITIALIZER:  // INITIALIZER
                value.move<FE::AST::InitDecl*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_UNARY_OP:  // UNARY_OP
                value.move<FE::AST::Operator>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_PARAM_DECLARATOR:  // PARAM_DECLARATOR
                value.move<FE::AST::ParamDeclarator*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_PROGRAM:  // PROGRAM
                value.move<FE::AST::Root*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_STMT:            // STMT
            case symbol_kind::S_CONTINUE_STMT:   // CONTINUE_STMT
            case symbol_kind::S_EXPR_STMT:       // EXPR_STMT
            case symbol_kind::S_VAR_DECL_STMT:   // VAR_DECL_STMT
            case symbol_kind::S_FUNC_BODY:       // FUNC_BODY
            case symbol_kind::S_FUNC_DECL_STMT:  // FUNC_DECL_STMT
            case symbol_kind::S_FOR_STMT:        // FOR_STMT
            case symbol_kind::S_IF_STMT:         // IF_STMT
            case symbol_kind::S_WHILE_STMT:      // WHILE_STMT
            case symbol_kind::S_RETURN_STMT:     // RETURN_STMT
            case symbol_kind::S_BREAK_STMT:      // BREAK_STMT
            case symbol_kind::S_BLOCK_STMT:      // BLOCK_STMT
                value.move<FE::AST::StmtNode*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_TYPE:  // TYPE
                value.move<FE::AST::Type*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_VAR_DECLARATION:  // VAR_DECLARATION
                value.move<FE::AST::VarDeclaration*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_VAR_DECLARATOR:  // VAR_DECLARATOR
                value.move<FE::AST::VarDeclarator*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_FLOAT_CONST:  // FLOAT_CONST
                value.move<float>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_INT_CONST:  // INT_CONST
                value.move<int>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_LL_CONST:  // LL_CONST
                value.move<long long>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_STR_CONST:      // STR_CONST
            case symbol_kind::S_ERR_TOKEN:      // ERR_TOKEN
            case symbol_kind::S_SLASH_COMMENT:  // SLASH_COMMENT
            case symbol_kind::S_IDENT:          // IDENT
                value.move<std::string>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_EXPR_LIST:                  // EXPR_LIST
            case symbol_kind::S_ARRAY_DIMENSION_EXPR_LIST:  // ARRAY_DIMENSION_EXPR_LIST
                value.move<std::vector<FE::AST::ExprNode*>*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_INITIALIZER_LIST:  // INITIALIZER_LIST
                value.move<std::vector<FE::AST::InitDecl*>*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_PARAM_DECLARATOR_LIST:  // PARAM_DECLARATOR_LIST
                value.move<std::vector<FE::AST::ParamDeclarator*>*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_STMT_LIST:  // STMT_LIST
                value.move<std::vector<FE::AST::StmtNode*>*>(YY_MOVE(that.value));
                break;

            case symbol_kind::S_VAR_DECLARATOR_LIST:  // VAR_DECLARATOR_LIST
                value.move<std::vector<FE::AST::VarDeclarator*>*>(YY_MOVE(that.value));
                break;

            default: break;
        }

        // that is emptied.
        that.kind_ = symbol_kind::S_YYEMPTY;
    }

#if YY_CPLUSPLUS < 201103L
    YaccParser ::stack_symbol_type& YaccParser ::stack_symbol_type::operator=(const stack_symbol_type& that)
    {
        state = that.state;
        switch (that.kind())
        {
            case symbol_kind::S_ASSIGN_EXPR:           // ASSIGN_EXPR
            case symbol_kind::S_EXPR:                  // EXPR
            case symbol_kind::S_NOCOMMA_EXPR:          // NOCOMMA_EXPR
            case symbol_kind::S_LOGICAL_OR_EXPR:       // LOGICAL_OR_EXPR
            case symbol_kind::S_LOGICAL_AND_EXPR:      // LOGICAL_AND_EXPR
            case symbol_kind::S_EQUALITY_EXPR:         // EQUALITY_EXPR
            case symbol_kind::S_RELATIONAL_EXPR:       // RELATIONAL_EXPR
            case symbol_kind::S_ADDSUB_EXPR:           // ADDSUB_EXPR
            case symbol_kind::S_MULDIV_EXPR:           // MULDIV_EXPR
            case symbol_kind::S_UNARY_EXPR:            // UNARY_EXPR
            case symbol_kind::S_BASIC_EXPR:            // BASIC_EXPR
            case symbol_kind::S_FUNC_CALL_EXPR:        // FUNC_CALL_EXPR
            case symbol_kind::S_ARRAY_DIMENSION_EXPR:  // ARRAY_DIMENSION_EXPR
            case symbol_kind::S_LEFT_VAL_EXPR:         // LEFT_VAL_EXPR
            case symbol_kind::S_LITERAL_EXPR:          // LITERAL_EXPR
                value.copy<FE::AST::ExprNode*>(that.value);
                break;

            case symbol_kind::S_INITIALIZER:  // INITIALIZER
                value.copy<FE::AST::InitDecl*>(that.value);
                break;

            case symbol_kind::S_UNARY_OP:  // UNARY_OP
                value.copy<FE::AST::Operator>(that.value);
                break;

            case symbol_kind::S_PARAM_DECLARATOR:  // PARAM_DECLARATOR
                value.copy<FE::AST::ParamDeclarator*>(that.value);
                break;

            case symbol_kind::S_PROGRAM:  // PROGRAM
                value.copy<FE::AST::Root*>(that.value);
                break;

            case symbol_kind::S_STMT:            // STMT
            case symbol_kind::S_CONTINUE_STMT:   // CONTINUE_STMT
            case symbol_kind::S_EXPR_STMT:       // EXPR_STMT
            case symbol_kind::S_VAR_DECL_STMT:   // VAR_DECL_STMT
            case symbol_kind::S_FUNC_BODY:       // FUNC_BODY
            case symbol_kind::S_FUNC_DECL_STMT:  // FUNC_DECL_STMT
            case symbol_kind::S_FOR_STMT:        // FOR_STMT
            case symbol_kind::S_IF_STMT:         // IF_STMT
            case symbol_kind::S_WHILE_STMT:      // WHILE_STMT
            case symbol_kind::S_RETURN_STMT:     // RETURN_STMT
            case symbol_kind::S_BREAK_STMT:      // BREAK_STMT
            case symbol_kind::S_BLOCK_STMT:      // BLOCK_STMT
                value.copy<FE::AST::StmtNode*>(that.value);
                break;

            case symbol_kind::S_TYPE:  // TYPE
                value.copy<FE::AST::Type*>(that.value);
                break;

            case symbol_kind::S_VAR_DECLARATION:  // VAR_DECLARATION
                value.copy<FE::AST::VarDeclaration*>(that.value);
                break;

            case symbol_kind::S_VAR_DECLARATOR:  // VAR_DECLARATOR
                value.copy<FE::AST::VarDeclarator*>(that.value);
                break;

            case symbol_kind::S_FLOAT_CONST:  // FLOAT_CONST
                value.copy<float>(that.value);
                break;

            case symbol_kind::S_INT_CONST:  // INT_CONST
                value.copy<int>(that.value);
                break;

            case symbol_kind::S_LL_CONST:  // LL_CONST
                value.copy<long long>(that.value);
                break;

            case symbol_kind::S_STR_CONST:      // STR_CONST
            case symbol_kind::S_ERR_TOKEN:      // ERR_TOKEN
            case symbol_kind::S_SLASH_COMMENT:  // SLASH_COMMENT
            case symbol_kind::S_IDENT:          // IDENT
                value.copy<std::string>(that.value);
                break;

            case symbol_kind::S_EXPR_LIST:                  // EXPR_LIST
            case symbol_kind::S_ARRAY_DIMENSION_EXPR_LIST:  // ARRAY_DIMENSION_EXPR_LIST
                value.copy<std::vector<FE::AST::ExprNode*>*>(that.value);
                break;

            case symbol_kind::S_INITIALIZER_LIST:  // INITIALIZER_LIST
                value.copy<std::vector<FE::AST::InitDecl*>*>(that.value);
                break;

            case symbol_kind::S_PARAM_DECLARATOR_LIST:  // PARAM_DECLARATOR_LIST
                value.copy<std::vector<FE::AST::ParamDeclarator*>*>(that.value);
                break;

            case symbol_kind::S_STMT_LIST:  // STMT_LIST
                value.copy<std::vector<FE::AST::StmtNode*>*>(that.value);
                break;

            case symbol_kind::S_VAR_DECLARATOR_LIST:  // VAR_DECLARATOR_LIST
                value.copy<std::vector<FE::AST::VarDeclarator*>*>(that.value);
                break;

            default: break;
        }

        location = that.location;
        return *this;
    }

    YaccParser ::stack_symbol_type& YaccParser ::stack_symbol_type::operator=(stack_symbol_type& that)
    {
        state = that.state;
        switch (that.kind())
        {
            case symbol_kind::S_ASSIGN_EXPR:           // ASSIGN_EXPR
            case symbol_kind::S_EXPR:                  // EXPR
            case symbol_kind::S_NOCOMMA_EXPR:          // NOCOMMA_EXPR
            case symbol_kind::S_LOGICAL_OR_EXPR:       // LOGICAL_OR_EXPR
            case symbol_kind::S_LOGICAL_AND_EXPR:      // LOGICAL_AND_EXPR
            case symbol_kind::S_EQUALITY_EXPR:         // EQUALITY_EXPR
            case symbol_kind::S_RELATIONAL_EXPR:       // RELATIONAL_EXPR
            case symbol_kind::S_ADDSUB_EXPR:           // ADDSUB_EXPR
            case symbol_kind::S_MULDIV_EXPR:           // MULDIV_EXPR
            case symbol_kind::S_UNARY_EXPR:            // UNARY_EXPR
            case symbol_kind::S_BASIC_EXPR:            // BASIC_EXPR
            case symbol_kind::S_FUNC_CALL_EXPR:        // FUNC_CALL_EXPR
            case symbol_kind::S_ARRAY_DIMENSION_EXPR:  // ARRAY_DIMENSION_EXPR
            case symbol_kind::S_LEFT_VAL_EXPR:         // LEFT_VAL_EXPR
            case symbol_kind::S_LITERAL_EXPR:          // LITERAL_EXPR
                value.move<FE::AST::ExprNode*>(that.value);
                break;

            case symbol_kind::S_INITIALIZER:  // INITIALIZER
                value.move<FE::AST::InitDecl*>(that.value);
                break;

            case symbol_kind::S_UNARY_OP:  // UNARY_OP
                value.move<FE::AST::Operator>(that.value);
                break;

            case symbol_kind::S_PARAM_DECLARATOR:  // PARAM_DECLARATOR
                value.move<FE::AST::ParamDeclarator*>(that.value);
                break;

            case symbol_kind::S_PROGRAM:  // PROGRAM
                value.move<FE::AST::Root*>(that.value);
                break;

            case symbol_kind::S_STMT:            // STMT
            case symbol_kind::S_CONTINUE_STMT:   // CONTINUE_STMT
            case symbol_kind::S_EXPR_STMT:       // EXPR_STMT
            case symbol_kind::S_VAR_DECL_STMT:   // VAR_DECL_STMT
            case symbol_kind::S_FUNC_BODY:       // FUNC_BODY
            case symbol_kind::S_FUNC_DECL_STMT:  // FUNC_DECL_STMT
            case symbol_kind::S_FOR_STMT:        // FOR_STMT
            case symbol_kind::S_IF_STMT:         // IF_STMT
            case symbol_kind::S_WHILE_STMT:      // WHILE_STMT
            case symbol_kind::S_RETURN_STMT:     // RETURN_STMT
            case symbol_kind::S_BREAK_STMT:      // BREAK_STMT
            case symbol_kind::S_BLOCK_STMT:      // BLOCK_STMT
                value.move<FE::AST::StmtNode*>(that.value);
                break;

            case symbol_kind::S_TYPE:  // TYPE
                value.move<FE::AST::Type*>(that.value);
                break;

            case symbol_kind::S_VAR_DECLARATION:  // VAR_DECLARATION
                value.move<FE::AST::VarDeclaration*>(that.value);
                break;

            case symbol_kind::S_VAR_DECLARATOR:  // VAR_DECLARATOR
                value.move<FE::AST::VarDeclarator*>(that.value);
                break;

            case symbol_kind::S_FLOAT_CONST:  // FLOAT_CONST
                value.move<float>(that.value);
                break;

            case symbol_kind::S_INT_CONST:  // INT_CONST
                value.move<int>(that.value);
                break;

            case symbol_kind::S_LL_CONST:  // LL_CONST
                value.move<long long>(that.value);
                break;

            case symbol_kind::S_STR_CONST:      // STR_CONST
            case symbol_kind::S_ERR_TOKEN:      // ERR_TOKEN
            case symbol_kind::S_SLASH_COMMENT:  // SLASH_COMMENT
            case symbol_kind::S_IDENT:          // IDENT
                value.move<std::string>(that.value);
                break;

            case symbol_kind::S_EXPR_LIST:                  // EXPR_LIST
            case symbol_kind::S_ARRAY_DIMENSION_EXPR_LIST:  // ARRAY_DIMENSION_EXPR_LIST
                value.move<std::vector<FE::AST::ExprNode*>*>(that.value);
                break;

            case symbol_kind::S_INITIALIZER_LIST:  // INITIALIZER_LIST
                value.move<std::vector<FE::AST::InitDecl*>*>(that.value);
                break;

            case symbol_kind::S_PARAM_DECLARATOR_LIST:  // PARAM_DECLARATOR_LIST
                value.move<std::vector<FE::AST::ParamDeclarator*>*>(that.value);
                break;

            case symbol_kind::S_STMT_LIST:  // STMT_LIST
                value.move<std::vector<FE::AST::StmtNode*>*>(that.value);
                break;

            case symbol_kind::S_VAR_DECLARATOR_LIST:  // VAR_DECLARATOR_LIST
                value.move<std::vector<FE::AST::VarDeclarator*>*>(that.value);
                break;

            default: break;
        }

        location = that.location;
        // that is emptied.
        that.state = empty_state;
        return *this;
    }
#endif

    template <typename Base>
    void YaccParser ::yy_destroy_(const char* yymsg, basic_symbol<Base>& yysym) const
    {
        if (yymsg) YY_SYMBOL_PRINT(yymsg, yysym);
    }

#if YYDEBUG
    template <typename Base>
    void YaccParser ::yy_print_(std::ostream& yyo, const basic_symbol<Base>& yysym) const
    {
        std::ostream& yyoutput = yyo;
        YY_USE(yyoutput);
        if (yysym.empty())
            yyo << "empty symbol";
        else
        {
            symbol_kind_type yykind = yysym.kind();
            yyo << (yykind < YYNTOKENS ? "token" : "nterm") << ' ' << yysym.name() << " (" << yysym.location << ": ";
            YY_USE(yykind);
            yyo << ')';
        }
    }
#endif

    void YaccParser ::yypush_(const char* m, YY_MOVE_REF(stack_symbol_type) sym)
    {
        if (m) YY_SYMBOL_PRINT(m, sym);
        yystack_.push(YY_MOVE(sym));
    }

    void YaccParser ::yypush_(const char* m, state_type s, YY_MOVE_REF(symbol_type) sym)
    {
#if 201103L <= YY_CPLUSPLUS
        yypush_(m, stack_symbol_type(s, std::move(sym)));
#else
        stack_symbol_type ss(s, sym);
        yypush_(m, ss);
#endif
    }

    void YaccParser ::yypop_(int n) YY_NOEXCEPT { yystack_.pop(n); }

#if YYDEBUG
    std::ostream& YaccParser ::debug_stream() const { return *yycdebug_; }

    void YaccParser ::set_debug_stream(std::ostream& o) { yycdebug_ = &o; }

    YaccParser ::debug_level_type YaccParser ::debug_level() const { return yydebug_; }

    void YaccParser ::set_debug_level(debug_level_type l) { yydebug_ = l; }
#endif  // YYDEBUG

    YaccParser ::state_type YaccParser ::yy_lr_goto_state_(state_type yystate, int yysym)
    {
        int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
        if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
            return yytable_[yyr];
        else
            return yydefgoto_[yysym - YYNTOKENS];
    }

    bool YaccParser ::yy_pact_value_is_default_(int yyvalue) YY_NOEXCEPT { return yyvalue == yypact_ninf_; }

    bool YaccParser ::yy_table_value_is_error_(int yyvalue) YY_NOEXCEPT { return yyvalue == yytable_ninf_; }

    int YaccParser ::operator()() { return parse(); }

    int YaccParser ::parse()
    {
        int yyn;
        /// Length of the RHS of the rule being reduced.
        int yylen = 0;

        // Error handling.
        int yynerrs_     = 0;
        int yyerrstatus_ = 0;

        /// The lookahead symbol.
        symbol_type yyla;

        /// The locations where the error started and ended.
        stack_symbol_type yyerror_range[3];

        /// The return value of parse ().
        int yyresult;

#if YY_EXCEPTIONS
        try
#endif  // YY_EXCEPTIONS
        {
            YYCDEBUG << "Starting parse\n";

            /* Initialize the stack.  The initial state will be set in
               yynewstate, since the latter expects the semantical and the
               location values to have been already stored, initialize these
               stacks with a primary value.  */
            yystack_.clear();
            yypush_(YY_NULLPTR, 0, YY_MOVE(yyla));

        /*-----------------------------------------------.
        | yynewstate -- push a new symbol on the stack.  |
        `-----------------------------------------------*/
        yynewstate:
            YYCDEBUG << "Entering state " << int(yystack_[0].state) << '\n';
            YY_STACK_PRINT();

            // Accept?
            if (yystack_[0].state == yyfinal_) YYACCEPT;

            goto yybackup;

        /*-----------.
        | yybackup.  |
        `-----------*/
        yybackup:
            // Try to take a decision without lookahead.
            yyn = yypact_[+yystack_[0].state];
            if (yy_pact_value_is_default_(yyn)) goto yydefault;

            // Read a lookahead token.
            if (yyla.empty())
            {
                YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
                try
#endif  // YY_EXCEPTIONS
                {
                    symbol_type yylookahead(yylex(scanner, parser));
                    yyla.move(yylookahead);
                }
#if YY_EXCEPTIONS
                catch (const syntax_error& yyexc)
                {
                    YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
                    error(yyexc);
                    goto yyerrlab1;
                }
#endif  // YY_EXCEPTIONS
            }
            YY_SYMBOL_PRINT("Next token is", yyla);

            if (yyla.kind() == symbol_kind::S_YYerror)
            {
                // The scanner already issued an error message, process directly
                // to error recovery.  But do not keep the error token as
                // lookahead, it is too special and may lead us to an endless
                // loop in error recovery. */
                yyla.kind_ = symbol_kind::S_YYUNDEF;
                goto yyerrlab1;
            }

            /* If the proper action on seeing token YYLA.TYPE is to reduce or
               to detect an error, take that action.  */
            yyn += yyla.kind();
            if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind()) { goto yydefault; }

            // Reduce or error.
            yyn = yytable_[yyn];
            if (yyn <= 0)
            {
                if (yy_table_value_is_error_(yyn)) goto yyerrlab;
                yyn = -yyn;
                goto yyreduce;
            }

            // Count tokens shifted since error; after three, turn off error status.
            if (yyerrstatus_) --yyerrstatus_;

            // Shift the lookahead token.
            yypush_("Shifting", state_type(yyn), YY_MOVE(yyla));
            goto yynewstate;

        /*-----------------------------------------------------------.
        | yydefault -- do the default action for the current state.  |
        `-----------------------------------------------------------*/
        yydefault:
            yyn = yydefact_[+yystack_[0].state];
            if (yyn == 0) goto yyerrlab;
            goto yyreduce;

        /*-----------------------------.
        | yyreduce -- do a reduction.  |
        `-----------------------------*/
        yyreduce:
            yylen = yyr2_[yyn];
            {
                stack_symbol_type yylhs;
                yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
                /* Variants are always initialized to an empty instance of the
                   correct type. The default '$$ = $1' action is NOT applied
                   when using variants.  */
                switch (yyr1_[yyn])
                {
                    case symbol_kind::S_ASSIGN_EXPR:           // ASSIGN_EXPR
                    case symbol_kind::S_EXPR:                  // EXPR
                    case symbol_kind::S_NOCOMMA_EXPR:          // NOCOMMA_EXPR
                    case symbol_kind::S_LOGICAL_OR_EXPR:       // LOGICAL_OR_EXPR
                    case symbol_kind::S_LOGICAL_AND_EXPR:      // LOGICAL_AND_EXPR
                    case symbol_kind::S_EQUALITY_EXPR:         // EQUALITY_EXPR
                    case symbol_kind::S_RELATIONAL_EXPR:       // RELATIONAL_EXPR
                    case symbol_kind::S_ADDSUB_EXPR:           // ADDSUB_EXPR
                    case symbol_kind::S_MULDIV_EXPR:           // MULDIV_EXPR
                    case symbol_kind::S_UNARY_EXPR:            // UNARY_EXPR
                    case symbol_kind::S_BASIC_EXPR:            // BASIC_EXPR
                    case symbol_kind::S_FUNC_CALL_EXPR:        // FUNC_CALL_EXPR
                    case symbol_kind::S_ARRAY_DIMENSION_EXPR:  // ARRAY_DIMENSION_EXPR
                    case symbol_kind::S_LEFT_VAL_EXPR:         // LEFT_VAL_EXPR
                    case symbol_kind::S_LITERAL_EXPR:          // LITERAL_EXPR
                        yylhs.value.emplace<FE::AST::ExprNode*>();
                        break;

                    case symbol_kind::S_INITIALIZER:  // INITIALIZER
                        yylhs.value.emplace<FE::AST::InitDecl*>();
                        break;

                    case symbol_kind::S_UNARY_OP:  // UNARY_OP
                        yylhs.value.emplace<FE::AST::Operator>();
                        break;

                    case symbol_kind::S_PARAM_DECLARATOR:  // PARAM_DECLARATOR
                        yylhs.value.emplace<FE::AST::ParamDeclarator*>();
                        break;

                    case symbol_kind::S_PROGRAM:  // PROGRAM
                        yylhs.value.emplace<FE::AST::Root*>();
                        break;

                    case symbol_kind::S_STMT:            // STMT
                    case symbol_kind::S_CONTINUE_STMT:   // CONTINUE_STMT
                    case symbol_kind::S_EXPR_STMT:       // EXPR_STMT
                    case symbol_kind::S_VAR_DECL_STMT:   // VAR_DECL_STMT
                    case symbol_kind::S_FUNC_BODY:       // FUNC_BODY
                    case symbol_kind::S_FUNC_DECL_STMT:  // FUNC_DECL_STMT
                    case symbol_kind::S_FOR_STMT:        // FOR_STMT
                    case symbol_kind::S_IF_STMT:         // IF_STMT
                    case symbol_kind::S_WHILE_STMT:      // WHILE_STMT
                    case symbol_kind::S_RETURN_STMT:     // RETURN_STMT
                    case symbol_kind::S_BREAK_STMT:      // BREAK_STMT
                    case symbol_kind::S_BLOCK_STMT:      // BLOCK_STMT
                        yylhs.value.emplace<FE::AST::StmtNode*>();
                        break;

                    case symbol_kind::S_TYPE:  // TYPE
                        yylhs.value.emplace<FE::AST::Type*>();
                        break;

                    case symbol_kind::S_VAR_DECLARATION:  // VAR_DECLARATION
                        yylhs.value.emplace<FE::AST::VarDeclaration*>();
                        break;

                    case symbol_kind::S_VAR_DECLARATOR:  // VAR_DECLARATOR
                        yylhs.value.emplace<FE::AST::VarDeclarator*>();
                        break;

                    case symbol_kind::S_FLOAT_CONST:  // FLOAT_CONST
                        yylhs.value.emplace<float>();
                        break;

                    case symbol_kind::S_INT_CONST:  // INT_CONST
                        yylhs.value.emplace<int>();
                        break;

                    case symbol_kind::S_LL_CONST:  // LL_CONST
                        yylhs.value.emplace<long long>();
                        break;

                    case symbol_kind::S_STR_CONST:      // STR_CONST
                    case symbol_kind::S_ERR_TOKEN:      // ERR_TOKEN
                    case symbol_kind::S_SLASH_COMMENT:  // SLASH_COMMENT
                    case symbol_kind::S_IDENT:          // IDENT
                        yylhs.value.emplace<std::string>();
                        break;

                    case symbol_kind::S_EXPR_LIST:                  // EXPR_LIST
                    case symbol_kind::S_ARRAY_DIMENSION_EXPR_LIST:  // ARRAY_DIMENSION_EXPR_LIST
                        yylhs.value.emplace<std::vector<FE::AST::ExprNode*>*>();
                        break;

                    case symbol_kind::S_INITIALIZER_LIST:  // INITIALIZER_LIST
                        yylhs.value.emplace<std::vector<FE::AST::InitDecl*>*>();
                        break;

                    case symbol_kind::S_PARAM_DECLARATOR_LIST:  // PARAM_DECLARATOR_LIST
                        yylhs.value.emplace<std::vector<FE::AST::ParamDeclarator*>*>();
                        break;

                    case symbol_kind::S_STMT_LIST:  // STMT_LIST
                        yylhs.value.emplace<std::vector<FE::AST::StmtNode*>*>();
                        break;

                    case symbol_kind::S_VAR_DECLARATOR_LIST:  // VAR_DECLARATOR_LIST
                        yylhs.value.emplace<std::vector<FE::AST::VarDeclarator*>*>();
                        break;

                    default: break;
                }

                // Default location.
                {
                    stack_type::slice range(yystack_, yylen);
                    YYLLOC_DEFAULT(yylhs.location, range, yylen);
                    yyerror_range[1].location = yylhs.location;
                }

                // Perform the reduction.
                YY_REDUCE_PRINT(yyn);
#if YY_EXCEPTIONS
                try
#endif  // YY_EXCEPTIONS
                {
                    switch (yyn)
                    {
                        case 2:  // PROGRAM: STMT_LIST
#line 140 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::Root*>() =
                                new Root(yystack_[0].value.as<std::vector<FE::AST::StmtNode*>*>());
                            parser.ast = yylhs.value.as<FE::AST::Root*>();
                        }
#line 1062 "frontend/parser/yacc.cpp"
                        break;

                        case 3:  // PROGRAM: PROGRAM END
#line 144 "frontend/parser/yacc.y"
                        {
                            YYACCEPT;
                        }
#line 1070 "frontend/parser/yacc.cpp"
                        break;

                        case 4:  // STMT_LIST: STMT
#line 150 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<std::vector<FE::AST::StmtNode*>*>() = new std::vector<StmtNode*>();
                            if (yystack_[0].value.as<FE::AST::StmtNode*>())
                                yylhs.value.as<std::vector<FE::AST::StmtNode*>*>()->push_back(
                                    yystack_[0].value.as<FE::AST::StmtNode*>());
                        }
#line 1079 "frontend/parser/yacc.cpp"
                        break;

                        case 5:  // STMT_LIST: STMT_LIST STMT
#line 154 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<std::vector<FE::AST::StmtNode*>*>() =
                                yystack_[1].value.as<std::vector<FE::AST::StmtNode*>*>();
                            if (yystack_[0].value.as<FE::AST::StmtNode*>())
                                yylhs.value.as<std::vector<FE::AST::StmtNode*>*>()->push_back(
                                    yystack_[0].value.as<FE::AST::StmtNode*>());
                        }
#line 1088 "frontend/parser/yacc.cpp"
                        break;

                        case 6:  // STMT: EXPR_STMT
#line 161 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() = yystack_[0].value.as<FE::AST::StmtNode*>();
                        }
#line 1096 "frontend/parser/yacc.cpp"
                        break;

                        case 7:  // STMT: VAR_DECL_STMT
#line 164 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() = yystack_[0].value.as<FE::AST::StmtNode*>();
                        }
#line 1104 "frontend/parser/yacc.cpp"
                        break;

                        case 8:  // STMT: FUNC_DECL_STMT
#line 167 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() = yystack_[0].value.as<FE::AST::StmtNode*>();
                        }
#line 1112 "frontend/parser/yacc.cpp"
                        break;

                        case 9:  // STMT: FOR_STMT
#line 170 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() = yystack_[0].value.as<FE::AST::StmtNode*>();
                        }
#line 1120 "frontend/parser/yacc.cpp"
                        break;

                        case 10:  // STMT: IF_STMT
#line 173 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() = yystack_[0].value.as<FE::AST::StmtNode*>();
                        }
#line 1128 "frontend/parser/yacc.cpp"
                        break;

                        case 11:  // STMT: CONTINUE_STMT
#line 176 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() = yystack_[0].value.as<FE::AST::StmtNode*>();
                        }
#line 1136 "frontend/parser/yacc.cpp"
                        break;

                        case 12:  // STMT: SEMICOLON
#line 179 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() = nullptr;
                        }
#line 1144 "frontend/parser/yacc.cpp"
                        break;

                        case 13:  // STMT: SLASH_COMMENT
#line 182 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() = nullptr;
                        }
#line 1152 "frontend/parser/yacc.cpp"
                        break;

                        case 14:  // STMT: WHILE_STMT
#line 185 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() = yystack_[0].value.as<FE::AST::StmtNode*>();
                        }
#line 1160 "frontend/parser/yacc.cpp"
                        break;

                        case 15:  // STMT: RETURN_STMT
#line 188 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() = yystack_[0].value.as<FE::AST::StmtNode*>();
                        }
#line 1168 "frontend/parser/yacc.cpp"
                        break;

                        case 16:  // STMT: BREAK_STMT
#line 191 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() = yystack_[0].value.as<FE::AST::StmtNode*>();
                        }
#line 1176 "frontend/parser/yacc.cpp"
                        break;

                        case 17:  // STMT: BLOCK_STMT
#line 194 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() = yystack_[0].value.as<FE::AST::StmtNode*>();
                        }
#line 1184 "frontend/parser/yacc.cpp"
                        break;

                        case 18:  // CONTINUE_STMT: CONTINUE SEMICOLON
#line 201 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() =
                                new ContinueStmt(yystack_[1].location.begin.line, yystack_[1].location.begin.column);
                        }
#line 1192 "frontend/parser/yacc.cpp"
                        break;

                        case 19:  // EXPR_STMT: EXPR SEMICOLON
#line 207 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() =
                                new ExprStmt(yystack_[1].value.as<FE::AST::ExprNode*>(),
                                    yystack_[1].location.begin.line,
                                    yystack_[1].location.begin.column);
                        }
#line 1200 "frontend/parser/yacc.cpp"
                        break;

                        case 20:  // VAR_DECLARATION: TYPE VAR_DECLARATOR_LIST
#line 213 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::VarDeclaration*>() =
                                new VarDeclaration(yystack_[1].value.as<FE::AST::Type*>(),
                                    yystack_[0].value.as<std::vector<FE::AST::VarDeclarator*>*>(),
                                    false,
                                    yystack_[1].location.begin.line,
                                    yystack_[1].location.begin.column);
                        }
#line 1208 "frontend/parser/yacc.cpp"
                        break;

                        case 21:  // VAR_DECLARATION: CONST TYPE VAR_DECLARATOR_LIST
#line 216 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::VarDeclaration*>() =
                                new VarDeclaration(yystack_[1].value.as<FE::AST::Type*>(),
                                    yystack_[0].value.as<std::vector<FE::AST::VarDeclarator*>*>(),
                                    true,
                                    yystack_[2].location.begin.line,
                                    yystack_[2].location.begin.column);
                        }
#line 1216 "frontend/parser/yacc.cpp"
                        break;

                        case 22:  // VAR_DECL_STMT: VAR_DECLARATION SEMICOLON
#line 222 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() =
                                new VarDeclStmt(yystack_[1].value.as<FE::AST::VarDeclaration*>(),
                                    yystack_[1].location.begin.line,
                                    yystack_[1].location.begin.column);
                        }
#line 1224 "frontend/parser/yacc.cpp"
                        break;

                        case 23:  // FUNC_BODY: LBRACE RBRACE
#line 228 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() = nullptr;
                        }
#line 1232 "frontend/parser/yacc.cpp"
                        break;

                        case 24:  // FUNC_BODY: LBRACE STMT_LIST RBRACE
#line 231 "frontend/parser/yacc.y"
                        {
                            if (!yystack_[1].value.as<std::vector<FE::AST::StmtNode*>*>() ||
                                yystack_[1].value.as<std::vector<FE::AST::StmtNode*>*>()->empty())
                            {
                                yylhs.value.as<FE::AST::StmtNode*>() = nullptr;
                                delete yystack_[1].value.as<std::vector<FE::AST::StmtNode*>*>();
                            }
                            else if (yystack_[1].value.as<std::vector<FE::AST::StmtNode*>*>()->size() == 1)
                            {
                                yylhs.value.as<FE::AST::StmtNode*>() =
                                    (*yystack_[1].value.as<std::vector<FE::AST::StmtNode*>*>())[0];
                                delete yystack_[1].value.as<std::vector<FE::AST::StmtNode*>*>();
                            }
                            else
                                yylhs.value.as<FE::AST::StmtNode*>() =
                                    new BlockStmt(yystack_[1].value.as<std::vector<FE::AST::StmtNode*>*>(),
                                        yystack_[2].location.begin.line,
                                        yystack_[2].location.begin.column);
                        }
#line 1250 "frontend/parser/yacc.cpp"
                        break;

                        case 25:  // FUNC_DECL_STMT: TYPE IDENT LPAREN PARAM_DECLARATOR_LIST RPAREN FUNC_BODY
#line 247 "frontend/parser/yacc.y"
                        {
                            Entry* entry = Entry::getEntry(yystack_[4].value.as<std::string>());
                            yylhs.value.as<FE::AST::StmtNode*>() =
                                new FuncDeclStmt(yystack_[5].value.as<FE::AST::Type*>(),
                                    entry,
                                    yystack_[2].value.as<std::vector<FE::AST::ParamDeclarator*>*>(),
                                    yystack_[0].value.as<FE::AST::StmtNode*>(),
                                    yystack_[5].location.begin.line,
                                    yystack_[5].location.begin.column);
                        }
#line 1259 "frontend/parser/yacc.cpp"
                        break;

                        case 26:  // FOR_STMT: FOR LPAREN VAR_DECLARATION SEMICOLON EXPR SEMICOLON EXPR RPAREN STMT
#line 254 "frontend/parser/yacc.y"
                        {
                            VarDeclStmt* initStmt = new VarDeclStmt(yystack_[6].value.as<FE::AST::VarDeclaration*>(),
                                yystack_[6].location.begin.line,
                                yystack_[6].location.begin.column);
                            yylhs.value.as<FE::AST::StmtNode*>() = new ForStmt(initStmt,
                                yystack_[4].value.as<FE::AST::ExprNode*>(),
                                yystack_[2].value.as<FE::AST::ExprNode*>(),
                                yystack_[0].value.as<FE::AST::StmtNode*>(),
                                yystack_[8].location.begin.line,
                                yystack_[8].location.begin.column);
                        }
#line 1268 "frontend/parser/yacc.cpp"
                        break;

                        case 27:  // FOR_STMT: FOR LPAREN EXPR SEMICOLON EXPR SEMICOLON EXPR RPAREN STMT
#line 258 "frontend/parser/yacc.y"
                        {
                            StmtNode* initStmt = new ExprStmt(yystack_[6].value.as<FE::AST::ExprNode*>(),
                                yystack_[6].value.as<FE::AST::ExprNode*>()->line_num,
                                yystack_[6].value.as<FE::AST::ExprNode*>()->col_num);
                            yylhs.value.as<FE::AST::StmtNode*>() = new ForStmt(initStmt,
                                yystack_[4].value.as<FE::AST::ExprNode*>(),
                                yystack_[2].value.as<FE::AST::ExprNode*>(),
                                yystack_[0].value.as<FE::AST::StmtNode*>(),
                                yystack_[8].location.begin.line,
                                yystack_[8].location.begin.column);
                        }
#line 1277 "frontend/parser/yacc.cpp"
                        break;

                        case 28:  // IF_STMT: IF LPAREN EXPR RPAREN STMT
#line 265 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() =
                                new IfStmt(yystack_[2].value.as<FE::AST::ExprNode*>(),
                                    yystack_[0].value.as<FE::AST::StmtNode*>(),
                                    nullptr,
                                    yystack_[4].location.begin.line,
                                    yystack_[4].location.begin.column);
                        }
#line 1285 "frontend/parser/yacc.cpp"
                        break;

                        case 29:  // IF_STMT: IF LPAREN EXPR RPAREN STMT ELSE STMT
#line 268 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() =
                                new IfStmt(yystack_[4].value.as<FE::AST::ExprNode*>(),
                                    yystack_[2].value.as<FE::AST::StmtNode*>(),
                                    yystack_[0].value.as<FE::AST::StmtNode*>(),
                                    yystack_[6].location.begin.line,
                                    yystack_[6].location.begin.column);
                        }
#line 1293 "frontend/parser/yacc.cpp"
                        break;

                        case 30:  // WHILE_STMT: WHILE LPAREN EXPR RPAREN STMT
#line 276 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() =
                                new WhileStmt(yystack_[2].value.as<FE::AST::ExprNode*>(),
                                    yystack_[0].value.as<FE::AST::StmtNode*>(),
                                    yystack_[4].location.begin.line,
                                    yystack_[4].location.begin.column);
                        }
#line 1301 "frontend/parser/yacc.cpp"
                        break;

                        case 31:  // RETURN_STMT: RETURN SEMICOLON
#line 282 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() = new ReturnStmt(
                                nullptr, yystack_[1].location.begin.line, yystack_[1].location.begin.column);
                        }
#line 1309 "frontend/parser/yacc.cpp"
                        break;

                        case 32:  // RETURN_STMT: RETURN EXPR SEMICOLON
#line 285 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() =
                                new ReturnStmt(yystack_[1].value.as<FE::AST::ExprNode*>(),
                                    yystack_[2].location.begin.line,
                                    yystack_[2].location.begin.column);
                        }
#line 1317 "frontend/parser/yacc.cpp"
                        break;

                        case 33:  // BREAK_STMT: BREAK SEMICOLON
#line 291 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() =
                                new BreakStmt(yystack_[1].location.begin.line, yystack_[1].location.begin.column);
                        }
#line 1325 "frontend/parser/yacc.cpp"
                        break;

                        case 34:  // BLOCK_STMT: LBRACE RBRACE
#line 297 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() = new BlockStmt(new std::vector<StmtNode*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1333 "frontend/parser/yacc.cpp"
                        break;

                        case 35:  // BLOCK_STMT: LBRACE STMT_LIST RBRACE
#line 300 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::StmtNode*>() =
                                new BlockStmt(yystack_[1].value.as<std::vector<FE::AST::StmtNode*>*>(),
                                    yystack_[2].location.begin.line,
                                    yystack_[2].location.begin.column);
                        }
#line 1341 "frontend/parser/yacc.cpp"
                        break;

                        case 36:  // PARAM_DECLARATOR: TYPE IDENT
#line 306 "frontend/parser/yacc.y"
                        {
                            Entry* entry = Entry::getEntry(yystack_[0].value.as<std::string>());
                            yylhs.value.as<FE::AST::ParamDeclarator*>() =
                                new ParamDeclarator(yystack_[1].value.as<FE::AST::Type*>(),
                                    entry,
                                    nullptr,
                                    yystack_[1].location.begin.line,
                                    yystack_[1].location.begin.column);
                        }
#line 1350 "frontend/parser/yacc.cpp"
                        break;

                        case 37:  // PARAM_DECLARATOR: TYPE IDENT LBRACKET RBRACKET
#line 310 "frontend/parser/yacc.y"
                        {
                            std::vector<ExprNode*>* dim = new std::vector<ExprNode*>();
                            dim->emplace_back(new LiteralExpr(
                                -1, yystack_[1].location.begin.line, yystack_[1].location.begin.column));
                            Entry* entry = Entry::getEntry(yystack_[2].value.as<std::string>());
                            yylhs.value.as<FE::AST::ParamDeclarator*>() =
                                new ParamDeclarator(yystack_[3].value.as<FE::AST::Type*>(),
                                    entry,
                                    dim,
                                    yystack_[3].location.begin.line,
                                    yystack_[3].location.begin.column);
                        }
#line 1361 "frontend/parser/yacc.cpp"
                        break;

                        case 38:  // PARAM_DECLARATOR: TYPE IDENT LBRACKET INT_CONST RBRACKET
#line 316 "frontend/parser/yacc.y"
                        {
                            std::vector<ExprNode*>* dim = new std::vector<ExprNode*>();
                            dim->emplace_back(new LiteralExpr(yystack_[1].value.as<int>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column));
                            Entry* entry = Entry::getEntry(yystack_[3].value.as<std::string>());
                            yylhs.value.as<FE::AST::ParamDeclarator*>() =
                                new ParamDeclarator(yystack_[4].value.as<FE::AST::Type*>(),
                                    entry,
                                    dim,
                                    yystack_[4].location.begin.line,
                                    yystack_[4].location.begin.column);
                        }
#line 1372 "frontend/parser/yacc.cpp"
                        break;

                        case 39:  // PARAM_DECLARATOR_LIST: %empty
#line 326 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<std::vector<FE::AST::ParamDeclarator*>*>() =
                                new std::vector<ParamDeclarator*>();
                        }
#line 1380 "frontend/parser/yacc.cpp"
                        break;

                        case 40:  // PARAM_DECLARATOR_LIST: PARAM_DECLARATOR
#line 329 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<std::vector<FE::AST::ParamDeclarator*>*>() =
                                new std::vector<ParamDeclarator*>();
                            yylhs.value.as<std::vector<FE::AST::ParamDeclarator*>*>()->push_back(
                                yystack_[0].value.as<FE::AST::ParamDeclarator*>());
                        }
#line 1389 "frontend/parser/yacc.cpp"
                        break;

                        case 41:  // PARAM_DECLARATOR_LIST: PARAM_DECLARATOR_LIST COMMA PARAM_DECLARATOR
#line 333 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<std::vector<FE::AST::ParamDeclarator*>*>() =
                                yystack_[2].value.as<std::vector<FE::AST::ParamDeclarator*>*>();
                            yylhs.value.as<std::vector<FE::AST::ParamDeclarator*>*>()->push_back(
                                yystack_[0].value.as<FE::AST::ParamDeclarator*>());
                        }
#line 1398 "frontend/parser/yacc.cpp"
                        break;

                        case 42:  // VAR_DECLARATOR: IDENT
#line 341 "frontend/parser/yacc.y"
                        {
                            Entry* entry = Entry::getEntry(yystack_[0].value.as<std::string>());
                            yylhs.value.as<FE::AST::VarDeclarator*>() = new VarDeclarator(
                                new LeftValExpr(
                                    entry, nullptr, yystack_[0].location.begin.line, yystack_[0].location.begin.column),
                                nullptr,
                                yystack_[0].location.begin.line,
                                yystack_[0].location.begin.column);
                        }
#line 1408 "frontend/parser/yacc.cpp"
                        break;

                        case 43:  // VAR_DECLARATOR: IDENT ARRAY_DIMENSION_EXPR_LIST
#line 346 "frontend/parser/yacc.y"
                        {
                            Entry* entry = Entry::getEntry(yystack_[1].value.as<std::string>());
                            yylhs.value.as<FE::AST::VarDeclarator*>() =
                                new VarDeclarator(new LeftValExpr(entry,
                                                      yystack_[0].value.as<std::vector<FE::AST::ExprNode*>*>(),
                                                      yystack_[1].location.begin.line,
                                                      yystack_[1].location.begin.column),
                                    nullptr,
                                    yystack_[1].location.begin.line,
                                    yystack_[1].location.begin.column);
                        }
#line 1418 "frontend/parser/yacc.cpp"
                        break;

                        case 44:  // VAR_DECLARATOR: IDENT ASSIGN INITIALIZER
#line 351 "frontend/parser/yacc.y"
                        {
                            Entry* entry = Entry::getEntry(yystack_[2].value.as<std::string>());
                            yylhs.value.as<FE::AST::VarDeclarator*>() = new VarDeclarator(
                                new LeftValExpr(
                                    entry, nullptr, yystack_[2].location.begin.line, yystack_[2].location.begin.column),
                                yystack_[0].value.as<FE::AST::InitDecl*>(),
                                yystack_[2].location.begin.line,
                                yystack_[2].location.begin.column);
                        }
#line 1428 "frontend/parser/yacc.cpp"
                        break;

                        case 45:  // VAR_DECLARATOR: IDENT ARRAY_DIMENSION_EXPR_LIST ASSIGN INITIALIZER
#line 356 "frontend/parser/yacc.y"
                        {
                            Entry* entry = Entry::getEntry(yystack_[3].value.as<std::string>());
                            yylhs.value.as<FE::AST::VarDeclarator*>() =
                                new VarDeclarator(new LeftValExpr(entry,
                                                      yystack_[2].value.as<std::vector<FE::AST::ExprNode*>*>(),
                                                      yystack_[3].location.begin.line,
                                                      yystack_[3].location.begin.column),
                                    yystack_[0].value.as<FE::AST::InitDecl*>(),
                                    yystack_[3].location.begin.line,
                                    yystack_[3].location.begin.column);
                        }
#line 1438 "frontend/parser/yacc.cpp"
                        break;

                        case 46:  // VAR_DECLARATOR_LIST: VAR_DECLARATOR
#line 364 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<std::vector<FE::AST::VarDeclarator*>*>() = new std::vector<VarDeclarator*>();
                            yylhs.value.as<std::vector<FE::AST::VarDeclarator*>*>()->push_back(
                                yystack_[0].value.as<FE::AST::VarDeclarator*>());
                        }
#line 1447 "frontend/parser/yacc.cpp"
                        break;

                        case 47:  // VAR_DECLARATOR_LIST: VAR_DECLARATOR_LIST COMMA VAR_DECLARATOR
#line 368 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<std::vector<FE::AST::VarDeclarator*>*>() =
                                yystack_[2].value.as<std::vector<FE::AST::VarDeclarator*>*>();
                            yylhs.value.as<std::vector<FE::AST::VarDeclarator*>*>()->push_back(
                                yystack_[0].value.as<FE::AST::VarDeclarator*>());
                        }
#line 1456 "frontend/parser/yacc.cpp"
                        break;

                        case 48:  // INITIALIZER: NOCOMMA_EXPR
#line 375 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::InitDecl*>() =
                                new Initializer(yystack_[0].value.as<FE::AST::ExprNode*>(),
                                    yystack_[0].location.begin.line,
                                    yystack_[0].location.begin.column);
                        }
#line 1464 "frontend/parser/yacc.cpp"
                        break;

                        case 49:  // INITIALIZER: LBRACE RBRACE
#line 378 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::InitDecl*>() = new InitializerList(new std::vector<InitDecl*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1472 "frontend/parser/yacc.cpp"
                        break;

                        case 50:  // INITIALIZER: LBRACE INITIALIZER_LIST RBRACE
#line 381 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::InitDecl*>() =
                                new InitializerList(yystack_[1].value.as<std::vector<FE::AST::InitDecl*>*>(),
                                    yystack_[2].location.begin.line,
                                    yystack_[2].location.begin.column);
                        }
#line 1480 "frontend/parser/yacc.cpp"
                        break;

                        case 51:  // INITIALIZER_LIST: INITIALIZER
#line 387 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<std::vector<FE::AST::InitDecl*>*>() = new std::vector<InitDecl*>();
                            yylhs.value.as<std::vector<FE::AST::InitDecl*>*>()->push_back(
                                yystack_[0].value.as<FE::AST::InitDecl*>());
                        }
#line 1489 "frontend/parser/yacc.cpp"
                        break;

                        case 52:  // INITIALIZER_LIST: INITIALIZER_LIST COMMA INITIALIZER
#line 391 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<std::vector<FE::AST::InitDecl*>*>() =
                                yystack_[2].value.as<std::vector<FE::AST::InitDecl*>*>();
                            yylhs.value.as<std::vector<FE::AST::InitDecl*>*>()->push_back(
                                yystack_[0].value.as<FE::AST::InitDecl*>());
                        }
#line 1498 "frontend/parser/yacc.cpp"
                        break;

                        case 53:  // ASSIGN_EXPR: LEFT_VAL_EXPR ASSIGN NOCOMMA_EXPR
#line 398 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new BinaryExpr(Operator::ASSIGN,
                                yystack_[2].value.as<FE::AST::ExprNode*>(),
                                yystack_[0].value.as<FE::AST::ExprNode*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1506 "frontend/parser/yacc.cpp"
                        break;

                        case 54:  // EXPR_LIST: NOCOMMA_EXPR
#line 404 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<std::vector<FE::AST::ExprNode*>*>() = new std::vector<ExprNode*>();
                            yylhs.value.as<std::vector<FE::AST::ExprNode*>*>()->push_back(
                                yystack_[0].value.as<FE::AST::ExprNode*>());
                        }
#line 1515 "frontend/parser/yacc.cpp"
                        break;

                        case 55:  // EXPR_LIST: EXPR_LIST COMMA NOCOMMA_EXPR
#line 408 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<std::vector<FE::AST::ExprNode*>*>() =
                                yystack_[2].value.as<std::vector<FE::AST::ExprNode*>*>();
                            yylhs.value.as<std::vector<FE::AST::ExprNode*>*>()->push_back(
                                yystack_[0].value.as<FE::AST::ExprNode*>());
                        }
#line 1524 "frontend/parser/yacc.cpp"
                        break;

                        case 56:  // EXPR: NOCOMMA_EXPR
#line 415 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = yystack_[0].value.as<FE::AST::ExprNode*>();
                        }
#line 1532 "frontend/parser/yacc.cpp"
                        break;

                        case 57:  // EXPR: EXPR COMMA NOCOMMA_EXPR
#line 418 "frontend/parser/yacc.y"
                        {
                            if (yystack_[2].value.as<FE::AST::ExprNode*>()->isCommaExpr())
                            {
                                CommaExpr* ce = static_cast<CommaExpr*>(yystack_[2].value.as<FE::AST::ExprNode*>());
                                ce->exprs->push_back(yystack_[0].value.as<FE::AST::ExprNode*>());
                                yylhs.value.as<FE::AST::ExprNode*>() = ce;
                            }
                            else
                            {
                                auto vec = new std::vector<ExprNode*>();
                                vec->push_back(yystack_[2].value.as<FE::AST::ExprNode*>());
                                vec->push_back(yystack_[0].value.as<FE::AST::ExprNode*>());
                                yylhs.value.as<FE::AST::ExprNode*>() = new CommaExpr(vec,
                                    yystack_[2].value.as<FE::AST::ExprNode*>()->line_num,
                                    yystack_[2].value.as<FE::AST::ExprNode*>()->col_num);
                            }
                        }
#line 1549 "frontend/parser/yacc.cpp"
                        break;

                        case 58:  // NOCOMMA_EXPR: LOGICAL_OR_EXPR
#line 433 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = yystack_[0].value.as<FE::AST::ExprNode*>();
                        }
#line 1557 "frontend/parser/yacc.cpp"
                        break;

                        case 59:  // NOCOMMA_EXPR: ASSIGN_EXPR
#line 436 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = yystack_[0].value.as<FE::AST::ExprNode*>();
                        }
#line 1565 "frontend/parser/yacc.cpp"
                        break;

                        case 60:  // LOGICAL_OR_EXPR: LOGICAL_AND_EXPR
#line 442 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = yystack_[0].value.as<FE::AST::ExprNode*>();
                        }
#line 1573 "frontend/parser/yacc.cpp"
                        break;

                        case 61:  // LOGICAL_OR_EXPR: LOGICAL_OR_EXPR OR LOGICAL_AND_EXPR
#line 445 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new BinaryExpr(Operator::OR,
                                yystack_[2].value.as<FE::AST::ExprNode*>(),
                                yystack_[0].value.as<FE::AST::ExprNode*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1581 "frontend/parser/yacc.cpp"
                        break;

                        case 62:  // LOGICAL_AND_EXPR: EQUALITY_EXPR
#line 451 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = yystack_[0].value.as<FE::AST::ExprNode*>();
                        }
#line 1589 "frontend/parser/yacc.cpp"
                        break;

                        case 63:  // LOGICAL_AND_EXPR: LOGICAL_AND_EXPR AND EQUALITY_EXPR
#line 454 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new BinaryExpr(Operator::AND,
                                yystack_[2].value.as<FE::AST::ExprNode*>(),
                                yystack_[0].value.as<FE::AST::ExprNode*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1597 "frontend/parser/yacc.cpp"
                        break;

                        case 64:  // EQUALITY_EXPR: RELATIONAL_EXPR
#line 460 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = yystack_[0].value.as<FE::AST::ExprNode*>();
                        }
#line 1605 "frontend/parser/yacc.cpp"
                        break;

                        case 65:  // EQUALITY_EXPR: EQUALITY_EXPR EQ RELATIONAL_EXPR
#line 463 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new BinaryExpr(Operator::EQ,
                                yystack_[2].value.as<FE::AST::ExprNode*>(),
                                yystack_[0].value.as<FE::AST::ExprNode*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1613 "frontend/parser/yacc.cpp"
                        break;

                        case 66:  // EQUALITY_EXPR: EQUALITY_EXPR NEQ RELATIONAL_EXPR
#line 466 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new BinaryExpr(Operator::NEQ,
                                yystack_[2].value.as<FE::AST::ExprNode*>(),
                                yystack_[0].value.as<FE::AST::ExprNode*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1621 "frontend/parser/yacc.cpp"
                        break;

                        case 67:  // RELATIONAL_EXPR: ADDSUB_EXPR
#line 472 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = yystack_[0].value.as<FE::AST::ExprNode*>();
                        }
#line 1629 "frontend/parser/yacc.cpp"
                        break;

                        case 68:  // RELATIONAL_EXPR: RELATIONAL_EXPR GT ADDSUB_EXPR
#line 475 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new BinaryExpr(Operator::GT,
                                yystack_[2].value.as<FE::AST::ExprNode*>(),
                                yystack_[0].value.as<FE::AST::ExprNode*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1637 "frontend/parser/yacc.cpp"
                        break;

                        case 69:  // RELATIONAL_EXPR: RELATIONAL_EXPR GE ADDSUB_EXPR
#line 478 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new BinaryExpr(Operator::GE,
                                yystack_[2].value.as<FE::AST::ExprNode*>(),
                                yystack_[0].value.as<FE::AST::ExprNode*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1645 "frontend/parser/yacc.cpp"
                        break;

                        case 70:  // RELATIONAL_EXPR: RELATIONAL_EXPR LT ADDSUB_EXPR
#line 481 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new BinaryExpr(Operator::LT,
                                yystack_[2].value.as<FE::AST::ExprNode*>(),
                                yystack_[0].value.as<FE::AST::ExprNode*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1653 "frontend/parser/yacc.cpp"
                        break;

                        case 71:  // RELATIONAL_EXPR: RELATIONAL_EXPR LE ADDSUB_EXPR
#line 484 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new BinaryExpr(Operator::LE,
                                yystack_[2].value.as<FE::AST::ExprNode*>(),
                                yystack_[0].value.as<FE::AST::ExprNode*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1661 "frontend/parser/yacc.cpp"
                        break;

                        case 72:  // ADDSUB_EXPR: MULDIV_EXPR
#line 490 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = yystack_[0].value.as<FE::AST::ExprNode*>();
                        }
#line 1669 "frontend/parser/yacc.cpp"
                        break;

                        case 73:  // ADDSUB_EXPR: ADDSUB_EXPR PLUS MULDIV_EXPR
#line 493 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new BinaryExpr(Operator::ADD,
                                yystack_[2].value.as<FE::AST::ExprNode*>(),
                                yystack_[0].value.as<FE::AST::ExprNode*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1677 "frontend/parser/yacc.cpp"
                        break;

                        case 74:  // ADDSUB_EXPR: ADDSUB_EXPR MINUS MULDIV_EXPR
#line 496 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new BinaryExpr(Operator::SUB,
                                yystack_[2].value.as<FE::AST::ExprNode*>(),
                                yystack_[0].value.as<FE::AST::ExprNode*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1685 "frontend/parser/yacc.cpp"
                        break;

                        case 75:  // MULDIV_EXPR: UNARY_EXPR
#line 502 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = yystack_[0].value.as<FE::AST::ExprNode*>();
                        }
#line 1693 "frontend/parser/yacc.cpp"
                        break;

                        case 76:  // MULDIV_EXPR: MULDIV_EXPR STAR UNARY_EXPR
#line 505 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new BinaryExpr(Operator::MUL,
                                yystack_[2].value.as<FE::AST::ExprNode*>(),
                                yystack_[0].value.as<FE::AST::ExprNode*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1701 "frontend/parser/yacc.cpp"
                        break;

                        case 77:  // MULDIV_EXPR: MULDIV_EXPR SLASH UNARY_EXPR
#line 508 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new BinaryExpr(Operator::DIV,
                                yystack_[2].value.as<FE::AST::ExprNode*>(),
                                yystack_[0].value.as<FE::AST::ExprNode*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1709 "frontend/parser/yacc.cpp"
                        break;

                        case 78:  // MULDIV_EXPR: MULDIV_EXPR MOD UNARY_EXPR
#line 511 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new BinaryExpr(Operator::MOD,
                                yystack_[2].value.as<FE::AST::ExprNode*>(),
                                yystack_[0].value.as<FE::AST::ExprNode*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1717 "frontend/parser/yacc.cpp"
                        break;

                        case 79:  // UNARY_EXPR: BASIC_EXPR
#line 517 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = yystack_[0].value.as<FE::AST::ExprNode*>();
                        }
#line 1725 "frontend/parser/yacc.cpp"
                        break;

                        case 80:  // UNARY_EXPR: UNARY_OP UNARY_EXPR
#line 520 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() =
                                new UnaryExpr(yystack_[1].value.as<FE::AST::Operator>(),
                                    yystack_[0].value.as<FE::AST::ExprNode*>(),
                                    yystack_[0].value.as<FE::AST::ExprNode*>()->line_num,
                                    yystack_[0].value.as<FE::AST::ExprNode*>()->col_num);
                        }
#line 1733 "frontend/parser/yacc.cpp"
                        break;

                        case 81:  // BASIC_EXPR: LITERAL_EXPR
#line 526 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = yystack_[0].value.as<FE::AST::ExprNode*>();
                        }
#line 1741 "frontend/parser/yacc.cpp"
                        break;

                        case 82:  // BASIC_EXPR: LEFT_VAL_EXPR
#line 529 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = yystack_[0].value.as<FE::AST::ExprNode*>();
                        }
#line 1749 "frontend/parser/yacc.cpp"
                        break;

                        case 83:  // BASIC_EXPR: LPAREN EXPR RPAREN
#line 532 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = yystack_[1].value.as<FE::AST::ExprNode*>();
                        }
#line 1757 "frontend/parser/yacc.cpp"
                        break;

                        case 84:  // BASIC_EXPR: FUNC_CALL_EXPR
#line 535 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = yystack_[0].value.as<FE::AST::ExprNode*>();
                        }
#line 1765 "frontend/parser/yacc.cpp"
                        break;

                        case 85:  // FUNC_CALL_EXPR: IDENT LPAREN RPAREN
#line 541 "frontend/parser/yacc.y"
                        {
                            std::string funcName = yystack_[2].value.as<std::string>();
                            if (funcName != "starttime" && funcName != "stoptime")
                            {
                                Entry* entry                         = Entry::getEntry(funcName);
                                yylhs.value.as<FE::AST::ExprNode*>() = new CallExpr(
                                    entry, nullptr, yystack_[2].location.begin.line, yystack_[2].location.begin.column);
                            }
                            else
                            {
                                funcName                     = "_sysy_" + funcName;
                                std::vector<ExprNode*>* args = new std::vector<ExprNode*>();
                                args->emplace_back(new LiteralExpr(static_cast<int>(yystack_[2].location.begin.line),
                                    yystack_[2].location.begin.line,
                                    yystack_[2].location.begin.column));
                                yylhs.value.as<FE::AST::ExprNode*>() = new CallExpr(Entry::getEntry(funcName),
                                    args,
                                    yystack_[2].location.begin.line,
                                    yystack_[2].location.begin.column);
                            }
                        }
#line 1785 "frontend/parser/yacc.cpp"
                        break;

                        case 86:  // FUNC_CALL_EXPR: IDENT LPAREN EXPR_LIST RPAREN
#line 556 "frontend/parser/yacc.y"
                        {
                            Entry* entry                         = Entry::getEntry(yystack_[3].value.as<std::string>());
                            yylhs.value.as<FE::AST::ExprNode*>() = new CallExpr(entry,
                                yystack_[1].value.as<std::vector<FE::AST::ExprNode*>*>(),
                                yystack_[3].location.begin.line,
                                yystack_[3].location.begin.column);
                        }
#line 1794 "frontend/parser/yacc.cpp"
                        break;

                        case 87:  // ARRAY_DIMENSION_EXPR: LBRACKET NOCOMMA_EXPR RBRACKET
#line 563 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = yystack_[1].value.as<FE::AST::ExprNode*>();
                        }
#line 1802 "frontend/parser/yacc.cpp"
                        break;

                        case 88:  // ARRAY_DIMENSION_EXPR_LIST: ARRAY_DIMENSION_EXPR
#line 569 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<std::vector<FE::AST::ExprNode*>*>() = new std::vector<ExprNode*>();
                            yylhs.value.as<std::vector<FE::AST::ExprNode*>*>()->push_back(
                                yystack_[0].value.as<FE::AST::ExprNode*>());
                        }
#line 1811 "frontend/parser/yacc.cpp"
                        break;

                        case 89:  // ARRAY_DIMENSION_EXPR_LIST: ARRAY_DIMENSION_EXPR_LIST ARRAY_DIMENSION_EXPR
#line 573 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<std::vector<FE::AST::ExprNode*>*>() =
                                yystack_[1].value.as<std::vector<FE::AST::ExprNode*>*>();
                            yylhs.value.as<std::vector<FE::AST::ExprNode*>*>()->push_back(
                                yystack_[0].value.as<FE::AST::ExprNode*>());
                        }
#line 1820 "frontend/parser/yacc.cpp"
                        break;

                        case 90:  // LEFT_VAL_EXPR: IDENT
#line 580 "frontend/parser/yacc.y"
                        {
                            Entry* entry                         = Entry::getEntry(yystack_[0].value.as<std::string>());
                            yylhs.value.as<FE::AST::ExprNode*>() = new LeftValExpr(
                                entry, nullptr, yystack_[0].location.begin.line, yystack_[0].location.begin.column);
                        }
#line 1829 "frontend/parser/yacc.cpp"
                        break;

                        case 91:  // LEFT_VAL_EXPR: IDENT ARRAY_DIMENSION_EXPR_LIST
#line 584 "frontend/parser/yacc.y"
                        {
                            Entry* entry                         = Entry::getEntry(yystack_[1].value.as<std::string>());
                            yylhs.value.as<FE::AST::ExprNode*>() = new LeftValExpr(entry,
                                yystack_[0].value.as<std::vector<FE::AST::ExprNode*>*>(),
                                yystack_[1].location.begin.line,
                                yystack_[1].location.begin.column);
                        }
#line 1838 "frontend/parser/yacc.cpp"
                        break;

                        case 92:  // LITERAL_EXPR: INT_CONST
#line 591 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new LiteralExpr(yystack_[0].value.as<int>(),
                                yystack_[0].location.begin.line,
                                yystack_[0].location.begin.column);
                        }
#line 1846 "frontend/parser/yacc.cpp"
                        break;

                        case 93:  // LITERAL_EXPR: LL_CONST
#line 594 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new LiteralExpr(yystack_[0].value.as<long long>(),
                                yystack_[0].location.begin.line,
                                yystack_[0].location.begin.column);
                        }
#line 1854 "frontend/parser/yacc.cpp"
                        break;

                        case 94:  // LITERAL_EXPR: FLOAT_CONST
#line 597 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::ExprNode*>() = new LiteralExpr(yystack_[0].value.as<float>(),
                                yystack_[0].location.begin.line,
                                yystack_[0].location.begin.column);
                        }
#line 1862 "frontend/parser/yacc.cpp"
                        break;

                        case 95:  // TYPE: INT
#line 603 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::Type*>() = intType;
                        }
#line 1870 "frontend/parser/yacc.cpp"
                        break;

                        case 96:  // TYPE: FLOAT
#line 606 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::Type*>() = floatType;
                        }
#line 1878 "frontend/parser/yacc.cpp"
                        break;

                        case 97:  // TYPE: VOID
#line 609 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::Type*>() = voidType;
                        }
#line 1886 "frontend/parser/yacc.cpp"
                        break;

                        case 98:  // UNARY_OP: PLUS
#line 615 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::Operator>() = Operator::ADD;
                        }
#line 1894 "frontend/parser/yacc.cpp"
                        break;

                        case 99:  // UNARY_OP: MINUS
#line 618 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::Operator>() = Operator::SUB;
                        }
#line 1902 "frontend/parser/yacc.cpp"
                        break;

                        case 100:  // UNARY_OP: NOT
#line 621 "frontend/parser/yacc.y"
                        {
                            yylhs.value.as<FE::AST::Operator>() = Operator::NOT;
                        }
#line 1910 "frontend/parser/yacc.cpp"
                        break;

#line 1914 "frontend/parser/yacc.cpp"

                        default: break;
                    }
                }
#if YY_EXCEPTIONS
                catch (const syntax_error& yyexc)
                {
                    YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
                    error(yyexc);
                    YYERROR;
                }
#endif  // YY_EXCEPTIONS
                YY_SYMBOL_PRINT("-> $$ =", yylhs);
                yypop_(yylen);
                yylen = 0;

                // Shift the result of the reduction.
                yypush_(YY_NULLPTR, YY_MOVE(yylhs));
            }
            goto yynewstate;

        /*--------------------------------------.
        | yyerrlab -- here on detecting error.  |
        `--------------------------------------*/
        yyerrlab:
            // If not already recovering from an error, report this error.
            if (!yyerrstatus_)
            {
                ++yynerrs_;
                context     yyctx(*this, yyla);
                std::string msg = yysyntax_error_(yyctx);
                error(yyla.location, YY_MOVE(msg));
            }

            yyerror_range[1].location = yyla.location;
            if (yyerrstatus_ == 3)
            {
                /* If just tried and failed to reuse lookahead token after an
                   error, discard it.  */

                // Return failure if at end of input.
                if (yyla.kind() == symbol_kind::S_YYEOF)
                    YYABORT;
                else if (!yyla.empty())
                {
                    yy_destroy_("Error: discarding", yyla);
                    yyla.clear();
                }
            }

            // Else will try to reuse lookahead token after shifting the error token.
            goto yyerrlab1;

        /*---------------------------------------------------.
        | yyerrorlab -- error raised explicitly by YYERROR.  |
        `---------------------------------------------------*/
        yyerrorlab:
            /* Pacify compilers when the user code never invokes YYERROR and
               the label yyerrorlab therefore never appears in user code.  */
            if (false) YYERROR;

            /* Do not reclaim the symbols of the rule whose action triggered
               this YYERROR.  */
            yypop_(yylen);
            yylen = 0;
            YY_STACK_PRINT();
            goto yyerrlab1;

        /*-------------------------------------------------------------.
        | yyerrlab1 -- common code for both syntax error and YYERROR.  |
        `-------------------------------------------------------------*/
        yyerrlab1:
            yyerrstatus_ = 3;  // Each real token shifted decrements this.
            // Pop stack until we find a state that shifts the error token.
            for (;;)
            {
                yyn = yypact_[+yystack_[0].state];
                if (!yy_pact_value_is_default_(yyn))
                {
                    yyn += symbol_kind::S_YYerror;
                    if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == symbol_kind::S_YYerror)
                    {
                        yyn = yytable_[yyn];
                        if (0 < yyn) break;
                    }
                }

                // Pop the current state because it cannot handle the error token.
                if (yystack_.size() == 1) YYABORT;

                yyerror_range[1].location = yystack_[0].location;
                yy_destroy_("Error: popping", yystack_[0]);
                yypop_();
                YY_STACK_PRINT();
            }
            {
                stack_symbol_type error_token;

                yyerror_range[2].location = yyla.location;
                YYLLOC_DEFAULT(error_token.location, yyerror_range, 2);

                // Shift the error token.
                error_token.state = state_type(yyn);
                yypush_("Shifting", YY_MOVE(error_token));
            }
            goto yynewstate;

        /*-------------------------------------.
        | yyacceptlab -- YYACCEPT comes here.  |
        `-------------------------------------*/
        yyacceptlab:
            yyresult = 0;
            goto yyreturn;

        /*-----------------------------------.
        | yyabortlab -- YYABORT comes here.  |
        `-----------------------------------*/
        yyabortlab:
            yyresult = 1;
            goto yyreturn;

        /*-----------------------------------------------------.
        | yyreturn -- parsing is finished, return the result.  |
        `-----------------------------------------------------*/
        yyreturn:
            if (!yyla.empty()) yy_destroy_("Cleanup: discarding lookahead", yyla);

            /* Do not reclaim the symbols of the rule whose action triggered
               this YYABORT or YYACCEPT.  */
            yypop_(yylen);
            YY_STACK_PRINT();
            while (1 < yystack_.size())
            {
                yy_destroy_("Cleanup: popping", yystack_[0]);
                yypop_();
            }

            return yyresult;
        }
#if YY_EXCEPTIONS
        catch (...)
        {
            YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
            // Do not try to display the values of the reclaimed symbols,
            // as their printers might throw an exception.
            if (!yyla.empty()) yy_destroy_(YY_NULLPTR, yyla);

            while (1 < yystack_.size())
            {
                yy_destroy_(YY_NULLPTR, yystack_[0]);
                yypop_();
            }
            throw;
        }
#endif  // YY_EXCEPTIONS
    }

    void YaccParser ::error(const syntax_error& yyexc) { error(yyexc.location, yyexc.what()); }

    /* Return YYSTR after stripping away unnecessary quotes and
       backslashes, so that it's suitable for yyerror.  The heuristic is
       that double-quoting is unnecessary unless the string contains an
       apostrophe, a comma, or backslash (other than backslash-backslash).
       YYSTR is taken from yytname.  */
    std::string YaccParser ::yytnamerr_(const char* yystr)
    {
        if (*yystr == '"')
        {
            std::string yyr;
            char const* yyp = yystr;

            for (;;) switch (*++yyp)
                {
                    case '\'':
                    case ',': goto do_not_strip_quotes;

                    case '\\':
                        if (*++yyp != '\\')
                            goto do_not_strip_quotes;
                        else
                            goto append;

                    append:
                    default: yyr += *yyp; break;

                    case '"': return yyr;
                }
        do_not_strip_quotes:;
        }

        return yystr;
    }

    std::string YaccParser ::symbol_name(symbol_kind_type yysymbol) { return yytnamerr_(yytname_[yysymbol]); }

    //  YaccParser ::context.
    YaccParser ::context::context(const YaccParser& yyparser, const symbol_type& yyla)
        : yyparser_(yyparser), yyla_(yyla)
    {}

    int YaccParser ::context::expected_tokens(symbol_kind_type yyarg[], int yyargn) const
    {
        // Actual number of expected tokens
        int yycount = 0;

        const int yyn = yypact_[+yyparser_.yystack_[0].state];
        if (!yy_pact_value_is_default_(yyn))
        {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            const int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            const int yychecklim = yylast_ - yyn + 1;
            const int yyxend     = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
                if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror &&
                    !yy_table_value_is_error_(yytable_[yyx + yyn]))
                {
                    if (!yyarg)
                        ++yycount;
                    else if (yycount == yyargn)
                        return 0;
                    else
                        yyarg[yycount++] = YY_CAST(symbol_kind_type, yyx);
                }
        }

        if (yyarg && yycount == 0 && 0 < yyargn) yyarg[0] = symbol_kind::S_YYEMPTY;
        return yycount;
    }

    int YaccParser ::yy_syntax_error_arguments_(const context& yyctx, symbol_kind_type yyarg[], int yyargn) const
    {
        /* There are many possibilities here to consider:
           - If this state is a consistent state with a default action, then
             the only way this function was invoked is if the default action
             is an error action.  In that case, don't check for expected
             tokens because there are none.
           - The only way there can be no lookahead present (in yyla) is
             if this state is a consistent state with a default action.
             Thus, detecting the absence of a lookahead is sufficient to
             determine that there is no unexpected or expected token to
             report.  In that case, just report a simple "syntax error".
           - Don't assume there isn't a lookahead just because this state is
             a consistent state with a default action.  There might have
             been a previous inconsistent state, consistent state with a
             non-default action, or user semantic action that manipulated
             yyla.  (However, yyla is currently not documented for users.)
           - Of course, the expected token list depends on states to have
             correct lookahead information, and it depends on the parser not
             to perform extra reductions after fetching a lookahead from the
             scanner and before detecting a syntax error.  Thus, state merging
             (from LALR or IELR) and default reductions corrupt the expected
             token list.  However, the list is correct for canonical LR with
             one exception: it will still contain any token that will not be
             accepted due to an error action in a later state.
        */

        if (!yyctx.lookahead().empty())
        {
            if (yyarg) yyarg[0] = yyctx.token();
            int yyn = yyctx.expected_tokens(yyarg ? yyarg + 1 : yyarg, yyargn - 1);
            return yyn + 1;
        }
        return 0;
    }

    // Generate an error message.
    std::string YaccParser ::yysyntax_error_(const context& yyctx) const
    {
        // Its maximum.
        enum
        {
            YYARGS_MAX = 5
        };
        // Arguments of yyformat.
        symbol_kind_type yyarg[YYARGS_MAX];
        int              yycount = yy_syntax_error_arguments_(yyctx, yyarg, YYARGS_MAX);

        char const* yyformat = YY_NULLPTR;
        switch (yycount)
        {
#define YYCASE_(N, S) \
    case N: yyformat = S; break
            default:  // Avoid compiler warnings.
                YYCASE_(0, YY_("syntax error"));
                YYCASE_(1, YY_("syntax error, unexpected %s"));
                YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
                YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
                YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
                YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
        }

        std::string yyres;
        // Argument number.
        std::ptrdiff_t yyi = 0;
        for (char const* yyp = yyformat; *yyp; ++yyp)
            if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
            {
                yyres += symbol_name(yyarg[yyi++]);
                ++yyp;
            }
            else
                yyres += *yyp;
        return yyres;
    }

    const signed char YaccParser ::yypact_ninf_ = -127;

    const signed char YaccParser ::yytable_ninf_ = -1;

    const short YaccParser ::yypact_[] = {57,
        -127,
        -127,
        -127,
        -127,
        -11,
        -127,
        -127,
        -127,
        -16,
        22,
        56,
        61,
        80,
        98,
        46,
        -127,
        276,
        146,
        -127,
        -127,
        -127,
        8,
        57,
        -127,
        -127,
        -127,
        83,
        -127,
        -127,
        -127,
        -127,
        -127,
        -127,
        -127,
        -127,
        -127,
        5,
        -127,
        74,
        89,
        -8,
        72,
        26,
        135,
        -127,
        -127,
        -127,
        82,
        -127,
        137,
        276,
        256,
        276,
        -127,
        123,
        276,
        17,
        276,
        -127,
        -127,
        -127,
        52,
        144,
        9,
        -127,
        177,
        -127,
        -127,
        -127,
        -127,
        -127,
        276,
        276,
        276,
        276,
        276,
        276,
        276,
        276,
        276,
        276,
        276,
        276,
        276,
        276,
        276,
        -15,
        -127,
        134,
        -127,
        -127,
        -127,
        59,
        -127,
        142,
        -127,
        65,
        166,
        71,
        144,
        111,
        -127,
        -14,
        134,
        -127,
        -127,
        -127,
        89,
        -8,
        72,
        72,
        26,
        26,
        26,
        26,
        135,
        135,
        -127,
        -127,
        -127,
        -127,
        46,
        264,
        -5,
        144,
        276,
        -127,
        -127,
        57,
        276,
        276,
        57,
        -127,
        170,
        194,
        95,
        -127,
        -127,
        264,
        -127,
        -127,
        192,
        86,
        93,
        -127,
        46,
        191,
        198,
        -127,
        -127,
        -9,
        -127,
        57,
        276,
        276,
        -127,
        208,
        -127,
        6,
        264,
        -127,
        -127,
        171,
        179,
        -127,
        239,
        200,
        -127,
        -127,
        57,
        57,
        -127,
        -127,
        -127,
        -127};

    const signed char YaccParser ::yydefact_[] = {0,
        92,
        93,
        94,
        13,
        90,
        95,
        96,
        97,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        12,
        0,
        0,
        100,
        98,
        99,
        0,
        2,
        4,
        11,
        6,
        0,
        7,
        8,
        9,
        10,
        14,
        15,
        16,
        17,
        59,
        0,
        56,
        58,
        60,
        62,
        64,
        67,
        72,
        75,
        79,
        84,
        82,
        81,
        0,
        0,
        0,
        0,
        88,
        91,
        0,
        0,
        0,
        18,
        33,
        31,
        0,
        0,
        0,
        34,
        0,
        1,
        3,
        5,
        22,
        19,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        42,
        46,
        20,
        80,
        82,
        85,
        0,
        54,
        0,
        89,
        0,
        0,
        0,
        0,
        0,
        32,
        42,
        21,
        83,
        35,
        57,
        61,
        63,
        65,
        66,
        68,
        69,
        70,
        71,
        73,
        74,
        76,
        77,
        78,
        53,
        39,
        0,
        43,
        0,
        0,
        86,
        87,
        0,
        0,
        0,
        0,
        40,
        0,
        0,
        0,
        44,
        48,
        0,
        47,
        55,
        28,
        0,
        0,
        30,
        0,
        0,
        36,
        49,
        51,
        0,
        45,
        0,
        0,
        0,
        41,
        0,
        25,
        0,
        0,
        50,
        29,
        0,
        0,
        23,
        0,
        0,
        37,
        52,
        0,
        0,
        24,
        38,
        26,
        27};

    const short YaccParser ::yypgoto_[] = {-127,
        -127,
        -17,
        -23,
        -127,
        -127,
        180,
        -127,
        -127,
        -127,
        -127,
        -127,
        -127,
        -127,
        -127,
        -127,
        90,
        -127,
        109,
        175,
        -126,
        -127,
        -127,
        -127,
        -10,
        -47,
        -127,
        185,
        201,
        66,
        88,
        147,
        -32,
        -127,
        -127,
        -53,
        248,
        245,
        -127,
        -12,
        -127};

    const unsigned char YaccParser ::yydefgoto_[] = {0,
        22,
        23,
        24,
        25,
        26,
        27,
        28,
        158,
        29,
        30,
        31,
        32,
        33,
        34,
        35,
        133,
        134,
        88,
        89,
        137,
        151,
        36,
        93,
        37,
        38,
        39,
        40,
        41,
        42,
        43,
        44,
        45,
        46,
        47,
        54,
        124,
        48,
        49,
        50,
        51};

    const unsigned char YaccParser ::yytable_[] = {69,
        66,
        96,
        63,
        62,
        94,
        95,
        64,
        67,
        167,
        150,
        56,
        122,
        152,
        53,
        53,
        52,
        160,
        53,
        90,
        1,
        2,
        3,
        161,
        53,
        107,
        5,
        6,
        7,
        8,
        71,
        72,
        123,
        123,
        169,
        72,
        168,
        105,
        75,
        121,
        76,
        15,
        139,
        69,
        17,
        100,
        97,
        99,
        101,
        57,
        19,
        118,
        119,
        120,
        20,
        21,
        6,
        7,
        8,
        68,
        1,
        2,
        3,
        81,
        82,
        4,
        5,
        6,
        7,
        8,
        9,
        96,
        10,
        11,
        12,
        13,
        138,
        102,
        72,
        141,
        14,
        15,
        16,
        58,
        17,
        126,
        59,
        127,
        18,
        138,
        19,
        72,
        138,
        129,
        20,
        21,
        131,
        72,
        1,
        2,
        3,
        1,
        2,
        3,
        5,
        60,
        142,
        5,
        70,
        145,
        135,
        154,
        72,
        138,
        77,
        78,
        79,
        80,
        155,
        72,
        143,
        144,
        17,
        61,
        73,
        17,
        136,
        149,
        19,
        86,
        162,
        19,
        20,
        21,
        135,
        20,
        21,
        72,
        74,
        132,
        166,
        110,
        111,
        69,
        163,
        164,
        87,
        174,
        175,
        1,
        2,
        3,
        53,
        103,
        4,
        5,
        6,
        7,
        8,
        9,
        125,
        10,
        11,
        12,
        13,
        112,
        113,
        114,
        115,
        14,
        15,
        16,
        128,
        17,
        83,
        84,
        85,
        18,
        65,
        19,
        1,
        2,
        3,
        20,
        21,
        4,
        5,
        6,
        7,
        8,
        9,
        130,
        10,
        11,
        12,
        13,
        146,
        72,
        147,
        170,
        14,
        15,
        16,
        148,
        17,
        72,
        153,
        171,
        18,
        106,
        19,
        1,
        2,
        3,
        20,
        21,
        4,
        5,
        6,
        7,
        8,
        9,
        157,
        10,
        11,
        12,
        13,
        159,
        116,
        117,
        173,
        14,
        15,
        16,
        140,
        17,
        156,
        98,
        104,
        18,
        165,
        19,
        1,
        2,
        3,
        20,
        21,
        4,
        5,
        6,
        7,
        8,
        9,
        55,
        10,
        11,
        12,
        13,
        108,
        1,
        2,
        3,
        14,
        15,
        16,
        5,
        17,
        1,
        2,
        3,
        18,
        172,
        19,
        5,
        0,
        109,
        20,
        21,
        0,
        1,
        2,
        3,
        0,
        17,
        92,
        5,
        0,
        0,
        0,
        19,
        0,
        17,
        0,
        20,
        21,
        136,
        91,
        19,
        0,
        0,
        0,
        20,
        21,
        17,
        0,
        0,
        0,
        0,
        0,
        19,
        0,
        0,
        0,
        20,
        21,
        0,
        0,
        0,
        91,
        91,
        91,
        91,
        91,
        91,
        91,
        91,
        91,
        91,
        91,
        91,
        91};

    const short YaccParser ::yycheck_[] = {23,
        18,
        55,
        15,
        14,
        52,
        53,
        17,
        0,
        3,
        136,
        27,
        27,
        139,
        29,
        29,
        27,
        26,
        29,
        51,
        3,
        4,
        5,
        32,
        29,
        72,
        9,
        10,
        11,
        12,
        25,
        26,
        47,
        47,
        160,
        26,
        30,
        28,
        46,
        86,
        48,
        24,
        47,
        66,
        27,
        57,
        56,
        57,
        58,
        27,
        33,
        83,
        84,
        85,
        37,
        38,
        10,
        11,
        12,
        51,
        3,
        4,
        5,
        37,
        38,
        8,
        9,
        10,
        11,
        12,
        13,
        124,
        15,
        16,
        17,
        18,
        123,
        25,
        26,
        126,
        23,
        24,
        25,
        27,
        27,
        26,
        25,
        28,
        31,
        136,
        33,
        26,
        139,
        28,
        37,
        38,
        25,
        26,
        3,
        4,
        5,
        3,
        4,
        5,
        9,
        25,
        129,
        9,
        25,
        132,
        122,
        25,
        26,
        160,
        42,
        43,
        44,
        45,
        25,
        26,
        130,
        131,
        27,
        25,
        50,
        27,
        31,
        32,
        33,
        47,
        153,
        33,
        37,
        38,
        146,
        37,
        38,
        26,
        49,
        28,
        157,
        75,
        76,
        166,
        154,
        155,
        9,
        170,
        171,
        3,
        4,
        5,
        29,
        9,
        8,
        9,
        10,
        11,
        12,
        13,
        26,
        15,
        16,
        17,
        18,
        77,
        78,
        79,
        80,
        23,
        24,
        25,
        30,
        27,
        39,
        40,
        41,
        31,
        32,
        33,
        3,
        4,
        5,
        37,
        38,
        8,
        9,
        10,
        11,
        12,
        13,
        25,
        15,
        16,
        17,
        18,
        26,
        26,
        28,
        28,
        23,
        24,
        25,
        9,
        27,
        26,
        14,
        28,
        31,
        32,
        33,
        3,
        4,
        5,
        37,
        38,
        8,
        9,
        10,
        11,
        12,
        13,
        31,
        15,
        16,
        17,
        18,
        29,
        81,
        82,
        30,
        23,
        24,
        25,
        125,
        27,
        146,
        57,
        63,
        31,
        32,
        33,
        3,
        4,
        5,
        37,
        38,
        8,
        9,
        10,
        11,
        12,
        13,
        5,
        15,
        16,
        17,
        18,
        73,
        3,
        4,
        5,
        23,
        24,
        25,
        9,
        27,
        3,
        4,
        5,
        31,
        32,
        33,
        9,
        -1,
        74,
        37,
        38,
        -1,
        3,
        4,
        5,
        -1,
        27,
        28,
        9,
        -1,
        -1,
        -1,
        33,
        -1,
        27,
        -1,
        37,
        38,
        31,
        51,
        33,
        -1,
        -1,
        -1,
        37,
        38,
        27,
        -1,
        -1,
        -1,
        -1,
        -1,
        33,
        -1,
        -1,
        -1,
        37,
        38,
        -1,
        -1,
        -1,
        73,
        74,
        75,
        76,
        77,
        78,
        79,
        80,
        81,
        82,
        83,
        84,
        85};

    const signed char YaccParser ::yystos_[] = {0,
        3,
        4,
        5,
        8,
        9,
        10,
        11,
        12,
        13,
        15,
        16,
        17,
        18,
        23,
        24,
        25,
        27,
        31,
        33,
        37,
        38,
        54,
        55,
        56,
        57,
        58,
        59,
        60,
        62,
        63,
        64,
        65,
        66,
        67,
        68,
        75,
        77,
        78,
        79,
        80,
        81,
        82,
        83,
        84,
        85,
        86,
        87,
        90,
        91,
        92,
        93,
        27,
        29,
        88,
        89,
        27,
        27,
        27,
        25,
        25,
        25,
        77,
        92,
        77,
        32,
        55,
        0,
        51,
        56,
        25,
        25,
        26,
        50,
        49,
        46,
        48,
        42,
        43,
        44,
        45,
        37,
        38,
        39,
        40,
        41,
        47,
        9,
        71,
        72,
        85,
        90,
        28,
        76,
        78,
        78,
        88,
        77,
        59,
        77,
        92,
        77,
        25,
        9,
        72,
        28,
        32,
        78,
        80,
        81,
        82,
        82,
        83,
        83,
        83,
        83,
        84,
        84,
        85,
        85,
        85,
        78,
        27,
        47,
        89,
        26,
        26,
        28,
        30,
        28,
        25,
        25,
        28,
        69,
        70,
        92,
        31,
        73,
        78,
        47,
        71,
        78,
        56,
        77,
        77,
        56,
        26,
        28,
        9,
        32,
        73,
        74,
        73,
        14,
        25,
        25,
        69,
        31,
        61,
        29,
        26,
        32,
        56,
        77,
        77,
        32,
        55,
        3,
        30,
        73,
        28,
        28,
        32,
        30,
        56,
        56};

    const signed char YaccParser ::yyr1_[] = {0,
        53,
        54,
        54,
        55,
        55,
        56,
        56,
        56,
        56,
        56,
        56,
        56,
        56,
        56,
        56,
        56,
        56,
        57,
        58,
        59,
        59,
        60,
        61,
        61,
        62,
        63,
        63,
        64,
        64,
        65,
        66,
        66,
        67,
        68,
        68,
        69,
        69,
        69,
        70,
        70,
        70,
        71,
        71,
        71,
        71,
        72,
        72,
        73,
        73,
        73,
        74,
        74,
        75,
        76,
        76,
        77,
        77,
        78,
        78,
        79,
        79,
        80,
        80,
        81,
        81,
        81,
        82,
        82,
        82,
        82,
        82,
        83,
        83,
        83,
        84,
        84,
        84,
        84,
        85,
        85,
        86,
        86,
        86,
        86,
        87,
        87,
        88,
        89,
        89,
        90,
        90,
        91,
        91,
        91,
        92,
        92,
        92,
        93,
        93,
        93};

    const signed char YaccParser ::yyr2_[] = {0,
        2,
        1,
        2,
        1,
        2,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        2,
        2,
        2,
        3,
        2,
        2,
        3,
        6,
        9,
        9,
        5,
        7,
        5,
        2,
        3,
        2,
        2,
        3,
        2,
        4,
        5,
        0,
        1,
        3,
        1,
        2,
        3,
        4,
        1,
        3,
        1,
        2,
        3,
        1,
        3,
        3,
        1,
        3,
        1,
        3,
        1,
        1,
        1,
        3,
        1,
        3,
        1,
        3,
        3,
        1,
        3,
        3,
        3,
        3,
        1,
        3,
        3,
        1,
        3,
        3,
        3,
        1,
        2,
        1,
        1,
        3,
        1,
        3,
        4,
        3,
        1,
        2,
        1,
        2,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1};

#if YYDEBUG || 1
    // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
    // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
    const char* const YaccParser ::yytname_[] = {"\"end of file\"",
        "error",
        "\"invalid token\"",
        "INT_CONST",
        "LL_CONST",
        "FLOAT_CONST",
        "STR_CONST",
        "ERR_TOKEN",
        "SLASH_COMMENT",
        "IDENT",
        "INT",
        "FLOAT",
        "VOID",
        "IF",
        "ELSE",
        "FOR",
        "WHILE",
        "CONTINUE",
        "BREAK",
        "SWITCH",
        "CASE",
        "GOTO",
        "DO",
        "RETURN",
        "CONST",
        "SEMICOLON",
        "COMMA",
        "LPAREN",
        "RPAREN",
        "LBRACKET",
        "RBRACKET",
        "LBRACE",
        "RBRACE",
        "NOT",
        "BITOR",
        "BITAND",
        "DOT",
        "PLUS",
        "MINUS",
        "STAR",
        "SLASH",
        "MOD",
        "GT",
        "GE",
        "LT",
        "LE",
        "EQ",
        "ASSIGN",
        "NEQ",
        "AND",
        "OR",
        "END",
        "THEN",
        "$accept",
        "PROGRAM",
        "STMT_LIST",
        "STMT",
        "CONTINUE_STMT",
        "EXPR_STMT",
        "VAR_DECLARATION",
        "VAR_DECL_STMT",
        "FUNC_BODY",
        "FUNC_DECL_STMT",
        "FOR_STMT",
        "IF_STMT",
        "WHILE_STMT",
        "RETURN_STMT",
        "BREAK_STMT",
        "BLOCK_STMT",
        "PARAM_DECLARATOR",
        "PARAM_DECLARATOR_LIST",
        "VAR_DECLARATOR",
        "VAR_DECLARATOR_LIST",
        "INITIALIZER",
        "INITIALIZER_LIST",
        "ASSIGN_EXPR",
        "EXPR_LIST",
        "EXPR",
        "NOCOMMA_EXPR",
        "LOGICAL_OR_EXPR",
        "LOGICAL_AND_EXPR",
        "EQUALITY_EXPR",
        "RELATIONAL_EXPR",
        "ADDSUB_EXPR",
        "MULDIV_EXPR",
        "UNARY_EXPR",
        "BASIC_EXPR",
        "FUNC_CALL_EXPR",
        "ARRAY_DIMENSION_EXPR",
        "ARRAY_DIMENSION_EXPR_LIST",
        "LEFT_VAL_EXPR",
        "LITERAL_EXPR",
        "TYPE",
        "UNARY_OP",
        YY_NULLPTR};
#endif

#if YYDEBUG
    const short YaccParser ::yyrline_[] = {0,
        140,
        140,
        144,
        150,
        154,
        161,
        164,
        167,
        170,
        173,
        176,
        179,
        182,
        185,
        188,
        191,
        194,
        201,
        207,
        213,
        216,
        222,
        228,
        231,
        247,
        254,
        258,
        265,
        268,
        276,
        282,
        285,
        291,
        297,
        300,
        306,
        310,
        316,
        326,
        329,
        333,
        341,
        346,
        351,
        356,
        364,
        368,
        375,
        378,
        381,
        387,
        391,
        398,
        404,
        408,
        415,
        418,
        433,
        436,
        442,
        445,
        451,
        454,
        460,
        463,
        466,
        472,
        475,
        478,
        481,
        484,
        490,
        493,
        496,
        502,
        505,
        508,
        511,
        517,
        520,
        526,
        529,
        532,
        535,
        541,
        556,
        563,
        569,
        573,
        580,
        584,
        591,
        594,
        597,
        603,
        606,
        609,
        615,
        618,
        621};

    void YaccParser ::yy_stack_print_() const
    {
        *yycdebug_ << "Stack now";
        for (stack_type::const_iterator i = yystack_.begin(), i_end = yystack_.end(); i != i_end; ++i)
            *yycdebug_ << ' ' << int(i->state);
        *yycdebug_ << '\n';
    }

    void YaccParser ::yy_reduce_print_(int yyrule) const
    {
        int yylno  = yyrline_[yyrule];
        int yynrhs = yyr2_[yyrule];
        // Print the symbols being reduced, and their result.
        *yycdebug_ << "Reducing stack by rule " << yyrule - 1 << " (line " << yylno << "):\n";
        // The symbols being reduced.
        for (int yyi = 0; yyi < yynrhs; yyi++)
            YY_SYMBOL_PRINT("   $" << yyi + 1 << " =", yystack_[(yynrhs) - (yyi + 1)]);
    }
#endif  // YYDEBUG

#line 4 "frontend/parser/yacc.y"
}  // namespace FE
#line 2545 "frontend/parser/yacc.cpp"

#line 626 "frontend/parser/yacc.y"

void FE::YaccParser::error(const FE::location& location, const std::string& message)
{
    std::cerr << "msg: " << message << ", error happened at: " << location << std::endl;
}
