//
// Created by lipan on 2019/4/24.
//

#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "utils.h"


void ByteBufferInit(ByteBuffer *buf)
{
    buf->datas = NULL;
    buf->count = 0;
    buf->capacity = 0;
}

void ByteBufferFillWrite(VM *vm, ByteBuffer *buf, Byte data, uint32_t fillCount)
{
    uint32_t newCounts = buf->count + fillCount;
    if (newCounts > buf->capacity)
    {
        size_t oldSize = buf->capacity * sizeof(Byte);

        buf->capacity = ceilToPowerOf2(newCounts);
        size_t newSize = buf->capacity * sizeof(Byte);
        ASSERT(newSize > oldSize, "faint...memory allocate!");

        buf->datas = (Byte *) memManager(vm, buf->datas, oldSize, newSize);
    }

    for (uint32_t cnt = 0; cnt < fillCount; cnt++)
    {
        buf->datas[buf->count] = data;
        buf->count++;
    }
}

void ByteBufferAdd(VM *vm, ByteBuffer *buf, Byte data)
{
    ByteBufferFillWrite(vm, buf, data, 1);
}

void ByteBufferClear(VM *vm, ByteBuffer *buf)
{
    size_t oldSize = buf->capacity * sizeof(buf->datas[0]);
    memManager(vm, buf->datas, oldSize, 0);
    ByteBufferInit(buf);
}


struct keywordToken
{
    char *keyword;
    uint8_t length;
    TokenType token;
};

// 关键字查找表
struct keywordToken keywordsToken[] = {
        {"var",      3, TOKEN_VAR},
        {"fun",      3, TOKEN_FUN},
        {"if",       2, TOKEN_IF},
        {"else",     4, TOKEN_ELSE},
        {"true",     4, TOKEN_TRUE},
        {"false",    5, TOKEN_FALSE},
        {"while",    5, TOKEN_WHILE},
        {"for",      3, TOKEN_FOR},
        {"break",    5, TOKEN_BREAK},
        {"continue", 8, TOKEN_CONTINUE},
        {"return",   6, TOKEN_RETURN},
        {"null",     4, TOKEN_NULL},
        {"class",    5, TOKEN_CLASS},
        {"is",       2, TOKEN_IS},
        {"static",   6, TOKEN_STATIC},
        {"this",     4, TOKEN_THIS},
        {"super",    5, TOKEN_SUPER},
        {"import",   6, TOKEN_IMPORT},
        {NULL,       0, TOKEN_UNKNOWN}
};

// 判断start是否为关键字并返回相应的token
static TokenType idOrkeyword(const char *start, uint32_t length)
{
    for (uint32_t idx = 0; keywordsToken[idx].keyword != NULL; idx++)
    {
        if (keywordsToken[idx].length == length &&
            memcmp(keywordsToken[idx].keyword, start, length) == 0)
        {
            return keywordsToken[idx].token;
        }
    }

    return TOKEN_ID;
}

// 向前看1个字符
char lookAheadChar(Parser *parser)
{
    return *(parser->nextCharPtr);
}

// 获取下一个字符
static void getNextChar(Parser *parser)
{
    parser->curChar = *parser->nextCharPtr++;
}

// 查看下一个字符是否为期望的,如果是就读进来,返回true,否则返回false
static bool matchNextChar(Parser *parser, char expectedChar)
{
    if (lookAheadChar(parser) == expectedChar)
    {
        getNextChar(parser);
        return true;
    }

    return false;
}

// 跳过连续的空白字符
static void skipBlanks(Parser *parser)
{
    while (isspace(parser->curChar)) // isspace判断字符是否为空格' ', 回车'\r', 制表符'\t', 换行'\n'
    {
        if (parser->curChar == '\n')
        {
            parser->curToken.lineNo++;
        }
        getNextChar(parser);
    }
}

// 跳过一行
static void skipALine(Parser *parser)
{
    getNextChar(parser);
    while (parser->curChar != '\0')
    {
        if (parser->curChar == '\n')
        {
            parser->curToken.lineNo++;
            getNextChar(parser);    // 更新curChar
            break;
        }
        getNextChar(parser);
    }
}

// 解析标识符
static void parseId(Parser *parser, TokenType type)
{
    while (isalnum(parser->curChar) || parser->curChar == '_')
    {
        getNextChar(parser);
    }

    uint32_t length = (parser->nextCharPtr - parser->curToken.start - 1);

    if (type != TOKEN_UNKNOWN)
    {
        parser->curToken.type = type;
    }
    else
    {
        parser->curToken.type = idOrkeyword(parser->curToken.start, length);
    }

    parser->curToken.length = length;
}

static void parseString(Parser *parser)
{
    ByteBuffer str;
    ByteBufferInit(&str);
    while (true)
    {
        getNextChar(parser);
        if (parser->curChar == '\0')
        {
            LEX_ERROR(parser, "unterminated string!");
        }

        if (parser->curChar == '"')
        {
            parser->curToken.type = TOKEN_STRING;
            break;
        }

        if (parser->curChar == '%')
        {
            if (!matchNextChar(parser, '('))
            {
                LEX_ERROR(parser, "'%' should followed by '('!");
            }
            if (parser->interpolationExpectRightParenNum > 0)
            {
                COMPILE_ERROR(parser, "sorry, I don`t support nest interpolate expression!");
            }
            parser->interpolationExpectRightParenNum = 1;
            parser->curToken.type = TOKEN_INTERPOLATION;
            break;
        }
        else
        {
            ByteBufferAdd(parser->vm, &str, parser->curChar);
        }
    }
    ByteBufferClear(parser->vm, &str);
}


