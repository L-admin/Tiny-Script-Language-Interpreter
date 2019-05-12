//
// Created by lipan on 2019/4/26.
//

#include "meta_obj.h"
#include "utils.h"
#include "class.h"
#include <string.h>

void StringBufferInit(StringBuffer *buf)
{
    buf->datas = NULL;
    buf->count = 0;
    buf->capacity = 0;
}

void StringBufferFillWrite(VM *vm, StringBuffer *buf, String data, uint32_t fillCount)
{
    uint32_t newCounts = buf->count + fillCount;
    if (newCounts > buf->capacity)
    {
        // 扩容
        size_t oldSize = buf->capacity * sizeof(Value);

        buf->capacity = ceilToPowerOf2(newCounts);
        size_t newSize = buf->capacity * sizeof(Value);
        ASSERT(newSize > oldSize, "faint...memory allocate!");

        buf->datas = (String *) memManager(vm, buf->datas, oldSize, newSize);
    }

    for (uint32_t cnt = 0; cnt < fillCount; cnt++)
    {
        buf->datas[buf->count++] = data;
    }
}

void StringBufferAdd(VM *vm, StringBuffer *buf, String data)
{
    StringBufferFillWrite(vm, buf, data, 1);
}

void StringBufferClear(VM *vm, StringBuffer *buf)
{
    size_t oldSize = buf->capacity * sizeof(buf->datas[0]);
    memManager(vm, buf->datas, oldSize, 0);
    StringBufferInit(buf);
}

// 创建模块
ObjModule *newObjModule(VM *vm, const char *modName)
{
    ObjModule* objModule = ALLOCATE(vm, ObjModule);
    if (objModule == NULL)
    {
        MEM_ERROR("allocate ObjModule failed!");
    }

    // ObjModule是元信息对象,不属于任何一个类
    initObjHeader(vm, &objModule->objHeader, OT_MODULE, NULL);

    StringBufferInit(&objModule->moduleVarName);
    ValueBufferInit(&objModule->moduleVarValue);

    objModule->name = NULL;
    if (modName != NULL)
    {
        objModule->name = newObjString(vm, modName, (uint32_t)(strlen(modName)));
    }

    return objModule;
}

// 创建对象实例
ObjInstance *newObjInstance(VM *vm, Class *class)
{
    ObjInstance* objInstance = ALLOCATE_EXTRA(vm, ObjInstance, sizeof(Value) * class->fieldNum);

    // 关联对象的类为参数class
    initObjHeader(vm, &objInstance->objHeader, OT_INSTANCE, class);

    // 初始化field为NULL.
    for (uint32_t idx = 0; idx < class->fieldNum; idx++)
    {
        objInstance->fields[idx] = ValueTypeToValue(VT_NULL);
    }

    return objInstance;
}