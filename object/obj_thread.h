//
// Created by lipan on 2019/5/7.
//

#ifndef SPARROW_OBJ_THREAD_H
#define SPARROW_OBJ_THREAD_H

#include "obj_fn.h"

// 线程对象
typedef struct objThread
{
    ObjHeader objHeader;

    Value *stack;               // 运行时栈的栈底
    Value *esp;                 // 运行时栈的栈顶
    uint32_t stackCapacity;     // 栈容量

    Frame *frames;               // 调用框架
    uint32_t usedFrameNum;      // 已使用的frame数量
    uint32_t frameCapacity;     // frame 容量

    ObjUpvalue *openUpvalues;   // 打开的upvalue的链表首节点

    struct objThread *caller;   // 当前thread的调用者

    Value errorObj;     // 导致运行时错误的对象会放在此处,否则为空
} ObjThread;


void prepareFrame(ObjThread* objThread, ObjClosure* objClosure, Value* stackStart);
ObjThread* newObjThread(VM* vm, ObjClosure* objClosure);
void resetThread(ObjThread* objThread, ObjClosure*  objClosure);


#endif //SPARROW_OBJ_THREAD_H
