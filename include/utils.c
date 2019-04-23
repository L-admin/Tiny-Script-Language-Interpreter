//
// Created by lipan on 2019/4/23.
//

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "utils.h"
#include "common.h"


// 通用报错函数
void errorReport(ErrorType errorType, const char* fmt, ...)
{
    char buffer[DEFAULT_BUFFER_SIZE] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, DEFAULT_BUFFER_SIZE, fmt, ap);
    va_end(ap);

    switch (errorType)
    {
        case ERROR_IO:
        case ERROR_MEM:
            fprintf(stderr, "%s:%d In function %s():%s\n", __FILE__, __LINE__, __func__, buffer);
            break;
        default:
            NOT_REACHED();
    }

    exit(1);
}
