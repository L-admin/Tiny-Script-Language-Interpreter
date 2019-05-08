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

VMResult executeModule(VM* vm, Value moduleName, const char* moduleCode)
{
    return VM_RESULT_ERROR;
}

void buildCore(VM* vm)
{
    // 创建核心模块, 将其加入到vm->allModules中
    ObjModule* coreModule = newObjModule(vm, NULL);
    mapSet(vm, vm->allModules, CORE_MODULE, OBJ_TO_VALUE(coreModule));
}