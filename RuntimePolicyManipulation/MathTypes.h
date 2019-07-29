#pragma once
#include <assert.h>

typedef long long longlong;
#define T4(T)                                                 \
        struct T##4                                           \
        {                                                     \
                T         T##_0;                              \
                T         T##_1;                              \
                T         T##_2;                              \
                T         T##_3;                              \
                inline T& operator[](unsigned i)              \
                {                                             \
                        return reinterpret_cast<T*>(this)[i]; \
                }                                             \
        };
#define T4_Tag(T, TAG)                                          \
        struct TAG##4                                           \
        {                                                       \
                T      T##_0;                                   \
                T      T##_1;                                   \
                T      T##_2;                                   \
                T      T##_3;                                   \
                inline operator T##4()                          \
                {                                               \
                        return *reinterpret_cast<T##4 *>(this); \
                }                                               \
                inline T& operator[](unsigned i)                \
                {                                               \
                        return reinterpret_cast<T*>(this)[i];   \
                }                                               \
        };
#define T3(T)                                                 \
        struct T##3                                           \
        {                                                     \
                T         T##_0;                              \
                T         T##_1;                              \
                T         T##_2;                              \
                inline T& operator[](unsigned i)              \
                {                                             \
                        return reinterpret_cast<T*>(this)[i]; \
                }                                             \
        };
#define T3_Tag(T, TAG)                                          \
        struct TAG##3                                           \
        {                                                       \
                T      T##_0;                                   \
                T      T##_1;                                   \
                T      T##_2;                                   \
                inline operator T##3()                          \
                {                                               \
                        return *reinterpret_cast<T##3 *>(this); \
                }                                               \
                inline T& operator[](unsigned i)                \
                {                                               \
                        return reinterpret_cast<T*>(this)[i];   \
                }                                               \
        };
#define T2(T)                                                 \
        struct T##2                                           \
        {                                                     \
                T         T##_0;                              \
                T         T##_1;                              \
                inline T& operator[](unsigned i)              \
                {                                             \
                        return reinterpret_cast<T*>(this)[i]; \
                }                                             \
        };
#define T2_Tag(T, TAG)                                          \
        struct TAG##2                                           \
        {                                                       \
                T      T##_0;                                   \
                T      T##_1;                                   \
                inline operator T##2()                          \
                {                                               \
                        return *reinterpret_cast<T##2 *>(this); \
                }                                               \
                inline T& operator[](unsigned i)                \
                {                                               \
                        return reinterpret_cast<T*>(this)[i];   \
                }                                               \
        };
#define T3_P(T)                                               \
        struct T##3_padded                                    \
        {                                                     \
                T         T##_0;                              \
                T         T##_1;                              \
                T         T##_2;                              \
                T         Padding##_3;                        \
                inline T& operator[](unsigned i)              \
                {                                             \
                        return reinterpret_cast<T*>(this)[i]; \
                }                                             \
        };
#define T3_P_Tag(T, TAG)                                               \
        struct TAG##3_padded                                           \
        {                                                              \
                T      T##_0;                                          \
                T      T##_1;                                          \
                T      T##_2;                                          \
                T      Padding##_3;                                    \
                inline operator T##3()                                 \
                {                                                      \
                        return *reinterpret_cast<T##3_padded *>(this); \
                }                                                      \
                inline T& operator[](unsigned i)                       \
                {                                                      \
                        return reinterpret_cast<T*>(this)[i];          \
                }                                                      \
        };
