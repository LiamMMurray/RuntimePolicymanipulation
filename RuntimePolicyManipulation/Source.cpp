#include "MemoryLeakDetection.h"

#include <benchmark/benchmark.h>
#include <windows.h>
#include "ErrorMessages.h"
#include "MemorySizes.h"
#include "NativeArray.h"
#include "NativeMemoryDefines.h"
#include "PhysicalPages.h"
#include "SystemInfo.h"


#ifdef BENCHMARK
static void _malloc(benchmark::State& state)
{
        void* mem;

        for (auto _ : state)
        {
                mem = 0;
                mem = malloc(state.range(0));
                assert(mem);
                free(mem);
        }
        state.iterations();
}
static void _valloc(benchmark::State& state)
{
        size_t page_size = GetMemoryInfo().PageSize;
        for (auto _ : state)
        {
                void* mem;
                bool  success;
                mem             = 0;
                size_t mem_size = RoundUpIntegerDivision(state.range(), page_size);
                mem             = VirtualAlloc(0, state.range(0), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
                assert(mem);
                success = VirtualFree(mem, 0, MEM_RELEASE);
                assert(success);
                // state.counters["NumPages"] = Pages_Required(state.range(0), page_size);
                // state.counters["MemSize"]  = mem_size;
                // state.counters["PageSize"] = page_size;
        }
}
static void _palloc(benchmark::State& state)
{
        size_t page_size = GetMemoryInfo().PageSize;
        for (auto _ : state)
        {
                bool success;

                ULONG_PTR  NumberOfPages = RoundUpIntegerDivision(state.range(), page_size);
                ULONG_PTR* page_array    = new ULONG_PTR[NumberOfPages];
                success                  = AllocateUserPhysicalPages(GetCurrentProcess(), &NumberOfPages, page_array);
                assert(success);
                success = FreeUserPhysicalPages(GetCurrentProcess(), &NumberOfPages, page_array);
                assert(success);
                delete[] page_array;
                // state.counters["NumPages"] = NumberOfPages;
                // state.counters["MemSize"]  = NumberOfPages * page_size;
                // state.counters["PageSize"] = page_size;
        }
}
static void _palloc_memset(benchmark::State& state)
{
        state.counters["Success"]         = 1;
        size_t     page_size              = GetMemoryInfo().PageSize;
        ULONG_PTR  NumberOfPagesRequested = RoundUpIntegerDivision(state.range(), page_size);
        ULONG_PTR  NumberOfPagesRecieved  = NumberOfPagesRequested;
        ULONG_PTR* page_array             = new ULONG_PTR[NumberOfPagesRequested];

// SUCCESS defaults to false if unspecified
#define cleanup(SUCCESS)                                                                             \
        {                                                                                            \
                bool success = {SUCCESS};                                                            \
                if (!success)                                                                        \
                {                                                                                    \
                        DisplayErrorW();                                                             \
                }                                                                                    \
                if (!FreeUserPhysicalPages(GetCurrentProcess(), &NumberOfPagesRecieved, page_array)) \
                        DisplayErrorW();                                                             \
                delete[] page_array;                                                                 \
                assert(success);                                                                     \
        }

        if (!AllocateUserPhysicalPages(GetCurrentProcess(), &NumberOfPagesRecieved, page_array))
        {
                cleanup();
                return;
        }
        if (NumberOfPagesRequested != NumberOfPagesRecieved)
        {
                cleanup();
                return;
        }
        void* lpMemReserved = VirtualAlloc(0, NumberOfPagesRecieved * page_size, MEM_RESERVE | MEM_PHYSICAL, PAGE_READWRITE);
        if (!lpMemReserved)
        {
                cleanup();
                return;
        }

        // Map
        if (!MapUserPhysicalPages(lpMemReserved, NumberOfPagesRecieved, page_array))
        {
                cleanup();
                return;
        }

        for (auto _ : state) {}

        // Unmap
        if (!MapUserPhysicalPages(lpMemReserved, NumberOfPagesRecieved, 0))
        {
                cleanup();
                return;
        }

        cleanup(1);

#undef cleanup()
}
static void _malloc_memset(benchmark::State& state)
{
        void*     lpMemReserved         = 0;
        size_t    page_size             = GetMemoryInfo().PageSize;
        ULONG_PTR NumberOfPagesRecieved = RoundUpIntegerDivision(state.range(), page_size);
        lpMemReserved                   = malloc(NumberOfPagesRecieved);
        assert(lpMemReserved);
        for (auto _ : state) {}
        free(lpMemReserved);
}
static void _valloc_memset(benchmark::State& state)
{
        void*     lpMemReserved         = 0;
        size_t    page_size             = GetMemoryInfo().PageSize;
        ULONG_PTR NumberOfPagesRecieved = RoundUpIntegerDivision(state.range(), page_size);
        lpMemReserved = VirtualAlloc(0, NumberOfPagesRecieved * page_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        assert(lpMemReserved);
        for (auto _ : state) {}
        VirtualFree(lpMemReserved, 0, MEM_RELEASE);
}
#endif
//
// BENCHMARK(_malloc_memset)->Range(8, 500 * MiB);
// BENCHMARK(_valloc_memset)->Range(8, 500 * MiB);
// BENCHMARK(_palloc_memset)->Range(8, 500 * MiB);
// BENCHMARK(_palloc)->Range(8, 500 * MiB);
// BENCHMARK(_valloc)->Range(8, 500 * MiB);
// BENCHMARK(_malloc)->Range(8, 500 * MiB);
// BENCHMARK_MAIN();
#include <tuple>
#include "NativeArray2D.h"
#include "PhysicalPageAllocator.h"

T4X4_TAG(float, Quaternion);

struct JobQueue
{};


#include <cmath>
#include "NativeMemoryDefines.h"

struct quad_int
{
        union
        {
                struct
                {
                        int32_t low;
                        int32_t high;
                };
                uint64_t quad;
        };
};

inline uint64_t rand64()
{
        quad_int _quad_int;
        _quad_int.low  = rand();
        _quad_int.high = rand();
        return _quad_int.quad;
}
inline uint32_t rand32()
{
        return rand();
}


template <typename T, typename... Ts>
void inplace_new(void* data, Ts&&... ts)
{
#pragma push_macro("new")
#undef new
        new (data) T(std::forward<Ts>(ts)...);
#pragma pop_macro("new")
}

enum MessageEnum
{
        Construct,
        Destroy,
        Modify,
        Deallocate,
        Allocate
};

template <typename... MessageArgs>
struct MessagePacker
{
        static void pack_message(void* _dst, MessageArgs... args)
        {
#pragma push_macro("new")
#undef new
                new (_dst) std::tuple<MessageArgs...>(std::forward<MessageArgs>(args)...);
#pragma pop_macro("new")
        }
        static std::tuple<MessageArgs...>& unpack_message(void* _src)
        {
                return *static_cast<std::tuple<MessageArgs...>*>(_src);
        }
        static constexpr size_t sizeof_message()
        {
                return sizeof(std::tuple<MessageArgs...>);
        }
};

template <typename PrefixType>
struct MessagePrefixer
{
        static std::tuple<PrefixType, void*> UnpackMessagePrefix(void* _src)
        {
                PrefixType* _srcT         = static_cast<PrefixType*>(_src);
                PrefixType  _prefix_value = *_srcT;
                _srcT++;
                return std::tuple<PrefixType, void*>{_prefix_value, _srcT};
        }
        static void* PackMessagePrefix(void* _dst, PrefixType prefixValue)
        {
                PrefixType* _dstT = static_cast<PrefixType*>(_dst);
                *_dstT            = prefixValue;
                _dstT++;
                return static_cast<void*>(_dstT);
				__builtin_prefet
        }
};


using ComponentAllocator            = PhysicalPageMapAllocator<64, KiB * 64>;
static void*      messageQueue      = (char*)malloc(KiB);
NativeArray<char> messageQueueStack = NativeArray<char>(messageQueue, KiB);

template <unsigned alignment>
struct Stack
{
        void*    src;
        unsigned head;
		static const unsigned alignment()
		{

		}
};

template <typename C>
struct Component
{
        using Allocator              = ComponentAllocator;
        using ConstructMessagePacker = MessagePacker<C>;
        using DestroyMessagePacker   = MessagePacker<C*>;
        using MessagePrefixer        = MessagePrefixer<MessageEnum>;
        static void defferedConstruct(C newData)
        {
                messageQueue = MessagePrefixer::PackMessagePrefix(messageQueue, MessageEnum::Construct);
                ConstructMessagePacker::pack_message(messageQueue, newData);
        }
        static void defferedDestroy(C* location)
        {
                messageQueue = MessagePrefixer::PackMessagePrefix(messageQueue, MessageEnum::Destroy);
                DestroyMessagePacker::pack_message(messageQueue, location);
        }
        static void inplaceCopyConstruct(void* _dst, void* _src)
        {
                *reinterpret_cast<C*>(_dst) = *reinterpret_cast<C*>(_src);
        }
};

struct Position : public float3, Component<Position>
{};

struct Rotation : public float3, Component<Position>
{};

int main()
{
        using MessagePrefixer = MessagePrefixer<MessageEnum>;


        Position::defferedConstruct(Position{7, 13, 69});
        Position::defferedConstruct(Position{5, 21, 18});
        Position::defferedDestroy((Position*)0xdeadbeef);
        Position::defferedConstruct(Position{1996, 14, 6});

        auto [messageType, memoryPos] = MessagePrefixer::UnpackMessagePrefix(messageQueue);

        auto [position] = Position::ConstructMessagePacker::unpack_message(memoryPos);

        auto [position2] = Position::ConstructMessagePacker::unpack_message(memoryPos);


        memset(messageQueue, 0, KiB);


        free(messageQueue);
}