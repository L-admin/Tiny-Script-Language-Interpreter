//
// Created by lipan on 2019/4/25.
//

#ifndef SPARROW_OBJ_STRING_H
#define SPARROW_OBJ_STRING_H

#include "header_obj.h"


typedef struct
{
    uint32_t length;        // 除结束'\0'之外的字符个数
    char start[0];          // 类似c99中的柔性数组
} CharValue;                // 字符串缓冲区


typedef struct
{
    ObjHeader objHeader;
    uint32_t hashCode;          // 字符串hash值
    CharValue value;
} ObjString;


uint32_t hashString(char *str, uint32_t length);

void hashObjString(ObjString *objString);

ObjString *newObjString(VM *vm, const char *str, uint32_t length);


#endif //SPARROW_OBJ_STRING_H
