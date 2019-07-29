#pragma once
#include "ContainerDefines.h"
#include "MemoryLeakDetection.h"

template <typename T>
struct NativeArray
{
    public:
        NativeArray<T>()                      = default;
        NativeArray<T>(const NativeArray<T>&) = default;
        NativeArray<T>(NativeArray<T>&&)      = default;
        NativeArray<T>(void*, size_t);
        struct iterator;
        T*       data;
        size_t   elementSize;
        T&       operator[](size_t);
        void     VirtualMap(void*, size_t);
        void     _malloc(memorysize);
        void     _malloc(elementsize);
        void     clear(unsigned);
        void     _free();
        iterator begin();
        iterator end();
};

template <typename T>
struct NativeArray<T>::iterator
{
    private:
        NativeArray<T>& _NativeArray;
        size_t          index;

    public:
        NativeArray<T>::iterator(NativeArray<T>&, size_t index);
        bool                            operator!=(NativeArray<T>::iterator const& other) const;
        T&                              operator*() const;
        NativeArray<T>::iterator const& operator++();
};
