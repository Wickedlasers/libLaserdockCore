//
// Created by Darren Otgaar on 2018/08/14.
//

#ifndef LASERDOCK_RAZER_LDDISC_H
#define LASERDOCK_RAZER_LDDISC_H

#include "ldRazerVec2.h"
#include "ldLine.h"

namespace ldRazer
{

    template<typename T>
    struct ldDisc
    {
        using type = T;
        using vector_t = ldRazerVec2<T>;

        ldDisc() = default;

        ldDisc(const vector_t &pCentre, type pRadius) : centre(pCentre), radius(pRadius)
        {
        }

        ldDisc &translate(const vector_t &V)
        {
            centre += V;
            return *this;
        }

        vector_t centre;
        float radius;
    };

    template<typename T>
    inline bool intersection(const ldDisc<T> &disc, const ldLine <T> &seg)
    {
        auto Q = projectToSegment(seg, disc.centre);
        return intersection(disc.centre, disc.radius, Q);
    }

    template<typename T>
    inline int intersection(const ldDisc<T> &disc, const ldLine <T> &seg, T parms[2])
    {
        auto D = seg.segment();
        auto d = D.length();
        if (d == 0) return 0;

        auto N = D / d;
        auto m = seg.A - disc.centre;
        auto b = m.dot(N);
        auto c = m.dot(m) - disc.radius * disc.radius;

        if (c > 0 && b > 0) return 0;

        auto discrim = b * b - c;
        if (discrim < 0) return 0;

        discrim = std::sqrt(discrim);
        parms[0] = std::max(-b - discrim, 0.f);
        if (parms[0] > d) return 0;
        parms[1] = std::min(discrim - b, d);
        return parms[0] == parms[1] ? 1 : 2;
    }

    template<typename T>
    inline bool intersection(const ldDisc<T> &disc, const ldRazerVec2 <T> &P)
    {
        return (P - disc.centre).lengthSq() <= disc.radius * disc.radius;
    }

}

#endif //LASERDOCK_RAZER_LDDISC_H
