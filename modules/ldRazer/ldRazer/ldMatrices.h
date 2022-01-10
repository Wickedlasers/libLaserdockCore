//
// Created by Darren Otgaar on 2018/09/12.
//

#ifndef LASERDOCKCORE_LDMATRICES_H
#define LASERDOCKCORE_LDMATRICES_H

#include "ldVector.h"
#include "ldMatrix.h"

// Various matrices and specialisations of use in the physics and rendering engines

namespace ldRazer
{

    template<typename T> using ldVector3 = ldVector<T, 3>;
    template<typename T> using ldVector4 = ldVector<T, 4>;

    template<typename T> using ldMat2 = ldMatrix<T, 2, 2>;
    template<typename T> using ldMat3 = ldMatrix<T, 3, 3>;
    template<typename T> using ldMat4 = ldMatrix<T, 4, 4>;

    template<typename T>
    ldMat2<T> makeRotation2(T phi)
    {
        ldMat2<T> mat;
        auto cphi = std::cos(phi), sphi = std::sin(phi);
        mat(0, 0) = cphi;
        mat(0, 1) = -sphi;
        mat(1, 0) = sphi;
        mat(1, 1) = cphi;
        return mat;
    }

    template<typename T>
    ldMat2<T> makeScale2(T x, T y)
    {
        ldMat2<T> mat(T(0));
        mat(0, 0) = x;
        mat(1, 1) = y;
        return mat;
    }

    template<typename T>
    ldMat2<T> makeScale2(const ldRazerVec2<T> &S)
    {
        return makeScale2(S.x, S.y);
    }

    template<typename T>
    ldRazerVec2<T> operator*(const ldMat2<T> &M, const ldRazerVec2<T> &v)
    {
        return ldRazerVec2<T>{
                M(0, 0) * v[0] + M(0, 1) * v[1],
                M(1, 0) * v[0] + M(1, 1) * v[1]
        };
    }

    template<typename T>
    ldMat2<T> transpose(const ldMat2<T> &M)
    {
        auto result = M;
        std::swap(result(0, 1), result(1, 0));
        return result;
    }

// A transposed matrix vector transform
    template<typename T>
    ldRazerVec2<T> operator^(const ldMat2<T> &M, const ldRazerVec2<T> &v)
    {
        return ldRazerVec2<T>{
                M(0, 0) * v[0] + M(1, 0) * v[1],
                M(0, 1) * v[0] + M(1, 1) * v[1]
        };
    }

// A transposed matrix matrix transform
    template<typename T>
    ldMat2<T> operator^(const ldMat2<T> &M, const ldMat2<T> &N)
    {
        ldMat2<T> result;
        result(0, 0) = M(0, 0) * N(0, 0) + M(0, 1) * N(0, 1);
        result(1, 0) = M(1, 0) * N(0, 0) + M(1, 1) * N(0, 1);
        result(0, 1) = M(0, 0) * N(1, 0) + M(0, 1) * N(1, 1);
        result(1, 1) = M(1, 0) * N(1, 0) + M(1, 1) * N(1, 1);
        return result;
    }

    template<typename T>
    ldMat3<T> makeRotation3(T phi)
    {
        ldMat3<T> mat(T(0));
        auto cphi = std::cos(phi), sphi = std::sin(phi);
        mat(0, 0) = cphi;
        mat(0, 1) = -sphi;
        mat(1, 0) = sphi;
        mat(1, 1) = cphi;
        mat(2, 2) = T(1);
        return mat;
    }

    template<typename T>
    ldMat3<T> makeScale3(T x)
    {
        ldMat3<T> mat(T(0));
        mat(0, 0) = x;
        mat(1, 1) = x;
        mat(2, 2) = T(1);
        return mat;
    }

    template<typename T>
    ldMat3<T> makeScale3(T x, T y)
    {
        ldMat3<T> mat(T(0));
        mat(0, 0) = x;
        mat(1, 1) = y;
        mat(2, 2) = T(1);
        return mat;
    }

    template<typename T>
    ldMat3<T> makeScale3(const ldRazerVec2<T> &S)
    {
        return makeScale3(S.x, S.y);
    }

    template<typename T>
    ldMat3<T> makeTranslation3(T x, T y)
    {
        ldMat3<T> mat(T(0));
        mat(0, 0) = T(1);
        mat(1, 1) = T(1), mat(2, 2) = T(1);
        mat(0, 2) = x;
        mat(1, 2) = y;
        return mat;
    }

    template<typename T>
    ldMat3<T> makeTranslation3(const ldRazerVec2<T> &P)
    {
        return makeTranslation3(P.x, P.y);
    }

    template<typename T>
    ldRazerVec2<T> operator*(const ldMat3<T> &M, const ldRazerVec2<T> &v)
    {
        return ldRazerVec2<T>{
                M(0, 0) * v[0] + M(0, 1) * v[1],
                M(1, 0) * v[0] + M(1, 1) * v[1]
        };
    }

    template<typename T>
    ldRazerVec2<T> transform(const ldMat3<T> &M, const ldRazerVec2<T> &v)
    {
        return ldRazerVec2<T>{
                M(0, 0) * v[0] + M(0, 1) * v[1] + M(0, 2),
                M(1, 0) * v[0] + M(1, 1) * v[1] + M(1, 2)
        };
    }

    template<typename T>
    ldVector3<T> operator*(const ldMat3<T> &M, const ldVector3<T> &v)
    {
        return ldVector3<T>{
                M(0, 0) * v[0] + M(0, 1) * v[1] + M(0, 2) * v[2],
                M(1, 0) * v[0] + M(1, 1) * v[1] + M(1, 2) * v[2],
                M(2, 0) * v[0] + M(2, 1) * v[1] + M(2, 2) * v[2]
        };
    }

}

#endif //LASERDOCKCORE_LDMATRICES_H
