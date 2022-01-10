//
// Created by Darren Otgaar on 2018/09/10.
//

#include "ldPhysicsEngine.h"
#include "ldQueries.h"
#include <QDebug>

namespace ldRazer
{

    ldPhysicsEngine::ldPhysicsEngine() = default;

    ldPhysicsEngine::~ldPhysicsEngine() = default;

    void ldPhysicsEngine::initialise()
    {
    }

    void ldPhysicsEngine::clear()
    {
        for (auto &b : m_bodies) b.isAllocated = false;
    }

    ldBody *ldPhysicsEngine::getBody(uint16_t id)
    {
        assert(id < m_bodies.size() && "Out-of-range");
        return id < m_bodies.size() && m_bodies[id].isAllocated ? &m_bodies[id] : nullptr;
    }

    const ldBody *ldPhysicsEngine::getBody(uint16_t id) const
    {
        assert(id < m_bodies.size() && "Out-of-range");
        return id < m_bodies.size() && m_bodies[id].isAllocated ? &m_bodies[id] : nullptr;
    }

    uint16_t ldPhysicsEngine::addBody(const ldBody &obj)
    {
        auto it = std::find_if(m_bodies.begin(), m_bodies.end(), [](const ldBody &b)
        { return !b.isAllocated; });
        if (it == m_bodies.end()) return INVALID_BODY;

        *it = obj;
        it->isAllocated = true;
        it->modelRot = makeRotation2(it->orn);
        auto T = makeTranslation3(it->pos) * makeRotation3(it->orn);
        it->worldBound.type = it->modelBound.type;
        transform(it->modelBound, T, it->worldBound);

        return uint16_t(it - m_bodies.begin());
    }

    void ldPhysicsEngine::createTree(const std::vector<ldBody *> &bodies)
    {
        m_quadTree.clear();
        m_quadTree.createTree(bodies);
        qDebug() << "Added" << bodies.size() << "to quadtree";
    }

    void ldPhysicsEngine::applyForce(uint16_t id, const vec2f &force)
    {
        applyForce(getBody(id), force);
    }

    void ldPhysicsEngine::applyForce(ldBody *bodyPtr, const vec2f &force)
    {
        if (bodyPtr) bodyPtr->applyForce(force);
    }

    void ldPhysicsEngine::applyForce(uint16_t id, const vec2f &pos, const vec2f &force)
    {
        applyForce(getBody(id), pos, force);
    }

    void ldPhysicsEngine::applyForce(ldBody *bodyPtr, const vec2f &pos, const vec2f &force)
    {
        if (bodyPtr) bodyPtr->applyForce(pos, force);
    }

    void ldPhysicsEngine::update(float /*t*/, float dt)
    {
        m_dynamics.clear();
        for (auto &b : m_bodies) if (!isZero(b.invMass)) m_dynamics.emplace_back(&b);

        for (uint16_t i = 0; i != m_bodies.size(); ++i) {
            auto *A = getBody(i);
            if (!A || isZero(A->invMass)) continue;

            std::vector<ldBody *> query;
            if (A->invMass > 0.f) {
                query = m_quadTree.queryBound(AABB2f_t{A->pos, 2.f});
                if (!query.empty()) {
                    for (const auto B : query) {
                        auto mag = testCollision(*A, *B, dt);
                        if (!isZero(mag) && m_collisionCallback)
                            m_collisionCallback(A, B, mag);
                    }
                }
            }

            for (auto &d : m_dynamics) {
                if (A == d) continue;
                auto mag = testCollision(*A, *d, dt);
                if (!isZero(mag) && m_collisionCallback)
                    m_collisionCallback(A, d, mag);
            }
        }

        for (uint16_t id = 0; id != m_bodies.size(); ++id) {
            auto *body = getBody(id);
            if (!body || isZero(body->invMass)) continue;

            body->pos += dt * body->vel;
            body->vel += dt * body->invMass * body->force;
            body->vel *= worldSettings.linearFriction;
            body->force.clear();

            body->orn = wrap(body->orn + dt * body->angVel, -M_2PIf, +M_2PIf);
            body->angVel += dt * body->invInertia * body->torque;
            body->angVel *= worldSettings.angularFriction;
            body->torque = 0.f;

            body->modelRot = makeRotation2(body->orn);
            auto T = makeTranslation3(body->pos) * makeRotation3(body->orn);
            transform(body->modelBound, T, body->worldBound);
        }
    }

// Hulls and Line Meshes for now
    float ldPhysicsEngine::testCollision(ldBody &bodyA, ldBody &bodyB, float dt)
    {
        if (isZero(bodyA.invMass) && isZero(bodyB.invMass))
            return 0.f;

        // Hull vs. Hull
        if (bodyA.modelBound.isHull() && bodyB.modelBound.isHull()) {
            return hullVsHull(bodyA, bodyB, dt);
        } else if (bodyA.modelBound.isMesh() || bodyB.modelBound.isMesh()) {      // Mesh vs Hull
            return meshVsHull(bodyA, bodyB, dt);
        } else if (bodyA.modelBound.isLine() || bodyB.modelBound.isLine()) {
            return lineVsHull(bodyA, bodyB, dt);
        }

        return 0.f;
    }

