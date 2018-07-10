#ifndef ldSpiralFighterPlayer_H
#define ldSpiralFighterPlayer_H

#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Visualizations/util/MathsHelper/ldMaths.h"

#include "ldCore/Visualizations/Visualizers/Games/Core/ldGameObject.h"

class ldSpiralFighterPlayer : public ldGameObject
{
public:
    // Constructor and destructor.
    ldSpiralFighterPlayer();
    ~ldSpiralFighterPlayer();

    // Functions
    Vec2 getMissileSpawnPosition();

    void givePowerup();
    bool hasUsedPowerup();

    // Callbacks.
    function<void(ldSpiralFighterPlayer*)> onFire;
    function<void()> onPowerup;

    // Input functions
    void onPressedLeft(bool pressed);
    void onPressedRight(bool pressed);
    void onPressedShoot(bool pressed);
    void onPressedPowerup(bool pressed);
    void releaseKeys();

protected:
    virtual void init() override;
    virtual void updateGameObject(float deltaTime) override;
    virtual void drawGameObject(ldRendererOpenlase* p_renderer) override;

private:
    float m_targetRotation;
    bool m_hasUsedPowerup;

    // Timers.
    float m_cooldownTimer;

    // Input.
    bool m_pressingLeft;
    bool m_pressingRight;
    bool m_pressingShoot;
    bool m_pressingPowerup;
};

#endif // ldSpiralFighterPlayer_H
