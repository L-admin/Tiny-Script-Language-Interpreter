//
// Created by lipan on 2019/5/1.
//
#include "obj_map.h"
#include "utils.h"
#include "vm.h"
#include "obj_string.h"
#include "obj_range.h"
#include "class.h"


static uint32_t hashNum(double num);
static uint32_t hashObj(ObjHeader *objHeader);
static bool addEntry(Entry *entries, uint32_t capacity, Value key, Value value);
static void resizeMap(VM *vm, ObjMap *objMap, uint32_t newCapacity);
static Entry *findEntry(ObjMap *objMap, Value key);


// 创建新的map对象
ObjMap *newObjMap(VM *vm)
{
    ObjMap *objMap = ALLOCATE(vm, ObjMap);
    initObjHeader(vm, &objMap->objHeader, OT_MAP, vm->mapClass);
    objMap->capacity = objMap->count = 0;
    objMap->entries = NULL;
    return objMap;
}

// 计算数字的hash值
static uint32_t hashNum(double num)
{
    Bits64 bits64;
    bits64.num = num;
    return bits64.bits32[0] ^ bits64.bits32[1];
}

// 计算对象的hash值
static uint32_t hashObj(ObjHeader *objHeader)
{
    switch (objHeader->type)
    {
        case OT_CLASS:  //计算class的哈希值(类名)
            return hashString(((Class *) objHeader)->name->value.start,
                              ((Class *) objHeader)->name->value.length);
        case OT_RANGE:
        {
            // 计算range对象哈希码
            ObjRange *objRange = (ObjRange *) objHeader;
            return hashNum(objRange->from) ^ hashNum(objRange->to);
        }
        case OT_STRING:  //对于字符串,直接返回其hashCode
            return ((ObjString *) objHeader)->hashCode;
        default:
            RUN_ERROR("the hashable are objstring, objrange and class.");
    }
    return 0;
}

// 根据value类型调用相应的hash函数
static uint32_t hashValue(Value value)
{
    switch (value.type)
    {
        case VT_FALSE:
            return 0;
        case VT_NULL:
            return 1;
        case VT_NUM:
            return hashNum(value.num);
        case VT_TRUE:
            return 2;
        case VT_OBJ:
            return hashObj(value.objHeader);
        default:
            RUN_ERROR("unsupport type hashed!");
    }
    return 0;
}


// 在objMap中实现key与value的关联:objMap[key]=value
void mapSet(VM *vm, ObjMap *objMap, Value key, Value value)
{
    // 当容量利用率达到80%时扩容
    if (objMap->count + 1 > objMap->capacity * MAP_LOAD_PERCENT)
    {
        uint32_t newCapacity = objMap->capacity * MAP_CAPACITY_GROW_FACTOR;
        if (newCapacity < MIN_CAPACITY)
        {
            newCapacity = MIN_CAPACITY;
        }
        resizeMap(vm, objMap, newCapacity);
    }

    if (addEntry(objMap->entries, objMap->capacity, key, value))
    {
        objMap->count++;
    }
}

// 使对象objMap的容量调整到newCapacity
static void resizeMap(VM *vm, ObjMap *objMap, uint32_t newCapacity)
{
    // 1. 先建立新的Entry数组
    Entry *newEntries = ALLOCATE_ARRAY(vm, Entry, newCapacity);
    for (uint32_t idx = 0; idx < newCapacity; idx++)
    {
        newEntries[idx].key = ValueTypeToValue(VT_UNDEFINED);
        newEntries[idx].value = ValueTypeToValue(VT_FALSE);
    }

    // 2. 再遍历老的数组,把有值的部分插入到新数组
    if (objMap->capacity > 0)
    {
        Entry *entryArr = objMap->entries;
        for (uint32_t idx = 0; idx < objMap->capacity; idx++)
        {
            if (entryArr[idx].key.type != VT_UNDEFINED)
            {
                addEntry(newEntries, newCapacity, entryArr[idx].key, entryArr[idx].value);
            }
        }
    }

    // 3. 回收老的entry数组空间
    DEALLOCATE_ARRAY(vm, objMap->entries, objMap->count);
    objMap->entries = newEntries;   // 更新指针为新的entry数组
    objMap->capacity = newCapacity; // 更新容量
}

static bool addEntry(Entry *entries, uint32_t capacity, Value key, Value value)
{
    // 用哈希值对容量取模计算槽位(slot)
    uint32_t index = hashValue(key) % capacity;

    // 通过开放探测法去找可用的slot.
    while (true)
    {
        if (entries[index].key.type == VT_UNDEFINED)
        {
            // 找到空闲的slot,说明目前没有此key,直接赋值返回
            entries[index].key = key;
            entries[index].value = value;
            return true;
        }
        else if (valueIsEqual(entries[index].key, key))
        {
            // key已经存在,直接更新其值.
            entries[index].value = value;
            return false;    // 未增加新的key就返回false
        }

        // 开放地址探测，尝试下一个slot.
        index = (index + 1) % capacity;
    }
}


Value mapGet(ObjMap *objMap, Value key)
{
    Entry *entry = findEntry(objMap, key);
    if (entry == NULL)
    {
        return ValueTypeToValue(VT_UNDEFINED);
    }
    return entry->value;
}

static Entry *findEntry(ObjMap *objMap, Value key)
{
    if (objMap->capacity == 0)
    {
        return NULL;
    }

    // 用哈希值对容量取模计算槽位(slot)
    uint32_t index = hashValue(key) % objMap->capacity;
    while (true)
    {
        Entry *entry = &objMap->entries[index];

        // 若该slot中的entry正好是该key的entry,找到返回
        if (valueIsEqual(entry->key, key))
        {
            return entry;
        }
        // key为VT_UNDEFINED且value为VT_TRUE表示探测链未断,可继续探测.
        // key为VT_UNDEFINED且value为VT_FALSE表示探测链结束,探测结束.
        if (IsValueUndefined(entry->key) && IsValueFalse(entry->value))
        {
            return NULL;    // 未找到
        }

        // 继续向下探测
        index = (index + 1) % objMap->capacity;
    }
}

void clearMap(VM *vm, ObjMap *objMap)
{
    DEALLOCATE_ARRAY(vm, objMap->entries, objMap->count);
    objMap->entries = NULL;
    objMap->capacity = 0;
    objMap->count = 0;
}

Value removeKey(VM *vm, ObjMap *objMap, Value key)
{
    Entry *entry = findEntry(objMap, key);

    if (entry == NULL)
    {
        return ValueTypeToValue(VT_NULL);
    }

    // 设置开放定址的伪删除
    Value value = entry->value;
    entry->key = ValueTypeToValue(VT_UNDEFINED);
    entry->value = ValueTypeToValue(VT_TRUE);   //值为真,伪删除

    objMap->count--;
    if (objMap->count == 0)
    {
        // 若删除该entry后map为空就回收该空间
        clearMap(vm, objMap);
    }
    else if (objMap->count < objMap->capacity / (MAP_CAPACITY_GROW_FACTOR) * MAP_LOAD_PERCENT &&
             objMap->count > MIN_CAPACITY)
    {   //若map容量利用率太低,就缩小map空间
        uint32_t newCapacity = objMap->capacity / MAP_CAPACITY_GROW_FACTOR;
        if (newCapacity < MIN_CAPACITY)
        {
            newCapacity = MIN_CAPACITY;
        }
        resizeMap(vm, objMap, newCapacity);
    }

    return value;
}
