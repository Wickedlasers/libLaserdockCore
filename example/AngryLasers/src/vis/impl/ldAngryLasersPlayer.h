//
// Laser Angry Lasers
// By Vander R. N. Dias a.k.a. "imerso" @ imersiva.com
// Copyright(c) 2019 Wicked Lasers
//

#ifndef ldAngryLasersPlayer_H
#define ldAngryLasersPlayer_H

#include <memory>

#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameObject.h"

#include <Box2D/Box2D.h>

#include "globals.h"



class ldAngryLasersPlayer : public ldGameObject
{
public:

    // Constructor and destructor.
    ldAngryLasersPlayer();
    ~ldAngryLasersPlayer();

    bool launched = false;

    void initWorld(std::shared_ptr<b2World> world);
    void resetPosition();

    // Callbacks.
    std::function<void(ldAngryLasersPlayer*)> onFire;
    std::function<void(ldAngryLasersPlayer*)> onStopped;

    // Input functions
    void onPressedLeft(bool pressed);
    void onPressedRight(bool pressed);
    void onPressedUp(bool pressed);
    void onPressedDown(bool pressed);
    void onPressedShoot(bool pressed);
    void releaseKeys();

protected:

    virtual void updateGameObject(float deltaTime) override;
    virtual void drawGameObject(ldRendererOpenlase* p_renderer) override;

private:

    std::shared_ptr<b2World> b2_world;
    b2BodyDef b2_bodydef;
    b2Body* b2_body;
    b2FixtureDef fixtureDef;
    b2Fixture* fixture;
    b2CircleShape b2_shape;

    // reference origin position for launching the player
    ldVec2 origin_pos;

    ldVec2 launch_pos;
    float time_stopped = 0;
    uint32_t color = 0x12345678;

    // Input.
    bool m_pressingShoot;
    bool m_pressingLeft;
    bool m_pressingRight;
    bool m_pressingUp;
    bool m_pressingDown;
};

#endif // ldAngryLasersPlayer_H
