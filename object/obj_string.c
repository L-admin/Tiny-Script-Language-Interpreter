//
// Created by lipan on 2019/4/25.
//

#include <string.h>
#include "vm.h"
#include "utils.h"
#include "obj_string.h"


inline ObjString* ValueToObjString(Value value)
{
    return (ObjString*)ValueToObj(value);
}

// fnv-1a算法
uint32_t hashString(char* str, uint32_t length)
{
    uint32_t hashCode = 2166136261;
    for (uint32_t idx = 0; idx < length; idx++)
    {
        hashCode ^= str[idx];
        hashCode *= 16777619;
    }

    return hashCode;
}

// 为string计算哈希码并将值存储到string->hash
void hashObjString(ObjString* objString)
{
    objString->hashCode = hashString(objString->value.start, objString->value.length);
}

// 以str字符串创建ObjString对象,允许空串""
ObjString* newObjString(VM* vm, const char* str, uint32_t length)
{
    ASSERT(length == 0 || str != NULL, "str length don`t match str!");

    ObjString* objString = ALLOCATE_EXTRA(vm, ObjString, length + 1);   // +1是为了结尾的'\0'
    if (objString == NULL)
    {
        MEM_ERROR("Allocating ObjString failed!");
    }

    initObjHeader(vm, &objString->objHeader, OT_STRING, vm->stringClass);
    objString->value.length = length;

    // 支持空字符串: str为null,length为0,如果非空则复制其内容
    if (length > 0)
    {
        memcpy(objString->value.start, str, length);
    }

    objString->value.start[length] = '\0';
    hashObjString(objString);

    return objString;
}