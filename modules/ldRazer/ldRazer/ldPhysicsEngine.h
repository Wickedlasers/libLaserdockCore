//
// Created by Darren Otgaar on 2018/09/10.
//

#ifndef LASERDOCKCORE_LDPHYSICSENGINE_H
#define LASERDOCKCORE_LDPHYSICSENGINE_H

#include "ldBody.h"
#include "ldQuadTree.h"

#include <QDebug>

namespace ldRazer
{

    const uint16_t MAX_BODY_COUNT = 10000;
    const uint16_t INVALID_BODY = uint16_t(-1);

/*
 * A 2D physics engine based on the Separating Axis Test with collision detection and response using arcade style
 * physics, e.g. reflected collisions, scalar friction, and restitution computations.  Should support discs, lines,
 * points, and convex hulls.
 */

    struct ldWorldSettings
    {
        float linearFriction = .95f;
        float angularFriction = .9f;
        vec2f gravity = vec2f{0.f, 0.f};
    };

// Note: The physics engine uses a basic memory management scheme so that objects can be created and destroyed as quickly
// as possible.  Dynamic memory allocation in a physics engine causes stuttering and results in bad quality, even in
// small systems.  The system uses a reusable id for each type.  When deleting a body, the id is recycled.  If you need
// to be informed about body creation/destruction, register an observer function.

    class ldPhysicsEngine
    {
    public:
        ldPhysicsEngine();

        ~ldPhysicsEngine();

        using collisionFnc = std::function<void(const ldBody *, const ldBody *, float mag)>;

        ldWorldSettings worldSettings;

        void initialise();

        void clear();

        ldBody *getBody(uint16_t id);

        const ldBody *getBody(uint16_t id) const;

        uint16_t addBody(const ldBody &obj);

        uint16_t addBody(const ldBody *ptr)
        {
            return addBody(*ptr);
        }

        // Add the static boundaries to the quadtree for faster lookup
        void createTree(const std::vector<ldBody *> &bodies);

        // Apply at CoM
        void applyForce(uint16_t id, const vec2f &force);

        void applyForce(ldBody *bodyPtr, const vec2f &force);

        // Apply at local coord
        void applyForce(uint16_t id, const vec2f &pos, const vec2f &force);

        void applyForce(ldBody *bodyPtr, const vec2f &pos, const vec2f &force);

        void update(float t, float dt);

        // Register for certain collisions
        void observeCollision(collisionFnc &&fnc)
        {
            m_collisionCallback = std::move(fnc);
        }

    protected:
        float testCollision(ldBody &A, ldBody &B, float dt);

        void resolve(const ldContact &c);

        void resolvePenetration(const vec2f &C0, const vec2f &C1, ldBody *Aptr, ldBody *Bptr);

        void
        resolveCollision(const vec2f &normal, float t, const vec2f &C0, const vec2f &C1, ldBody *Aptr, ldBody *Bptr);

        void resolveCollision(const vec2f &N, float t, float fric, float rest,
                              const vec2f &C0, const vec2f &P0, vec2f &V0, float &w0, float m0, float i0,
                              const vec2f &C1, const vec2f &P1, vec2f &V1, float &w1, float m1, float i1);

        void transform(const ldBound &model, const mat3f &T, ldBound &world);

        float hullVsHull(ldBody &bodyA, ldBody &bodyB, float dt);

        float lineVsHull(ldBody &bodyA, ldBody &bodyB, float dt);

        float meshVsHull(ldBody &bodyA, ldBody &bodyB, float dt);

    private:
        std::array<ldBody, MAX_BODY_COUNT> m_bodies;
        ldRazer::ldQuadTree<ldBody> m_quadTree;
        std::vector<ldBody *> m_dynamics;
        collisionFnc m_collisionCallback;
    };

}

#endif //LASERDOCKCORE_LDPHYSICSENGINE_H
