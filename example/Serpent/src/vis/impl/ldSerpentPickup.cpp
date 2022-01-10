#include "ldSerpentPickup.h"

using namespace ldRazer;

ldSerpentPickup::ldSerpentPickup() = default;

ldSerpentPickup::ldSerpentPickup(const vec2s &position, ldPickupType type, float timer) {
    reset(position, type, timer);
}

ldSerpentPickup::~ldSerpentPickup() = default;

void ldSerpentPickup::reset(const vec2s& position, ldPickupType type, float timer) {
    setPosition(position);
    m_type = type;
    m_timer = timer;
    m_visible = true;

    updatePosition();
}

bool ldSerpentPickup::intersection(const AABB2s_t &bound) const {
    return open_intersection(m_bound, bound);
}

void ldSerpentPickup::setPosition(const vec2s &position) {
    m_pos = position;
    m_bound = AABB2s_t{m_pos, m_pos + vec2s{1, 1}};
}

void ldSerpentPickup::drawTarget(ldMesh2f_t& mesh, const AABB2f_t& bound) {
    static constexpr uint32_t startColour = NO_RED_LASER ? makeARGB(0, 0, 255) : makeARGB(255, 0, 0);
    static constexpr uint32_t endColour = makeARGB(0, 255, 0);
    const auto colour = lerpColour(startColour, endColour, m_oscillator);

    const auto vidx = mesh.vertices.size();
    mesh.vertices.emplace_back(ldVertex2f_t{vec2f(bound.left_bottom.x, bound.left_bottom.y), colour});
    mesh.vertices.emplace_back(ldVertex2f_t{vec2f(bound.right_top.x, bound.right_top.y), colour});
    mesh.indices.emplace_back(ldLineStrip(static_cast<uint32_t>(vidx), 2));
    mesh.vertices.emplace_back(ldVertex2f_t{vec2f(bound.left_bottom.x, bound.right_top.y), colour});
    mesh.vertices.emplace_back(ldVertex2f_t{vec2f(bound.right_top.x, bound.left_bottom.y), colour});
    mesh.indices.emplace_back(ldLineStrip(static_cast<uint32_t>(vidx+2), 2));
}

void ldSerpentPickup::drawShrink(ldMesh2f_t& mesh, const AABB2f_t& bound) {
    static constexpr uint32_t startColour = NO_RED_LASER ? makeARGB(0, 0, 255) : makeARGB(255, 255, 0);
    static constexpr uint32_t endColour = makeARGB(0, 255, 255);
    const auto colourA = lerpColour(startColour, endColour, m_oscillator);
    const auto colourB = lerpColour(endColour, startColour, m_oscillator);

    const auto lP = bound.centre();
    const auto rot = 2.f*(m_oscillator -.5f);
    const auto scale = vec2f(std::min(m_timer*.2f, 1.f));
    const auto vidx = mesh.vertices.size();
    const auto dt = float(M_PI) / 5.f;
    const auto radius = bound.halfExtent().length();
    for(auto i = 0; i != 11; ++i) {
        const auto theta = i*dt;
        mesh.vertices.emplace_back(ldVertex2f_t{lP + scale*radius*vec2f(rot*std::cos(theta), std::sin(theta)), colourA});
    }
    mesh.indices.emplace_back(ldLineStrip(static_cast<uint32_t>(vidx), static_cast<uint32_t>(mesh.vertices.size()-vidx)));

    auto b = m_font.getStringAABB("S");
    m_font.setScale(vec2f(.02f*rot, .02f)*scale);
    m_font.buildString(mesh, lP - b.halfExtent(), "S", colourB, true);
}

