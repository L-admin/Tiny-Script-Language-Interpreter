//
// Created by lipan on 2019/4/23.
//

#ifndef SPARROW_UTILS_H
#define SPARROW_UTILS_H

#include <stdint.h>
#include "common.h"


uint32_t ceilToPowerOf2(uint32_t v);


void* memManager(VM* vm, void* ptr, uint32_t oldSize, uint32_t newSize);

// 分配类型为type的内存块
#define ALLOCATE(vmPtr, type) \
   (type*)memManager(vmPtr, NULL, 0, sizeof(type))

// 分配类型为type + extraSize的内存块, 用于柔性数组
#define ALLOCATE_EXTRA(vmPtr, mainType, extraSize) \
   (mainType*)memManager(vmPtr, NULL, 0, sizeof(mainType) + extraSize)

// 分配动态数组内存
#define ALLOCATE_ARRAY(vmPtr, type, count) \
   (type*)memManager(vmPtr, NULL, 0, sizeof(type) * count)

// 释放动态数据内存
#define DEALLOCATE_ARRAY(vmPtr, arrayPtr, count) \
   memManager(vmPtr, arrayPtr, sizeof(arrayPtr[0]) * count, 0)

// 释放内存
#define DEALLOCATE(vmPtr, memPtr) memManager(vmPtr, memPtr, 0, 0)


#define DEFAULT_BUFFER_SIZE 512


typedef enum {
    ERROR_IO,
    ERROR_MEM,
    ERROR_LEX,
    ERROR_COMPILE,
} ErrorType;

void errorReport(void* parser, ErrorType errorType, const char* fmt, ...);

#define IO_ERROR(...)\
    errorReport(NULL, ERROR_IO, __VA_ARGS__)

#define MEM_ERROR(...)\
    errorReport(NULL, ERROR_MEM, __VA_ARGS__)

#define LEX_ERROR(parser, ...)\
   errorReport(parser, ERROR_LEX, __VA_ARGS__)

#define COMPILE_ERROR(parser, ...)\
   errorReport(parser, ERROR_COMPILE, __VA_ARGS__)

#endif //SPARROW_UTILS_H
