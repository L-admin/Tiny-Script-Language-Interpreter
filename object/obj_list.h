//
// Created by lipan on 2019/5/1.
//

#ifndef SPARROW_OBJ_LISH_H
#define SPARROW_OBJ_LISH_H

#include "header_obj.h"


#define LIST_CAPACITY_GROW_FACTOR 4

// list对象
typedef struct
{
    ObjHeader objHeader;
    ValueBuffer elements;
} ObjList;

ObjList *newObjList(VM *vm, uint32_t elementNum);
void insertElement(VM *vm, ObjList *objList, uint32_t index, Value value);
Value removeElement(VM *vm, ObjList *objList, uint32_t index);


#endif //SPARROW_OBJ_LISH_H
