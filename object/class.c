//
// Created by lipan on 2019/5/6.
//

#include "class.h"
#include "obj_range.h"
#include <string.h>


bool valueIsEqual(Value a, Value b)
{
    // 类型不同则无需进行后面比较
    if (a.type != b.type)
    {
        return false;
    }

    // 同为数字, 比较数值
    if (a.type == VT_NUM)
    {
        return a.num == b.num;
    }

    // 同为对象, 并且所指的对象是同一个直接返回true
    if (a.type == VT_OBJ && b.type == VT_OBJ)
    {
        return a.objHeader == b.objHeader;
    }

    // 对象类型不同无须比较
    if (a.objHeader->type != b.objHeader->type)
    {
        return false;
    }

    // 以下处理类型相同的对象
    // 若对象同为字符串
    if (a.objHeader->type == OT_STRING)
    {
        ObjString *strA = (ObjString *) (a.objHeader);
        ObjString *strB = (ObjString *) (b.objHeader);
        return (strA->value.length == strB->value.length &&
                memcmp(strA->value.start, strB->value.start, strA->value.length) == 0);
    }

    // 若对象同为range
    if (a.objHeader->type == OT_RANGE)
    {
        ObjRange *rgA = (ObjRange *) (a.objHeader);
        ObjRange *rgB = (ObjRange *) (b.objHeader);
        return (rgA->from == rgB->from && rgA->to == rgB->to);
    }

    return false;  //其它对象不可比较
}


void MethodBufferInit(MethodBuffer *buf)
{
    buf->datas = NULL;
    buf->count = 0;
    buf->capacity = 0;
}

void MethodBufferFillWrite(VM *vm, MethodBuffer *buf, Method data, uint32_t fillCount)
{
    uint32_t newCounts = buf->count + fillCount;
    if (newCounts > buf->capacity)
    {
        // 扩容
        size_t oldSize = buf->capacity * sizeof(Value);

        buf->capacity = ceilToPowerOf2(newCounts);
        size_t newSize = buf->capacity * sizeof(Value);
        ASSERT(newSize > oldSize, "faint...memory allocate!");

        buf->datas = (Method *) memManager(vm, buf->datas, oldSize, newSize);
    }

    for (uint32_t cnt = 0; cnt < fillCount; cnt++)
    {
        buf->datas[buf->count++] = data;
    }
}

void MethodBufferAdd(VM *vm, MethodBuffer *buf, Method data)
{
    MethodBufferFillWrite(vm, buf, data, 1);
}

void MethodBufferClear(VM *vm, MethodBuffer *buf)
{
    size_t oldSize = buf->capacity * sizeof(buf->datas[0]);
    memManager(vm, buf->datas, oldSize, 0);
    MethodBufferInit(buf);
}

