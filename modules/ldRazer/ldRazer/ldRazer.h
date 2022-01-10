//
// Created by Darren Otgaar on 2018/09/09.
//

#ifndef LASERDOCKCORE_LDRAZER_H
#define LASERDOCKCORE_LDRAZER_H

#include <cmath>
#include <array>
#include <memory>
#include <random>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <functional>
#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldRazerVec2.h"
#include "ldLine.h"
#include "ldAABB.h"
#include "ldDisc.h"
#include "ldHull.h"
#include "ldLineMesh.h"
#include "ldMatrices.h"

// NOTE:  This library is placed here temporarily, it will move later.

namespace ldRazer
{     // Renderer for Laser (to keep it short)

    static const bool NO_RED_LASER = false;

    using byte = unsigned char;
    using vec2f = ldRazerVec2<float>;
    using vec2d = ldRazerVec2<double>;
    using vec2i = ldRazerVec2<int32_t>;
    using vec2s = ldRazerVec2<int16_t>;
    using mat2f = ldMat2<float>;
    using mat3f = ldMat3<float>;
    using line2s_t = ldLine<int16_t>;
    using line2f_t = ldLine<float>;
    using ray2f_t = ldLine<float>;
    using AABB2s_t = ldAABB<int16_t>;
    using AABB2f_t = ldAABB<float>;
    using disc2f_t = ldDisc<float>;
    using hull2f_t = ldHull<float, 4>;
    using lineMesh2f_t = ldLineMesh<float>;
    using line2s_arr_t = std::vector<line2s_t>;
    using line2f_arr_t = std::vector<line2f_t>;
    using AABB2s_arr_t = std::vector<AABB2s_t>;
    using colour_t = uint32_t;
    using linecolf_t = std::pair<line2f_t, colour_t>;
    using linecol_arr_t = std::vector<std::pair<line2s_t, colour_t>>;
    using linecolf_arr_t = std::vector<std::pair<line2f_t, colour_t>>;

    template<typename T>
    struct ldRandom;

    template<>
    struct ldRandom<int16_t>
    {
        std::random_device rd;
        mutable std::mt19937 gen;
        mutable std::uniform_int_distribution<int16_t> dist;

        explicit ldRandom(int16_t min = std::numeric_limits<int16_t>::lowest(),
                        int16_t max = std::numeric_limits<int16_t>::max())
                : gen(rd()), dist(min, max)
        {
        }

        int16_t random1() const
        {
            return dist(gen);
        }

        int16_t random1(int16_t mod) const
        {
            return (dist(gen) % mod);
        }

        float random1(int16_t min, int16_t max)
        {
            return min + random1(max - min);
        }

        vec2s random2() const
        {
            return vec2s(random1(), random1());
        }

        vec2s random2(int16_t modX, int16_t modY) const
        {
            return vec2s(random1(modX), random1(modY));
        }
    };

    template<>
    struct ldRandom<float>
    {
        std::random_device rd;
        mutable std::mt19937 gen;
        mutable std::uniform_real_distribution<float> dist;

        explicit ldRandom(float min = 0.f, float max = 1.f) : gen(rd()), dist(min, max)
        {
        }

        float random1() const
        {
            return dist(gen);
        }

        float random1(float min, float max)
        {
            return min + random1() * (max - min);
        }

        vec2f random2() const
        {
            return vec2f(dist(gen), dist(gen));
        }

        vec2f random2(float min, float max)
        {
            return vec2f(random1(min, max), random1(min, max));
        }
    };

    template<typename T>
    T clamp(const T &value, const T &min, const T &max)
    {
        return value < min ? min : value > max ? max : value;
    }

    template<typename T>
    T wrap(T value, const T &min, const T &max)
    {
        return value < min ? (value - min) + max : (value > max) ? (value - max) + min : value;
    }

    template<typename T>
    T bias(T base, T value)
    {
        static const auto logH = std::log(T(.5));
        return std::pow(value, std::log(base) / logH);
    }

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

    inline float computeAngle(const vec2f &A, float epsilon = .001f)
    {
        Q_UNUSED(epsilon)
        assert(A.isUnit(epsilon));
        auto angle = std::atan2(A.y, A.x);
        return angle >= 0 ? angle : 2.f * float(M_PI) + angle;
    }

#ifdef __clang__
#pragma COMPILER_DIAGNOSTIC diagnostic pop
#endif

    constexpr uint32_t makeARGB(byte red, byte green, byte blue, byte alpha = 255)
    {
        return uint32_t(uint32_t(alpha) << 24 | uint32_t(red) << 16 | uint32_t(green) << 8 | uint32_t(blue));
    }

    template<typename T, typename S>
    T lerp(const T &A, const T &B, const S &s)
    {
        return T((S(1) - s) * A + s * B);
    }

    inline uint32_t lerpColour(uint32_t A, uint32_t B, float s)
    {
        return makeARGB(
                lerp<uint32_t>((A & 0x00FF0000) >> 16, (B & 0x00FF0000) >> 16, s),
                lerp<uint32_t>((A & 0x0000FF00) >> 8, (B & 0x0000FF00) >> 8, s),
                lerp<uint32_t>((A & 0x000000FF), (B & 0x000000FF), s),
                lerp<uint32_t>((A & 0xFF000000) >> 24, (B & 0xFF000000) >> 24, s)
        );
    }

    // Make a bright, mostly primary colour
    uint32_t randomColour();

    uint32_t randomColour(const vec2s &redRange, const vec2s &greenRange, const vec2s &blueRange);

    inline line2f_t convert(const line2s_t &l)
    {
        return line2f_t(vec2f(l.A.x, l.A.y), vec2f(l.B.x, l.B.y));
    }

    inline vec2f convert(const vec2s &v)
    {
        return vec2f(v.x, v.y);
    }

    size_t findInterval(const std::vector<float> &seq, float t);

}

#endif //LASERDOCKCORE_LDRAZER_H
