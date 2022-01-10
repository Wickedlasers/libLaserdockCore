//
// Created by Darren Otgaar on 2018/09/12.
//

#ifndef LASERDOCKCORE_LDMATRIX_H
#define LASERDOCKCORE_LDMATRIX_H

#include "ldRazerVec2.h"
#include "ldVector.h"

#include <cstring>

namespace ldRazer
{

    // Note, the ldMatrix struct uses the mathematical convention of specifying rows first and columns second, thus
    // operator() takes Rows, Columns to return the entry [Row, Column] of the matrix.  The matrix is Column-Major

    template<typename T, size_t Rows, size_t Cols>
    struct ldMatrix
    {
        using type = T;
        using row_t = std::array<T, Cols>;
        using col_t = std::array<T, Rows>;

        enum
        {
            rowSize = Rows,
            colSize = Cols,
            count = Rows * Cols
        };

        static size_t idx(size_t r, size_t c)
        {
            return c * rowSize + r;
        }

        ldMatrix() = default;

        explicit ldMatrix(const T *m);

        explicit ldMatrix(T init);

        explicit ldMatrix(std::initializer_list<T> lst);

        ldMatrix(const ldMatrix &m);

        ldMatrix &operator=(const ldMatrix &m);

        bool operator==(const ldMatrix &m) const;

        bool operator!=(const ldMatrix &m) const;

        bool eq(const ldMatrix &m) const;

        bool neq(const ldMatrix &m) const;

        bool is_square() const;

        T operator[](size_t idx) const;

        T &operator[](size_t idx);

        T operator()(size_t r, size_t c) const;

        T &operator()(size_t r, size_t c);

        const T *data() const;

        T *buffer();

        row_t row(size_t r) const;

        col_t col(size_t c) const;

        void row(size_t r, const row_t &v);

        void col(size_t c, const col_t &v);

        ldMatrix &operator+=(const ldMatrix &m);

        ldMatrix &operator-=(const ldMatrix &m);

        template<typename S>
        ldMatrix &operator*=(S s);

        template<typename S>
        ldMatrix &operator/=(S s);

        ldMatrix &operator*=(const ldMatrix &m);

        col_t operator*(const col_t &v) const;

        std::array<T, Rows - 1> transform(const std::array<T, Rows - 1> &v) const;

        ldMatrix &clean();

        ldMatrix &clear();

        std::array<T, count> arr;
    };

    template<typename T, size_t Rows, size_t Cols>
    ldMatrix<T, Rows, Cols>::ldMatrix(const T *m)
    {
        for (size_t r = 0; r < Rows; ++r) {
            for (size_t c = 0; c < Cols; ++c) {
                arr[idx(r, c)] = m[idx(c, r)];
            }
        }
    }

    template<typename T, size_t Rows, size_t Cols>
    ldMatrix<T, Rows, Cols>::ldMatrix(T init)
    {
        for (size_t idx = 0; idx < count; ++idx) arr[idx] = init;
    }

    template<typename T, size_t Rows, size_t Cols>
    ldMatrix<T, Rows, Cols>::ldMatrix(std::initializer_list<T> lst)
    {
        assert(count == lst.size() && "Initializer list must match size of matrix");
        for (size_t idx = 0; idx < count; ++idx) arr[idx] = *(lst.begin() + idx);
    }

    template<typename T, size_t Rows, size_t Cols>
    ldMatrix<T, Rows, Cols>::ldMatrix(const ldMatrix<T, Rows, Cols> &m)
    {
        memcpy(&arr[0], &m.arr[0], sizeof(T) * count);
    }

    template<typename T, size_t Rows, size_t Cols>
    ldMatrix<T, Rows, Cols> &ldMatrix<T, Rows, Cols>::operator=(const ldMatrix<T, Rows, Cols> &m)
    {
        if (this != &m) memcpy(&arr[0], &m.arr[0], sizeof(T) * count);
        return *this;
    }

    template<typename T, size_t Rows, size_t Cols>
    bool ldMatrix<T, Rows, Cols>::operator==(const ldMatrix<T, Rows, Cols> &m) const
    {
        if (this != &m)
            for (size_t idx = 0; idx < count; ++idx)
                if (arr[idx] != m[idx]) return false;
        return true;
    }