void ldSerpentPickup::drawSlow(ldMesh2f_t& mesh, const AABB2f_t& bound) {
    static constexpr uint32_t startColour = makeARGB(0, 255, 0);
    static constexpr uint32_t endColour = NO_RED_LASER ? makeARGB(0, 0, 255) : makeARGB(255, 255, 0);
    const auto colourA = lerpColour(startColour, endColour, m_oscillator);
    const auto colourB = lerpColour(endColour, startColour, m_oscillator);

    const auto lP = bound.centre();
    const auto rot = 2.f*(m_oscillator -.5f);
    const auto scale = vec2f(std::min(m_timer*.2f, 1.f));
    const auto vidx = mesh.vertices.size();
    const auto dt = float(M_PI) / 5.f;
    const auto radius = bound.halfExtent().length();
    for(auto i = 0; i != 11; ++i) {
        const auto theta = i*dt;
        mesh.vertices.emplace_back(ldVertex2f_t{lP + scale*radius*vec2f(rot*std::cos(theta), std::sin(theta)), colourA});
    }
    mesh.indices.emplace_back(ldLineStrip(static_cast<uint32_t>(vidx), static_cast<uint32_t>(mesh.vertices.size()-vidx)));

    auto b = m_font.getStringAABB("R");
    m_font.setScale(vec2f(.02f*rot, .02f)*scale);
    m_font.buildString(mesh, lP - b.halfExtent(), "R", colourB, true);
}

void ldSerpentPickup::drawExtraLife(ldMesh2f_t& mesh, const AABB2f_t& bound) {
    static constexpr uint32_t startColour = NO_RED_LASER ? makeARGB(0, 255, 0) : makeARGB(255, 0, 0);
    static constexpr uint32_t endColour = NO_RED_LASER ? makeARGB(0, 255, 255) : makeARGB(255, 255, 255);
    const auto colourA = lerpColour(startColour, endColour, m_oscillator);
    const auto colourB = lerpColour(endColour, startColour, m_oscillator);

    const auto lP = bound.centre();
    const auto rot = 2.f*(m_oscillator -.5f);
    const auto scale = vec2f(std::min(m_timer*.2f, 1.f));
    const auto vidx = mesh.vertices.size();
    const auto dt = float(M_PI) / 5.f;
    const auto radius = bound.halfExtent().length();
    for(auto i = 0; i != 11; ++i) {
        const auto theta = i*dt;
        mesh.vertices.emplace_back(ldVertex2f_t{lP + scale*radius*vec2f(std::cos(theta), std::sin(theta)), colourA});
    }
    mesh.indices.emplace_back(ldLineStrip(static_cast<uint32_t>(vidx), static_cast<uint32_t>(mesh.vertices.size()-vidx)));

    auto b = m_font.getStringAABB("E");
    m_font.setScale(vec2f(.02f*rot, .02f)*scale);
    m_font.buildString(mesh, lP - b.halfExtent(), "E", colourB, true);
}

void ldSerpentPickup::update(float dt) {
    updatePosition();

    // If we're a timed pickup, decrease the timer and set invisible once done
    if(m_type != ldPickupType::PT_TARGET && m_timer >= 0.f) {
        m_timer -= dt;
        if(m_timer <= 0.f) {
            m_visible = false;
            m_timer = 0.f;
        }
    }

    m_oscillator += m_oscdir*dt;

    if(m_oscillator > 1.f) {
        m_oscillator = 1.f;
        m_oscdir = -1;
    } else if(m_oscillator < 0.f) {
        m_oscillator = 0.f;
        m_oscdir = 1;
    }

}

void ldSerpentPickup::updatePosition() {
    static constexpr float radar_len = VIEWPORT_SIZE/2.f - 1.f;

    // If the distance is greater than half the viewport size, clamp to a disc around the snake
    const auto B = vec2f(m_pos.x, m_pos.y);
    const auto D = B - m_snakePos;
    const auto len = D.length();

    if(len >= radar_len) {
        const auto pos = normalise(D) * radar_len + m_snakePos;
        m_boundf = AABB2f_t(pos, pos+vec2f{1.f, 1.f});
    } else {
        m_boundf = AABB2f_t(vec2f(m_bound.min().x, m_bound.min().y), vec2f(m_bound.max().x, m_bound.max().y));
    }
}

void ldSerpentPickup::draw(ldMesh2f_t &mesh) {
    if(!m_visible) return;

    switch(m_type) {
        case ldPickupType::PT_TARGET:
            drawTarget(mesh, m_boundf);
            break;
        case ldPickupType::PT_SHRINK:
            drawShrink(mesh, m_boundf);
            break;
        case ldPickupType::PT_SLOW:
            drawSlow(mesh, m_boundf);
            break;
        case ldPickupType::PT_EXTRA_LIFE:
            drawExtraLife(mesh, m_boundf);
            break;
    }
}
