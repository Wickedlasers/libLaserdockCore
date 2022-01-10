//
// Laser Angry Lasers
// By Vander R. N. Dias a.k.a. "imerso" @ imersiva.com
// Copyright(c) 2019 Wicked Lasers
//

#include "ldAngryLasersBox.h"
#include <QtCore/QDebug>

using namespace std;


ldAngryLasersBox::ldAngryLasersBox(std::shared_ptr<b2World> world, ldVec2 pos, ldVec2 size, eAB_BoxType type)
    : ldGradientObject(0.33f)
{
    b2_bodydef.position.Set(0, 0);
    b2_world = world;

    // our stored size is in screen units (-1 .. 1)
    // so, we scale all sizes down by world scale
    m_size = size;
    m_size.x *= World_Scale;
    m_size.y *= World_Scale;

    addMesh();

    switch (type)
    {
        case eAB_BoxType::Wood:
            b2_bodydef.type = b2_dynamicBody;
            fixtureDef.density = 10.0f;
            fixtureDef.friction = 0.75f;
            addVertex(ldVec2(-m_size.x, +m_size.y), 0x959525);
            addVertex(ldVec2(-m_size.x, -m_size.y), 0x151510);
            addVertex(ldVec2(+m_size.x, -m_size.y), 0x151510);
            addVertex(ldVec2(+m_size.x, +m_size.y), 0x959525, true);
        break;

        case eAB_BoxType::Metal:
            b2_bodydef.type = b2_dynamicBody;
            fixtureDef.density = 35.0f;
            fixtureDef.friction = 0.8f;
            addVertex(ldVec2(-m_size.x, +m_size.y), 0x505090);
            addVertex(ldVec2(-m_size.x, -m_size.y), 0x20FF1F);
            addVertex(ldVec2(+m_size.x, -m_size.y), 0x20FF1F);
            addVertex(ldVec2(+m_size.x, +m_size.y), 0x505090, true);
        break;

        case eAB_BoxType::Fixed:
            b2_bodydef.type = b2_staticBody;
            fixtureDef.density = 35.0f;
            fixtureDef.friction = 0.8f;
            addVertex(ldVec2(-m_size.x, +m_size.y), 0xC02010);
            addVertex(ldVec2(-m_size.x, -m_size.y), 0xA01015);
            addVertex(ldVec2(+m_size.x, -m_size.y), 0xA01015);
            addVertex(ldVec2(+m_size.x, +m_size.y), 0xC02010, true);
        break;
    }

    b2_body = b2_world->CreateBody(&b2_bodydef);
    b2_boxshape.SetAsBox(size.x, size.y);
    fixtureDef.shape = &b2_boxshape;
    fixture = b2_body->CreateFixture(&fixtureDef);

    b2_body->SetTransform(b2Vec2(pos.x, pos.y), 0);
    b2_body->SetLinearVelocity(b2Vec2(0, 0));
    b2_body->SetAngularVelocity(0);
    b2_body->SetAwake(true);
}


ldAngryLasersBox::~ldAngryLasersBox()
{
    b2_body->DestroyFixture(fixture);
    b2_world->DestroyBody(b2_body);
}


void ldAngryLasersBox::render(ldRendererOpenlase* p_renderer)
{
    // get physics position
    b2Vec2 pos = b2_body->GetPosition();

    // get physics angle
    float32 angle = b2_body->GetAngle();

    // scale physics position to screen -1..1
    pos.x *= World_Scale;
    pos.y *= World_Scale;

    p_renderer->translate(pos.x, pos.y);
    p_renderer->rotate(-angle);
    renderGradient(p_renderer, 2);
    p_renderer->loadIdentity();
}
