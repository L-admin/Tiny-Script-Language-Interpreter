//
// Created by lipan on 2019/4/23.
//

#ifndef SPARROW_UTILS_H
#define SPARROW_UTILS_H


#define DEFAULT_BUFFER_SIZE 512

typedef enum {
    ERROR_IO,
    ERROR_MEM,
} ErrorType;

void errorReport(ErrorType errorType, const char* fmt, ...);

#define IO_ERROR(...)\
    errorReport(ERROR_IO, __VA_ARGS__)

#define MEM_ERROR(...)\
    errorReport(ERROR_MEM, __VA_ARGS__)


#endif //SPARROW_UTILS_H
