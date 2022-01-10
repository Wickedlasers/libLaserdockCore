//
// Laser Angry Lasers
// By Vander R. N. Dias a.k.a. "imerso" @ imersiva.com
// Copyright(c) 2019 Wicked Lasers
//

#include <QtCore/QtDebug>
#include "ldAngryLasersPlayer.h"

using namespace std;


ldAngryLasersPlayer::ldAngryLasersPlayer()
{
    ldGameObject::init();
    useGravity = false;
}


void ldAngryLasersPlayer::initWorld(std::shared_ptr<b2World> world)
{
    b2_bodydef.position.Set(-World_Size*0.8f, -World_Size * 0.5f);
    b2_bodydef.type = b2_dynamicBody;

    b2_world = world;
    b2_body = world->CreateBody(&b2_bodydef);
    b2_body->SetLinearDamping(0.25f);
    b2_body->SetAngularDamping(0.5f);

    b2_shape.m_p.Set(0, 0);
    b2_shape.m_radius = 0.75f;
    size.x = b2_shape.m_radius * World_Scale;
    size.y = b2_shape.m_radius * World_Scale;

    fixtureDef.shape = &b2_shape;
    fixtureDef.density = 35.0f;
    fixtureDef.friction = 0.5f;
    fixtureDef.restitution = 0.35f;

    fixture = b2_body->CreateFixture(&fixtureDef);

    resetPosition();
}


ldAngryLasersPlayer::~ldAngryLasersPlayer()
{
    b2_body->DestroyFixture(fixture);
    b2_world->DestroyBody(b2_body);
}


void ldAngryLasersPlayer::resetPosition()
{
    launched = false;
    origin_pos = ldVec2(-World_Size * 0.75f, -World_Size * 0.5f);
    origin_pos.y += -10 + (rand()%32768) * (20.0f / 32768);
    launch_pos = origin_pos;
    time_stopped = 0;

    b2_body->SetTransform(b2Vec2(launch_pos.x, launch_pos.y), 0);
    b2_body->SetLinearVelocity(b2Vec2(0, 0));
    b2_body->SetAngularVelocity(0);
    b2_body->SetAwake(true);

    visible = true;
}


void ldAngryLasersPlayer::updateGameObject(float deltaTime)
{
    //ldGameObject::updateGameObject(deltaTime);

    color = ((color * 126453) ^ 654321);

    if (!launched)
    {
        b2_body->SetTransform(b2Vec2(launch_pos.x, launch_pos.y), 0);
        b2_body->SetLinearVelocity(b2Vec2(0, 0));
        b2_body->SetAngularVelocity(0);
        b2_body->SetAwake(true);

        if (m_pressingLeft && launch_pos.x > -World_Size)
        {
            launch_pos.x -= 0.2f;
        }

        if (m_pressingRight && launch_pos.x < World_Size)
        {
            launch_pos.x += 0.2f;
        }

        if (m_pressingUp && launch_pos.y < World_Size)
        {
            launch_pos.y += 0.2f;
        }

        if (m_pressingDown && launch_pos.y > -World_Size)
        {
            launch_pos.y -= 0.2f;
        }

        if (m_pressingShoot)
        {
            launched = true;
            ldVec2 force = origin_pos - launch_pos;
            force.x *= 7;
            force.y *= 7;
            b2_body->SetLinearVelocity(b2Vec2(force.x, force.y));
        }
    }
    else
    {
        // check if stopped
        if (b2_body->GetLinearVelocity().Length() < 0.25f)
        {
            time_stopped += deltaTime;
            if (time_stopped > 1)
            {
                // respawn
                resetPosition();
                onStopped(this);
            }
        }
        else time_stopped = 0;
    }
}


void ldAngryLasersPlayer::drawGameObject(ldRendererOpenlase* p_renderer)
{
    //ldGameObject::drawGameObject(p_renderer);

    //
    // before launching
    //

    if (!launched)
    {
        p_renderer->begin(OL_LINESTRIP);
        p_renderer->vertex(origin_pos.x*World_Scale, origin_pos.y*World_Scale, color, 1);
        p_renderer->vertex(launch_pos.x*World_Scale, launch_pos.y*World_Scale, color ^ 0x12345678, 1);
        p_renderer->end();
    }

    //
    // draw player
    //

    // get physics position
    b2Vec2 pos = b2_body->GetPosition();

    // check if out of world bounds
    if (pos.x < -World_Size-3 || pos.x > World_Size || pos.y < -World_Size)
    {
        // out - respawn
        resetPosition();
        onStopped(this);
        return;
    }

    // get physics angle
    float32 angle = b2_body->GetAngle();

    // scale physics position to screen -1..1
    pos.x *= World_Scale;
    pos.y *= World_Scale;

    p_renderer->begin(OL_LINESTRIP);
    p_renderer->translate(pos.x, pos.y);
    p_renderer->rotate(-angle);
    p_renderer->drawCircle(0, 0, size.x, color);
    p_renderer->end();

    p_renderer->loadIdentity();
}


/*
 * Input functions.
 */


void ldAngryLasersPlayer::onPressedLeft(bool pressed)
{
    m_pressingLeft = pressed;
}


void ldAngryLasersPlayer::onPressedRight(bool pressed)
{
    m_pressingRight = pressed;
}


void ldAngryLasersPlayer::onPressedUp(bool pressed)
{
    m_pressingUp = pressed;
}


void ldAngryLasersPlayer::onPressedDown(bool pressed)
{
    m_pressingDown = pressed;
}


void ldAngryLasersPlayer::onPressedShoot(bool pressed)
{
    m_pressingShoot = pressed;
}


void ldAngryLasersPlayer::releaseKeys()
{
    m_pressingLeft = false;
    m_pressingRight = false;
    m_pressingUp = false;
    m_pressingDown = false;
    m_pressingShoot = false;
}
