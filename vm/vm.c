//
// Created by lipan on 2019/4/24.
//

#include <malloc.h>
#include <stdlib.h>
#include "utils.h"
#include "vm.h"


void initVM(VM* vm)
{
    vm->curParser = NULL;
}

VM* newVM(void)
{
    VM* vm = malloc(sizeof(VM));
    if (vm == NULL)
    {
        MEM_ERROR("allocate VM failed!");
    }

    initVM(vm);
    return vm;
}
