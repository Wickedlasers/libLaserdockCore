//
// Created by Darren Otgaar on 2018/11/13.
//

#ifndef LASERDOCKCORE_LDVECTOR_H
#define LASERDOCKCORE_LDVECTOR_H

#include <array>
#include <cassert>

// A generic, n-tuple vector used for doing linear algebra with ldMatrix and ldVector.

namespace ldRazer
{

    template<typename T, size_t N>
    struct ldVector
    {
        using type = T;

        enum
        {
            size = uint32_t(N),
            byteSize = sizeof(type) * uint32_t(N)
        };

        ldVector() = default;

        explicit ldVector(const T &v)
        {
            for (size_t i = 0; i != N; ++i) arr[i] = v;
        }

        explicit ldVector(std::initializer_list<T> lst)
        {
            if (lst.size() == 1) for (size_t i = 0; i != N; ++i) arr[i] = *(lst.begin());
            else if (lst.size() == N) for (size_t i = 0; i != N; ++i) arr[i] = *(lst.begin() + i);
            else
                assert(false && "Initializer list must match size of vector");
        }

        ldVector(const ldVector &rhs)
        {
            for (size_t i = 0; i != N; ++i) arr[i] = rhs.arr[i];
        }

        const T &operator[](size_t i) const
        {
            return arr[i];
        }

        T &operator[](size_t i)
        {
            return arr[i];
        }

        ldVector &operator=(const ldVector &rhs)
        {
            if (this != &rhs) {
                for (size_t i = 0; i != N; ++i) {
                    arr[i] = rhs[i];
                }
            }
            return *this;
        }

        ldVector operator-() const
        {
            ldVector result;
            for (size_t i = 0; i != N; ++i) {
                result[i] = -arr[i];
            }
            return result;
        }

        ldVector &operator+=(const ldVector &v)
        {
            for (size_t i = 0; i != N; ++i) {
                arr[i] += v[i];
            }
            return *this;
        }

        ldVector &operator-=(const ldVector &v)
        {
            for (size_t i = 0; i != N; ++i) {
                arr[i] -= v[i];
            }
            return *this;
        }

        template<typename S>
        ldVector &operator*=(S s)
        {
            for (size_t i = 0; i != N; ++i) {
                arr[i] *= s;
            }
            return *this;
        }

        template<typename S>
        ldVector &operator/=(S s)
        {
            for (size_t i = 0; i != N; ++i) {
                arr[i] /= s;
            }
            return *this;
        }

        ldVector &operator*=(const ldVector &v)
        {
            for (size_t i = 0; i != N; ++i) {
                arr[i] *= v[i];
            }
            return *this;
        }

        std::array<type, N> arr;
    };

    template<typename T, size_t N>
    ldVector<T, N> operator+(const ldVector<T, N> &A, const ldVector<T, N> &B)
    {
        ldVector<T, N> result{T(0)};
        for (size_t i = 0; i != N; ++i) {
            result[i] = A[i] + B[i];
        }
        return result;
    }

    template<typename T, size_t N>
    ldVector<T, N> operator-(const ldVector<T, N> &A, const ldVector<T, N> &B)
    {
        ldVector<T, N> result{T(0)};
        for (size_t i = 0; i != N; ++i) {
            result[i] = A[i] - B[i];
        }
        return result;
    }

    template<typename T, size_t N>
    ldVector<T, N> operator*(const ldVector<T, N> &A, const ldVector<T, N> &B)
    {
        ldVector<T, N> result{T(0)};
        for (size_t i = 0; i != N; ++i) {
            result[i] = A[i] * B[i];
        }
        return result;
    }

    template<typename T, size_t N>
    ldVector<T, N> operator/(const ldVector<T, N> &A, const ldVector<T, N> &B)
    {
        ldVector<T, N> result{T(0)};
        for (size_t i = 0; i != N; ++i) {
            result[i] = A[i] / B[i];
        }
        return result;
    }

    template<typename S, typename T, size_t N>
    ldVector<T, N> operator*(const ldVector<T, N> &v, S s)
    {
        ldVector<T, N> result;
        for (size_t i = 0; i != N; ++i) {
            result[i] = v[i] * s;
        }

        return result;
    }

    template<typename S, typename T, size_t N>
    ldVector<T, N> operator*(S s, const ldVector<T, N> &v)
    {
        return operator*(v, s);
    }

}

#endif //LASERDOCKCORE_LDVECTOR_H
