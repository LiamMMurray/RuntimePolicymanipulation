#pragma once
#include "MathTypes.h"
#define WRAPTAG(TAG, T)                            \
        struct TAG                                 \
        {                                          \
                inline TAG(T value) : value(value) \
                {}                                 \
                                                   \
                inline operator T()                \
                {                                  \
                        return value;              \
                }                                  \
                T value;                           \
        };
T2_Tag(size_t, elementsize);
T2_Tag(size_t, memorysize);
WRAPTAG(elementsize, size_t);
WRAPTAG(memorysize, size_t);

 