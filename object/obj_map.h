//
// Created by lipan on 2019/5/1.
//

#ifndef SPARROW_OBJ_MAP_H
#define SPARROW_OBJ_MAP_H

#include "header_obj.h"


#define MAP_LOAD_PERCENT 0.8

// 存储64位数据
typedef union
{
    uint64_t bits64;
    uint32_t bits32[2];
    double num;
} Bits64;


#define MIN_CAPACITY 64
#define MAP_CAPACITY_GROW_FACTOR 4

// key-value 对
typedef struct
{
    Value key;
    Value value;
} Entry;

// Entry数组
typedef struct
{
    ObjHeader objHeader;
    uint32_t capacity;          // Entry的容量，包括已经使用和未使用
    uint32_t count;             // map中使用的Entry的数量
    Entry *entries;             // Entry数组
} ObjMap;


ObjMap *newObjMap(VM *vm);

void mapSet(VM *vm, ObjMap *objMap, Value key, Value value);
Value mapGet(ObjMap *objMap, Value key);
void clearMap(VM *vm, ObjMap *objMap);
Value removeKey(VM *vm, ObjMap *objMap, Value key);




#endif //SPARROW_OBJ_MAP_H
