//
// Created by lipan on 2019/4/24.
//

#include <malloc.h>
#include <stdlib.h>
#include "utils.h"
#include "vm.h"


void initVM(VM *vm)
{
    vm->curParser = NULL;
    vm->allocatedBytes = 0;
    vm->allObjects = NULL;
    vm->stringClass = NULL;
}

VM *newVM()
{
    VM *vm = malloc(sizeof(VM));
    if (vm == NULL)
    {
        MEM_ERROR("allocate VM failed!");
    }

    initVM(vm);
    return vm;
}
