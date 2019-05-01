//
// Created by lipan on 2019/4/25.
//

#ifndef SPARROW_HEADER_OBJ_H
#define SPARROW_HEADER_OBJ_H

#include "common.h"
#include <stdint.h>

typedef enum
{
    OT_CLASS,           // class,以下都是object类型
    OT_LIST,            // list
    OT_MAP,             // map
    OT_MODULE,          // module
    OT_RANGE,           // range
    OT_STRING,          // string
    OT_UPVALUE,         // upvalue
    OT_FUNCTION,        // function
    OT_CLOSURE,         // closure
    OT_INSTANCE,        // instance
    OT_THREAD           // thread
} ObjType;


// 对象头
typedef struct objHeader
{
    ObjType type;               // 对象类型
    bool isDark;                // 对象是否可达
    Class *class;               // 对象所属的类
    struct objHeader *next;     // 用于链接所有已经分配的对象
} ObjHeader;


typedef enum
{
    VT_UNDEFINED,
    VT_NULL,
    VT_FALSE,
    VT_TRUE,
    VT_NUM,
    VT_OBJ          // 值为对象,指向对象头
} ValueType;        // value类型

// 通用的值结构
typedef struct
{
    ValueType type;
    union
    {
        double num;
        ObjHeader *objHeader;
    };
} Value;


typedef struct
{
    Value       *datas;            // 数据缓冲区
    uint32_t    count;             // 缓冲区已经使用的元素个数
    uint32_t    capacity;          // 缓冲区容量
} ValueBuffer;

void ValueBufferInit(ValueBuffer *buf);
void ValueBufferFillWrite(VM *vm, ValueBuffer *buf, Value data, uint32_t fillCount);
void ValueBufferAdd(VM *vm, ValueBuffer *buf, Value data);
void ValueBufferClear(VM *vm, ValueBuffer *buf);


void initObjHeader(VM *vm, ObjHeader *objHeader, ObjType objType, Class *class);


#endif //SPARROW_HEADER_OBJ_H
