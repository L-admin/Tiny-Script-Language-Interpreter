//
// Created by lipan on 2019/4/23.
//

#ifndef SPARROW_CORE_H
#define SPARROW_CORE_H

#include "vm.h"

extern char *rootDir;   // 源码所在目录

char *readFile(const char *sourceFile);
VMResult executeModule(VM* vm, Value moduleName, const char* moduleCode);
void buildCore(VM* vm);
int getIndexFromSymbolTable(SymbolTable* table, const char* symbol, uint32_t length);
int addSymbol(VM* vm, SymbolTable* table, const char* symbol, uint32_t length);

#endif //SPARROW_CORE_H
