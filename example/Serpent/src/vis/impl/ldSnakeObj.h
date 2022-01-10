#ifndef LASERDOCK_LDSERPENTSNAKE_H__
#define LASERDOCK_LDSERPENTSNAKE_H__

#include "ldSerpentCommon.h"
#include <deque>
#include <functional>
#include "ldRazer/ldMesh.h"

class ldSnakeObj {
public:
    explicit ldSnakeObj(const ldRazer::vec2s& pos={0,0});
    ~ldSnakeObj() = default;

    void setPosition(const ldRazer::vec2s& pos) { m_pos = pos; }
    void setDir(const ldRazer::vec2s& dir) { m_dir = dir; }

    uint16_t getLength() const;
    bool increaseLength();
    bool decreaseLength(uint16_t amount);
    bool setLength(uint16_t length);

    uint16_t getSpeed() const { return m_speed; }
    void increaseSpeed() { m_speed += 1; }
    void decreaseSpeed(uint16_t amount) { m_speed = std::max<uint16_t>(m_speed-amount, 0); }
    void setSpeed(uint16_t speed) { m_speed = std::max<uint16_t>(speed, 0); }

    bool canMove(const ldRazer::vec2s& dir);
    void reset(const ldRazer::vec2s& pos);
    void move(const ldRazer::vec2s& dir);
    bool isSelfIntersection() const;

    void update(float dt);
    void buildGeometry();

    ldRazer::AABB2s_t getHead() const;
    ldRazer::vec2s getPosition() const { return m_pos; }
    ldRazer::vec2s getDir() const { return m_dir; }
    const ldRazer::line2s_arr_t& getGeometry() const;
    ldRazer::vec2f getWorldTranslation() const;

    const ldRazer::ldMesh2f_t& getMesh() const { return m_stripCache; }
    void draw(ldRazer::ldMesh2f_t& mesh);

protected:
    void removeDuplicates(ldRazer::line2s_arr_t& buffer);
    void mergeLines();

private:
    ldRazer::vec2s m_pos = {0, 0};        // The current position in integer coordinates
    ldRazer::vec2f m_posf;                // The current position in real coordinates
    ldRazer::vec2s m_dir = {1, 0};        // The current direction in integer coordinates
    ldRazer::vec2s m_move = {1, 0};       // The next move direction in integer coordinates
    uint16_t m_speed = 8;
    std::deque<ldRazer::vec2s> m_segments;

    // Working Caches - we don't want to reallocate these every frame

    ldRazer::line2s_arr_t m_segmentCache;   // To avoid reallocating this vector per frame...
    ldRazer::line2s_arr_t m_uniqueCache;   // Ditto, for the output of removeDuplicates
    ldRazer::ldMesh2f_t m_stripCache;
};

#endif //__LASERDOCK_LDSERPENTSNAKE_H__
