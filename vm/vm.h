//
// Created by lipan on 2019/4/24.
//

#ifndef SPARROW_VM_H
#define SPARROW_VM_H

#include "common.h"
#include <stdint.h>
#include "meta_obj.h"
#include "obj_map.h"
#include "header_obj.h"


// 将所有操作码加上前缀, 比如: JUMP 会变成 OPCODE_JUMP
#define OPCODE_SLOTS(opcode, effect) OPCODE_##opcode,
typedef enum
{
    #include "opcode.inc"
} OpCode;
#undef OPCODE_SLOTS


// 虚拟机执行结果如果执行无误,可以将字符码输出到文件缓存,避免下次重新编译
typedef enum vmResult
{
    VM_RESULT_SUCCESS,
    VM_RESULT_ERROR
} VMResult;

struct vm
{
    Class *stringClass;
    Class *listClass;
    Class *rangeClass;
    Class *fnClass;
    Class *mapClass;
    Class *threadClass;
    Class *nullClass;
    Class *boolClass;
    Class *numClass;
    Class *objectClass;
    Class *classOfClass;

    Parser *curParser;              // 当前词法分析器
    uint32_t allocatedBytes;        // 累计已分配的内存大小
    ObjHeader *allObjects;          // 跟踪已经分配对象的链表
    ObjMap *allModules;             // 记录所有已加载的模块<模块名，模块对象>
    SymbolTable allMethodNames;     // (所有)类的方法名
};

void initVM(VM *vm);

VM *newVM();


#endif //SPARROW_VM_H
