//
// Created by lipan on 2019/4/24.
//

#ifndef SPARROW_PARSER_H
#define SPARROW_PARSER_H

#include <stdint.h>
#include "compiler.h"
#include "common.h"


typedef enum
{
    TOKEN_UNKNOWN,

    // 数据类型
            TOKEN_NUM,              // 数字
    TOKEN_STRING,           // 字符串
    TOKEN_ID,               // 变量
    TOKEN_INTERPOLATION,    // 内嵌表达式

    // 关键字
            TOKEN_VAR,              // var
    TOKEN_FUN,              // fun
    TOKEN_IF,               // if
    TOKEN_ELSE,             // else
    TOKEN_TRUE,             // true
    TOKEN_FALSE,            // false
    TOKEN_WHILE,            // while
    TOKEN_FOR,              // for
    TOKEN_BREAK,            // break
    TOKEN_CONTINUE,         // continue
    TOKEN_RETURN,           // return
    TOKEN_NULL,             // null

    // 类和模块导入
            TOKEN_CLASS,            // class
    TOKEN_THIS,             // this
    TOKEN_STATIC,           // static
    TOKEN_IS,               // is
    TOKEN_SUPER,            // super
    TOKEN_IMPORT,           // import

    // 分隔符
            TOKEN_COMMA,            // ,
    TOKEN_COLON,            // :
    TOKEN_LEFT_PAREN,       // (
    TOKEN_RIGHT_PAREN,      // )
    TOKEN_LEFT_BRACKET,     // [
    TOKEN_RIGHT_BRACKET,    // ]
    TOKEN_LEFT_BRACE,       // {
    TOKEN_RIGHT_BRACE,      // }
    TOKEN_DOT,              // .
    TOKEN_DOT_DOT,          // ..

    // 简单双目运算符
            TOKEN_ADD,              // +
    TOKEN_SUB,              // -
    TOKEN_MUL,              // *
    TOKEN_DIV,              // /
    TOKEN_MOD,              // %

    // 赋值运算符
            TOKEN_ASSIGN,           // =

    // 位运算符
            TOKEN_BIT_AND,          // &
    TOKEN_BIT_OR,           // |
    TOKEN_BIT_NOT,          // ~
    TOKEN_BIT_SHIFT_RIGHT,  // >>
    TOKEN_BIT_SHIFT_LEFT,   // <<

    // 逻辑运算符
            TOKEN_LOGIC_AND,        // &&
    TOKEN_LOGIC_OR,         // ||
    TOKEN_LOGIC_NOT,        // !

    // 关系操作符
            TOKEN_EQUAL,            // ==
    TOKEN_NOT_EQUAL,        // !=
    TOKEN_GREATE,           // >
    TOKEN_GREATE_EQUAL,     // >=
    TOKEN_LESS,             // <
    TOKEN_LESS_EQUAL,       // <=

    TOKEN_QUESTION,         // ?

    // 文件结束标记
            TOKEN_EOF               // EOF
} TokenType;


typedef struct
{
    TokenType type;
    const char *start;
    uint32_t length;
    uint32_t lineNo;
} Token;


struct parser
{
    const char *file;
    const char *sourceCode;     // sourceCode可能为文件内容或者字符串

    char curChar;
    const char *nextCharPtr;

    Token preToken;
    Token curToken;

    ObjModule *curModule;    // 当前正在编译的模块
    CompileUnit *curCompileUnit;   // 当前编译单元

    int interpolationExpectRightParenNum;   // 处于内嵌表达式之中时,期望的右括号数量(跟踪小括号对儿的嵌套)

    struct parser *parent;      // 指向父parser

    VM *vm;
};


void getNextToken(Parser *parser);

void initParser(VM *vm, Parser *parser, const char *file,
                const char *sourceCode, ObjModule *objModule);
bool matchToken(Parser* parser, TokenType expected);

#endif //SPARROW_PARSER_H
