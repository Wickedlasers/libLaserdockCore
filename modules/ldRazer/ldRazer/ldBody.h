//
// Created by Darren Otgaar on 2018/10/22.
//

#ifndef LASERDOCKCORE_LDBODY_H
#define LASERDOCKCORE_LDBODY_H

#include "ldRazer.h"
#include "ldBound.h"
#include <QDebug>

namespace ldRazer
{

    struct ldBody;

    float calculateMass(const ldBody &b, float density);

    float calculateInertiaTensor(const ldBody &b);

    // Note: the ldBody can be configured as any of point, line, disc, or convex hull
    struct ldBody
    {
        vec2f pos{0.f, 0.f};    // Position (Centre of Mass)    (r)
        vec2f vel{0.f, 0.f};    // Velocity                     (v)
        vec2f force{0.f, 0.f};  // Linear Force                 (F)
        float orn = 0.f;        // Orientation                  (Omega) - uppercase
        float angVel = 0.f;     // Angular Velocity             (omega) - lowercase
        float torque = 0.f;     // Torque (Angular Force)       (tau)
        float mass = 1.f;       // Mass                         (M)
        float inertia = 1.f;    // Inertia tensor               (I)
        float invMass = 1.f;    // Inv Mass
        float invInertia = 1.f; // Inv Inertia "tensor"
        ldBound modelBound;     // The model bound
        ldBound worldBound;     // The world bound

        // Cached values
        mat2f modelRot;         // The rotation matrix of the orientation

        float separation = 1.f;
        float friction = .9f;
        float restitution = .1f;

        // These values are for configuring the AI Manager (the controlled racers)
        float angularSpeed = 52.5f;
        float linearSpeed = 50.f;

        bool isAllocated = false;               // Is the ldBody allocated in the physics engine

        // Note: A mass of zero means the object is immovable or static
        void setBody(const vec2f &pPos, float pOrn, float pMass, const ldBound &pModelBound)
        {
            this->pos = pPos;
            this->orn = pOrn;
            this->mass = pMass;
            this->invMass = isZero(pMass) ? 0.f : 1.f / pMass;
            this->modelBound = pModelBound;
            this->inertia = calculateInertiaTensor(*this);
            this->invInertia = isZero(this->inertia) ? .0001f : 1.f / this->inertia;
        }

        void applyForce(const vec2f &pForce)
        {
            if (!isZero(invMass)) this->force += pForce;
        }

        void applyForce(const vec2f &pPos, const vec2f &pForce)
        {
            if (!isZero(invMass)) {
                this->force += pForce;
                this->torque += dotPerp((pPos - this->pos), pForce);
            }
        }
    };

    const uint16_t MAX_CONTACT_COUNT = 2;

    struct ldContact
    {

        ldContact() = default;

        ldContact(const vec2f *conA, const vec2f *conB, uint16_t lenC, const vec2f &N, float pT, ldBody *bodyA,
                  ldBody *bodyB)
                : normal(N), t(pT), count(0)
        {
            bodies[0] = bodyA;
            bodies[1] = bodyB;
            for (int i = 0; i != std::min(lenC, MAX_CONTACT_COUNT); ++i) {
                contactsA[count] = conA[i];
                contactsB[count] = conB[i];
                count++;
            }
        }

        std::array<ldBody *, 2> bodies;     // The two bodies involved in the collision
        std::array<vec2f, MAX_CONTACT_COUNT> contactsA;
        std::array<vec2f, MAX_CONTACT_COUNT> contactsB;
        vec2f normal;
        float t;                            // Time of collision
        uint16_t count;                     // Number of contacts (in contactsA & B, max 2 for now)
    };

}

#endif //LASERDOCKCORE_LDBODY_H
