//
//  ldSerpentAABB.h
//  LaserdockVisualizer
//
//  Created by Darren Otgaar 31/07/2018
//  Copyright (c) 2018 Wicked Lasers. All rights reserved.
//

#ifndef LASERDOCK_RAZER_LDAABB_H__
#define LASERDOCK_RAZER_LDAABB_H__

#include "ldRazerVec2.h"

namespace ldRazer
{

    /**
     * Defines an Axis-aligned bounding box and related mathematical operators
     * Using the centre, half-extent definition
     */

    template<typename T>
    struct ldAABB
    {
        using type = T;
        using vector_t = ldRazerVec2<T>;

        ldAABB() = default;

        ldAABB(const vector_t &centre, T dim) :
                left_bottom(centre - ldRazerVec2<T>(dim, dim)),
                right_top(centre + ldRazerVec2<T>(dim, dim))
        {
        }

        ldAABB(const vector_t &pLeft_bottom, const vector_t &pRight_top) : left_bottom(pLeft_bottom), right_top(pRight_top)
        {
        }

        size_t getVertexCount() const
        {
            return 5;
        }

        size_t getSides() const
        {
            return 4;
        }

        type left() const
        {
            return left_bottom.x;
        }

        type right() const
        {
            return right_top.x;
        }

        type bottom() const
        {
            return left_bottom.y;
        }

        type top() const
        {
            return right_top.y;
        }

        vector_t leftBottom() const
        {
            return left_bottom;
        }

        vector_t rightBottom() const
        {
            return vector_t(right(), bottom());
        }

        vector_t leftTop() const
        {
            return vector_t(left(), top());
        }

        vector_t rightTop() const
        {
            return right_top;
        }

        type width() const
        {
            return right_top.x - left_bottom.x;
        }

        type height() const
        {
            return right_top.y - left_bottom.y;
        }

        const vector_t centre() const
        {
            return (left_bottom + right_top) / T(2);
        }

        const vector_t &min() const
        {
            return left_bottom;
        }

        const vector_t &max() const
        {
            return right_top;
        }

        const vector_t halfExtent() const
        {
            return vector_t(width() / 2, height() / 2);
        }

        vector_t operator[](uint16_t id) const;      // 0 - bl, 1 - br, 2 - tr, 3 - tl (counter-clockwise)
        vector_t
        edge(uint16_t id) const;            // 0 - bottom, 1 - right, 2 - top, 3 - left (counter-clockwise) (not normalised)
        vector_t edgeN(uint16_t id) const;           // Normalised version of above
        vector_t edgePerp(uint16_t id) const;        // Perpendicular of above

        void setCorners(const vector_t &lb, const vector_t &rt)
        {
            left_bottom = lb;
            right_top = rt;
        }

        ldAABB &translate(const vector_t &V)
        {
            left_bottom += V;
            right_top += V;
            return *this;
        }

        ldAABB &scale(const vector_t &S)
        {
            left_bottom *= S;
            right_top *= S;
            return *this;
        }

        std::array<vector_t, 4> getVertices() const
        {
            return {leftBottom(), rightBottom(), rightTop(), leftTop()};
        }

        vector_t left_bottom;
        vector_t right_top;
    };

// Use open intervals for AABB intersections in integer space (they do not collide if neighbouring)
    inline bool open_intersection(const ldAABB<int16_t> &A, const ldAABB<int16_t> &B)
    {
        if (A.max()[0] <= B.min()[0] || A.min()[0] >= B.max()[0]) return false;
        if (A.max()[1] <= B.min()[1] || A.min()[1] >= B.max()[1]) return false;
        return true;
    }

// Find separating axis, unroll loop
    template<typename T>
    inline bool intersection(const ldAABB<T> &A, const ldAABB<T> &B)
    {
        if (A.max()[0] < B.min()[0] || A.min()[0] > B.max()[0]) return false;
        if (A.max()[1] < B.min()[1] || A.min()[1] > B.max()[1]) return false;
        return true;
    }

    template<typename T>
    inline bool intersection(const ldAABB<T> &B, const ldRazerVec2 <T> &P)
    {
        return B.min().x <= P.x && P.x <= B.max().x && B.min().y <= P.y && P.y <= B.max().y;
    }

//    template<typename T>
//    typename ldAABB<T>::vector_t ldAABB<T>::operator[](uint16_t /*id*/) const
//    {

//    }

    template<typename T>
    typename ldAABB<T>::vector_t ldAABB<T>::edge(uint16_t id) const
    {
        switch (id) {
            case 0:
                return rightBottom() - leftBottom();
            case 1:
                return rightTop() - rightBottom();
            case 2:
                return leftTop() - rightTop();
            case 3:
                return leftBottom() - leftTop();
            default:
                assert(false && "Invalid Edge specified");
        }
    }

    template<typename T>
    typename ldAABB<T>::vector_t ldAABB<T>::edgeN(uint16_t id) const
    {
        return normalise(edge(id));
    }

    template<typename T>
    typename ldAABB<T>::vector_t ldAABB<T>::edgePerp(uint16_t id) const
    {
        return perp(edgeN(id));
    }

}

#endif //__LASERDOCK_RAZER_AABB_H__
