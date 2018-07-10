#ifndef ldSpiralFighterMissile_H
#define ldSpiralFighterMissile_H

#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Visualizations/util/MathsHelper/ldMaths.h"
#include "ldCore/Visualizations/util/ColorHelper/ldColorUtil.h"

#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameObject.h"

class ldSpiralFighterMissile : public ldGameObject
{
public:
    // Constructor and destructor.
    ldSpiralFighterMissile(Vec2 direction);
    ~ldSpiralFighterMissile();

protected:
    virtual void init() override;
    virtual void updateGameObject(float deltaTime) override;
    virtual void drawGameObject(ldRendererOpenlase* p_renderer) override;

private:

};

#endif // ldSpiralFighterMissile_H
