#include "NativeArray2D.h"
#include <assert.h>
#include <stdlib.h>
#include <cstring>
#include "MathTypes.h"
#include "MemoryLeakDetection.h"

template <typename T, typename LIFETIME_TAG>
void NativeArray2D<T, LIFETIME_TAG>::_malloc(size_t2 _sz)
{
        elementSizeWH = _sz;
        elementSize = elementSizeWH[0] * elementSizeWH[1];
        data       = reinterpret_cast<T*>(::malloc(elementSize *sizeof(T)));
}

template <typename T, typename LIFETIME_TAG>
void NativeArray2D<T, LIFETIME_TAG>::clear(unsigned value)
{
        ::memset(data, value, elementSize);
}

template <typename T, typename LIFETIME_TAG>
void NativeArray2D<T, LIFETIME_TAG>::_free()
{
        ::free(data);
        data = 0;
}

template <typename T, typename LIFETIME_TAG>
typename NativeArray2D<T, LIFETIME_TAG>::iterator NativeArray2D<T, LIFETIME_TAG>::begin()
{
        return iterator(*this, 0);
}
template <typename T, typename LIFETIME_TAG>
typename NativeArray2D<T, LIFETIME_TAG>::iterator NativeArray2D<T, LIFETIME_TAG>::end()
{
        return iterator(*this, elementSize / sizeof(T));
}

template <typename T, typename LIFETIME_TAG>
NativeArray2D<T, LIFETIME_TAG>::iterator::iterator(NativeArray2D<T, LIFETIME_TAG>& _NativeArray2D, size_t index) :
    _NativeArray2D(_NativeArray2D),
    index(index)
{}

template <typename T, typename LIFETIME_TAG>
bool NativeArray2D<T, LIFETIME_TAG>::iterator::operator!=(NativeArray2D<T, LIFETIME_TAG>::iterator const& other) const
{
        return other.index != this->index;
}

template <typename T, typename LIFETIME_TAG>
T& NativeArray2D<T, LIFETIME_TAG>::iterator::operator*() const
{
        return _NativeArray2D[index];
}

template <typename T, typename LIFETIME_TAG>
typename NativeArray2D<T, LIFETIME_TAG>::iterator const& NativeArray2D<T, LIFETIME_TAG>::iterator::operator++()
{
        index++;
        return *this;
}

TStructDeclContainers_0(NativeArray2D);
TStructDeclContainers_1(NativeArray2D);
