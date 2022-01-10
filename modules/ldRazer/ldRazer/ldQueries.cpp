//
// Created by Darren Otgaar on 2018/09/10.
//

#include "ldQueries.h"

namespace ldRazer
{

    int8_t intersection(float u0, float u1, float v0, float v1, float parms[2])
    {
        if (u1 < v0 || u0 > v1) return 0;
        if (u1 > v0) {
            if (u0 < v1) {
                parms[0] = (u0 < v0) ? v0 : u0;
                parms[1] = (u1 > v1) ? v1 : u1;
                return 2;
            } else {
                parms[0] = u0;
                return 1;
            }
        } else {
            parms[0] = u1;
            return 1;
        }
    }

    int8_t intersection(const line2f_t &P0, const line2f_t &P1, float parms[2])
    {
        return intersection(P0.A, P0.B, P1.A, P1.B, parms);
    }

    int8_t intersection(const vec2f &A0, const vec2f &A1, const vec2f &B0, const vec2f &B1, float *parms)
    {
        static constexpr auto sqrEpsilon =
                std::numeric_limits<float>::epsilon() * std::numeric_limits<float>::epsilon();

        auto E = B0 - A0;
        const auto D0 = A1 - A0, D1 = B1 - B0;

        auto kr = kross(D0, D1);
        auto sqrKr = kr * kr;
        auto sqrLen0 = D0.lengthSq();
        auto sqrLen1 = D1.lengthSq();

        if (sqrKr > sqrEpsilon * sqrLen0 * sqrLen1) {
            auto s = kross(E, D1) / kr;
            if (s < 0 || s > 1) return 0;

            auto t = kross(E, D0) / kr;
            if (t < 0 || t > 1) return 0;

            parms[0] = s;
            return 1;
        }

        auto sqrLenE = E.lengthSq();
        kr = kross(E, D0);
        sqrKr = kr * kr;
        if (sqrKr > sqrEpsilon * sqrLen0 * sqrLenE) return 0;

        auto s0 = dot(D0, E) / sqrLen0, s1 = s0 + dot(D0, D1) / sqrLen0;
        auto sMin = std::min(s0, s1), sMax = std::max(s0, s1);
        int8_t iMax = intersection(0.f, 1.f, sMin, sMax, parms);
        return iMax;
    }

    int8_t intersection(const line2f_t &P0, const line2f_t &P1, vec2f points[2])
    {
        float parms[2];
        auto count = intersection(P0, P1, parms);
        for (auto i = 0; i != count; ++i) points[i] = P0.A + parms[i] * P0.segment();
        return count;
    }

// Returns the closest intersection between line and lines
    bool query(const line2f_t &line, const std::vector<line2f_t> &lines, line2f_t &result)
    {
        auto t = std::numeric_limits<float>::max();
        const line2f_t *linePtr = nullptr;
        float parms[2];
        for (const auto &l : lines) {
            int8_t count = intersection(line, l, parms);
            if (count == 0) continue;
            if (count > 0 && parms[0] < t) {
                t = parms[0];
                linePtr = &l;
            }
            if (count > 1 && parms[1] < t) {
                t = parms[1];
                linePtr = &l;
            }
        }

        if (linePtr) result = *linePtr;
        return linePtr != nullptr;
    }

    size_t query(const line2f_t &line, const std::vector<line2f_t> &lines)
    {
        auto t = std::numeric_limits<float>::max();
        size_t idx = size_t(-1);
        float parms[2];
        for (size_t i = 0, end = lines.size(); i != end; ++i) {
            auto count = intersection(line, lines[i], parms);
            if (count == 0) continue;
            if (count > 0 && parms[0] < t) {
                t = parms[0];
                idx = i;
            }
            if (count > 1 && parms[1] < t) {
                t = parms[1];
                idx = i;
            }
        }

        return idx;
    }

    std::vector<size_t> queryAll(const line2f_t &line, const line2f_arr_t &lines)
    {
        std::vector<size_t> result;
        float parms[2];
        for (size_t i = 0, end = lines.size(); i != end; ++i) {
            auto count = intersection(line, lines[i], parms);
            if (count > 0) result.emplace_back(i);
        }
        return result;
    }

/*
 * SAT CODE
 */

