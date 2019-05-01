//
// Created by lipan on 2019/4/25.
//

#include <vm.h>
#include "header_obj.h"
#include "utils.h"

void ValueBufferInit(ValueBuffer *buf)
{
    buf->datas = NULL;
    buf->count = 0;
    buf->capacity = 0;
}

void ValueBufferFillWrite(VM *vm, ValueBuffer *buf, Value data, uint32_t fillCount)
{
    uint32_t newCounts = buf->count + fillCount;
    if (newCounts > buf->capacity)
    {
        // 扩容
        size_t oldSize = buf->capacity * sizeof(Value);

        buf->capacity = ceilToPowerOf2(newCounts);
        size_t newSize = buf->capacity * sizeof(Value);
        ASSERT(newSize > oldSize, "faint...memory allocate!");

        buf->datas = (Value *) memManager(vm, buf->datas, oldSize, newSize);
    }

    for (uint32_t cnt = 0; cnt < fillCount; cnt++)
    {
        buf->datas[buf->count++] = data;
    }
}

void ValueBufferAdd(VM *vm, ValueBuffer *buf, Value data)
{
    ValueBufferFillWrite(vm, buf, data, 1);
}

void ValueBufferClear(VM *vm, ValueBuffer *buf)
{
    size_t oldSize = buf->capacity * sizeof(buf->datas[0]);
    memManager(vm, buf->datas, oldSize, 0);
    ValueBufferInit(buf);
}


void initObjHeader(VM *vm, ObjHeader *objHeader, ObjType objType, Class *class)
{
    objHeader->type = objType;
    objHeader->class = class;
    objHeader->isDark = false;
    objHeader->next = vm->allObjects;
    vm->allObjects = objHeader;
}