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

// value类型
typedef enum
{
    VT_UNDEFINED,
    VT_NULL,
    VT_FALSE,
    VT_TRUE,
    VT_NUM,
    VT_OBJ          // 值为对象,指向对象头
} ValueType;

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

#define VT_TO_VALUE(vt) ((Value){vt, {0}})
#define BOOL_TO_VALUE(boolean) (boolean ? VT_TO_VALUE(VT_TRUE) : VT_TO_VALUE(VT_FALSE))
#define VALUE_TO_BOOL(value) ((value).type == VT_TRUE ? true : false)

#define NUM_TO_VALUE(num) ((Value){VT_NUM, {num}})
#define VALUE_TO_NUM(value) value.num

#define OBJ_TO_VALUE(objPtr) ({ \
   Value value; \
   value.type = VT_OBJ; \
   value.objHeader = (ObjHeader*)(objPtr); \
   value; \
})

#define VALUE_IS_UNDEFINED(value) ((value).type == VT_UNDEFINED)
#define VALUE_IS_NULL(value) ((value).type == VT_NULL)
#define VALUE_IS_TRUE(value) ((value).type == VT_TRUE)
#define VALUE_IS_FALSE(value) ((value).type == VT_FALSE)
#define VALUE_IS_NUM(value) ((value).type == VT_NUM)
#define VALUE_IS_OBJ(value) ((value).type == VT_OBJ)


typedef struct
{
    Value *datas;               // 数据缓冲区
    uint32_t count;             // 缓冲区已经使用的元素个数
    uint32_t capacity;          // 缓冲区容量
} ValueBuffer;

void ValueBufferInit(ValueBuffer *buf);
void ValueBufferFillWrite(VM *vm, ValueBuffer *buf, Value data, uint32_t fillCount);
void ValueBufferAdd(VM *vm, ValueBuffer *buf, Value data);
void ValueBufferClear(VM *vm, ValueBuffer *buf);


void initObjHeader(VM *vm, ObjHeader *objHeader, ObjType objType, Class *class);


#endif //SPARROW_HEADER_OBJ_H