    void interval(const vec2f *vertices, size_t len, const vec2f &axis, float &tmin, float &tmax)
    {
        tmin = tmax = dot(vertices[0], axis);
        for (size_t i = 1; i != len; ++i) {
            const auto d = dot(vertices[i], axis);
            if (d < tmin) tmin = d;
            else if (d > tmax) tmax = d;
        }
    }

    bool findMinimumPenetration(vec2f *A, float *taxis, size_t lenA, vec2f &N, float &t, int &idx)
    {
        idx = -1;
        t = 0.f;
        for (size_t i = 0; i != lenA; ++i) {
            if (taxis[i] > 0 && taxis[i] > t) {
                idx = static_cast<int>(i);
                t = taxis[i];
                N = normalise(A[i]);
            }
        }

        if (idx != -1) return true;

        for (size_t i = 0; i != lenA; ++i) {
            float inv_n = 1.f / A[i].length();
            A[i] *= inv_n;
            taxis[i] *= inv_n;

            if (taxis[i] > t || idx == -1) {
                idx = static_cast<int>(i);
                t = taxis[i];
                N = A[i];
            }
        }

        if (idx == -1) assert(false && "Error");
        return idx != -1;
    }

    bool intervalIntersection(const vec2f *A, size_t lenA, const vec2f *B, size_t lenB, const vec2f &axis, float &tAxis)
    {
        float min0, max0;
        float min1, max1;
        interval(A, lenA, axis, min0, max0);
        interval(B, lenB, axis, min1, max1);
        auto d0 = min0 - max1, d1 = min1 - max0;
        if (d0 > 0.f || d1 > 0.f) return false;
        tAxis = (d0 > d1) ? d0 : d1;
        return true;
    }

    bool
    intervalIntersection(const vec2f *A, size_t lenA, const vec2f *B, size_t lenB, const vec2f &axis,
                         const vec2f &offset,
                         const vec2f &velocity, const mat2f &rotation, float &tAxis, float tMax)
    {
        float min0, max0;
        float min1, max1;

        interval(A, lenA, rotation ^ axis, min0, max0);
        interval(B, lenB, axis, min1, max1);
        auto h = dot(offset, axis);
        min0 += h;
        max0 += h;

        auto d0 = min0 - max1, d1 = min1 - max0;
        if (d0 > 0.f || d1 > 0.f) {
            auto v = dot(velocity, axis);
            if (isZero(v)) return false;
            auto t0 = -d0 / v, t1 = d1 / v;
            if (t0 > t1) std::swap(t0, t1);
            tAxis = t0 > 0.f ? t0 : t1;
            return !(tAxis < 0.f || tAxis > tMax);
        } else {
            tAxis = (d0 > d1) ? d0 : d1;
            return true;
        }
    }

    bool intersection(const vec2f *A, size_t lenA, const vec2f & /*posA*/, const vec2f *B, size_t lenB, const vec2f & /*posB*/)
    {
        if (lenA <= 2 || lenB <= 2) assert(false && "HullVsHull or HullVsAABB only");

//        const size_t MAX = 50;
//        const float epsilon = .001f;
        float tAxis = 0.f;

        for (size_t i = 0, j = lenA - 1; i != lenA; j = i, ++i) {
            auto seg = perp(A[i] - A[j]);
            if (!intervalIntersection(A, lenA, B, lenB, seg, tAxis)) return false;
        }

        for (size_t i = 0, j = lenB - 1; i != lenB; j = i, ++i) {
            auto seg = perp(B[i] - B[j]);
            if (!intervalIntersection(A, lenA, B, lenB, seg, tAxis)) return false;
        }

        return true;
    }

