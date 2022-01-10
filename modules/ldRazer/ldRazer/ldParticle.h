//
// Created by Darren Otgaar on 2018/08/23.
//

#ifndef LASERDOCK_SIM_LDSERPENTPARTICLE_H
#define LASERDOCK_SIM_LDSERPENTPARTICLE_H

#include "ldRazer.h"
#include "ldMesh.h"

#include <QDebug>

namespace ldRazer
{

    enum particleType
    {
        PT_NONE = 0,
        PT_GRAV = 1 << 0,           // Apply gravity
        PT_ACCEL = 1 << 1,          // Apply acceleration
        PT_ANG = 1 << 2,            // Angular velocity
        PT_GROUND_CLIP = 1 << 3,    // Clip with ground
        PT_GROUND_COLL = 1 << 4,    // Collide with ground
        PT_DAMPING = 1 << 5         // Apply damping
    };

    template<size_t MaxCount>
    struct ldParticleSystem
    {
        std::array<bool, MaxCount> alive;
        std::array<uint32_t, MaxCount> type;
        std::array<vec2f, MaxCount> curr_position;
        std::array<vec2f, MaxCount> prev_position;
        std::array<float, MaxCount> orn;
        std::array<vec2f, MaxCount> vel;
        std::array<vec2f, MaxCount> acc;
        std::array<float, MaxCount> ang;
        std::array<float, MaxCount> age;
        std::array<float, MaxCount> damping;
        std::array<float, MaxCount> restitution;
        std::array<float, MaxCount> maxAge;
        std::array<float, MaxCount> maxInvAge;
        std::array<uint32_t, MaxCount> curr_colour;
        std::array<uint32_t, MaxCount> prev_colour;
        std::array<uint32_t, MaxCount> sourceColour;
        std::array<uint32_t, MaxCount> targetColour;

        ldRandom<float> rnd;

        vec2f gravity = vec2f{0.f, -9.8f};
        vec2f groundP = vec2f{0.f, 0.f}, groundN = vec2f{0.f, 1.f};

        void clear()
        {
            for (size_t i = 0; i != alive.size(); ++i) alive[i] = false;
        }

        void update(float dt)
        {
            for (size_t i = 0; i != MaxCount; ++i) {
                if (!alive[i]) continue;

                // The damping ratio
                auto zeta = (type[i] & PT_DAMPING) ? damping[i] : 1.f;
                prev_position[i] = curr_position[i];
                curr_position[i] += dt * zeta * vel[i];
                if (type[i] & PT_ACCEL) vel[i] += dt * zeta * acc[i];
                if (type[i] & PT_GRAV) vel[i] += dt * zeta * gravity;
                if (type[i] & PT_ANG) orn[i] += dt * zeta * ang[i];
                age[i] += dt;
                prev_colour[i] = curr_colour[i];
                curr_colour[i] = lerpColour(sourceColour[i], targetColour[i], age[i] * maxInvAge[i]);
                if (type[i] & PT_GROUND_CLIP) alive[i] = dot(normalise(curr_position[i] - groundP), groundN) > 0.f;
                if ((type[i] & PT_GROUND_COLL) && dot(normalise(curr_position[i] - groundP), groundN) <= 0)
                    vel[i] = restitution[i] *
                             (vel[i] - 2.f * (dot(vel[i], groundN)) * groundN);    // Reflect the velocity
                if (age[i] > maxAge[i]) alive[i] = false;
            }
        }

        void draw(ldMesh2f_t &mesh)
        {
            for (size_t i = 0; i != MaxCount; ++i) {
                if (!alive[i]) continue;
                const auto vidx = mesh.vertices.size();
                mesh.vertices.emplace_back(ldVertex2f_t{prev_position[i], prev_colour[i]});
                mesh.vertices.emplace_back(ldVertex2f_t{curr_position[i], curr_colour[i]});
                mesh.indices.emplace_back(ldLineStrip(vidx, 2));
            }
        }

        void draw(ldMesh2f_t &mesh, const mat3f &T)
        {
            for (size_t i = 0; i != MaxCount; ++i) {
                if (!alive[i]) continue;
                const auto vidx = mesh.vertices.size();
                mesh.vertices.emplace_back(ldVertex2f_t{transform(T, prev_position[i]), prev_colour[i]});
                mesh.vertices.emplace_back(ldVertex2f_t{transform(T, curr_position[i]), curr_colour[i]});
                mesh.indices.emplace_back(ldLineStrip(vidx, 2));
            }
        }

