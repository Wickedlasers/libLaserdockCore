//
// Created by Darren Otgaar on 2018/09/23.
//

#ifndef LASERDOCKCORE_LDWORLD_H
#define LASERDOCKCORE_LDWORLD_H

// Defines a set of common interfaces for getting the physics engine, players (including AI), and map/level structure.

#include "ldMesh.h"
#include "ldRazer.h"
#include "ldBound.h"
#include "ldPhysicsEngine.h"

namespace ldRazer
{

// Any static object such as walls, boundaries, trees, obstacles.  May be points, lines, discs, or hull bound with
// LOD rendering options, for now, just render the bound

    struct WorldObject
    {
        enum ObjectType
        {
            WO_NONE,
            WO_STATIC,
            WO_DYNAMIC
        } type = WO_NONE;
        ldBody *bodyPtr = nullptr;
        uint32_t colour = 0x00FFFFFF;
        bool renderBound = false;

        explicit WorldObject(ObjectType pType) : type(pType)
        {
        }

        WorldObject(ObjectType pType, ldBody *pBodyPtr, bool pRenderBound = false, uint32_t pColour = 0x00FFFFFF) : type(pType),
                                                                                                                bodyPtr(pBodyPtr),
                                                                                                                colour(pColour),
                                                                                                                renderBound(pRenderBound)
        {
        }

        bool isStatic() const
        {
            return type == WO_STATIC;
        }

        bool isDynamic() const
        {
            return type == WO_DYNAMIC;
        }

        void clear()
        {
            type = WO_NONE;
            bodyPtr = nullptr;
        }

        const vec2f *position() const
        {
            return bodyPtr ? &bodyPtr->pos : nullptr;
        }

        const ldBound *modelBound() const
        {
            return bodyPtr ? &bodyPtr->modelBound : nullptr;
        }

        const ldBound *worldBound() const
        {
            return bodyPtr ? &bodyPtr->worldBound : nullptr;
        }
    };

    // A mesh that provides methods for navigating the world
    struct NavMesh
    {
        std::vector<vec2f> mesh;
        std::vector<float> speeds;
        bool isLoop = false;

        vec2f getPosition(float t) const
        {
            if (speeds.empty()) return vec2f{0.f, 0.f};

            t = clamp(t, 0.f, 1.f);
//            const float scale = 1.f / (isLoop ? mesh.size() : mesh.size() - 1);

            uint16_t idx = speeds.size() - 1;
            for (auto i = 0, end = int(speeds.size() - 1); i != end; ++i) {
                if (speeds[i] <= t && t < speeds[i + 1]) {
                    idx = i;
                    break;
                }
            }

            const float v = (t - speeds[idx]) / (speeds[idx + 1] - speeds[idx]);
            if (isLoop) return lerp(mesh[idx], mesh[(idx + 1) % mesh.size()], v);
            else return lerp(mesh[idx], mesh[idx + 1], v);
        }

        std::pair<vec2f, float>
        projectToMesh(const ldRazer::vec2f &wP, float step = .01f, float prev = 0.f, float dist = 1.f) const
        {
            float t = prev;
            std::pair<vec2f, float> minP{getPosition(t), t};
            float minDist = (minP.first - wP).lengthSq();

            while (dist > 0.f) {
                auto P = getPosition(t);
                auto d = (P - wP).lengthSq();

                if (d < minDist) {
                    minP.first = P;
                    minP.second = t;
                    minDist = d;
                }

                t = wrap(t + step, 0.f, 1.f);
                dist -= step;
            }

            return minP;
        }

        // Calculates the proportion of the length of each segment over the length of the NavMesh
        void computeSpeeds()
        {
            speeds.resize(mesh.size());
            float total = 0.f;
            speeds[0] = 0.f;
            for (int i = 0, end = mesh.size() - 1; i != end; ++i) {
                speeds[i + 1] = total + (mesh[i + 1] - mesh[i]).length();
                total = speeds[i + 1];
            }

            if (isLoop) {
                speeds.back() = total + (mesh[0] - mesh.back()).length();
                total = speeds.back();
            } else {
                speeds.back() = 1.f;
            }

            const float inv = 1.f / total;
            std::transform(speeds.begin(), speeds.end(), speeds.begin(), [inv](float &v)
            {
                return inv * v;
            });
        }
    };

    class ldWorld
    {
    public:
        const uint16_t MAX_OBJECTS = 2000;
        const uint16_t MAX_NAVMESHES = 100;

        ldWorld();

        ~ldWorld();

        void clear();

        bool initialise(ldPhysicsEngine *enginePtr);

        const AABB2f_t &worldBound() const;

        ldPhysicsEngine *getPhysicsEngine() const
        {
            return m_physicsEngine;
        }

        uint16_t addStaticObject(const ldBound &bound, uint32_t colour = 0x00000000, float orn = 0.f);

        uint16_t addStaticObject(const vec2f &pos, const ldBound &bound, uint32_t colour = 0x00000000, float orn = 0.f);

        uint16_t addDynamicObject(ldBody *b, uint32_t colour = 0x00000000);

        uint16_t
        addDynamicObject(const vec2f &pos, const ldBound &bound, float mass = 1.f, uint32_t colour = 0x00000000,
                         float orn = 0.f);

        const WorldObject *getObject(uint16_t id) const;

        WorldObject *getObject(uint16_t id);

        std::vector<const WorldObject *> query(const vec2f &pos, float radius) const;

        std::vector<const WorldObject *> query(const AABB2f_t &bound) const;

        uint16_t addNavMesh(const NavMesh &navMesh);

        const NavMesh *getNavMesh(uint16_t id) const;

        NavMesh *getNavMesh(uint16_t id);

        void loadQuadTree();

    protected:

    private:
        AABB2f_t m_worldBound;
        ldPhysicsEngine *m_physicsEngine = nullptr;
        std::vector<WorldObject> m_scene;
        std::vector<NavMesh> m_navMeshes;
    };

    void drawBound(const ldBound &bound, uint32_t colour, const mat3f &T, ldRazer::ldMesh2f_t &mesh);

    void drawNavMesh(const NavMesh &navMesh, uint32_t colour, const mat3f &T, ldRazer::ldMesh2f_t &mesh);

}

#endif //LASERDOCKCORE_LDWORLD_H
