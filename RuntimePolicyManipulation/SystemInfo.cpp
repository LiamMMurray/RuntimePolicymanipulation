#include "SystemInfo.h"
#include "ErrorMessages.h"

MemoryInfo GetMemoryInfo()
{
        constexpr bool CHECK_INDIVIDUAL_CACHE_DESCRIPTOR_COUNTS = 0;

        SYSTEM_LOGICAL_PROCESSOR_INFORMATION* slpinfo = 0;
        DWORD                                 length  = 0;
        GetLogicalProcessorInformation(slpinfo, &length);

        slpinfo = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)malloc(length);
        if (!slpinfo)
                DisplayDbgErrorW();

        BOOL  result = GetLogicalProcessorInformation(slpinfo, &length);
        DWORD Error  = 0;
        if (!result)
        {
                Error = GetLastError();
                return MemoryInfo();
        }

        unsigned count = length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);

        if (CHECK_INDIVIDUAL_CACHE_DESCRIPTOR_COUNTS)
        {
                unsigned cache_1_desc_count = 0;
                unsigned cache_2_desc_count = 0;
                unsigned cache_3_desc_count = 0;
                unsigned cache_x_desc_count = 0;
                for (unsigned int i = 0; i < count; i++)
                {
                        if (slpinfo[i].Relationship == RelationCache)
                        {
                                if (slpinfo[i].Cache.Level == 1)
                                        cache_1_desc_count++;
                                else if (slpinfo[i].Cache.Level == 2)
                                        cache_2_desc_count++;
                                else if (slpinfo[i].Cache.Level == 3)
                                        cache_3_desc_count++;
                                else
                                        cache_x_desc_count++;
                        }
                }
        }

        MemoryInfo _MemoryInfo = MemoryInfo{};
        for (unsigned int i = 0; i < count; i++)
        {
                if (slpinfo[i].Relationship == RelationCache)
                {
                        if (slpinfo[i].Cache.Level == 1)
                                _MemoryInfo.L1 = slpinfo[i].Cache;
                        else if (slpinfo[i].Cache.Level == 2)
                                _MemoryInfo.L2 = slpinfo[i].Cache;
                        else if (slpinfo[i].Cache.Level == 3)
                                _MemoryInfo.L3 = slpinfo[i].Cache;
                }
        }

        SYSTEM_INFO sinfo;
        GetNativeSystemInfo(&sinfo);
        _MemoryInfo.PageSize = sinfo.dwPageSize;
        _MemoryInfo.allocationGranularity = sinfo.dwAllocationGranularity;
        _MemoryInfo.LargePageSize = GetLargePageMinimum();
        free(slpinfo);

        return _MemoryInfo;
}

DWORD GetNumLogicalCores()
{
        SYSTEM_INFO sinfo;
        GetNativeSystemInfo(&sinfo);
        return sinfo.dwNumberOfProcessors;
}