// 获得下一个token
void getNextToken(Parser *parser)
{
    parser->preToken = parser->curToken;

    skipBlanks(parser); // 跳过待识别单词之前的空格

    parser->curToken.type = TOKEN_EOF;
    parser->curToken.length = 0;
    parser->curToken.start = parser->nextCharPtr - 1;

    while (parser->curChar != '\0')
    {
        switch (parser->curChar)
        {
            case ',':
                parser->curToken.type = TOKEN_COMMA;
                break;
            case ':':
                parser->curToken.type = TOKEN_COLON;
                break;
            case '(':
                if (parser->interpolationExpectRightParenNum > 0)
                {
                    parser->interpolationExpectRightParenNum++;
                }
                parser->curToken.type = TOKEN_LEFT_PAREN;
                break;
            case ')':
                if (parser->interpolationExpectRightParenNum > 0)
                {
                    parser->interpolationExpectRightParenNum--;
                    if (parser->interpolationExpectRightParenNum == 0)
                    {
                        parseString(parser);
                        break;
                    }
                }
                parser->curToken.type = TOKEN_RIGHT_PAREN;
                break;
            case '[':
                parser->curToken.type = TOKEN_LEFT_BRACKET;
                break;
            case ']':
                parser->curToken.type = TOKEN_RIGHT_BRACKET;
                break;
            case '{':
                parser->curToken.type = TOKEN_LEFT_BRACE;
                break;
            case '}':
                parser->curToken.type = TOKEN_RIGHT_BRACE;
                break;
            case '.':
                if (matchNextChar(parser, '.'))
                {
                    parser->curToken.type = TOKEN_DOT_DOT;
                }
                else
                {
                    parser->curToken.type = TOKEN_DOT;
                }
                break;
            case '=':
                if (matchNextChar(parser, '='))
                {
                    parser->curToken.type = TOKEN_EQUAL;
                }
                else
                {
                    parser->curToken.type = TOKEN_ASSIGN;
                }
                break;
            case '+':
                parser->curToken.type = TOKEN_ADD;
                break;
            case '-':
                parser->curToken.type = TOKEN_SUB;
                break;
            case '*':
                parser->curToken.type = TOKEN_MUL;
                break;
            case '/':
                parser->curToken.type = TOKEN_DIV;
                break;
            case '%':
                parser->curToken.type = TOKEN_MOD;
                break;
            case '&':
                if (matchNextChar(parser, '&'))
                {
                    parser->curToken.type = TOKEN_LOGIC_AND;
                }
                else
                {
                    parser->curToken.type = TOKEN_BIT_AND;
                }
                break;
            case '|':
                if (matchNextChar(parser, '|'))
                {
                    parser->curToken.type = TOKEN_LOGIC_OR;
                }
                else
                {
                    parser->curToken.type = TOKEN_BIT_OR;
                }
                break;
            case '~':
                parser->curToken.type = TOKEN_BIT_NOT;
                break;
            case '?':
                parser->curToken.type = TOKEN_QUESTION;
                break;
            case '>':
                if (matchNextChar(parser, '='))
                {
                    parser->curToken.type = TOKEN_GREATE_EQUAL;
                }
                else if (matchNextChar(parser, '>'))
                {
                    parser->curToken.type = TOKEN_BIT_SHIFT_RIGHT;
                }
                else
                {
                    parser->curToken.type = TOKEN_GREATE;
                }
                break;
            case '<':
                if (matchNextChar(parser, '='))
                {
                    parser->curToken.type = TOKEN_LESS_EQUAL;
                }
                else if (matchNextChar(parser, '<'))
                {
                    parser->curToken.type = TOKEN_BIT_SHIFT_LEFT;
                }
                else
                {
                    parser->curToken.type = TOKEN_LESS;
                }
                break;
            case '!':
                if (matchNextChar(parser, '='))
                {
                    parser->curToken.type = TOKEN_NOT_EQUAL;
                }
                else
                {
                    parser->curToken.type = TOKEN_LOGIC_NOT;
                }
                break;
            case '"':
                parseString(parser);
                break;
            default:
                // 处理变量名和数字, 进入此分支的字符肯定是数字或者变量名的首字符
                if (isalpha(parser->curChar) || parser->curChar == '_')
                {
                    parseId(parser, TOKEN_UNKNOWN);
                }
                else
                {
                    // 类似于shell脚本中#!/bin/bash， 指明调用的解释器
                    if (parser->curChar == '#' && matchNextChar(parser, '!'))
                    {
                        skipALine(parser);
                        parser->curToken.start = parser->nextCharPtr - 1;  // 重置下一个token起始地址
                        continue;
                    }
                    LEX_ERROR(parser, "unsupport char: \'%c\', quit.", parser->curChar);
                }
                return;
        }
        // 大部分case的出口
        parser->curToken.length = (uint32_t) (parser->nextCharPtr - parser->curToken.start);
        getNextChar(parser);
        return;
    }
}

//
void initParser(VM *vm, Parser *parser, const char *file, const char *sourceCode)
{
    parser->file = file;
    parser->sourceCode = sourceCode;

    parser->curChar = *parser->sourceCode;
    parser->nextCharPtr = parser->sourceCode + 1;

    parser->curToken.lineNo = 1;

    parser->curToken.type = TOKEN_UNKNOWN;
    parser->curToken.start = NULL;
    parser->curToken.length = 0;

    parser->preToken = parser->curToken;

    parser->interpolationExpectRightParenNum = 0;
    parser->vm = vm;
}