        // Used to generate a position in an area or on a line to easily restrict where the systems are created.
        vec2f generatePosition(const AABB2f_t &area)
        {
            return vec2f{
                    rnd.random1(area.min().x, area.max().x),
                    rnd.random1(area.min().y, area.max().y)
            };
        }

        vec2f generatePosition(const disc2f_t &area)
        {
            return area.centre +
                   area.radius * rnd.random1() * normalise(vec2f{rnd.random1(-1.f, 1.f), rnd.random1(-1.f, 1.f)});
        }

        vec2f generatePosition(const line2f_t &line)
        {
            return line.A + rnd.random1() * line.segment();
        }

        vec2f generateDirection(const vec2f &dir, float radius, float angle)
        {
//            const float len = rnd.random1(.33f, 1.f) * radius;
            const float d = computeAngle(dir);
            const float phi = wrap(d + (rnd.random1() < .5f ? -1.f : +1.f) * rnd.random1(0.f, angle / 2), 0.f, M_2PIf);
            return polarToCartesian(phi, radius);
        }

        ldVertex2f_t generateVertex(const ldVertex2f_t &A, const ldVertex2f_t &B)
        {
            auto p = rnd.random1();
            return ldVertex2f_t{A.position + p * (B.position - A.position), lerpColour(A.colour, B.colour, p)};
        }

        vec2f generatePosition(const ldMesh2f_t &segs)
        {
            auto idx = int(rnd.random1(0.f, segs.indices.size()));
            auto off = segs.indices[idx].start + int(rnd.random1(0, segs.indices[idx].count - 1));
            return generatePosition(line2f_t(segs.vertices[off].position, segs.vertices[off + 1].position));
        }

        ldVertex2f_t generateVertex(const ldMesh2f_t &segs)
        {
            auto idx = int(rnd.random1(0.f, segs.indices.size()));
            auto off = segs.indices[idx].start + int(rnd.random1(0, segs.indices[idx].count - 1));
            return generateVertex(segs.vertices[off], segs.vertices[off + 1]);
        }

        template<typename BoundT>
        bool createStarBurst(const BoundT &area)
        {
            auto count = std::count(alive.begin(), alive.end(), false);
            if (count < 4) return false;       // At least 4 lines

            const auto lP = generatePosition(area);

            const auto colA = randomColour();
            const auto colB = randomColour();

            size_t offset = 0;
            for (size_t i = 0, end = std::min<size_t>(count, size_t(rnd.random1(8.f, 32.f))); i != end; ++i) {
                auto it = std::find(alive.begin() + offset, alive.end(), false);
                if (it == alive.end()) break;

                offset = std::distance(alive.begin(), it);
                type[offset] = PT_GRAV | PT_GROUND_COLL | PT_DAMPING;
                prev_position[offset] = lP;
                vel[offset] = normalise(rnd.random2(-1.f, 1.f));
                orn[offset] = computeAngle(vel[offset], .0001f);
                vel[offset] *= 5.f;
                curr_position[offset] = lP + .016f * vel[offset];
                age[offset] = .16f;
                damping[offset] = .95f;
                restitution[offset] = .5f;
                maxAge[offset] = rnd.random1(.5f, 2.f);
                maxInvAge[offset] = 1.f / maxAge[offset];
                sourceColour[offset] = colA;
                targetColour[offset] = colB;
                prev_colour[offset] = sourceColour[offset];
                curr_colour[offset] = lerpColour(sourceColour[offset], targetColour[offset], .16f);
                alive[offset] = true;
            }

            return true;
        }

