//
// Created by lipan on 2019/5/1.
//

#include "obj_fn.h"

void IntBufferInit(IntBuffer *buf)
{
    buf->datas = NULL;
    buf->count = 0;
    buf->capacity = 0;
}

void IntBufferFillWrite(VM *vm, IntBuffer *buf, Int data, uint32_t fillCount)
{
    uint32_t newCounts = buf->count + fillCount;
    if (newCounts > buf->capacity)
    {
        // 扩容
        size_t oldSize = buf->capacity * sizeof(Value);

        buf->capacity = ceilToPowerOf2(newCounts);
        size_t newSize = buf->capacity * sizeof(Value);
        ASSERT(newSize > oldSize, "faint...memory allocate!");

        buf->datas = (Int *) memManager(vm, buf->datas, oldSize, newSize);
    }

    for (uint32_t cnt = 0; cnt < fillCount; cnt++)
    {
        buf->datas[buf->count++] = data;
    }
}

void IntBufferAdd(VM *vm, IntBuffer *buf, Int data)
{
    IntBufferFillWrite(vm, buf, data, 1);
}

void IntBufferClear(VM *vm, IntBuffer *buf)
{
    size_t oldSize = buf->capacity * sizeof(buf->datas[0]);
    memManager(vm, buf->datas, oldSize, 0);
    IntBufferInit(buf);
}


ObjFn *newObjFn(VM *vm, ObjModule *objModule, uint32_t slotNum)
{
    ObjFn *objFn = ALLOCATE(vm, ObjFn);
    if (objFn == NULL)
    {
        MEM_ERROR("allocate ObjFn failed!");
    }

    initObjHeader(vm, &objFn->objHeader, OT_FUNCTION, vm->fnClass);
    ByteBufferInit(&objFn->instrStream);
    ValueBufferInit(&objFn->constants);
    objFn->module = objModule;
    objFn->maxStackSlotUsedNum = slotNum;
    objFn->upvalueNum = 0;
    objFn->argNum = 0;

#ifdef DEBUG
    objFn->debug = ALLOCATE(vm, FnDebug);
    objFn->debug->fnName = NULL;
    IntBufferInit(&objFn->debug->lineNo);
#endif
    return objFn;
}


ObjClosure *newObjClosure(VM *vm, ObjFn *objFn)
{
    ObjClosure* objClosure =
            ALLOCATE_EXTRA(vm, ObjClosure, sizeof(ObjUpvalue*) * objFn->upvalueNum);
    initObjHeader(vm, &objClosure->objHeader, OT_CLOSURE, vm->fnClass);
    objClosure->fn = objFn;

    // 清除upvalue数组
    for (uint32_t idx = 0; idx < objFn->upvalueNum; idx++)
    {
        objClosure->upvalues[idx] = NULL;
    }

    return objClosure;
}


ObjUpvalue *newObjUpvalue(VM *vm, Value *localVarPtr)
{
    ObjUpvalue* objUpvalue = ALLOCATE(vm, ObjUpvalue);
    initObjHeader(vm, &objUpvalue->objHeader, OT_UPVALUE, NULL);
    objUpvalue->localVarPtr = localVarPtr;
    objUpvalue->closedUpvalue = ValueTypeToValue(VT_NULL);
    objUpvalue->next = NULL;
    return objUpvalue;
}

