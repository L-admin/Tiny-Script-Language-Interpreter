//
// Created by lipan on 2019/5/1.
//

#include "obj_list.h"
#include "utils.h"
#include "vm.h"
#include <stdint.h>


static void shrinkList(VM* vm, ObjList* objList, uint32_t newCapacity);

// 新建list对象,元素个数为elementNum
ObjList *newObjList(VM *vm, uint32_t elementNum)
{
    Value *elementArray = NULL;
    if (elementNum > 0)
    {
        elementArray = ALLOCATE_ARRAY(vm, Value, elementNum);
    }

    ObjList *objList = ALLOCATE(vm, ObjList);
    objList->elements.datas = elementArray;
    objList->elements.count = elementNum;       // TODO: count == 0 ?
    objList->elements.capacity = elementNum;

    initObjHeader(vm, &objList->objHeader, OT_LIST, vm->listClass);
    return objList;
}


// 在objlist中索引为index处插入value, 类似于list[index] = value
void insertElement(VM *vm, ObjList *objList, uint32_t index, Value value)
{
    if (index > objList->elements.count - 1)
    {
        RUN_ERROR("index out bounded!");
    }

    // 准备一个Value空间以容纳新元素产生的空间波动
    ValueBufferAdd(vm, &objList->elements, (Value){VT_NULL, 0});

    // 使index后面的整体后移一位
    for (uint32_t idx = objList->elements.count - 1; idx > index; idx--)
    {
        objList->elements.datas[idx] = objList->elements.datas[idx - 1];
    }

    // 在index处插入数值
    objList->elements.datas[index] = value;
}

// 删除list中索引为index处的元素，即删除list[index]
Value removeElement(VM *vm, ObjList *objList, uint32_t index)
{
    Value valueRemoved = objList->elements.datas[index];    // TODO: 越界？

    // 使index后面的元素前移一位,覆盖index处的元素
    for (uint32_t idx = index; idx < objList->elements.count; idx++)
    {
        objList->elements.datas[idx] = objList->elements.datas[idx + 1];
    }

    // 容量利用率过低就减小容量
    uint32_t _capacity = objList->elements.capacity / CAPACITY_GROW_FACTOR;
    if (_capacity > objList->elements.count)
    {
        shrinkList(vm, objList, _capacity);
    }

    objList->elements.count--;
    return valueRemoved;
}

// 调整list容量
static void shrinkList(VM* vm, ObjList* objList, uint32_t newCapacity)
{
    uint32_t oldSize = objList->elements.capacity * sizeof(Value);
    uint32_t newSize = newCapacity * sizeof(Value);
    memManager(vm, objList->elements.datas, oldSize, newSize);
    objList->elements.capacity = newCapacity;
}

