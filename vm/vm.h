//
// Created by lipan on 2019/4/24.
//

#ifndef SPARROW_VM_H
#define SPARROW_VM_H

#include "common.h"
#include <stdint.h>
#include "obj_map.h"
#include "header_obj.h"


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
    Parser *curParser;              // 当前词法分析器
    uint32_t allocatedBytes;        // 累计已分配的内存大小
    ObjHeader *allObjects;          // 跟踪已经分配对象的链表
    ObjMap* allModules;             // 记录所有的模块
};

void initVM(VM *vm);

VM *newVM();


#endif //SPARROW_VM_H