    void ldPhysicsEngine::resolve(const ldContact &c)
    {
        if (!c.bodies[0] || !c.bodies[1]) return;

        for (int i = 0; i != c.count; ++i) {
            resolveCollision(c.normal, c.t, c.contactsA[i], c.contactsB[i], c.bodies[0], c.bodies[1]);
        }

        if (c.t < 0.f) {
            for (int i = 0; i != c.count; ++i)
                resolvePenetration(c.contactsA[i], c.contactsB[i], c.bodies[0], c.bodies[1]);
        }
    }

    void ldPhysicsEngine::resolvePenetration(const vec2f &C0, const vec2f &C1, ldBody *Aptr, ldBody *Bptr)
    {
        const auto &m0 = Aptr->invMass, &m1 = Bptr->invMass;
        const auto m = m0 + m1;

        auto D = C1 - C0;
        auto sep = (Aptr->separation + Bptr->separation) / 2;
        D *= sep;
        vec2f DD;
        if (m0 > 0.f) {
            DD = D * (m0 / m);
            Aptr->pos += DD;
        }
        if (m1 > 0.f) {
            DD = D * -(m1 / m);
            Bptr->pos += DD;
        }
    }

    void ldPhysicsEngine::resolveCollision(const vec2f &normal, float t, const vec2f &C0, const vec2f &C1, ldBody *Aptr,
                                           ldBody *Bptr)
    {
        const auto &m0 = Aptr->invMass, &m1 = Bptr->invMass;
        const auto &i0 = Aptr->invInertia, &i1 = Bptr->invInertia;
        const auto &P0 = Aptr->pos, &P1 = Bptr->pos;

        auto &V0 = Aptr->vel, &V1 = Bptr->vel;
        auto &w0 = Aptr->angVel, &w1 = Bptr->angVel;

        auto rest = (Aptr->restitution + Bptr->restitution) / 2;
        auto fric = (Aptr->friction + Bptr->friction) / 2;

        resolveCollision(-normal, t, fric, rest,
                         C1, P1, V1, w1, m1, i1,
                         C0, P0, V0, w0, m0, i0);
    }

