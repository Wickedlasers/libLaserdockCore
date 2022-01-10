//
// Laser Angry Lasers
// By Vander R. N. Dias a.k.a. "imerso" @ imersiva.com
// Copyright(c) 2019 Wicked Lasers
//

#ifndef ldAngryLasersBox_H
#define ldAngryLasersBox_H

#include <Box2D/Box2D.h>
#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldAngryLasersPlayer.h"
#include "ldGradientObject.h"
#include "spline.h"
#include "globals.h"



class ldAngryLasersBox : public ldGradientObject
{
public:

    enum eAB_BoxType { Wood, Metal, Fixed };

    // Constructor and destructor.
    ldAngryLasersBox(std::shared_ptr<b2World> world, ldVec2 pos, ldVec2 m_size, eAB_BoxType type);
    ~ldAngryLasersBox();

    void render(ldRendererOpenlase* p_renderer);

protected:

    ldVec2 m_size;
    std::shared_ptr<b2World> b2_world;
    b2BodyDef b2_bodydef;
    b2Body* b2_body;
    b2FixtureDef fixtureDef;
    b2Fixture* fixture;
    b2PolygonShape b2_boxshape;
};

#endif // ldAngryLasersBox_H