    template<typename T, size_t Rows, size_t Cols>
    bool ldMatrix<T, Rows, Cols>::operator!=(const ldMatrix<T, Rows, Cols> &m) const
    {
        return !operator==(m);
    }

    template<typename T, size_t Rows, size_t Cols>
    bool ldMatrix<T, Rows, Cols>::eq(const ldMatrix<T, Rows, Cols> &m) const
    {
        if (this != &m)
            for (size_t idx = 0; idx < count; ++idx)
                if (!ldRazer::eq(arr[idx], m[idx])) return false;
        return true;
    }

    template<typename T, size_t Rows, size_t Cols>
    bool ldMatrix<T, Rows, Cols>::neq(const ldMatrix<T, Rows, Cols> &m) const
    {
        return !eq(m);
    }

    template<typename T, size_t Rows, size_t Cols>
    bool ldMatrix<T, Rows, Cols>::is_square() const
    {
        return Rows == Cols;
    }

    template<typename T, size_t Rows, size_t Cols>
    T ldMatrix<T, Rows, Cols>::operator[](size_t idx) const
    {
        return arr[idx];
    }

    template<typename T, size_t Rows, size_t Cols>
    T &ldMatrix<T, Rows, Cols>::operator[](size_t idx)
    {
        return arr[idx];
    }

    template<typename T, size_t Rows, size_t Cols>
    T ldMatrix<T, Rows, Cols>::operator()(size_t r, size_t c) const
    {
        return arr[idx(r, c)];
    }

    template<typename T, size_t Rows, size_t Cols>
    T &ldMatrix<T, Rows, Cols>::operator()(size_t r, size_t c)
    {
        return arr[idx(r, c)];
    }

    template<typename T, size_t Rows, size_t Cols>
    const T *ldMatrix<T, Rows, Cols>::data() const
    {
        return &arr[0];
    }

    template<typename T, size_t Rows, size_t Cols>
    T *ldMatrix<T, Rows, Cols>::buffer()
    {
        return &arr[0];
    }

    template<typename T, size_t Rows, size_t Cols>
    ldMatrix<T, Rows, Cols> &ldMatrix<T, Rows, Cols>::operator+=(const ldMatrix<T, Rows, Cols> &m)
    {
        for (size_t idx = 0; idx < count; ++idx)
            arr[idx] += m[idx];
        return *this;
    }

    template<typename T, size_t Rows, size_t Cols>
    typename ldMatrix<T, Rows, Cols>::row_t ldMatrix<T, Rows, Cols>::row(size_t r) const
    {
        ldMatrix<T, Rows, Cols>::row_t row;
        for (size_t c = 0; c < colSize; ++c)
            row[c] = arr[idx(r, c)];
        return row;
    }

    template<typename T, size_t Rows, size_t Cols>
    typename ldMatrix<T, Rows, Cols>::col_t ldMatrix<T, Rows, Cols>::col(size_t c) const
    {
        ldMatrix<T, Rows, Cols>::col_t col;
        for (size_t r = 0; r < rowSize; ++r)
            col[r] = arr[idx(r, c)];
        return col;
    }

    template<typename T, size_t Rows, size_t Cols>
    void ldMatrix<T, Rows, Cols>::row(size_t r, const typename ldMatrix<T, Rows, Cols>::row_t &v)
    {
        for (size_t c = 0; c < colSize; ++c)
            arr[idx(r, c)] = v[c];
    }

    template<typename T, size_t Rows, size_t Cols>
    void ldMatrix<T, Rows, Cols>::col(size_t c, const typename ldMatrix<T, Rows, Cols>::col_t &v)
    {
        for (size_t r = 0; r < colSize; ++r)
            arr[idx(r, c)] = v[r];
    }

    template<typename T, size_t Rows, size_t Cols>
    ldMatrix<T, Rows, Cols> &ldMatrix<T, Rows, Cols>::operator-=(const ldMatrix<T, Rows, Cols> &m)
    {
        for (size_t idx = 0; idx < count; ++idx)
            arr[idx] -= m[idx];
        return *this;
    }

