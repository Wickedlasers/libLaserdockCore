//
// Laser Angry Lasers
// By Vander R. N. Dias a.k.a. "imerso" @ imersiva.com
// Copyright(c) 2019 Wicked Lasers
//

#include "ldAngryLasersEnemy.h"
#include <QtCore/QDebug>

using namespace std;


ldAngryLasersEnemy::ldAngryLasersEnemy(std::shared_ptr<b2World> world, ldVec2 pos, eAB_EnemyType p_type)
    : ldGradientObject(0.33f)
{
    type = p_type;
    dead = false;

    b2_bodydef.position.Set(0, 0);
    b2_bodydef.type = b2_dynamicBody;

    b2_world = world;
    b2_body = b2_world->CreateBody(&b2_bodydef);
    b2_body->SetLinearDamping(0.35f);
    b2_body->SetAngularDamping(0.5f);

    b2_circleshape.m_p.Set(0, 0);
    addMesh();

    uint32_t color = C_BLACK;

    switch (type)
    {
        case eAB_EnemyType::t0:
            b2_circleshape.m_radius = 1.0f;
            b2_boxshape.SetAsBox(1.0f, 1.0f);
            fixtureDef.density = 5.0f;
            fixtureDef.friction = 0.75f;
            color = 0x30DF30;
            resistence = 90;
            fixtureDef.shape = &b2_circleshape;
        break;

        case eAB_EnemyType::t1:
            b2_circleshape.m_radius = 1.25f;
            b2_boxshape.SetAsBox(1.25f, 1.25f);
            fixtureDef.density = 20.0f;
            fixtureDef.friction = 0.8f;
            color = 0x75DF10;
            resistence = 190;
            fixtureDef.shape = &b2_circleshape;
        break;

        case eAB_EnemyType::t2:
            b2_circleshape.m_radius = 0.5f;
            b2_boxshape.SetAsBox(0.5f, 0.5f);
            fixtureDef.density = 15.0f;
            fixtureDef.friction = 0.6f;
            color = 0x10DF50;
            resistence = 130;
            fixtureDef.shape = &b2_boxshape;
        break;
    }

    // our stored size is in screen units (-1 .. 1)
    // so, we scale all sizes down by world scale
    m_size.x = b2_circleshape.m_radius * World_Scale;
    m_size.y = b2_circleshape.m_radius * World_Scale;

#if false
    for (float a=0; a<=2*M_PIf; a+=(2*M_PIf/16))
    {
        float px = m_size.x * cos(a);
        float py = m_size.y * sin(a);
        addVertex(ldVec2(px, py), color, a>=2*M_PIf);
        color = (uint32_t)color*0.99f;
    }
#else
    float start = M_PIf/2 - 0.5f;
    float end = M_PIf + M_PIf/2 + 0.5f;
    float step = (end-start) / 16;

    for (float a=start; a<=end+step; a+=step)
    {
        float px = m_size.x * sin(a);
        float py = m_size.y * cos(a);

        addVertex(ldVec2(px, py), brightness(color, 1 + py/m_size.y));
    }

    switch (type)
    {
        case eAB_EnemyType::t0:
            addVertex(ldVec2(-m_size.x*0.9f, m_size.y*0.75f), brightness(color, (1+m_size.y*0.75f) / m_size.y), false);
            addVertex(ldVec2(-m_size.x*0.8f, m_size.y*0.5f), brightness(color, (1+m_size.y*0.5f) / m_size.y), false);
            addVertex(ldVec2(m_size.x*0.8f, m_size.y*0.5f), brightness(color, (1+m_size.y*0.5f) / m_size.y), false);
            addVertex(ldVec2(m_size.x*0.9f, m_size.y*0.75f), brightness(color, (1+m_size.y*0.75f) / m_size.y), true);
            addMesh();
            addVertex(ldVec2(-m_size.x*0.8f, m_size.y*0.15f), 0x90A0B0);
            addVertex(ldVec2(-m_size.x*0.5f, m_size.y*0.00f), 0x90A0B0);
            addMesh();
            addVertex(ldVec2(m_size.x*0.5f, m_size.y*0.00f), 0x90A0B0);
            addVertex(ldVec2(m_size.x*0.8f, m_size.y*0.15f), 0x90A0B0);
        break;

        case eAB_EnemyType::t1:
            addVertex(ldVec2(-m_size.x-0.01f, m_size.y*0.9f), brightness(color, (1+m_size.y*0.9f) / m_size.y), false);
            addVertex(ldVec2(0, m_size.y*0.25f), brightness(color, (1+m_size.y*0.25f) / m_size.y), false);
            addVertex(ldVec2(m_size.x+0.02f, m_size.y*0.9f), brightness(color, (1+m_size.y*0.9f) / m_size.y), true);
            addMesh();
            addVertex(ldVec2(-m_size.x*0.75f, m_size.y*0.2f), 0xFFA000);
            addVertex(ldVec2(-m_size.x*0.15f, m_size.y*0.1f), 0xFFFF00);
            addMesh();
            addVertex(ldVec2(m_size.x*0.15f, m_size.y*0.1f), 0xFFFF00);
            addVertex(ldVec2(m_size.x*0.75f, m_size.y*0.2f), 0xFFA000);
        break;

        case eAB_EnemyType::t2:
            addVertex(ldVec2(-m_size.x*0.5f, m_size.y*0.8f), brightness(color, (1+m_size.y*0.8f) / m_size.y), false);
            addVertex(ldVec2(m_size.x*0.5f, m_size.y*0.8f), brightness(color, (1+m_size.y*0.8f) / m_size.y), true);
            addMesh();
            addVertex(ldVec2(-m_size.x*0.85f, m_size.y*0.2f), 0xFFA000);
            addVertex(ldVec2(-m_size.x*0.05f, m_size.y*0.1f), 0xFFFF00);
            addMesh();
            addVertex(ldVec2(m_size.x*0.05f, m_size.y*0.1f), 0xFFFF00);
            addVertex(ldVec2(m_size.x*0.85f, m_size.y*0.2f), 0xFFA000);
        break;
    }

#endif

    fixture = b2_body->CreateFixture(&fixtureDef);
    fixture->SetUserData(this);

    b2_body->SetTransform(b2Vec2(pos.x, pos.y), 0);
    b2_body->SetLinearVelocity(b2Vec2(0, 0));
    b2_body->SetAngularVelocity(0);
    b2_body->SetAwake(true);
}


ldAngryLasersEnemy::~ldAngryLasersEnemy()
{
    b2_body->DestroyFixture(fixture);
    b2_world->DestroyBody(b2_body);
}


ldVec2 ldAngryLasersEnemy::get_position()
{
    // get physics position
    b2Vec2 pos = b2_body->GetPosition();
    pos.x *= World_Scale;
    pos.y *= World_Scale;
    return ldVec2(pos.x, pos.y);
}

bool ldAngryLasersEnemy::render(ldRendererOpenlase* p_renderer)
{
    // get physics position
    b2Vec2 pos = b2_body->GetPosition();

    // check if out of world bounds
    if (pos.x < -World_Size-2 || pos.x > World_Size || pos.y < -World_Size || pos.y > World_Size)
    {
        // out - mission failed
        return false;
    }

    // get physics angle
    float32 angle = b2_body->GetAngle();

    // scale physics position to screen -1..1
    pos.x *= World_Scale;
    pos.y *= World_Scale;

    p_renderer->translate(pos.x, pos.y);
    p_renderer->rotate(-angle);
    renderGradientCyclic(p_renderer, 1);
    p_renderer->loadIdentity();

    return true;
}
