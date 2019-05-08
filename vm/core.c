//
// Created by lipan on 2019/4/23.
//
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include "obj_map.h"
#include "meta_obj.h"
#include "core.h"
#include "utils.h"

char *rootDir = NULL;

#define CORE_MODULE VT_TO_VALUE(VT_NULL)

char *readFile(const char *path)
{
    FILE *file = fopen(path, "rb"); // r 和 rb 区别
    if (file == NULL)
    {
        IO_ERROR("Could`t open file \"%s\".\n", path);
    }

    struct stat fileStat;
    stat(path, &fileStat);
    size_t fileSize = (size_t) fileStat.st_size;

    char *fileContent = (char *) malloc(fileSize + 1);
    if (fileContent == NULL)
    {
        MEM_ERROR("Could`t allocate memory for reading file \"%s\".\n", path);
    }

    size_t numRead = fread(fileContent, sizeof(char), fileSize, file);
    if (numRead < fileSize)
    {
        IO_ERROR("Could`t read file \"%s\".\n", path);
    }
    fileContent[fileSize] = '\0';

    fclose(file);

    return fileContent;
}

VMResult executeModule(VM *vm, Value moduleName, const char *moduleCode)
{
    return VM_RESULT_ERROR;
}

void buildCore(VM *vm)
{
    // 创建核心模块, 将其加入到vm->allModules中
    ObjModule *coreModule = newObjModule(vm, NULL);
    mapSet(vm, vm->allModules, CORE_MODULE, OBJ_TO_VALUE(coreModule));
}


// table中查找符号symbol, 找到后返回索引,否则返回-1.
int getIndexFromSymbolTable(SymbolTable *table, const char *symbol, uint32_t length)
{
    ASSERT(length != 0, "length of symbol is 0!");

    for (uint32_t idx = 0; idx < table->count; idx++)
    {
        if (length == table->datas[idx].length &&
            memcmp(table->datas[idx].str, symbol, length) == 0)
        {
            return idx;
        }
    }

    return -1;
}

// 往table中添加符号symbol,返回其索引
int addSymbol(VM *vm, SymbolTable *table, const char *symbol, uint32_t length)
{
    ASSERT(length != 0, "length of symbol is 0!");

    String string;
    string.str = ALLOCATE_ARRAY(vm, char, length + 1);

    memcpy(string.str, symbol, length);

    string.str[length] = '\0';
    string.length = length;
    StringBufferAdd(vm, table, string);

    return table->count - 1;
}