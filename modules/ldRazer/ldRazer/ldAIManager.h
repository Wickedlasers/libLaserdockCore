//
// Created by Darren Otgaar on 2018/09/23.
//

#ifndef LASERDOCKCORE_AIMANAGER_H
#define LASERDOCKCORE_AIMANAGER_H

#include "ldRazer.h"
#include "ldWorld.h"

namespace ldRazer
{
    class ldWorld;

    enum SteeringType
    {
        TT_NONE,
        TT_FOLLOW,
        TT_CHASE,
        TT_FLEE,
        TT_FOLLOW_PATH,
        TT_RANDOM_WALK
    };

    struct agent
    {
        using targetFnc = std::function<vec2f()>;

        static vec2f defaultTarget()
        {
            return vec2f{0.f, 0.f};
        }

        void clear()
        {
            bodyId = uint16_t(-1);
            bodyPtr = nullptr;
            getTarget = agent::defaultTarget;
            steering = SteeringType::TT_NONE;
        }

        bool isAllocated() const
        {
            return bodyPtr != nullptr;
        }

        uint16_t bodyId = uint16_t(-1);
        ldBody *bodyPtr = nullptr;
        targetFnc getTarget = agent::defaultTarget;
        NavMesh *navMeshPtr = nullptr;
        float navParm = 0.f;
        float navScale = 1.f;
        SteeringType steering = SteeringType::TT_NONE;

        agent() = default;

        agent(uint16_t pBodyId, ldBody *pBodyPtr, targetFnc tgtFnc, SteeringType steer) : bodyId(pBodyId),
                                                                                        bodyPtr(pBodyPtr),
                                                                                        getTarget(tgtFnc),
                                                                                        steering(steer)
        {
        }

        agent(uint16_t pBodyId, ldBody *pBodyPtr, NavMesh *mesh, float scale) : bodyId(pBodyId), bodyPtr(pBodyPtr),
                                                                              navMeshPtr(mesh), navScale(scale),
                                                                              steering(TT_FOLLOW_PATH)
        {
        }
    };

    class ldAIManager
    {
    public:
        ldAIManager(ldWorld *worldPtr);

        ~ldAIManager();

        void clear();

        uint16_t addAgent(const agent &agt);

        agent *getAgent(uint16_t id);

        const agent *getAgent(uint16_t id) const;

        void update(float t, float dt);

        void debugOutput();

    protected:
        void follow(ldBody *bodyPtr, const vec2f &targetPos, const vec2f &linearPos, const vec2f &angularPos);

        void avoid(ldBody *bodyPtr, const vec2f &targetPos, const vec2f &linearPos, const vec2f &angularPos);

        void randomWalk(ldBody *bodyPtr, const vec2f &linearPos, const vec2f &angularPos);

        void follow(const ldWorld *worldPtr, agent &agt);

        void flee(const ldWorld *worldPtr, agent &agt);

        void chase(const ldWorld *worldPtr, agent &agt);

        void followPath(const ldWorld *worldPtr, agent &agt, float dt);

        void randomWalk(const ldWorld *worldPtr, agent &agt);

        void applyForce(uint16_t id, const vec2f &force);

        void applyForce(uint16_t id, const vec2f &pos, const vec2f &force);

        bool computeMovement();

    private:
        ldRazer::ldRandom<float> m_rand;
        ldWorld *m_worldPtr;
        std::vector<agent> m_agents;
    };

}

#endif //LASERDOCKCORE_AIMANAGER_H
