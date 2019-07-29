#pragma once
inline namespace NativeMemory
{
        struct Block
        {
                void*    _p;
                unsigned _sz;
        };

        constexpr size_t RoundUpIntegerDivision(size_t dividend, size_t divisor)
        {
                return (dividend + divisor - 1) / divisor;
        }

#define GET_BIT(VAL, N) ((VAL >> N) & 1)
        // Get the index of the most significant set bit
        constexpr size_t ConstBitScanReverse(size_t value)
        {
                // if the value is 0 do not scan. Return 0 even though the return value is technically udnefined.
                if (!value)
                        return 0;
                size_t bit = sizeof(size_t) * 8 - 1;
                while (GET_BIT(value, bit) == 0)
                {
                        bit = bit - 1;
                }
                return bit;
        }
        constexpr size_t log2Int(size_t value)
        {
                return ConstBitScanReverse(value);
        }
        constexpr size_t MinimumBitsRequiredToStoreValue(size_t value)
        {
                if (value)
                        return ConstBitScanReverse(value) + 1;
                else
                        return 0;
        }
        constexpr size_t MinimumBitsRequiredToIndexNumberOfElements(size_t N)
        {
                return ConstBitScanReverse(N);
        }
}; // namespace NativeMemory