    bool collide(const vec2f *A, size_t lenA, const vec2f &posA, const vec2f &velA, const mat2f &rotA, const vec2f *B,
                 size_t lenB, const vec2f &posB, const vec2f &velB, const mat2f &rotB, vec2f &N, float &t)
    {
        const size_t MAX = 50;
//        const float epsilon = .001f;
        if (lenA < 2 && lenB < 2) return false;

        auto TrotB = transpose(rotB);
        mat2f rot = TrotB * rotA;
        vec2f offset = TrotB * (posA - posB);
        vec2f velocity = TrotB * (velA - velB);

        vec2f axes[MAX];
        float taxes[MAX];
        float disp[MAX];
        size_t count = 0;

        if (lenA == 2 || lenB == 2) {
            if (lenA == 2) {
                auto seg = rot * perp(A[1] - A[0]);
                disp[count] = dot(offset, seg);
                axes[count] = disp[count] < 0 ? -seg : seg;
                if (!intervalIntersection(A, lenA, B, lenB, axes[count], offset, velocity, rot, taxes[count], t)) {
                    return false;
                }
                count++;
            }

            if (lenB == 2) {
                auto seg = perp(B[1] - B[0]);
                disp[count] = dot(offset, seg);
                axes[count] = disp[count] < 0 ? -seg : seg;
                if (!intervalIntersection(A, lenA, B, lenB, axes[count], offset, velocity, rot, taxes[count], t)) {
                    return false;
                }
                count++;
            }
        }

        auto velLenSq = velocity.lengthSq();
        if (!isZero(velLenSq, .001f)) {
            axes[count] = perp(velocity);
            if (!intervalIntersection(A, lenA, B, lenB, axes[count], offset, velocity, rot, taxes[count], t)) {
                return false;
            }
            count++;
        }

        // Test all axes of A
        if (lenA > 2) {
            for (size_t i = 0, j = lenA - 1; i != lenA; j = i, ++i) {
                auto seg = rot * perp(A[i] - A[j]);
                axes[count] = seg;
                if (!intervalIntersection(A, lenA, B, lenB, axes[count], offset, velocity, rot, taxes[count], t)) {
                    return false;
                }
                count++;
            }
        }

        // Test all axes of B
        if (lenB > 2) {
            for (size_t i = 0, j = lenB - 1; i != lenB; j = i, ++i) {
                auto seg = perp(B[i] - B[j]);
                axes[count] = seg;
                if (!intervalIntersection(A, lenA, B, lenB, axes[count], offset, velocity, rot, taxes[count], t)) {
                    return false;
                }
                count++;
            }
        }

        /*
        qDebug() << "******************************************************";
        qDebug() << "Printing Intersections:";
        for(auto i = 0; i != count; ++i) {
            qDebug() << "Count:" << i;
            qDebug() << "Axis:" << axes[i].x << axes[i].y << "t:" << taxes[i] << "disp:" << disp[i];
        }
        qDebug() << "\n";
        */
        // Intersection found, return penetration depth
        int idx = -1;
        if (!findMinimumPenetration(axes, taxes, count, N, t, idx)) return false;

        //qDebug() << "Axis to push:" << idx << "x:" << axes[idx].x << "y:" << axes[idx].y;

        if (dot(N, offset) < 0.f) N *= -1.f;
        N = rotB * N;

        /*
        qDebug() << "Push Vector";
        qDebug() << "N:" << N.x << N.y << "t:" << t << "\n\n\n";
        */

        return true;
    }

    void projectToSegment(const vec2f &V, const vec2f &A, const vec2f &B, vec2f &d, float &t)
    {
        auto AV = V - A;
        auto AB = B - A;
        t = clamp(dot(AV, AB) / AB.lengthSq(), 0.f, 1.f);
        d = A + t * AB;
    }

    vec2f transformToWorld(const vec2f &vert, const vec2f &P, const vec2f &V, const mat2f &rot, float t)
    {
        auto T = P + (rot * vert);
        if (t > 0.f) T += t * V;
        return T;
    }

