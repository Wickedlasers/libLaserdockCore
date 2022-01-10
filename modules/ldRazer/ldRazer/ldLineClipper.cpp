//
// Created by Darren Otgaar on 2018/10/19.
//

#include "ldLineClipper.h"
#include <QDebug>

using namespace ldRazer;

ldMesh2f_t ldRazer::clipMesh(const ldMesh2f_t &mesh, const AABB2f_t &bound, bool strict)
{
    linecolf_arr_t clipped;
    line2f_t line;

    for (const auto &strip : mesh.indices) {
        const size_t end = strip.start + strip.count;
        for (size_t i = strip.start, j = strip.start + 1; j != end; i = j, ++j) {
            if (lineClipper(line2f_t{mesh.vertices[i].position, mesh.vertices[j].position}, bound, line)) {
                clipped.emplace_back(line, mesh.vertices[i].colour);
            }
        }
    }

    ldMesh2f_t newMesh;
    mergeSegments(clipped, newMesh, strict);
    return newMesh;
}