#define T4X4(T)                                                                                   \
        struct T##4X4                                                                             \
        {                                                                                         \
                T T##00;                                                                          \
                T T##01;                                                                          \
                T T##02;                                                                          \
                T T##04;                                                                          \
                T T##10;                                                                          \
                T T##11;                                                                          \
                T T##12;                                                                          \
                T T##14;                                                                          \
                T T##20;                                                                          \
                T T##21;                                                                          \
                T T##22;                                                                          \
                T T##24;                                                                          \
                T T##30;                                                                          \
                T T##31;                                                                          \
                T T##32;                                                                          \
                T T##34;                                                                          \
                struct Proxy                                                                      \
                {                                                                                 \
                        T##4X4 & const _T##4X4;                                                   \
                        unsigned       x;                                                         \
                        inline Proxy(T##4X4 & const _T##4X4, unsigned x) : _T##4X4(_T##4X4), x(x) \
                        {}                                                                        \
                        inline operator T&()                                                      \
                        {                                                                         \
                                return reinterpret_cast<T* const>(&_T##4X4)[x];                   \
                        }                                                                         \
                        inline T& operator[](unsigned _row)                                       \
                        {                                                                         \
                                assert(_row < 4 & x < 4);                                         \
                                return reinterpret_cast<T* const>(&_T##4X4)[x * 4 + _row];        \
                        }                                                                         \
                };                                                                                \
                inline Proxy operator[](unsigned x)                                               \
                {                                                                                 \
                        assert(x < 16);                                                           \
                        return Proxy(*this, x);                                                   \
                }                                                                                 \
        };
#define T4X4_TAG(T, TAG)                                                                            \
        struct TAG##4X4                                                                             \
        {                                                                                           \
                T      T##00;                                                                       \
                T      T##01;                                                                       \
                T      T##02;                                                                       \
                T      T##04;                                                                       \
                T      T##10;                                                                       \
                T      T##11;                                                                       \
                T      T##12;                                                                       \
                T      T##14;                                                                       \
                T      T##20;                                                                       \
                T      T##21;                                                                       \
                T      T##22;                                                                       \
                T      T##24;                                                                       \
                T      T##30;                                                                       \
                T      T##31;                                                                       \
                T      T##32;                                                                       \
                T      T##34;                                                                       \
                inline operator T##4X4()                                                            \
                {                                                                                   \
                        return *reinterpret_cast<T##4X4 * const>(this);                             \
                }                                                                                   \
                struct Proxy                                                                        \
                {                                                                                   \
                        TAG##4X4 & const _T##4X4;                                                   \
                        unsigned         x;                                                         \
                        inline Proxy(TAG##4X4 & const _T##4X4, unsigned x) : _T##4X4(_T##4X4), x(x) \
                        {}                                                                          \
                        inline operator T&()                                                        \
                        {                                                                           \
                                return reinterpret_cast<T* const>(&_T##4X4)[x];                     \
                        }                                                                           \
                        inline T& operator[](unsigned _row)                                         \
                        {                                                                           \
                                assert(_row < 4 & x < 4);                                           \
                                return reinterpret_cast<T* const>(&_T##4X4)[x * 4 + _row];          \
                        }                                                                           \
                };                                                                                  \
                inline Proxy operator[](unsigned x)                                                 \
                {                                                                                   \
                        assert(x < 16);                                                             \
                        return Proxy(*this, x);                                                     \
                }                                                                                   \
        };


#define TX(T)    \
        T4(T);   \
        T3(T);   \
        T2(T);   \
        T3_P(T); \
        T4X4(T);

#define TStructDecl_0(TSTRUCT, T)             \
        template struct TSTRUCT<T>;           \
        template struct TSTRUCT<T##2>;        \
        template struct TSTRUCT<T##3>;        \
        template struct TSTRUCT<T##4>;        \
        template struct TSTRUCT<T##3_padded>; \
        template struct TSTRUCT<T##4X4>;

#define TStructDeclContainers_0(TSTRUCT)  \
        TStructDecl_0(TSTRUCT, char);    \
        TStructDecl_0(TSTRUCT, float);    \
        TStructDecl_0(TSTRUCT, int);      \
        TStructDecl_0(TSTRUCT, unsigned); \
        TStructDecl_0(TSTRUCT, double);   \
        TStructDecl_0(TSTRUCT, longlong); \
        TStructDecl_0(TSTRUCT, size_t);

#define TStructDecl_1(TSTRUCT, T) template struct TSTRUCT<TSTRUCT<T>>;
#define TStructDecl_2(TSTRUCT1, TSTRUCT2, T) template struct TSTRUCT1<TSTRUCT2<T>>;

#define TStructDeclContainers_1(TSTRUCT)  \
        TStructDecl_1(TSTRUCT, char);    \
        TStructDecl_1(TSTRUCT, float);    \
        TStructDecl_1(TSTRUCT, int);      \
        TStructDecl_1(TSTRUCT, unsigned); \
        TStructDecl_1(TSTRUCT, double);   \
        TStructDecl_1(TSTRUCT, longlong); \
        TStructDecl_1(TSTRUCT, size_t);

#define TStructDeclContainers_2(TSTRUCT1, TSTRUCT2)  \
        TStructDecl_2(TSTRUCT1, TSTRUCT2, char);     \
        TStructDecl_2(TSTRUCT1, TSTRUCT2, float);    \
        TStructDecl_2(TSTRUCT1, TSTRUCT2, int);      \
        TStructDecl_2(TSTRUCT1, TSTRUCT2, unsigned); \
        TStructDecl_2(TSTRUCT1, TSTRUCT2, double);   \
        TStructDecl_2(TSTRUCT1, TSTRUCT2, longlong); \
        TStructDecl_2(TSTRUCT1, TSTRUCT2, size_t);

//  if an assert occured here - it usually means you tried to index into the struct out of bounds
TX(char);
TX(unsigned);
TX(float)
TX(int)
TX(size_t)
TX(double)
TX(longlong)

static_assert(sizeof(char) == 1, "size unexpected");
static_assert(sizeof(float) == 4, "size unexpected");
static_assert(sizeof(unsigned) == 4, "size unexpected");
static_assert(sizeof(int) == 4, "size unexpected");
static_assert(sizeof(longlong) == 8, "size unexpected");
static_assert(sizeof(size_t) == 8, "size unexpected");
static_assert(sizeof(double) == 8, "size unexpected");