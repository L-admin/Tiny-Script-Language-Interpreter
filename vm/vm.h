//
// Created by lipan on 2019/4/24.
//

#ifndef SPARROW_VM_H
#define SPARROW_VM_H

#include "common.h"
#include <stdint.h>
#include "header_obj.h"


struct vm
{
    Parser *curParser;              // 当前词法分析器
    uint32_t allocatedBytes;        // 累计已分配的内存大小
    ObjHeader *allObjects;          // 跟踪已经分配对象的链表
    Class* stringClass;
    Class* listClass;
};

void initVM(VM *vm);

VM *newVM();


#endif //SPARROW_VM_H