    void
    ldPhysicsEngine::resolveCollision(const vec2f &N, float t, float fric, float rest,
                                      const vec2f &C0, const vec2f &P0, vec2f &V0, float &w0, float m0, float i0,
                                      const vec2f &C1, const vec2f &P1, vec2f &V1, float &w1, float m1, float i1)
    {
        auto tcoll = t > 0.f ? t : 0.f;

        auto Q0 = P0 + tcoll * V0, Q1 = P1 + tcoll * V1;
        auto R0 = C0 - Q0, R1 = C1 - Q1;
        auto T0 = perp(R0), T1 = perp(R1);
        auto VP0 = V0 - w0 * T0, VP1 = V1 - w1 * T1;

        auto Vcoll = VP0 - VP1;
        auto vn = dot(Vcoll, N);
        auto Vn = vn * N, Vt = Vcoll - Vn;

        auto vt = Vt.length();

        vec2f J, Jt, Jn;
        auto t0 = dotPerp(R0, N), t1 = dotPerp(R1, N);
        t0 *= t0 * i0;
        t1 *= t1 * i1;

        auto m = m0 + m1, div = m + t0 + t1, jn = vn / div;
        Jn = (-(1.f + rest) * jn) * N;
        if (vt > 0) Jt = (fric * jn) * normalise(Vt);
        J = Jn + Jt;

        auto dV0 = J * m0, dV1 = -J * m1;
        auto dw0 = -dotPerp(R0, J) * i0, dw1 = dotPerp(R1, J) * i1;
        if (m0 > 0.f) {
            V0 += dV0;
            w0 += dw0;
        }

        if (m1 > 0.f) {
            V1 += dV1;
            w1 += dw1;
        }

        if (vn < 0.f && vt > 0.f && fric > 0.f) {
            float c = -vt / vn;
            if (c < fric) {
                auto Nf = isZero(vt) ? vec2f{0.f, 0.f} : -normalise(Vt);
                resolveCollision(Nf, 0.f, 0.f, fric, C0, P0, V0, w0, m0, i0, C1, P1, V1, w1, m1, i1);
            }
        }
    }

    void ldPhysicsEngine::transform(const ldBound &model, const mat3f &T, ldBound &world)
    {
        if (world.type != model.type) world.type = model.type;
        switch (model.type) {
            case BT_LINE:
                world.line = model.line.transform(T);
                break;
            case BT_HULL:
                world.hull = model.hull.transform(T);
                break;
            default:
                qDebug() << "Not Implemented";
        }
    }

    float ldPhysicsEngine::hullVsHull(ldBody &bodyA, ldBody &bodyB, float dt)
    {
        vec2f N;
        const auto *A = bodyA.modelBound.hull.vertices.data();
        size_t lenA = bodyA.modelBound.hull.getSides();
        const auto *B = bodyB.modelBound.hull.vertices.data();
        size_t lenB = bodyB.modelBound.hull.getSides();
        if (collide(A, lenA, bodyA.pos, bodyA.vel, bodyA.modelRot,
                    B, lenB, bodyB.pos, bodyB.vel, bodyB.modelRot,
                    N, dt)) {
            vec2f conA[4], conB[4];
            int count;
            if (findContacts(A, lenA, bodyA.pos, bodyA.vel, bodyA.modelRot,
                             B, lenB, bodyB.pos, bodyB.vel, bodyB.modelRot,
                             N, dt, conA, conB, count)) {
                ldContact c(conA, conB, count, N, dt, &bodyA, &bodyB);
                resolve(c);
                return dt;
            }
        }

        return 0.f;
    }

    float ldPhysicsEngine::lineVsHull(ldBody &bodyA, ldBody &bodyB, float dt)
    {
        ldBody *linePtr = bodyA.modelBound.isLine() ? &bodyA : &bodyB;
        ldBody *hullPtr = bodyA.modelBound.isHull() ? &bodyA : &bodyB;
//        const size_t lenHull = hullPtr->modelBound.hull.getSides();

        vec2f N;
        if (collide(linePtr->worldBound.line.arr, 2, linePtr->pos, linePtr->vel, linePtr->modelRot,
                    hullPtr->worldBound.hull.vertices.data(), hullPtr->worldBound.hull.getSides(), hullPtr->pos,
                    hullPtr->vel, hullPtr->modelRot, N, dt)) {
            vec2f conA[4], conB[4];
            int count;
            if (findContacts(linePtr->worldBound.line.arr, 2, linePtr->pos, linePtr->vel, linePtr->modelRot,
                             hullPtr->worldBound.hull.vertices.data(), hullPtr->worldBound.hull.getSides(),
                             hullPtr->pos,
                             hullPtr->vel, hullPtr->modelRot,
                             N, dt, conA, conB, count)) {
                ldContact c(conA, conB, static_cast<uint16_t>(count), N, dt, &bodyA, &bodyB);
                resolve(c);
                return dt;
            }
        }

        return 0.f;
    }

