//
// Created by Darren Otgaar on 2018/09/27.
//

#ifndef LASERDOCKCORE_LDLINEMESH_H
#define LASERDOCKCORE_LDLINEMESH_H

#include "ldRazer.h"

namespace ldRazer
{

    template<typename T>
    struct ldLineMesh
    {
        using type = T;
        using vector_t = ldRazerVec2<T>;
        using transform_t = ldMat3<T>;

        ldLineMesh() = default;

        ldLineMesh(const ldLineMesh &rhs) : vertices(rhs.vertices), indices(rhs.indices)
        {
        }

        ldLineMesh &operator=(const ldLineMesh &rhs)
        {
            if (this != &rhs) {
                vertices.resize(rhs.vertices.size());
                vertices = rhs.vertices;
                indices.resize(rhs.indices.size());
                indices = rhs.indices;
            }
            return *this;
        }

        struct strip
        {
            uint16_t start;
            uint16_t count;

            strip() = default;

            strip(uint16_t pStart, uint16_t pCount) : start(pStart), count(pCount)
            {
            }
        };

        size_t getStripCount() const
        {
            return indices.size();
        }

        size_t getVertexCount() const
        {
            return vertices.size();
        };

        size_t getStripLength(uint16_t id) const
        {
            return id < indices.size() ? indices[id].count : uint16_t(-1);
        }

        ldLineMesh translate(const vector_t &v) const
        {
            ldLineMesh t;
            std::transform(vertices.begin(), vertices.end(), t.vertices.begin(), [&v](const vector_t &u)
            {
                return u + v;
            });
            return t;
        }

        ldLineMesh transform(const transform_t &M) const
        {
            ldLineMesh t;
            std::transform(vertices.begin(), vertices.end(), t.vertices.begin(), [&M](const vector_t &v)
            {
                return ldRazer::transform(M, v);
            });
            return t;
        }

        const vector_t &operator()(uint16_t lid, uint16_t id) const
        {
            assert(lid < indices.size() && id < indices[lid].count && "Index out of range");
            return vertices[indices[lid].start + id];
        }

        vector_t &operator()(uint16_t lid, uint16_t id)
        {
            assert(lid < indices.size() && id < indices[lid].count && "Index out of range");
            return vertices[indices[lid].start + id];
        }

        vector_t edge(uint16_t lid, uint16_t id) const
        {
            assert(lid < indices.size() && id < indices[lid].count - 1 && "index out of range");
            return operator()(lid, id + 1) - operator()(lid, id);
        }

        vector_t edgeN(uint16_t lid, uint16_t id) const
        {
            return normalise(edge(lid, id));
        }

        vector_t edgePerp(uint16_t lid, uint16_t id) const
        {
            return perp(edgeN(lid, id));
        }

        const vector_t *data(uint16_t lid) const
        {
            return &vertices[indices[lid].start];
        }

        std::vector<vector_t> vertices;
        std::vector<strip> indices;
    };

}

#endif //LASERDOCKCORE_LDLINEMESH_H
