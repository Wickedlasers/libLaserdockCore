#ifndef LASERDOCK_SIM_LDSERPENTPICKUP_H
#define LASERDOCK_SIM_LDSERPENTPICKUP_H

#include "ldSerpentCommon.h"
#include "ldRazer/ldMesh.h"
#include "ldRazer/ldLaserFont.h"

enum class ldPickupType {
    PT_TARGET,              // The target the snake has to "eat" to complete the level
    PT_SHRINK,              // A shrink snake powerup
    PT_SLOW,                // A reduce speed powerup
    PT_EXTRA_LIFE           // An extra life
};

class ldSerpentPickup {
public:
    ldSerpentPickup();
    ldSerpentPickup(const ldRazer::vec2s& position, ldPickupType type, float timer=0.f);
    ~ldSerpentPickup();

    void reset(const ldRazer::vec2s& position, ldPickupType type, float timer);

    bool isVisible() const { return m_visible; }
    void setVisible(bool v) { m_visible = v; }
    ldPickupType getType() const { return m_type; }

    bool intersection(const ldRazer::AABB2s_t& bound) const;

    void setSnakePosition(const ldRazer::vec2f& pos) { m_snakePos = pos; }

    void setPosition(const ldRazer::vec2s& position);
    const ldRazer::vec2s& getPosition() const { return m_pos; }

    void update(float dt);

    const ldRazer::AABB2s_t& worldBound() const { return m_bound; }
    void draw(ldRazer::ldMesh2f_t& mesh);

protected:
    void updatePosition();

    void drawTarget(ldRazer::ldMesh2f_t& mesh, const ldRazer::AABB2f_t& bound);
    void drawShrink(ldRazer::ldMesh2f_t& mesh, const ldRazer::AABB2f_t& bound);
    void drawSlow(ldRazer::ldMesh2f_t& mesh, const ldRazer::AABB2f_t& bound);
    void drawExtraLife(ldRazer::ldMesh2f_t& mesh, const ldRazer::AABB2f_t& bound);

private:
    ldRazer::vec2s m_pos = {-1, -1};      // Invalid pos
    ldPickupType m_type = ldPickupType::PT_TARGET;
    ldRazer::AABB2s_t m_bound = ldRazer::AABB2s_t{ldRazer::vec2s{-1, -1}, ldRazer::vec2s{0, 0}};
    ldRazer::AABB2f_t m_boundf = ldRazer::AABB2f_t{ldRazer::vec2f{-1, -1}, ldRazer::vec2f{0, 0}};
    ldRazer::vec2f m_snakePos = {-1.f, -1.f};
    ldRazer::ldLaserFont m_font;
    float m_oscillator = 0.f;
    float m_timer = 0.f;
    int8_t m_oscdir = 1;
    bool m_visible = false;
};

#endif //LASERDOCK_SIM_LDSERPENTPICKUP_H
