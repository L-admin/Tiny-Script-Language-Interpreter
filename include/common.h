//
// Created by lipan on 2019/4/23.
//

#ifndef SPARROW_COMMON_H
#define SPARROW_COMMON_H

typedef struct vm VM;
typedef struct parser Parser;

#define bool char
#define true   1
#define false  0

#define UNUSED __attribute__ ((unused))

#ifdef DEBUG
    #define ASSERT(condition, errMsg) \
        do {\
            if (!(condition)) {\
                fprintf(stderr, "ASSERT failed! %s:%d In funccxdtion %s(): %s\n", \
                        __FILE__, __LINE__, __func__, errMsg); \
                abort();\
            }\
        } while (0);
#else
    #define ASSERT(condition, errMsg) ((void)0)
#endif

#define NOT_REACHED()\
    do {\
        fprintf(stderr, "NOT_REACHED: %s:%d In function %s()\n",\
                __FILE__, __LINE__, __func__);\
        while (1)\
            ;\
    } while (0);


#endif //SPARROW_COMMON_H
