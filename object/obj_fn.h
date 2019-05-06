//
// Created by lipan on 2019/5/1.
//

#ifndef SPARROW_OBJ_FN_H
#define SPARROW_OBJ_FN_H

#include "utils.h"
#include "vm.h"
#include "meta_obj.h"

typedef int Int;

typedef struct
{
    Int *datas;             // 数据缓冲区
    uint32_t count;         // 缓冲区中已使用的元素个数
    uint32_t capacity;      // 缓冲区容量用
} IntBuffer;

void IntBufferInit(IntBuffer *buf);
void IntBufferFillWrite(VM *vm, IntBuffer *buf, Int data, uint32_t fillCount);
void IntBufferAdd(VM *vm, IntBuffer *buf, Int data);
void IntBufferClear(VM *vm, IntBuffer *buf);


// 在函数中的调试结构
typedef struct
{
    char *fnName;       // 函数名
    IntBuffer lineNo;   // 行号
} FnDebug;


// 函数对象
typedef struct
{
    ObjHeader objHeader;
    ByteBuffer instrStream;      // 函数编译后的指令流
    ValueBuffer constants;      // 函数中的常量表

    ObjModule *module;          // 本函数所属的模块

    uint32_t maxStackSlotUsedNum;
    uint32_t upvalueNum;        // 本函数所拥有的upvalue数量
    uint32_t argNum;            // 函数期望的参数个数
#if DEBUG
    FnDebug* debug;
#endif
} ObjFn;


// upValue对象
typedef struct upvalue
{
    ObjHeader objHeader;
    Value* localVarPtr;
    Value closedUpvalue;
    struct upvalue* next;       // 用以链接openUpvalue链表
} ObjUpvalue;


// 闭包对象
typedef struct
{
    ObjHeader objHeader;
    ObjFn* fn;                  // 闭包中所要引用的函数
    ObjUpvalue* upvalues[0];    // 用于存储此函数的 "close upvalue" 柔性数组
} ObjClosure;


// 调用框架
typedef struct
{
    uint8_t *ip;                // 程序计数器，指向下一个将被执行的指令
    ObjClosure *closure;        // 在本frame中执行的闭包函数
    Value *stackStart;          // frame共享thread.stack, 用于指向本frame所在的thread运行时栈的起始地址
} Frame;


#define INITIAL_FRAME_NUM 4

ObjUpvalue *newObjUpvalue(VM *vm, Value *localVarPtr);
ObjClosure *newObjClosure(VM *vm, ObjFn *objFn);
ObjFn *newObjFn(VM *vm, ObjModule *objModule, uint32_t maxStackSlotUsedNum);


#endif //SPARROW_OBJ_FN_H