        // Create Sparks on line segments
        bool createSparks(const ldMesh2f_t &segs)
        {
            auto count = std::count(alive.begin(), alive.end(), false);
            if (count < 2) return false;       // At least 4 lines

            const auto vert = generateVertex(segs);

            size_t offset = 0;
            for (size_t i = 0, end = std::min<size_t>(count, size_t(rnd.random1(2.f, 6.f))); i != end; ++i) {
                auto it = std::find(alive.begin() + offset, alive.end(), false);
                if (it == alive.end()) break;

                offset = std::distance(alive.begin(), it);
                type[offset] = PT_DAMPING;
                prev_position[offset] = vert.position;
                vel[offset] = normalise(rnd.random2(-1.f, 1.f));
                orn[offset] = computeAngle(vel[offset], .0001f);
                vel[offset] *= 5.f;
                curr_position[offset] = vert.position + .016f * vel[offset];
                age[offset] = .016f;
                damping[offset] = .9f;
                restitution[offset] = .3f;
                maxAge[offset] = rnd.random1(.1f, .2f);
                maxInvAge[offset] = 1.f / maxAge[offset];
                sourceColour[offset] = vert.colour;
                targetColour[offset] = makeARGB(255, 255, 0);
                prev_colour[offset] = sourceColour[offset];
                curr_colour[offset] = lerpColour(sourceColour[offset], targetColour[offset], .016f);
                alive[offset] = true;
            }

            return true;
        }

        template<typename BoundT>
        bool
        createTopDownExplosion(const BoundT &bound, float radius, const vec2f &dir = {1.f, 0.f}, float angle = M_2PIf)
        {
            auto count = std::count(alive.begin(), alive.end(), false);
            if (count < 2) return false;

            size_t offset = 0;
            for (size_t i = 0, end = std::min<size_t>(count, size_t(rnd.random1(5.f, 10.f))); i != end; ++i) {
                auto it = std::find(alive.begin() + offset, alive.end(), false);
                if (it == alive.end()) break;

                offset = std::distance(alive.begin(), it);
                type[offset] = PT_DAMPING;
                prev_position[offset] = generatePosition(bound);
                vel[offset] = generateDirection(dir, radius, angle);
                orn[offset] = computeAngle(normalise(vel[offset]), .0001f);
                curr_position[offset] = prev_position[offset] + .016f * vel[offset];
                age[offset] = .0f;
                damping[offset] = .5f;
                restitution[offset] = .3f;
                maxAge[offset] = rnd.random1(.5f, 2.f);
                maxInvAge[offset] = 1.f / maxAge[offset];

                sourceColour[offset] = makeARGB(255, 0, 0);
                targetColour[offset] = makeARGB(255, 255, 0);
                prev_colour[offset] = sourceColour[offset];
                curr_colour[offset] = lerpColour(sourceColour[offset], targetColour[offset], .016f);
                alive[offset] = true;
            }

            return true;
        }

        template<typename BoundT>
        bool createSmoke(const BoundT &bound, float radius, const vec2f &dir = {1.f, 0.f}, float angle = M_2PIf)
        {
            auto count = std::count(alive.begin(), alive.end(), false);
            if (count < 2) return false;

            const auto sredRange = vec2s{60, 100}, sgreenRange = vec2s{60, 100}, sblueRange = vec2s{60, 100};
            const auto eredRange = vec2s{30, 60}, egreenRange = vec2s{30, 60}, eblueRange = vec2s{30, 60};

            size_t offset = 0;
            for (size_t i = 0, end = std::min<size_t>(count, size_t(rnd.random1(2.f, 4.f))); i != end; ++i) {
                auto it = std::find(alive.begin() + offset, alive.end(), false);
                if (it == alive.end()) break;

                offset = std::distance(alive.begin(), it);
                type[offset] = PT_DAMPING;
                prev_position[offset] = generatePosition(bound);
                vel[offset] = generateDirection(dir, radius, angle);
                orn[offset] = computeAngle(normalise(vel[offset]), .001f);
                curr_position[offset] = prev_position[offset] + .016f * vel[offset];
                age[offset] = .0f;
                damping[offset] = .5f;
                ang[offset] = rnd.random1();
                restitution[offset] = .3f;
                maxAge[offset] = rnd.random1(.2f, .8f);
                maxInvAge[offset] = 1.f / maxAge[offset];

                sourceColour[offset] = randomColour(sredRange, sgreenRange, sblueRange);
                targetColour[offset] = randomColour(eredRange, egreenRange, eblueRange);
                prev_colour[offset] = sourceColour[offset];
                curr_colour[offset] = lerpColour(sourceColour[offset], targetColour[offset], .016f);
                alive[offset] = true;
            }

            return true;
        }
    };

}

#endif //LASERDOCK_SIM_LDSERPENTPARTICLE_H
