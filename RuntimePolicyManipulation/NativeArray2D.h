#pragma once
#include "ContainerDefines.h"
struct local_lifetime
{
        static void destruct(void* data)
        {
		}
};
struct persistent_lifetime
{
        static void destruct(void* data)
        {}
};
template <typename T, typename LIFETIME_TAG = persistent_lifetime>
struct NativeArray2D
{
    public:
        using lifetime = LIFETIME_TAG;
        struct iterator;
        T*      data;
        size_t  elementSize;
        size_t2 elementSizeWH;
        // T&        operator[](unsigned index);
        void     _malloc(size_t2);
        void     clear(unsigned value);
        void     _free();
        iterator begin();
        iterator end();

        // 2D subscript operator Proxy
        struct SubscriptProxy
        {
                NativeArray2D<T>& const _NativeArray2D;
                unsigned                _row;
                inline SubscriptProxy(NativeArray2D<T>& const _NativeArray2D, unsigned _row) :
                    _NativeArray2D(_NativeArray2D),
                    _row(_row)
                {}
                inline operator T&()
                {
                        return _NativeArray2D.data[_row];
                }
                inline T& operator[](unsigned _column)
                {
                        assert((_column < _NativeArray2D.elementSizeWH[0]) && (_row < _NativeArray2D.elementSizeWH[1]));
                        return _NativeArray2D.data[_row * _NativeArray2D.elementSizeWH[0] + _column];
                }
        };
        inline SubscriptProxy operator[](unsigned _row)
        {
                assert(_row < elementSize);
                return SubscriptProxy(*this, _row);
        }
};

template <typename T, typename LIFETIME_TAG>
struct NativeArray2D<T, LIFETIME_TAG>::iterator
{
    private:
        NativeArray2D<T, LIFETIME_TAG>& _NativeArray2D;
        size_t            index;

    public:
        NativeArray2D<T, LIFETIME_TAG>::iterator(NativeArray2D<T, LIFETIME_TAG>&, size_t index);
        bool                              operator!=(NativeArray2D<T, LIFETIME_TAG>::iterator const& other) const;
        T&                                operator*() const;
        NativeArray2D<T, LIFETIME_TAG>::iterator const& operator++();
};