    template<typename T, size_t Rows, size_t Cols>
    template<typename S>
    ldMatrix<T, Rows, Cols> &ldMatrix<T, Rows, Cols>::operator*=(S s)
    {
        for (size_t idx = 0; idx < count; ++idx)
            arr[idx] *= s;
        return *this;
    }

    template<typename T, size_t Rows, size_t Cols>
    template<typename S>
    ldMatrix<T, Rows, Cols> &ldMatrix<T, Rows, Cols>::operator/=(S s)
    {
        for (size_t idx = 0; idx < count; ++idx)
            arr[idx] /= s;
        return *this;
    }

    template<typename T, size_t Rows, size_t Cols>
    ldMatrix<T, Rows, Cols> &ldMatrix<T, Rows, Cols>::operator*=(const ldMatrix<T, Rows, Cols> &m)
    {
        ldMatrix<T, Rows, Cols> res(T(0));
        for (size_t r = 0; r < rowSize; ++r)
            for (size_t c = 0; c < colSize; ++c)
                for (size_t i = 0; i < rowSize; ++i)
                    res(r, c) += arr[idx(r, i)] * m(i, c);
        operator=(res);
        return *this;
    }

    template<typename T, size_t Rows, size_t Cols>
    typename ldMatrix<T, Rows, Cols>::col_t ldMatrix<T, Rows, Cols>::operator*(const col_t &v) const
    {
        col_t u(type(0));
        for (size_t r = 0; r < rowSize; ++r) {
            for (size_t c = 0; c < colSize; ++c) {
                u[r] += arr[idx(r, c)] * v[r];
            }
        }

        return u;
    }

    template<typename T, size_t Rows, size_t Cols>
    std::array<T, Rows - 1> ldMatrix<T, Rows, Cols>::transform(const std::array<T, Rows - 1> &v) const
    {
        std::array<T, Rows - 1> u(type(0));
        for (size_t r = 0; r < rowSize - 1; ++r) {
            for (size_t c = 0; c < colSize - 1; ++c) {
                u[r] += arr[idx(r, c)] * v[r];
            }
            u[r] += arr[idx(r, colSize - 1)];
        }

        return u;
    }

    template<typename T, size_t Rows, size_t Cols>
    ldMatrix<T, Rows, Cols> &ldMatrix<T, Rows, Cols>::clean()
    {
        for (size_t idx = 0; idx < count; ++idx)
            if (ldRazer::isZero(arr[idx])) arr[idx] = T(0);
        return *this;
    }

    template<typename T, size_t Rows, size_t Cols>
    ldMatrix<T, Rows, Cols> &ldMatrix<T, Rows, Cols>::clear()
    {
        for (size_t idx = 0; idx < count; ++idx)
            arr[idx] = 0;
        return *this;
    }

    template<typename T, size_t Rows, size_t Cols>
    ldMatrix<T, Rows, Cols> operator+(const ldMatrix<T, Rows, Cols> &m, const ldMatrix<T, Rows, Cols> &n)
    {
        ldMatrix<T, Rows, Cols> res;
        for (size_t r = 0; r < ldMatrix<T, Rows, Cols>::rowSize; ++r)
            for (size_t c = 0; c < ldMatrix<T, Rows, Cols>::colSize; ++c)
                res(r, c) = m(r, c) + n(r, c);
        return res;
    }

    template<typename T, size_t Rows, size_t Cols>
    ldMatrix<T, Rows, Cols> operator-(const ldMatrix<T, Rows, Cols> &m, const ldMatrix<T, Rows, Cols> &n)
    {
        ldMatrix<T, Rows, Cols> res;
        for (size_t r = 0; r < ldMatrix<T, Rows, Cols>::rowSize; ++r)
            for (size_t c = 0; c < ldMatrix<T, Rows, Cols>::colSize; ++c)
                res(r, c) = m(r, c) - n(r, c);
        return res;
    }

