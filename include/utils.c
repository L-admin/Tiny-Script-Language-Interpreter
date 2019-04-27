//
// Created by lipan on 2019/4/23.
//

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "utils.h"
#include "common.h"
#include "parser.h"
#include "vm.h"

// 找出大于等于v最近的2次幂
uint32_t ceilToPowerOf2(uint32_t v)
{
    v += (v == 0);  //修复当v等于0时结果为0的边界情况
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

//内存管理三种功能:
//   1 申请内存
//   2 修改空间大小
//   3 释放内存
void* memManager(VM* vm, void* ptr, uint32_t oldSize, uint32_t newSize)
{
    // 累计系统分配的总内存
    vm->allocatedBytes += newSize - oldSize;

    // realloc(NULL, 0)会返回一个地址
    if (newSize == 0)
    {
        free(ptr);
        return NULL;
    }

    // 函数realloc将ptr指向的内存调整为size大小, 但此函数具备3个功能:
    // a. 若ptr == NULL, 且size != 0, 功能等同于malloc(size), 用于申请内存.
    // b. 若ptr != NULL, 且size == 0, 功能等同于free(ptr),用于释放内存.
    // c. 若ptr != NULL, 且size != 0, 用于扩容，扩容有可能是原地扩容或者重新分配一块新的内存.
    return realloc(ptr, newSize);
}


// 通用报错函数
void errorReport(void* parser, ErrorType errorType, const char* fmt, ...)
{
    char buffer[DEFAULT_BUFFER_SIZE] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, DEFAULT_BUFFER_SIZE, fmt, ap);
    va_end(ap);

    switch (errorType)
    {
        case ERROR_IO:
        case ERROR_MEM:
            fprintf(stderr, "%s:%d In function %s():%s\n", __FILE__, __LINE__, __func__, buffer);
            break;
        case ERROR_LEX:
            ASSERT(parser != NULL, "parser is null!");
            fprintf(stderr, "%s:%d \"%s\"\n", ((Parser*)parser)->file, ((Parser*)parser)->preToken.lineNo, buffer);
        default:
            NOT_REACHED();
    }

    exit(1);
}
