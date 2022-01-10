#ifndef LASERDOCK_RAZER_LDVEC2_H__
#define LASERDOCK_RAZER_LDVEC2_H__

#include <cmath>
#include <cassert>
#include <cstdint>
#include <algorithm>

/**
 * Note: There is a strong case for using vector algebra in both floating-point and integer vector spaces.  The use of
 * integer vectors in 2D games is really helpful and unfortunately the Vec2 definition in ldMaths is for floating point
 * only.  This struct should really be merged with the Vec2 class, but this will work for now.
 */

namespace ldRazer
{

    // C++14 only: template <typename T> const T PI = T(3.14159265358979323846);
    /*
    const float PI = 3.141593f;
    const float TWO_PI = 2.f * PI;
    const float HALF_PI = PI / 2.f;
    */

    /*
    #define M_PIf static_cast<float> (M_PI)
    #define M_PI_2f static_cast<float> (M_PI_2)
    #define M_PI_4f static_cast<float> (M_PI_4)
    #define M_2PI (2.0*M_PI)
    #define M_2PIf static_cast<float> (2.0*M_PI)
    */

    template<typename T>
    bool eq(T A, T B, T epsilon = std::numeric_limits<T>::epsilon())
    {
        return std::abs(A - B) <= std::max(std::abs(A), std::abs(B)) * epsilon;
    }

    template<typename T>
    bool isZero(T value, T epsilon = std::numeric_limits<T>::epsilon())
    {
        return eq(value, T(0), epsilon);
    }

    template<typename T>
    bool neq(T A, T B, T epsilon = std::numeric_limits<T>::epsilon())
    {
        return !eq(A, B, epsilon);
    }

    template<typename T>
    bool lessThan(T A, T B, T epsilon = std::numeric_limits<T>::epsilon())
    {
        return (B - A) > std::max(std::abs(A), std::abs(B)) * epsilon;
    }

    template<typename T>
    struct ldRazerVec2
    {
        using type = T;

        ldRazerVec2()
        {
            clear();
        }

        explicit ldRazerVec2(T value) : x(value), y(value)
        {
        }

        explicit ldRazerVec2(const T *value_ptr) : x(*value_ptr), y(*(value_ptr + 1))
        {
        }

        ldRazerVec2(T pX, T pY) : x(pX), y(pY)
        {
        }

        ldRazerVec2(const ldRazerVec2 &rhs) : x(rhs.x), y(rhs.y)
        {
        }

        ldRazerVec2 &operator=(const ldRazerVec2 &rhs)
        {
            if (this != &rhs)
            {
                x = rhs.x;
                y = rhs.y;
            }
            return *this;
        }

        bool operator==(const ldRazerVec2 &rhs) const
        {
            return x == rhs.x && y == rhs.y;
        }

        bool operator!=(const ldRazerVec2 &rhs) const
        {
            return !operator==(rhs);
        }

        bool operator<(const ldRazerVec2 &rhs) const
        {
            return y < rhs.y || (y == rhs.y && x < rhs.x);
        }

        bool operator>(const ldRazerVec2 &rhs) const
        {
            return y > rhs.y || (y == rhs.y && x > rhs.x);
        }

        bool operator<=(const ldRazerVec2 &rhs) const
        {
            return !operator>(rhs);
        }

        bool operator>=(const ldRazerVec2 &rhs) const
        {
            return !operator<(rhs);
        }

        bool eq(const ldRazerVec2 &rhs, type epsilon = std::numeric_limits<T>::epsilon()) const
        {
            return ldRazer::eq(x, rhs.x, epsilon) && ldRazer::eq(y, rhs.y, epsilon);
        }

        bool neq(const ldRazerVec2 &rhs, type epsilon = std::numeric_limits<T>::epsilon()) const
        {
            return ldRazer::neq(x, rhs.x, epsilon) || ldRazer::neq(y, rhs.y, epsilon);
        }

        bool lessThan(const ldRazerVec2 &rhs, type epsilon = std::numeric_limits<T>::epsilon()) const
        {
            return ldRazer::lessThan(y, rhs.y, epsilon) ||
                   (ldRazer::eq(y, rhs.y, epsilon) && ldRazer::lessThan(x, rhs.x, epsilon));
        }

        T &operator[](size_t i)
        {
            assert(i < 2 && "Invalid Index");
            return i == 0 ? x : y;
        }

        const T &operator[](size_t i) const
        {
            assert(i < 2 && "Invalid Index");
            return i == 0 ? x : y;
        }

