//
// Created by Darren Otgaar on 2018/08/17.
//

#ifndef LASERDOCK_SIM_LDRENDERBATCH_H
#define LASERDOCK_SIM_LDRENDERBATCH_H

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

        ldLineStrip() = default;

        ldLineStrip(uint32_t start, uint32_t count) : start(start), count(count)
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
    };

    template<typename MeshT>
    struct ldRenderBatch
    {
    public:
        ldRenderBatch(size_t reserve = 200, size_t reserve_indices = 20)
        {
            m_mesh.vertices.reserve(reserve);
            m_mesh.indices.reserve(reserve_indices);
        }

        ~ldRenderBatch() = default;

        ldLineStrip addStrip(uint32_t count)
        {
            ldLineStrip strip(m_mesh.vertices.size(), count);
            m_mesh.vertices.resize(m_mesh.vertices.size() + count);
            m_mesh.indices.push_back(strip);
            return strip;
        }

        MeshT m_mesh;
    };

    // An algorithm to combine all line strips in a set of line segments in a mesh
    // Returns number of strips (i.e. draw calls olBegin())
    inline size_t mergeSegments(const linecolf_arr_t &segs, ldMesh2f_t &mesh)
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
            mesh.indices.emplace_back(ldLineStrip(mesh.vertices.size(), 0));
            for (const auto &l : strip) {
                mesh.vertices.emplace_back(ldVertex2f_t{l.first.A, l.second});
            }
            mesh.vertices.emplace_back(ldVertex2f_t{strip.back().first.B, strip.back().second});
            mesh.indices.back().count = mesh.vertices.size() - mesh.indices.back().start;
        }

        return mesh.indices.size() - startCount;
    }

}

#endif //LASERDOCK_SIM_LDRENDERBATCH_H