    float ldPhysicsEngine::meshVsHull(ldBody &bodyA, ldBody &bodyB, float dt)
    {
        ldBody *hullPtr = bodyA.modelBound.isHull() ? &bodyA : &bodyB;
        ldBody *meshPtr = bodyA.modelBound.isMesh() ? &bodyA : &bodyB;
        const size_t lenHull = hullPtr->modelBound.hull.getSides(), strips = meshPtr->modelBound.meshPtr->getStripCount();

        vec2f N;

        for (uint16_t s = 0; s != strips; ++s) {
            const auto lenStrip = meshPtr->modelBound.meshPtr->getStripLength(s);
            for (uint16_t i = 1; i != lenStrip; ++i) {
                vec2f pos[2] = {*(meshPtr->modelBound.meshPtr->data(s) + (i - 1)),
                                *(meshPtr->modelBound.meshPtr->data(s) + i)};
                auto lP = .5f * (pos[0] + pos[1]);
                pos[0] -= lP;
                pos[1] -= lP;
                if (collide(hullPtr->modelBound.hull.vertices.data(), lenHull, hullPtr->pos, hullPtr->vel,
                            hullPtr->modelRot,
                            pos, 2, lP + meshPtr->pos, meshPtr->vel, meshPtr->modelRot,
                            N, dt)) {

                    vec2f conA[4], conB[4];
                    int count;
                    if (findContacts(hullPtr->modelBound.hull.vertices.data(), lenHull, hullPtr->pos, hullPtr->vel,
                                     hullPtr->modelRot,
                                     pos, 2, lP + meshPtr->pos, meshPtr->vel, meshPtr->modelRot,
                                     N, dt, conA, conB, count)) {
                        ldContact c(conA, conB, static_cast<uint16_t>(count), N, dt, hullPtr, meshPtr);
                        resolve(c);
                        return dt;
                    }
                }
            }
        }

        return 0.f;
    }

    float calculateMass(const ldBody &b, float density)
    {
        if (b.modelBound.type == BT_POINT) return 5.f * density;
        else if (b.modelBound.type == BT_LINE) return 2 * 5.f * density;
        else if (b.modelBound.type == BT_DISC) {
            assert(false && "TODO");
            return -1.f;
        }
        else if (b.modelBound.type == BT_HULL) {
            float mass = 0.f;
            const auto count = b.modelBound.hull.getVertexCount();
            for (uint i = 0/*, j = count - 1*/; i != count; /*j = i,*/ ++i) {
//                vec2f A = b.modelBound.hull[i];
//                vec2f B = b.modelBound.hull[j];
                mass += std::abs(dotPerp(b.modelBound.line.A, b.modelBound.line.B));
            }
            mass *= .5f * density;
            return mass;
        } else {
            return 0.f;
        }
    }

    float calculateInertiaTensor(const ldBody &b)
    {
        if (b.modelBound.type == BT_POINT) return 0.f;
        else if (b.modelBound.type == BT_LINE) {
            const auto &A = b.modelBound.line.A, &B = b.modelBound.line.B;
            const auto dP = clamp(std::abs(dotPerp(A, B)), .1f, 1.f);
            const auto L = A.lengthSq() + dot(A, B) + B.lengthSq();
            return (b.mass / 6.f) * (dP * L) / dP;
        } else if (b.modelBound.type == BT_HULL) {
            if (isZero(b.invMass)) return 0.f;

            auto dP = 0.f;
            auto L = 0.f;
            const auto count = b.modelBound.hull.getVertexCount();
            for (size_t i = 0, j = count - 1; i != count; j = i, ++i) {
                const auto &A = b.modelBound.hull[i], &B = b.modelBound.hull[j];
                auto d = std::abs(dotPerp(A, B));
                auto l = A.lengthSq() + dot(A, B) + B.lengthSq();
                dP += d * l;
                L += d;
            }
            return (b.mass / 6.f) * (dP / L);
        } else {
            return 0.f;
        }
    }

}
