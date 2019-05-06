//
// Created by lipan on 2019/4/26.
//

#ifndef SPARROW_META_OBJ_H
#define SPARROW_META_OBJ_H

#include "obj_string.h"


typedef struct
{
    char *str;
    uint32_t length;
} String;

typedef struct
{
    String *datas;              // 数据缓冲区
    uint32_t count;             // 缓冲区中已使用的元素个数
    uint32_t capacity;          // 缓冲区容量
} StringBuffer;

void StringBufferInit(StringBuffer *buf);
void StringBufferFillWrite(VM *vm, StringBuffer *buf, String data, uint32_t fillCount);
void StringBufferAdd(VM *vm, StringBuffer *buf, String data);
void StringBufferClear(VM *vm, StringBuffer *buf);


#define SymbolTable StringBuffer

// 模块对象
typedef struct
{
    ObjHeader objHeader;
    SymbolTable moduleVarName;      // 模块中的模块变量名
    ValueBuffer moduleVarValue;     // 模块中的模块变量值
    ObjString *name;                // 模块中的模块变量值
} ObjModule;


// 对象实例
typedef struct
{
    ObjHeader objHeader;
    Value fields[0];                // 具体的字段名(柔性数组)
} ObjInstance;


ObjModule *newObjModule(VM *vm, const char *modName);
ObjInstance *newObjInstance(VM *vm, Class *class);


#endif //SPARROW_META_OBJ_H
