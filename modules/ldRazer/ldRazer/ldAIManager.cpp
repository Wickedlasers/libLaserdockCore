//
// Created by Darren Otgaar on 2018/09/23.
//

#include "ldAIManager.h"

namespace ldRazer
{

    ldAIManager::ldAIManager(ldWorld *worldPtr) : m_worldPtr(worldPtr)
    {
    }

    ldAIManager::~ldAIManager()
    {
    }

    void ldAIManager::clear()
    {
        m_agents.clear();
    }

    uint16_t ldAIManager::addAgent(const agent &agt)
    {
        auto it = std::find_if(m_agents.begin(), m_agents.end(), [](const agent &eachAgt)
        { return !eachAgt.isAllocated(); });
        if (it != m_agents.end()) {
            auto id = uint16_t(it - m_agents.begin());
            m_agents[id] = agt;
            return id;
        } else if (m_agents.size() < uint16_t(-1)) {
            auto id = uint16_t(m_agents.size());
            m_agents.emplace_back(agt);
            return id;
        } else return uint16_t(-1);
    }

    agent *ldAIManager::getAgent(uint16_t id)
    {
        return (id < m_agents.size() && m_agents[id].isAllocated()) ? &m_agents[id] : nullptr;
    }

    const agent *ldAIManager::getAgent(uint16_t id) const
    {
        return (id < m_agents.size() && m_agents[id].isAllocated()) ? &m_agents[id] : nullptr;
    }

    void ldAIManager::follow(ldBody *bodyPtr, const vec2f &targetPos, const vec2f &linearPos, const vec2f &angularPos)
    {
        const auto dir = polarToCartesian(bodyPtr->orn, 1.f), nPerp = perp(dir);
        const auto D = targetPos - bodyPtr->pos;
        const auto d = D.length();
        const auto dV = dotPerp(dir, D / d) / M_PIf;
        if (std::abs(dV) > .05f) bodyPtr->applyForce(angularPos, -bodyPtr->angularSpeed * dV * nPerp);
        if (std::abs(d) > .05f) bodyPtr->applyForce(linearPos, bodyPtr->linearSpeed * dir * clamp(d, 0.f, 2.f));
    }

    void ldAIManager::avoid(ldBody *bodyPtr, const vec2f &targetPos, const vec2f &linearPos, const vec2f &angularPos)
    {
        const auto dir = polarToCartesian(bodyPtr->orn, 1.f), nPerp = perp(dir);
        const auto D = targetPos - bodyPtr->pos;
        const auto d = D.length();
        auto dV = dotPerp(dir, D / d) / M_PIf, mag = clamp(d, 0.f, 2.f);
        bodyPtr->applyForce(angularPos, +bodyPtr->angularSpeed / 2.f * dV * nPerp * mag);
        bodyPtr->applyForce(linearPos, -bodyPtr->linearSpeed / 2.f * mag * dir);
    }

    void ldAIManager::randomWalk(ldBody *bodyPtr, const vec2f &linearPos, const vec2f &angularPos)
    {
        const auto dir = polarToCartesian(bodyPtr->orn, 1.f), nPerp = perp(dir);
        bodyPtr->applyForce(angularPos, +bodyPtr->angularSpeed / 2.f * m_rand.random1(-.1f, +1.f) * nPerp);
        bodyPtr->applyForce(linearPos, -bodyPtr->linearSpeed / 2.f * m_rand.random1() * dir);
    }

    void ldAIManager::follow(const ldWorld *worldPtr, agent &agt)
    {
        const auto target = agt.getTarget();
        const auto dir = polarToCartesian(agt.bodyPtr->orn, 1.f);
        const auto linearPos = agt.bodyPtr->pos - .06f * dir, angularPos = agt.bodyPtr->pos - .075f * dir;
        follow(agt.bodyPtr, target, linearPos, angularPos);
        auto scene = worldPtr->query(agt.bodyPtr->pos, 2.f);
        for (auto &s : scene) {
            if (s->isDynamic() && agt.bodyPtr != s->bodyPtr && !target.eq(*s->position())) {
                avoid(agt.bodyPtr, *s->position(), linearPos, angularPos);
            }
        }
    }

    void ldAIManager::flee(const ldWorld *worldPtr, agent &agt)
    {
        Q_UNUSED(worldPtr);
        const auto target = agt.getTarget();
        const auto dir = polarToCartesian(agt.bodyPtr->orn, 1.f);
        const auto linearPos = agt.bodyPtr->pos - .06f * dir, angularPos = agt.bodyPtr->pos - .075f * dir;
        avoid(agt.bodyPtr, target, linearPos, angularPos);
    }

    void ldAIManager::chase(const ldWorld *worldPtr, agent &agt)
    {
        Q_UNUSED(worldPtr);
        const auto target = agt.getTarget();
        const auto dir = polarToCartesian(agt.bodyPtr->orn, 1.f);
        const auto linearPos = agt.bodyPtr->pos - .06f * dir, angularPos = agt.bodyPtr->pos - .075f * dir;
        follow(agt.bodyPtr, target, linearPos, angularPos);
    }

    void ldAIManager::followPath(const ldWorld *worldPtr, agent &agt, float /*dt*/)
    {
        Q_UNUSED(worldPtr);
        auto target = agt.navMeshPtr->projectToMesh(agt.bodyPtr->pos, .001f, agt.navParm, 1.f);
        target.first = agt.navMeshPtr->getPosition(wrap(target.second + .01f, 0.f, 1.f));
        const auto dir = polarToCartesian(agt.bodyPtr->orn, 1.f);
        const auto linearPos = agt.bodyPtr->pos - .06f * dir, angularPos = agt.bodyPtr->pos - .075f * dir;
        follow(agt.bodyPtr, target.first, linearPos, angularPos);
    }

    void ldAIManager::randomWalk(const ldWorld *worldPtr, agent &agt)
    {
        Q_UNUSED(worldPtr);
        const auto dir = polarToCartesian(agt.bodyPtr->orn, 1.f);
        const auto linearPos = agt.bodyPtr->pos - .06f * dir, angularPos = agt.bodyPtr->pos - .075f * dir;
        randomWalk(agt.bodyPtr, linearPos, angularPos);
    }

    void ldAIManager::update(float t, float dt)
    {
        Q_UNUSED(t);
        for (auto &agt : m_agents) {
            if (!agt.isAllocated()) continue;
            switch (agt.steering) {
                case SteeringType::TT_NONE:
                    break;
                case SteeringType::TT_FOLLOW:
                    follow(m_worldPtr, agt);
                    break;
                case SteeringType::TT_CHASE:
                    chase(m_worldPtr, agt);
                    break;
                case SteeringType::TT_FLEE:
                    flee(m_worldPtr, agt);
                    break;
                case SteeringType::TT_FOLLOW_PATH:
                    followPath(m_worldPtr, agt, dt);
                    break;
                case SteeringType::TT_RANDOM_WALK:
                    randomWalk(m_worldPtr, agt);
                    break;
            }
        }
    }

    void ldAIManager::debugOutput()
    {
    }

    void ldAIManager::applyForce(uint16_t id, const vec2f &force)
    {
        if (m_worldPtr->getPhysicsEngine()) m_worldPtr->getPhysicsEngine()->applyForce(id, force);
    }

    void ldAIManager::applyForce(uint16_t id, const vec2f &pos, const vec2f &force)
    {
        if (m_worldPtr->getPhysicsEngine()) m_worldPtr->getPhysicsEngine()->applyForce(id, pos, force);
    }

    bool ldAIManager::computeMovement()
    {
        return false;
    }

}