        ldRazerVec2 &operator+=(const ldRazerVec2 &rhs)
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        ldRazerVec2 &operator-=(const ldRazerVec2 &rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        ldRazerVec2 &operator*=(const T &s)
        {
            x *= s;
            y *= s;
            return *this;
        }

        ldRazerVec2 &operator*=(const ldRazerVec2 &rhs)
        {
            x *= rhs.x;
            y *= rhs.y;
            return *this;
        }

        ldRazerVec2 &operator/=(const T &s)
        {
            x /= s;
            y /= s;
            return *this;
        }

        ldRazerVec2 &operator/=(const ldRazerVec2 &rhs)
        {
            x /= rhs.x;
            y /= rhs.y;
            return *this;
        }

        void set(T x1, T y1)
        {
            x = x1;
            y = y1;
        }

        void clear()
        {
            x = T(0);
            y = T(0);
        }

        T dot(const ldRazerVec2 &B) const
        {
            return x * B.x + y * B.y;
        }

        T lengthSq() const
        {
            return dot(*this);
        }

        T length() const
        {
            return T(std::sqrt(lengthSq()));
        }

        bool isUnit(T epsilon = std::numeric_limits<T>::epsilon()) const
        {
            return ldRazer::eq(lengthSq(), T(1), epsilon);
        }

        T x, y;
    };

    template<typename T>
    bool eq(const ldRazerVec2<T> &A, const ldRazerVec2<T> &B, T epsilon = std::numeric_limits<T>::epsilon())
    {
        return eq(A.x, B.x, epsilon) && eq(A.y, B.y, epsilon);
    }

    template<typename T>
    bool lessThan(const ldRazerVec2<T> &A, const ldRazerVec2<T> &B, T epsilon = std::numeric_limits<T>::epsilon())
    {
        return A.lessThan(B, epsilon);
    }

    template<typename T>
    ldRazerVec2<T> operator-(const ldRazerVec2<T> &A)
    {
        return ldRazerVec2<T>(-A.x, -A.y);
    }

    template<typename T>
    ldRazerVec2<T> operator+(const ldRazerVec2<T> &A, const ldRazerVec2<T> &B)
    {
        return ldRazerVec2<T>(A.x + B.x, A.y + B.y);
    }

    template<typename T>
    ldRazerVec2<T> operator-(const ldRazerVec2<T> &A, const ldRazerVec2<T> &B)
    {
        return ldRazerVec2<T>(A.x - B.x, A.y - B.y);
    }

    template<typename T>
    ldRazerVec2<T> operator*(const ldRazerVec2<T> &A, const ldRazerVec2<T> &B)
    {
        return ldRazerVec2<T>(A.x * B.x, A.y * B.y);
    }

    template<typename T>
    ldRazerVec2<T> operator/(const ldRazerVec2<T> &A, const ldRazerVec2<T> &B)
    {
        return ldRazerVec2<T>(A.x / B.x, A.y / B.y);
    }

    template<typename T, typename S>
    ldRazerVec2<T> operator*(S scalar, const ldRazerVec2<T> &B)
    {
        return ldRazerVec2<T>(scalar * B.x, scalar * B.y);
    }

    template<typename T, typename S>
    ldRazerVec2<T> operator*(const ldRazerVec2<T> &A, S scalar)
    {
        return operator*(scalar, A);
    }

    template<typename T, typename S>
    ldRazerVec2<T> operator/(const ldRazerVec2<T> &A, S scalar)
    {
        return ldRazerVec2<T>(A.x / scalar, A.y / scalar);
    }

    template<typename T>
    ldRazerVec2<T> perp(const ldRazerVec2<T> &A)
    {
        return ldRazerVec2<T>(-A.y, A.x);
    }

    template<typename T>
    T dot(const ldRazerVec2<T> &A, const ldRazerVec2<T> &B)
    {
        return A.x * B.x + A.y * B.y;
    }

    template<typename T>
    T dotPerp(const ldRazerVec2<T> &A, const ldRazerVec2<T> &B)
    {
        return A.x * B.y - A.y * B.x;
    }

    template<typename T>
    ldRazerVec2<T> normalise(const ldRazerVec2<T> &A)
    {
        return A / A.length();
    }

    template<typename T>
    T kross(const ldRazerVec2<T> &A, const ldRazerVec2<T> &B)
    {
        return A.x * B.y - A.y * B.x;
    }

    template<typename T>
    ldRazerVec2<T> polarToCartesian(T phi, T radius = T(1))
    {
        return radius * ldRazerVec2<T>(std::cos(phi), std::sin(phi));
    }

}

#endif //__LASERDOCK_RAZER_LDVEC2_H__
