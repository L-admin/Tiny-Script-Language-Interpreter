//
// Created by lipan on 2019/4/23.
//
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include "parser.h"
#include "vm.h"
#include "cli.h"
#include "core.h"

static void runFile(const char *path)
{
#ifdef WIN32
    const char *lastSlash = strrchr(path, '\\');
#else
    const char* lastSlash = strrchr(path, '/');
#endif
    if (lastSlash != NULL)
    {
        char *root = (char *) malloc(lastSlash - path + 2);
        memcpy(root, path, lastSlash - path + 1);
        root[lastSlash - path + 1] = '\0';
        rootDir = root;
    }

    const char *sourceCode = readFile(path);

    fprintf(stdout, "%s", sourceCode);
    fprintf(stdout, "\n\n");

    VM *vm = newVM();
    struct parser parser;
    initParser(vm, &parser, path, sourceCode);

    #include "token.list"
    while (parser.curToken.type != TOKEN_EOF)
    {
        getNextToken(&parser);
        printf("%dL: %s [", parser.curToken.lineNo, tokenArray[parser.curToken.type]);
        for (uint32_t idx = 0; idx < parser.curToken.length; idx++)
        {
            printf("%c", *(parser.curToken.start + idx));
        }
        printf("]\n");
    }
}


int main(int argc, char **argv)
{
    if (argc == 1)
    { ;
    } else
    {
        runFile(argv[1]);
    }

    return 0;
}