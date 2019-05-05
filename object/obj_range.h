//
// Created by lipan on 2019/5/1.
//

#ifndef SPARROW_OBJ_RANGE_H
#define SPARROW_OBJ_RANGE_H

#include "header_obj.h"

// range 对象
typedef struct
{
    ObjHeader objHeader;
    int from;               // 范围的起始
    int to;                 // 范围的结束
} ObjRange;

ObjRange *newObjRange(VM *vm, int from, int to);

#endif //SPARROW_OBJ_RANGE_H
