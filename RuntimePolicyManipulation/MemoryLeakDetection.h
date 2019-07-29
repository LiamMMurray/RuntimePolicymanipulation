#pragma once
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>

#if defined _DEBUG
#define ENABLE_LEAK_DETECTION() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
#else
#define ENABLE_LEAK_DETECTION()
#endif

#ifdef _DEBUG
#define new new (_CLIENT_BLOCK, __FILE__, __LINE__)
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#ifdef _DEBUG
#define malloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define DBG_MALLOC malloc
#endif
#include <iostream>
class DumpMemoryLeaksSentinel
{
    public:
        DumpMemoryLeaksSentinel()
        {
                std::cout <<"constructing";
		}
        ~DumpMemoryLeaksSentinel()
        {
                std::cout << "destructing";

                _CrtDumpMemoryLeaks();
        }
};

static DumpMemoryLeaksSentinel _DumpMemoryLeaksSentinel;
