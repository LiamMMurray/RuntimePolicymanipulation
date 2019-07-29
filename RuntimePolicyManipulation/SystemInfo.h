#pragma once
#define WIN_32_LEAN_AND_MEAN 
#include <Windows.h>

struct MemoryInfo
{
        CACHE_DESCRIPTOR L1;
        CACHE_DESCRIPTOR L2;
        CACHE_DESCRIPTOR L3;
        size_t           allocationGranularity;
        size_t            PageSize;
        size_t           LargePageSize;
};

MemoryInfo GetMemoryInfo();

DWORD GetNumLogicalCores();