//
// Created by lipan on 2019/4/23.
//
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include "compiler.h"
#include "class.h"
#include "obj_map.h"
#include "meta_obj.h"
#include "core.h"
#include "utils.h"

//返回值类型是Value类型,且是放在args[0], args是Value数组
//RET_VALUE的参数就是Value类型,无须转换直接赋值.
//它是后面"RET_其它类型"的基础
#define RET_VALUE(value)\
   do {\
      args[0] = value;\
      return true;\
   } while(0);


char *rootDir = NULL;

// 读取源码文件
char *readFile(const char *path)
{
    FILE *file = fopen(path, "rb"); // TODO: r 和 rb 区别
    if (file == NULL)
    {
        IO_ERROR("Could`t open file \"%s\".\n", path);
    }

    struct stat fileStat;
    stat(path, &fileStat);
    size_t fileSize = (size_t) fileStat.st_size;

    char *fileContent = (char *) malloc(fileSize + 1);
    if (fileContent == NULL)
    {
        MEM_ERROR("Could`t allocate memory for reading file \"%s\".\n", path);
    }

    size_t numRead = fread(fileContent, sizeof(char), fileSize, file);
    if (numRead < fileSize)
    {
        IO_ERROR("Could`t read file \"%s\".\n", path);
    }
    fileContent[fileSize] = '\0';

    fclose(file);

    return fileContent;
}

VMResult executeModule(VM *vm, Value moduleName, const char *moduleCode)
{
    return VM_RESULT_ERROR;
}


// !object: object取反, 结果返回false
static bool primObjectNot(VM* vm UNUSED, Value* args)
{
    args[0] = ValueTypeToValue(VT_FALSE);
    return true;
}

// args[0] == args[1]: 返回object 是否相等
static bool primObjectEqual(VM* vm UNUSED, Value* args)
{
    Value boolValue = BoolToValue(valueIsEqual(args[0], args[1]));
    args[0] = boolValue;
    return true;
}

// args[0] != args[1]: 返回object是否不等
static bool primObjectNotEqual(VM* vm UNUSED, Value* args)
{
    Value boolValue = BoolToValue(!valueIsEqual(args[0], args[1]));
    args[0] = boolValue;
    return true;
}

// args[0] is args[1]:类args[0]是否为类args[1]的子类
static bool primObjectIs(VM* vm, Value* args)
{
    // args[1] 必须是class
    if (!IsValueClass(args[1]))
    {
        RUN_ERROR("argument must be class!");
    }

    Class* thisClass = getClassOfObj(vm, args[0]);
    Class* baseClass = (Class*)(args[1].objHeader);

    // 有可能是多级继承, 因此自上而下便利基类链表
    while(baseClass != NULL)
    {
        if (thisClass == baseClass)
        {
            args[0] = BoolToValue(VT_TRUE);
            return true;
        }
        baseClass = baseClass->superClass;
    }

    // 若未找到基类,说明不具备is_a关系
    args[0] = BoolToValue(VT_FALSE);
    return true;
}

// args[0].tostring: 返回args[0]所属class的名字
static bool primObjectToString(VM* vm UNUSED, Value* args)
{
    Class* class = args[0].objHeader->class;
    Value nameValue = ObjToValue((ObjHeader*)(class->name));
    args[0] = nameValue;
    return true;
}

// args[0].type: 返回对象args[0]的类
static bool primObjectType(VM* vm, Value* args)
{
    Class* class = getClassOfObj(vm, args[0]);
    args[0] = ObjToValue((ObjHeader*)class);
    return true;
}

// args[0].name 返回类名
static bool primClassName(VM* vm UNUSED, Value* args)
{
    ObjString* namePtr = (ValueToClass(args[0])->name);
    args[0] = ObjToValue((ObjHeader*)namePtr);  // 再将obj转换为Value后做为返回值
    return true;
}

// args[0].supertype: 返回args[0]的基类
static bool primClassSupertype(VM* vm UNUSED, Value* args)
{
    Class* class = ValueToClass(args[0]);
    if (class->superClass != NULL)
    {
        args[0] = ObjToValue((ObjHeader*)(class->superClass));
        return true;
    }
    args[0] = ValueTypeToValue(VT_NULL);
    return true;
}

// args[0].toString: 返回类名
static bool primClassToString(VM* vm UNUSED, Value* args)
{
    ObjString* namePtr = (ValueToClass(args[0])->name);
    args[0] = ObjToValue((ObjHeader*)namePtr);  // 再将obj转换为Value后做为返回值
    return true;
}

// args[0].same(args[1], args[2]): 返回args[1]和args[2]是否相等
static bool primObjectmetaSame(VM* vm UNUSED, Value* args)
{
    Value boolValue = BoolToValue(valueIsEqual(args[1], args[2]));
    args[0] = boolValue;
    return true;
}

// table中查找符号symbol, 找到后返回索引,否则返回-1.
int getIndexFromSymbolTable(SymbolTable *table, const char *symbol, uint32_t length)
{
    ASSERT(length != 0, "length of symbol is 0!");

    for (uint32_t idx = 0; idx < table->count; idx++)
    {
        if (length == table->datas[idx].length &&
            memcmp(table->datas[idx].str, symbol, length) == 0)
        {
            return idx;
        }
    }

    return -1;
}

// 往table中添加符号symbol,返回其索引
int addSymbol(VM *vm, SymbolTable *table, const char *symbol, uint32_t length)
{
    ASSERT(length != 0, "length of symbol is 0!");

    String string;
    string.str = ALLOCATE_ARRAY(vm, char, length + 1);

    memcpy(string.str, symbol, length);

    string.str[length] = '\0';
    string.length = length;
    StringBufferAdd(vm, table, string);

    return table->count - 1;
}

// 定义类
static Class* defineClass(VM* vm, ObjModule* objModule, const char* name)
{
    // 1.先创建类
    Class* class = newRawClass(vm, name, 0);

    // 2. 把类作为普通变量在模块中定义
    defineModuleVar(vm, objModule, name, (uint32_t)strlen(name), ObjToValue((ObjHeader*)class));

    return class;
}

void bindMethod(VM* vm, Class* class, uint32_t index, Method method)
{
    if (index > class->methods.count)
    {
        Method emptyPad = {MT_NONE, {0}};
        MethodBufferFillWrite(vm, &class->methods, emptyPad, index - class->methods.count + 1);
    }
    class->methods.datas[index] = method;
}

void bindSuperClass(VM* vm, Class* subClass, Class* superClass)
{
    subClass->superClass = superClass;
    // 继承基类属性
    subClass->fieldNum += superClass->fieldNum;

    // 继承基类方法
    for (uint32_t idx = 0; idx < superClass->methods.count; idx++)
    {
        bindMethod(vm, subClass, idx, superClass->methods.datas[idx]);
    }
}

void buildCore(VM *vm)
{
    // 创建核心模块, 将其加入到vm->allModules中
    ObjModule *coreModule = newObjModule(vm, NULL);
    mapSet(vm, vm->allModules, ValueTypeToValue(VT_NULL), ObjToValue((ObjHeader*)coreModule));
}