    template<typename S, typename T, size_t Rows, size_t Cols>
    ldMatrix<T, Rows, Cols> operator*(const ldMatrix<T, Rows, Cols> &m, S s)
    {
        ldMatrix<T, Rows, Cols> res;
        for (size_t r = 0; r < ldMatrix<T, Rows, Cols>::rowSize; ++r)
            for (size_t c = 0; c < ldMatrix<T, Rows, Cols>::colSize; ++c)
                res(r, c) = m(r, c) * s;
        return res;
    }

    template<typename S, typename T, size_t Rows, size_t Cols>
    ldMatrix<T, Rows, Cols> operator*(S s, const ldMatrix<T, Rows, Cols> &n)
    {
        ldMatrix<T, Rows, Cols> res;
        for (size_t r = 0; r < ldMatrix<T, Rows, Cols>::rowSize; ++r)
            for (size_t c = 0; c < ldMatrix<T, Rows, Cols>::colSize; ++c)
                res(r, c) = s * n(r, c);
        return res;
    }

    template<typename S, typename T, size_t Rows, size_t Cols>
    ldMatrix<T, Rows, Cols> operator/(const ldMatrix<T, Rows, Cols> &m, S s)
    {
        ldMatrix<T, Rows, Cols> res;
        for (size_t r = 0; r < ldMatrix<T, Rows, Cols>::rowSize; ++r)
            for (size_t c = 0; c < ldMatrix<T, Rows, Cols>::colSize; ++c)
                res(r, c) = m(r, c) / s;
        return res;
    }

    template<typename T>
    T det(const ldMatrix<T, 2, 2> &m)
    {
        return m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0);
    }

    template<typename T>
    T det(const ldMatrix<T, 3, 3> &m)
    {
        T c0 = m[4] * m[8] - m[5] * m[7];
        T c3 = m[2] * m[7] - m[1] * m[8];
        T c6 = m[1] * m[5] - m[2] * m[4];
        return m[0] * c0 + m[3] * c3 + m[6] * c6;
    }

    template<typename T>
    T det(const ldMatrix<T, 4, 4> & /*m*/)
    {
        assert(false && "TODO");
    }

    template<typename T, size_t RC>
    T det(const ldMatrix<T, RC, RC> & /*m*/)
    {
        assert(false && "TODO");
    }

    template<typename T, size_t Rows, size_t CR, size_t Cols>
    ldMatrix<T, Rows, Cols> operator*(const ldMatrix<T, Rows, CR> &m, const ldMatrix<T, CR, Cols> &n)
    {
        ldMatrix<T, Rows, Cols> res(T(0));
        for (size_t r = 0; r < ldMatrix<T, Rows, CR>::rowSize; ++r)
            for (size_t c = 0; c < ldMatrix<T, CR, Cols>::colSize; ++c)
                for (size_t i = 0; i < ldMatrix<T, CR, Cols>::rowSize; ++i)
                    res(r, c) += m(r, i) * n(i, c);
        return res;
    }

    template<typename MT, typename NT, size_t Rows, size_t CR, size_t Cols>
    ldMatrix<NT, Rows, Cols> operator*(const ldMatrix<MT, Rows, CR> &m, const ldMatrix<NT, CR, Cols> &n)
    {
        ldMatrix<NT, Rows, Cols> res(NT(typename NT::type(0)));
        for (size_t r = 0; r < ldMatrix<MT, Rows, CR>::rowSize; ++r)
            for (size_t c = 0; c < ldMatrix<NT, CR, Cols>::colSize; ++c)
                for (size_t i = 0; i < ldMatrix<NT, CR, Cols>::rowSize; ++i)
                    res(r, c) += m(r, i) * n(i, c);
        return res;
    }

    template<typename MT, typename NT, size_t Rows, size_t Cols>
    ldVector <NT, Rows> operator*(const ldMatrix<MT, Rows, Cols> &m, const ldVector <NT, Cols> &n)
    {
        ldVector<NT, Rows> result(0);
        for (size_t c = 0; c != Cols; ++c) {
            for (size_t r = 0; r != Rows; ++r) {
                result[r] += m(r, c) * n[r];
            }
        }
        return result;
    }

}

#endif //LASERDOCKCORE_LDMATRIX_H
