//
// Created by Darren Otgaar on 2018/09/13.
//

#ifndef LASERDOCKCORE_LDHULL_H
#define LASERDOCKCORE_LDHULL_H

#include "ldRazerVec2.h"
#include "ldMatrices.h"

namespace ldRazer
{

// isHull determines whether the input polygon (consisting of counter-clockwise ordered points) is a convex hull or not.
template<typename T>
bool isHull(const std::vector<ldRazerVec2<T>> &vertices)
{
    for (size_t i = 1, j = 2, end = vertices.size(); i != end; ++i, j = (i + 1) % end) {
        if (dotPerp(vertices[j] - vertices[i], vertices[i] - vertices[i-1]) > T(0)) return false;
    }
    return true;
}

template<typename T, size_t sides>
bool isHull(const std::array<ldRazerVec2<T>, sides> &vertices)
{
    for (size_t i = 1, j = 2, end = sides; i != end; ++i, j = (i + 1) % end) {
        if (dotPerp(vertices[j] - vertices[i], vertices[i] - vertices[i-1]) > T(0)) return false;
    }
    return true;
}

// Note, the loop does not need to be closed, the hull automatically closes the loop itself, resulting in
// sides + 1 vertices

template<typename T, size_t sides>
struct ldHull
{
    using type = T;
    using vector_t = ldRazerVec2<T>;
    using transform_t = ldMat3<T>;

    ldHull() = default;

    ldHull(const std::array<vector_t, sides> &pVertices) : vertices(pVertices)
    {
        assert(isHull(vertices) && "Vertices must be convex hull");
    }

    size_t getVertexCount() const
    {
        return vertices.size() + 1;
    }

    size_t getSides() const
    {
        return vertices.size();
    }

    ldHull translate(const vector_t &v) const
    {
        ldHull t;
        std::transform(vertices.begin(), vertices.end(), t.vertices.begin(), [&v](const vector_t &u)
        {
            return u + v;
        });
        return t;
    }

    ldHull transform(const transform_t &M) const
    {
        ldHull t;
        std::transform(vertices.begin(), vertices.end(), t.vertices.begin(), [&M](const vector_t &v)
        {
            return ldRazer::transform(M, v);
        });
        return t;
    }

    void buildHull(const std::array<vector_t, sides> &pVertices)
    {
        if (!isHull(pVertices)) return;
        vertices = pVertices;
    }

    const vector_t &operator[](uint16_t id) const
    {
        assert(id <= sides && "Index out of range");
        return id == sides ? vertices[0] : vertices[id];
    }

    vector_t &operator[](uint16_t id)
    {
        assert(id <= sides && "Index out of range");
        return id == sides ? vertices[0] : vertices[id];
    }

    vector_t edge(uint16_t id) const
    {
        return vertices[(id + 1) % vertices.size()] - vertices[id];
    }

    vector_t edgeN(uint16_t id) const
    {
        return normalise(edge(id));
    }

    vector_t edgePerp(uint16_t id) const
    {
        return perp(edgeN(id));
    }

    std::array<vector_t, sides> vertices;
};

template<typename T, size_t sides>
ldHull<T, sides> buildHull(const std::array<ldRazerVec2<T>, sides> &vertices)
{
    ldHull<T, sides> hull;
    hull.buildHull(vertices);
    return hull;
}

template<typename T>
bool intersection(const std::vector<ldRazerVec2<T>> &hull, const ldRazerVec2 <T> &P)
{
    const size_t hull_size = hull.size();
    for (size_t i = 0; i != hull_size; ++i) {
        if (dotPerp(P - hull[i], hull[(i+1)%hull_size] - hull[i]) < T(0)) return false;
    }
    return true;
}

template<typename T>
std::vector<ldRazerVec2<T>> convexHull(const std::vector<ldRazerVec2<T>> &input)
{
    std::vector<size_t> edges;

    const size_t samples = input.size();
    for (size_t p = 0; p != samples; ++p) {
        for (size_t q = 0; q != samples; ++q) {
            if (p == q) continue;
            bool isHullEdge = true;
            ldRazerVec2 <T> E = input[q] - input[p];
            for (size_t r = 0; r < samples; ++r) {
                if (r != p && r != q) {
                    if (dotPerp(input[r] - input[p], E) < 0.f) {
                        isHullEdge = false;
                        break;
                    }
                }
            }

            if (isHullEdge) {
                edges.emplace_back(p);
                edges.emplace_back(q);
            }
        }
    }

    std::vector<ldRazerVec2<T>> hull;
    hull.emplace_back(input[edges[0]]);
    hull.emplace_back(input[edges[1]]);

    const ldRazerVec2 <T> *first = &input[edges[0]];
    const ldRazerVec2 <T> *last = &input[edges[1]];
    while (last != first) {
        for (size_t i = 0; i != edges.size(); i += 2) {
            if (last == &input[edges[i]]) {
                if (&input[edges[i+1]] == first)
                    last = &input[edges[i + 1]];
                else {
                    hull.emplace_back(input[edges[i + 1]]);
                    last = &input[edges[i + 1]];
                }
                break;
            }
        }
    }

    return hull;
}

}

#endif //LASERDOCKCORE_LDHULL_H
