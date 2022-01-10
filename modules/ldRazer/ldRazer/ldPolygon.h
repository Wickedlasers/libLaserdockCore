//
// Created by Darren Otgaar on 2018/09/11.
//

#ifndef LASERDOCKCORE_LDPOLYGON_H
#define LASERDOCKCORE_LDPOLYGON_H

#include "ldRazerVec2.h"
#include "ldMatrices.h"

namespace ldRazer
{

    template<typename T, size_t sides>
    struct ldPolygon
    {
        using type = T;
        using vector_t = ldRazerVec2<T>;
        using transform_t = ldMat3<T>;

        ldPolygon() = default;

        size_t getVertexCount() const
        {
            return vertices.size() + 1;
        }

        size_t getSides() const
        {
            return vertices.size();
        }

        ldPolygon transform(const transform_t &M) const
        {
            ldPolygon poly;
            std::transform(vertices.begin(), vertices.end(), poly.vertices.begin(), [&M](const vector_t &v)
            {
                return M.transform(v);
            });
            return poly;
        }

        void buildPolygon(T radius);

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
    void ldPolygon<T, sides>::buildPolygon(T radius)
    {
        const T delta = T(2) * PI < T >/(sides);
        for (size_t i = 0, end = sides; i != end; ++i) {
            T phi = i * delta;
            vertices[i].set(radius * std::cos(phi), radius * std::sin(phi));
        }
    }

}

#endif //LASERDOCKCORE_LDPOLYGON_H
