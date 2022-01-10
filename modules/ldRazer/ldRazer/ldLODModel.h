//
// Created by Darren Otgaar on 2018/10/11.
//

#ifndef LASERDOCKCORE_LDLODMODEL_H
#define LASERDOCKCORE_LDLODMODEL_H

#include "ldMesh.h"
#include <QDebug>

// The LOD model allows multiple models to be used for representing a model and selecting the appropriate model at
// runtime using a number of different methods (but mainly distance from the camera).

namespace ldRazer
{

    struct ldLODModel
    {
        struct LODLevel
        {
            float selector = 0.f;
            size_t vertexStart = 0;
            size_t vertexCount = 0;
            size_t indexStart = 0;
            size_t indexCount = 0;

            LODLevel(float pSelector, size_t vtxStart, size_t vtxCount, size_t idxStart, size_t idxCount)
                    : selector(pSelector), vertexStart(vtxStart), vertexCount(vtxCount), indexStart(idxStart),
                      indexCount(idxCount)
            {
            }
        };

        ldLODModel() = default;

        bool isEmpty() const
        {
            return m_selector.empty();
        }

        void clear()
        {
            m_levels.clear();
            m_selector.clear();
            m_mesh.indices.clear();
            m_mesh.vertices.clear();
        }

        // Append an entire mesh level consisting of multiple lines
        bool appendMesh(float max, const ldMesh2f_t &mesh)
        {
            if (!m_selector.empty() && max <= m_selector.back()) return false;

            m_levels.emplace_back(max, m_mesh.vertices.size(), mesh.vertices.size(), m_mesh.indices.size(),
                                  mesh.indices.size());
            auto &lvl = m_levels.back();

            auto rhs_indices = mesh.indices;
            auto offset = lvl.vertexStart;
            for (auto &idx : rhs_indices) idx.start += offset;
            std::copy(rhs_indices.begin(), rhs_indices.end(), std::back_inserter(m_mesh.indices));
            std::copy(mesh.vertices.begin(), mesh.vertices.end(), std::back_inserter(m_mesh.vertices));
            m_selector.emplace_back(max);

            return true;
        }

        bool appendMesh(float max, const std::vector<ldVertex2f_t> &mesh)
        {
            if (!m_selector.empty() && max <= m_selector.back()) return false;

            m_levels.emplace_back(max, m_mesh.vertices.size(), mesh.size(), m_mesh.indices.size(), 1);

            const auto offset = m_mesh.vertices.size();
            m_mesh.indices.emplace_back(ldLineStrip(offset, mesh.size()));
            std::copy(mesh.begin(), mesh.end(), std::back_inserter(m_mesh.vertices));
            m_selector.emplace_back(max);
            return true;
        }

        size_t selectMesh(float distance, const mat3f &T, ldMesh2f_t &mesh, uint32_t colour = 0x00000000) const
        {
            auto idx = findInterval(m_selector, distance);
            if (idx != size_t(-1)) {
                auto lvl = m_levels[idx];
                const auto offset = mesh.vertices.size();
                const auto start = m_mesh.vertices.begin() + lvl.vertexStart;
                const auto end = start + lvl.vertexCount;

                mesh.vertices.resize(offset + lvl.vertexCount);
                std::transform(start, end, mesh.vertices.begin() + offset, [&](const ldVertex2f_t &v)
                {
                    return ldVertex2f_t{transform(T, v.position), colour == 0 ? v.colour : colour};
                });

                for (size_t i = lvl.indexStart, iend = lvl.indexStart + lvl.indexCount; i != iend; ++i) {
                    const auto &index = m_mesh.indices[i];
                    mesh.indices.emplace_back(uint32_t(index.start - lvl.vertexStart + offset), index.count);
                }
            }
            return idx;
        }

        std::vector<float> m_selector;
        std::vector<LODLevel> m_levels;
        ldMesh2f_t m_mesh;
    };

}

#endif //LASERDOCKCORE_LDLODMODEL_H
