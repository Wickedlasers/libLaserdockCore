//
// Laser Angry Lasers
// By Vander R. N. Dias a.k.a. "imerso" @ imersiva.com
// Copyright(c) 2019 Wicked Lasers
//

#ifndef ldAngryLasersEnemy_H
#define ldAngryLasersEnemy_H

#include "Box2D/Box2D.h"
#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameObject.h"
#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldAngryLasersPlayer.h"
#include "ldGradientObject.h"
#include "spline.h"
#include "globals.h"



class ldAngryLasersEnemy : public ldGradientObject
{
public:

    enum eAB_EnemyType { t0, t1, t2 };
    bool dead;
    float resistence;

    // Constructor and destructor.
    ldAngryLasersEnemy(std::shared_ptr<b2World> world, ldVec2 pos, eAB_EnemyType p_type);
    ~ldAngryLasersEnemy();

    bool render(ldRendererOpenlase* p_renderer);
    ldVec2 get_position();

protected:

    eAB_EnemyType type;
    ldVec2 m_size;
    std::shared_ptr<b2World> b2_world;
    b2BodyDef b2_bodydef;
    b2Body* b2_body;
    b2FixtureDef fixtureDef;
    b2Fixture* fixture;
    b2CircleShape b2_circleshape;
    b2PolygonShape b2_boxshape;
};

#endif // ldAngryLasersEnemy_H
