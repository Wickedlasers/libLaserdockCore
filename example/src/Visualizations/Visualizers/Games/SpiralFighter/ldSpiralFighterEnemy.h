#ifndef ldSpiralFighterEnemy_H
#define ldSpiralFighterEnemy_H

#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Visualizations/util/MathsHelper/ldMaths.h"

#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameObject.h"

class ldSpiralFighterEnemy : public ldGameObject
{
public:
    // Constructor and destructor.
    ldSpiralFighterEnemy(Vec2 pos);
    ~ldSpiralFighterEnemy();

    void rotate();
    bool hasRotated();

protected:
    virtual void init() override;
    virtual void updateGameObject(float deltaTime) override;
    virtual void drawGameObject(ldRendererOpenlase* p_renderer) override;

private:
    float m_rotatedAngle;
    float m_targetRotation;
    bool m_hasRotated;
    bool m_rotating;
    int m_type;
    float m_speed;
};

#endif // ldSpiralFighterEnemy_H
