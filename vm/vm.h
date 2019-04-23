//
// Created by lipan on 2019/4/24.
//

#ifndef SPARROW_VM_H
#define SPARROW_VM_H

#include "common.h"

struct vm {
    Parser* curParser;          // 当前词法分析器
};

void initVM(VM* vm);
VM* newVM(void);


#endif //SPARROW_VM_H