    int
    findSupports(const vec2f &N, float t, const vec2f *A, size_t lenA, const vec2f &PA, const vec2f &VA,
                 const mat2f &rot,
                 vec2f *S)
    {
        const int MAX = 50;
        auto norm = transpose(rot) * N;
        float d[MAX], dmin;
        dmin = d[0] = dot(A[0], norm);
        for (size_t i = 1; i != lenA; ++i) {
            d[i] = dot(A[i], norm);
            if (d[i] < dmin) dmin = d[i];
        }

        int count = 0;
        const float epsilon = 0.001f;
        float s[2];
        bool sign = false;

        auto nPerp = perp(norm);
        for (size_t i = 0; i != lenA; ++i) {
            if (d[i] < dmin + epsilon) {
                auto contact = transformToWorld(A[i], PA, VA, rot, t);
                auto c = dot(contact, nPerp);
                if (count < 2) {
                    s[count] = c;
                    S[count] = contact;
                    count++;

                    if (count > 1) sign = (s[1] > s[0]);
                } else {
                    auto &min = sign ? s[0] : s[1];
                    auto &max = sign ? s[1] : s[0];
                    auto &Min = sign ? S[0] : S[1];
                    auto &Max = sign ? S[1] : S[0];

                    if (c < min) {
                        min = c;
                        Min = contact;
                    } else if (c > max) {
                        max = c;
                        Max = contact;
                    }
                }
            }
        }

        return count;
    }

    bool
    supportsToContacts(const vec2f &N, vec2f *S0, size_t lenS0, vec2f *S1, size_t lenS1, vec2f *C0, vec2f *C1,
                       int &count)
    {
        count = 0;

        if (lenS0 == 0 || lenS1 == 0) return false;
        if (lenS0 == 1 && lenS1 == 1) {  // Point
            C0[count] = S0[0];
            C1[count] = S1[0];
            count++;
            return true;
        }

        auto nPerp = perp(N);
        auto min0 = dot(S0[0], nPerp), max0 = min0;
        auto min1 = dot(S1[0], nPerp), max1 = min1;

        if (lenS0 == 2) {
            max0 = dot(S0[1], nPerp);
            if (max0 < min0) {
                std::swap(min0, max0);
                std::swap(S0[0], S0[1]);
            }
        }

        if (lenS1 == 2) {
            max1 = dot(S1[1], nPerp);
            if (max1 < min1) {
                std::swap(min1, max1);
                std::swap(S1[0], S1[1]);
            }
        }

        if (min0 > max1 || min1 > max0) return false;

        if (min0 > min1) {
            vec2f seg;
            float parm;
            projectToSegment(S0[0], S1[0], S1[1], seg, parm);
            C0[count] = S0[0];
            C1[count] = seg;
            count++;
        } else {
            vec2f seg;
            float parm;
            projectToSegment(S1[0], S0[0], S0[1], seg, parm);
            C0[count] = seg;
            C1[count] = S1[0];
            count++;
        }

        if (!cmpf(max0, min0) && !cmpf(max1, min1)) {
            if (max0 < max1) {
                vec2f seg;
                float parm;
                projectToSegment(S0[1], S1[0], S1[1], seg, parm);
                C0[count] = S0[1];
                C1[count] = seg;
                count++;
            } else {
                vec2f seg;
                float parm;
                projectToSegment(S1[1], S0[0], S0[1], seg, parm);
                C0[count] = seg;
                C1[count] = S1[1];
                count++;
            }
        }

        return true;
    }

    bool
    findContacts(const vec2f *A, size_t lenA, const vec2f &posA, const vec2f &velA, const mat2f &rotA, const vec2f *B,
                 size_t lenB, const vec2f &posB, const vec2f &velB, const mat2f &rotB, const vec2f &N, float t,
                 vec2f *conA, vec2f *conB, int &count)
    {
        vec2f S0[4];
        vec2f S1[4];

        int lenS0 = findSupports(N, t, A, lenA, posA, velA, rotA, S0);
        int lenS1 = findSupports(-N, t, B, lenB, posB, velB, rotB, S1);
        if (!supportsToContacts(N, S0, static_cast<size_t>(lenS0), S1, static_cast<size_t>(lenS1), conA, conB, count)) return false;
        return true;
    }

    bool intersection(const AABB2f_t &aabb, const hull2f_t &hull)
    {
        auto Averts = aabb.getVertices();
        auto &Bverts = hull.vertices;
        return intersection(Averts.data(), Averts.size(), vec2f{0.f, 0.f}, Bverts.data(), Bverts.size(),
                            vec2f{0.f, 0.f});
    }


}
