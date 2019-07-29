#pragma once
#include "NativeArray.h"
#include "NativeArray2D.h"
#include "NativeMemoryDefines.h"
#include "PhysicalPages.h"
#include "SystemInfo.h"
inline namespace NativeMemory
{
        // template <unsigned MapBits, unsigned OffsetBits, typename UnionType = size_t, bool Is_Bitfield = true>
        // struct MapHandleImpl
        //{
        //        static_assert(sizeof(UnionType) * 8 >= OffsetBits + MapBits, "More bits than available have been specified");

        //        constexpr UnionType   map_max()
        //        {
        //                return ~static_cast<UnionType>(0) >> static_cast<UnionType>(sizeof(UnionType) * 8 - MapBits);
        //        }
        //        constexpr UnionType offset_max()
        //        {
        //                return ~static_cast<UnionType>(0) >> static_cast<UnionType>(sizeof(UnionType) * 8 - OffsetBits);
        //        }

        //        union
        //        {
        //                struct
        //                {
        //                        UnionType map : MapBits;
        //                        UnionType offset : OffsetBits;
        //                };
        //                UnionType _union;
        //        };
        //};
        constexpr size_t NearestPrimitiveAlignment(size_t _byte_sz)
        {
                if (_byte_sz)
                {
                        // get the integer logbase2 of the value, this floors the value to the closest exponent of 2 below the
                        // current value, causing it to increment in log2 values e.g. { 1 2 3 4 5 6 7 8 } -> { 1 2 2 4 4 4 4 8 }
                        size_t tmp = 1ULL << log2Int(_byte_sz);
                        // if the byte size isn't a perfect fit  for the byte size we are returning, multiply by 2(add it by
                        // itself) to get the next highest iteration of 2^N
                        return tmp += tmp * (_byte_sz % tmp != 0ULL);
                }
                else
                        return 0;
        }

        template <unsigned _Sz>
        struct encapsulating_primitive
        {
                using type = void;
        };
        template <>
        struct encapsulating_primitive<1>
        {
                using type = uint8_t;
        };
        template <>
        struct encapsulating_primitive<2>
        {
                using type = uint16_t;
        };
        template <>
        struct encapsulating_primitive<4>
        {
                using type = uint32_t;
        };
        template <>
        struct encapsulating_primitive<8>
        {
                using type = uint64_t;
        };

        template <unsigned MapBits, unsigned OffsetBits, typename UnionType>
        struct is_primitive_union_constructable
        {
                static constexpr bool value = NearestPrimitiveAlignment(RoundUpIntegerDivision(MapBits, 8)) +
                                                  NearestPrimitiveAlignment(RoundUpIntegerDivision(OffsetBits, 8)) <=
                                              sizeof(UnionType);
        };
        template <unsigned MapBits, unsigned OffsetBits, typename UnionType, bool IsPrimitiveUnion = true>
        struct MapHandleImpl
        {
                constexpr UnionType map_max()
                {
                        return ~static_cast<UnionType>(0) >> static_cast<UnionType>(sizeof(UnionType) * 8 - MapBits);
                }
                constexpr UnionType offset_max()
                {
                        return ~static_cast<UnionType>(0) >> static_cast<UnionType>(sizeof(UnionType) * 8 - OffsetBits);
                }

                using MapType =
                    typename encapsulating_primitive<NearestPrimitiveAlignment(RoundUpIntegerDivision(MapBits, 8))>::type;
                using OffsetType =
                    typename encapsulating_primitive<NearestPrimitiveAlignment(RoundUpIntegerDivision(OffsetBits, 8))>::type;
                union
                {
                        struct
                        {
                                typename encapsulating_primitive<NearestPrimitiveAlignment(
                                    RoundUpIntegerDivision(MapBits, 8))>::type    map;
                                typename encapsulating_primitive<NearestPrimitiveAlignment(
                                    RoundUpIntegerDivision(OffsetBits, 8))>::type offset;
                        };
                        UnionType _union;
                };
        };
        template <unsigned MapBits, unsigned OffsetBits, typename UnionType>
        struct MapHandleImpl<MapBits, OffsetBits, UnionType, false>
        {
                static_assert(sizeof(UnionType) * 8 >= OffsetBits + MapBits, "More bits than available have been specified");

                constexpr UnionType map_max()
                {
                        return ~static_cast<UnionType>(0) >> static_cast<UnionType>(sizeof(UnionType) * 8 - MapBits);
                }
                constexpr UnionType offset_max()
                {
                        return ~static_cast<UnionType>(0) >> static_cast<UnionType>(sizeof(UnionType) * 8 - OffsetBits);
                }

                union
                {
                        struct
                        {
                                UnionType map : MapBits;
                                UnionType offset : OffsetBits;
                        };
                        UnionType _union;
                };
        };
        template <unsigned MapBits, unsigned OffsetBits, typename UnionType = size_t>
        struct MapHandle : public MapHandleImpl<MapBits,
                                                OffsetBits,
                                                UnionType,
                                                is_primitive_union_constructable<MapBits, OffsetBits, UnionType>::value>
        {};

        template <size_t BLOCK_COUNT, size_t MEMORY_SIZE>
        struct PhysicalPageMapAllocator
        {
            private:
                static constexpr size_t page_size = 4096;

                NativeArray<size_t>   _request_sizes;
                NativeArray2D<size_t> _block_page_map;
                size_t                page_count;
                size_t                block_count;
                void*                 mappableMemorySpace;


            public:
                // declares a Handle with the exact number of bits required to represent the size specified
                using Handle = MapHandle<MinimumBitsRequiredToIndexNumberOfElements(BLOCK_COUNT),
                                         MinimumBitsRequiredToIndexNumberOfElements(MEMORY_SIZE)>;

                static constexpr unsigned alignment = 0;
                static constexpr size_t   goodSize(size_t _sz)
                {
                        return RoundUpIntegerDivision(_sz, page_size);
                }
                Block allocate(size_t)
                {}
                bool expand(Block&, size_t)
                {
                        return false;
                }
                void reallocate(Block&, size_t)
                {}
                bool owns(Block)
                {
                        return false;
                }
                void deallocate(Block)
                {}
                void deallocateAll()
                {}

                void* Create(unsigned block_mem_size, unsigned block_count)
                {
                        this->page_size   = GetMemoryInfo().PageSize;
                        this->page_count  = RoundUpIntegerDivision(block_mem_size, page_size);
                        this->block_count = block_count;

                        _request_sizes._malloc(elementsize(block_count));
                        _block_page_map._malloc(elementsize2{page_count, block_count});

                        for (auto& itr : _request_sizes)
                                itr = page_count;

                        for (unsigned int row = 0; row < block_count; row++)
                        {
                                for (unsigned int column = 0; column < page_count; column++)
                                        _block_page_map[row][column] = 23;
                        }
                        mappableMemorySpace = VirtualAlloc(0, block_mem_size, MEM_RESERVE | MEM_PHYSICAL, PAGE_READWRITE);

                        for (unsigned int block = 0; block < block_count; block++)
                        {
                                if (!AllocateUserPhysicalPages(
                                        GetCurrentProcess(), &_request_sizes[block], &_block_page_map[block][0]))
                                        DisplayDbgErrorW();
                        }

                        assert(mappableMemorySpace != 0);
                        return mappableMemorySpace;
                }
                void Map(unsigned block)
                {
                        if (block >= _block_page_map.elementSizeWH[1])
                                DisplayDbgErrorW(L"Block" + std::to_wstring(block) + L"is out of bounds");
                        if (!MapUserPhysicalPages(mappableMemorySpace, page_count, &_block_page_map[block][0]))
                                DisplayDbgErrorW();
                }
                void UnMap(unsigned block)
                {
                        if (!MapUserPhysicalPages(mappableMemorySpace, page_count, 0))
                                DisplayDbgErrorW();
                }
                void Free()
                {
                        for (unsigned int block = 0; block < block_count; block++)
                        {
                                if (!FreeUserPhysicalPages(GetCurrentProcess(), &_request_sizes[0], &_block_page_map[block][0]))
                                        DisplayDbgErrorW();
                        }
                        block_count = 0;
                        page_count  = 0;
                        _request_sizes._free();
                        _block_page_map._free();
                }
        };
} // namespace NativeMemory
