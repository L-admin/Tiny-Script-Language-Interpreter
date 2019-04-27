//
// Created by lipan on 2019/4/24.
//

#ifndef SPARROW_VM_H
#define SPARROW_VM_H

#include "common.h"
#include <stdint.h>

struct vm {
    Parser*     curParser;          // 当前词法分析器
    uint32_t    allocatedBytes;     // 累计已分配的内存大小
};

void initVM(VM* vm);
VM* newVM(void);


#endif //SPARROW_VM_H
