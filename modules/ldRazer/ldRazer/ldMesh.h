//
// Created by Darren Otgaar on 2018/08/17.
//

#ifndef LASERDOCK_SIM_LDRENDERBATCH_H
#define LASERDOCK_SIM_LDRENDERBATCH_H

#include <algorithm>
#include <deque>
#include "ldRazer.h"

namespace ldRazer
{

// The ldRenderBatch is responsible for holding vertices (consisting of position and colour (uint32_t)) and indices to
// store line strips (the optimal output type for the laser projector).  Gather everything to be rendered into a single
// batch and pass it to the output engine in one go - less processor stalls, less random pauses, easier to add effects.

    struct ldLineStrip
    {
        uint32_t start;
        uint32_t count;
        bool strict;

        ldLineStrip() = default;

        ldLineStrip(uint32_t pStart, uint32_t pCount, bool pStrict = false) : start(pStart), count(pCount), strict(pStrict)
        {
        }
    };

    struct ldVertex2s_t
    {
        vec2s position;
        uint32_t colour;
    };

    struct ldVertex2f_t
    {
        vec2f position;
        uint32_t colour;
    };

    struct ldMesh2s_t
    {
        std::vector<ldVertex2s_t> vertices;
        std::vector<ldLineStrip> indices;
    };

    struct ldMesh2f_t
    {
        std::vector<ldVertex2f_t> vertices;
        std::vector<ldLineStrip> indices;

        void clear()
        {
            vertices.clear();
            indices.clear();
        }

        ldMesh2f_t &operator+=(const ldMesh2f_t &rhs)
        {
            auto rhs_indices = rhs.indices;
            auto offset = vertices.size();
            for (auto &idx : rhs_indices) idx.start += offset;
            std::copy(rhs_indices.begin(), rhs_indices.end(), std::back_inserter(indices));
            std::copy(rhs.vertices.begin(), rhs.vertices.end(), std::back_inserter(vertices));
            return *this;
        }

        template<typename Fnc>
        void for_each(Fnc &&fnc)
        {
            std::for_each(vertices.begin(), vertices.end(), fnc);
        }

        template<typename Fnc>
        ldMesh2f_t transform(Fnc &&fnc) const
        {
            ldMesh2f_t ret;
            ret.vertices = std::vector<ldVertex2f_t>(vertices.size());
            ret.indices = indices;
            std::transform(vertices.begin(), vertices.end(), ret.vertices.begin(), fnc);
            return ret;
        }

        void centreMesh()
        {
            vec2f centre = vec2f{0.f, 0.f};
            for (const auto &p : vertices) {
                centre += p.position;
            }

            centre /= vertices.size();

            for (auto &p : vertices) {
                p.position -= centre;
            }
        }
    };

    // An algorithm to combine all line strips in a set of line segments in a mesh
    // Returns number of strips (i.e. draw calls olBegin())
    inline size_t mergeSegments(const linecolf_arr_t &segs, ldMesh2f_t &mesh, bool strict = false)
    {
        constexpr static auto epsilon = .0001f;

        auto sorted = segs;

        // Order points
        for (auto &l : sorted) if (l.first.B < l.first.A) std::swap(l.first.A, l.first.B);

        std::sort(sorted.begin(), sorted.end(), [](const linecolf_t &A, const linecolf_t &B)
        {
            return lessThan(A.first.A, B.first.A, epsilon)
                   || (eq(A.first.A, B.first.A, epsilon) && lessThan(A.first.B, B.first.B, epsilon));
        });

        std::vector<std::deque<linecolf_t>> strips;
        for (size_t i = 0; i != sorted.size(); ++i) {
            auto &line = sorted[i];
            bool assigned = false;
            for (auto &strip : strips) {
                if (eq(strip.back().first.B, line.first.A, epsilon)) {   // Concatenate
                    if (eq(dot(strip.back().first.dir(), line.first.dir()), 1.f, epsilon)) { // Same direction, join
                        strip.back().first.B = line.first.B;
                        assigned = true;
                        break;
                    } else {
                        strip.push_back(line);
                        assigned = true;
                        break;
                    }
                } else if (eq(strip.front().first.A, line.first.A, epsilon)) { // Flip and concatenate
                    std::swap(line.first.A, line.first.B);
                    if (eq(dot(strip.front().first.dir(), line.first.dir()), 1.f, epsilon)) {
                        strip.front().first.A = line.first.A;
                        assigned = true;
                        break;
                    } else {
                        strip.push_front(line);
                        assigned = true;
                        break;
                    }
                }
            }

            if (!assigned) strips.push_back({line});
        }

        const size_t startCount = mesh.indices.size();

        for (const auto &strip : strips) {
            mesh.indices.emplace_back(ldLineStrip(static_cast<uint32_t>(mesh.vertices.size()), 0, strict));
            for (const auto &l : strip) {
                mesh.vertices.emplace_back(ldVertex2f_t{l.first.A, l.second});
            }
            mesh.vertices.emplace_back(ldVertex2f_t{strip.back().first.B, strip.back().second});
            mesh.indices.back().count = static_cast<uint32_t>(mesh.vertices.size() - mesh.indices.back().start);
        }

        return mesh.indices.size() - startCount;
    }

    // Applies a colour interpolation across a space
    inline void colourMesh(ldMesh2f_t &mesh, const line2f_t &line, uint32_t source, uint32_t target)
    {
        const auto len = line.segment().length(), inv_len = 1.f / len;
        mesh.for_each([&](ldVertex2f_t &v)
                      {
                          auto pP = projectToSegment(line, v.position);
                          auto l = (pP - line.A).length() * inv_len;
                          v.colour = lerpColour(source, target, l);
                      });
    }

    // Fades out to the colour (note, cannot modify the source because of lerp) p = [0, 1]
    inline ldMesh2f_t fadeOut(const ldMesh2f_t &sourceMesh, uint32_t colour, float p)
    {
        return sourceMesh.transform([&](const ldVertex2f_t &v)
                                    {
                                        auto copy = v;
                                        copy.colour = lerpColour(v.colour, colour, p);
                                        return copy;
                                    });
    }

    inline disc2f_t computeDisc(const std::vector<ldVertex2f_t> &mesh)
    {
        using ldRazer::vec2f;

        const vec2f *xmin = &mesh[0].position, *xmax = xmin;
        const vec2f *ymin = xmin, *ymax = ymin;

        vec2f centre{0.f, 0.f};
        for (const auto &v : mesh) {
            centre += v.position;
            if (v.position.x < xmin->x) xmin = &v.position;
            if (v.position.x > xmax->x) xmax = &v.position;
            if (v.position.y < ymin->y) ymin = &v.position;
            if (v.position.y > ymax->y) ymax = &v.position;
        }
        centre /= mesh.size();

        auto A = (*xmax - *xmin).length(), B = (*ymax - *ymin).length();
        return ldRazer::disc2f_t{centre, A > B ? A : B};
    }

}

#endif //LASERDOCK_SIM_LDRENDERBATCH_H
