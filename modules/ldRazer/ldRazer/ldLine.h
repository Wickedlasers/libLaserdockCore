//
//  ldSerpentLine.h
//  LaserdockVisualizer
//
//  Created by Darren Otgaar on 31/07/2018
//  Copyright (c) 2018 Wicked Lasers. Allrights reserved.
//

#ifndef LASERDOCK_RAZER_LDLINE_H
#define LASERDOCK_RAZER_LDLINE_H

#include "ldRazerVec2.h"
#include "ldMatrices.h"

// ignore  "anonumous struct" warning
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

namespace ldRazer
{

    template<typename T>
    struct ldLine
    {
        using type = T;
        using vector_t = ldRazerVec2<T>;
        using transform_t = ldMat3<T>;

        ldLine(){}

        ldLine(const vector_t &pA, const vector_t &pB) : A(pA), B(pB)
        {
        }

        ldLine(const vector_t &P, const vector_t &dir, T len) : A(P), B(P + len * dir)
        {
        }

        ldLine(const ldLine &rhs) : A(rhs.A), B(rhs.B)
        {
        }

        ~ldLine() = default;

        ldLine &operator=(const ldLine &rhs)
        {
            if (this != &rhs) {
                A = rhs.A;
                B = rhs.B;
            }
            return *this;
        }

        bool isDegenerate() const
        {
            return eq<float>(segment().lengthSq(), 0);
        }

        const vector_t dir() const
        {
            return normalise(B - A);
        }

        const vector_t segment() const
        {
            return B - A;
        }

        ldLine &translate(const vector_t &V)
        {
            A += V;
            B += V;
            return *this;
        }

        ldLine &scale(const vector_t &S)
        {
            A *= S;
            B *= S;
            return *this;
        }

        ldLine transform(const transform_t &M) const
        {
            ldLine t;
            t.A = ldRazer::transform(M, A);
            t.B = ldRazer::transform(M, B);
            return t;
        }

        union
        {
            struct
            {
                vector_t A;
                vector_t B;
            };
            vector_t arr[2];
        };
    };

// Note: two lines are the same if their endpoints match in any order (integer and only integer!)
    template<typename T>
    inline bool operator==(const ldLine<T> &A, const ldLine<T> &B)
    {
        static_assert(std::is_integral<T>::value, "Integral value required");
        return &A == &B || (A.A == B.A && A.B == B.B) || (A.A == B.B && A.B == B.A);
    }

    template<typename T>
    inline bool eq(const ldLine<T> &A, const ldLine<T> &B)
    {
        return &A == &B || (eq(A.A, B.A) && eq(A.B, B.B));
    }

    template<typename T>
    inline ldRazerVec2<T> projectToSegment(const ldLine<T> &seg, const ldRazerVec2<T> &P)
    {
        auto C = P - seg.A;
        auto D = seg.segment();
        auto d = D.lengthSq();
        if (d == 0) return seg.A;
        auto u = dot(C, D) / d;
        if (u < 0) u = 0;
        if (u > 1) u = 1;
        return seg.A + u * D;
    }

}

#ifdef __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

#endif //LASERDOCK_RAZER_LDLINE_H
