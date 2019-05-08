//
// Created by lipan on 2019/4/26.
//

#ifndef SPARROW_CLASS_H
#define SPARROW_CLASS_H

#include "common.h"
#include "header_obj.h"
#include "obj_string.h"
#include "obj_fn.h"


// 方法类型
typedef enum
{
    MT_NONE,        // 空方法类型
    MT_PRIMITIVE,   // 在vm中使用C实现的原生方法
    MT_SCRIPT,      // 脚本中定义的方法
    MT_FN_CALL,     // 方法类型
} MethodType;

// 原生方法指针
typedef bool (*Primitive)(VM *vm, Value *args);

typedef struct
{
    MethodType type;  // union中的值由type的值决定
    union
    {
        Primitive primFn;   // 指向脚本方法所关联的c实现
        ObjClosure *obj;    // 指向脚本代码编译后的ObjClosure或ObjFn
    };
} Method;

// Methodbuffer类型
typedef struct
{
    Method *datas;      // 数据缓冲区
    uint32_t count;     // 缓冲区中已使用的元素个数
    uint32_t capacity;  // 缓冲区容量用
} MethodBuffer;

void MethodBufferInit(MethodBuffer *buf);
void MethodBufferFillWrite(VM *vm, MethodBuffer *buf, Method data, uint32_t fillCount);
void MethodBufferAdd(VM *vm, MethodBuffer *buf, Method data);
void MethodBufferClear(VM *vm, MethodBuffer *buf);


// 类是通用的数据模板
struct class
{
    ObjHeader objHeader;
    struct class *superClass;       // 指向父类
    uint32_t fieldNum;              // 本类的字段数量
    MethodBuffer methods;           // 本类的方法
    ObjString *name;                // 类名
};

bool valueIsEqual(Value a, Value b);

#endif //SPARROW_CLASS_H
