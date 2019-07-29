#include "NativeArray.h"
#include <assert.h>
#include <cstring>
#include "MathTypes.h"
#include "MemoryLeakDetection.h"

template <typename T>
T& NativeArray<T>::operator[](size_t index)
{
        // if data array size was specified check that its not out of bounds
        assert(!elementSize || index < elementSize);
        return data[index];
}

template <typename T>
NativeArray<T>::NativeArray(void* data, size_t sz) : data(static_cast<T*>(data)), elementSize(sz / sizeof(T))
{}

template <typename T>
void NativeArray<T>::VirtualMap(void* data, size_t _sz)
{
        this->elementSize = _sz;
        this->data        = static_cast<T*>(data);
}

template <typename T>
void NativeArray<T>::_malloc(elementsize _sz)
{
        this->elementSize = _sz;
        this->data        = static_cast<T*>(::malloc(_sz * sizeof(T)));
}

template <typename T>
void NativeArray<T>::clear(unsigned value)
{
        ::memset(data, value, elementSize);
}

template <typename T>
void NativeArray<T>::_free()
{
        ::free(data);
        data = 0;
}

template <typename T>
void NativeArray<T>::_malloc(memorysize _sz)
{
        elementSize = _sz;
        data        = reinterpret_cast<T*>(::malloc(_sz));
}
template <typename T>
typename NativeArray<T>::iterator NativeArray<T>::begin()
{
        return iterator(*this, 0);
}
template <typename T>
typename NativeArray<T>::iterator NativeArray<T>::end()
{
        return iterator(*this, elementSize);
}

template <typename T>
NativeArray<T>::iterator::iterator(NativeArray<T>& _NativeArray, size_t index) : _NativeArray(_NativeArray), index(index)
{}

template <typename T>
bool NativeArray<T>::iterator::operator!=(NativeArray<T>::iterator const& other) const
{
        return other.index != this->index;
}

template <typename T>
T& NativeArray<T>::iterator::operator*() const
{
        return _NativeArray[index];
}

template <typename T>
typename NativeArray<T>::iterator const& NativeArray<T>::iterator::operator++()
{
        index++;
        return *this;
}


TStructDeclContainers_0(NativeArray);
TStructDeclContainers_1(NativeArray);
