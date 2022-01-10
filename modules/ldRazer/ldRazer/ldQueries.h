//
// Created by otgaard on 2018/08/07.
//

#ifndef LASERDOCK_SIM_LDSERPENTQUERIES_H
#define LASERDOCK_SIM_LDSERPENTQUERIES_H

#include "ldRazer.h"
#include "ldBound.h"

#include <QDebug>

namespace ldRazer
{

    int8_t intersection(float u0, float u1, float v0, float v1, float parms[2]);

    // Note: This function returns parms between [0, 1] interpolated over the segment (P0.segment())
    int8_t intersection(const line2f_t &P0, const line2f_t &P1, float parms[2]);

    int8_t intersection(const vec2f &A0, const vec2f &A1, const vec2f &B0, const vec2f &B1, float parms[2]);

    int8_t intersection(const line2f_t &P0, const line2f_t &P1, vec2f points[2]);

    bool query(const line2f_t &line, const std::vector<line2f_t> &lines, line2f_t &result);

    size_t query(const line2f_t &line, const std::vector<line2f_t> &lines);

    std::vector<size_t> queryAll(const line2f_t &line, const line2f_arr_t &lines);

    // Separating Axis Tests

    void interval(const vec2f *vertices, size_t len, const vec2f &axis, float &tmin, float &tmax);

    bool findMinimumPenetration(const vec2f *A, float *taxis, size_t lenA, vec2f &N, float &t, int &idx);

    bool intervalIntersection(const vec2f *A, size_t lenA, const vec2f *B, size_t lenB, const vec2f &axis,
                              const vec2f &offset, const vec2f &velocity, const mat2f &rotation, float &tAxis,
                              float tMax);

    bool collide(const vec2f *A, size_t lenA, const vec2f &posA, const vec2f &velA, const mat2f &rotA,
                 const vec2f *B, size_t lenB, const vec2f &posB, const vec2f &velB, const mat2f &rotB,
                 vec2f &N, float &t);

    // Contact Calculation

    void projectToSegment(const vec2f &V, const vec2f &A, const vec2f &B, vec2f &d, float &t);

    vec2f transformToWorld(const vec2f &vert, const vec2f &P, const vec2f &V, const mat2f &rot, float t);

    int findSupports(const vec2f &N, float t, const vec2f *A, size_t lenA, const vec2f &PA, const vec2f &VA,
                     const mat2f &rot, vec2f *S);

    bool supportsToContacts(const vec2f &N, vec2f *S0, size_t lenS0, vec2f *S1, size_t lenS1, vec2f *C0, vec2f *C1,
                            int &count);

    bool findContacts(const vec2f *A, size_t lenA, const vec2f &posA, const vec2f &velA, const mat2f &rotA,
                      const vec2f *B, size_t lenB, const vec2f &posB, const vec2f &velB, const mat2f &rotB,
                      const vec2f &N, float t, vec2f *conA, vec2f *conB, int &count);

    bool intervalIntersection(const vec2f *A, size_t lenA, const vec2f *B, size_t lenB, const vec2f &axis,
                              float &tAxis);

    bool intersection(const AABB2f_t &aabb, const hull2f_t &hull);

}

#endif //LASERDOCK_SIM_LDSERPENTQUERIES_H
