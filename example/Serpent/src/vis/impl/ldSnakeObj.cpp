#include "ldSnakeObj.h"

using namespace ldRazer;

ldSnakeObj::ldSnakeObj(const vec2s& pos) : m_pos(pos) {
    m_segmentCache.resize(MAX_SNAKE_LEN*4);         // Cache for duplicated lines
    m_uniqueCache.resize(MAX_SNAKE_LEN*3);          // Cache for unique lines (at least 1 shared segment)
    m_posf.set(pos.x, pos.y);
}

uint16_t ldSnakeObj::getLength() const {
    return uint16_t(m_segments.size());
}

bool ldSnakeObj::increaseLength() {
    if(m_segments.size() < MAX_SNAKE_LEN) {
        m_segments.push_back(m_segments.empty() ? m_pos : m_segments.back());
        return true;
    }
    return false;
}

bool ldSnakeObj::decreaseLength(uint16_t amount) {
    if(m_segments.size() > amount) {
        m_segments.resize(m_segments.size()-amount);
        return true;
    }
    return false;
}

bool ldSnakeObj::setLength(uint16_t length) {
    if(m_segments.size() > length) {
        m_segments.resize(length);
        return true;
    } else if(length < MAX_SNAKE_LEN) {
        m_segments.resize(length, m_segments.back());
        return true;
    }
    return false;
}

bool ldSnakeObj::isSelfIntersection() const {
    return m_segments.size() < 2 ? false : std::find(m_segments.begin(), m_segments.end(), m_pos) != m_segments.end();
}

bool ldSnakeObj::canMove(const ldRazer::vec2s &dir) {
    return m_dir != -dir;
}

void ldSnakeObj::reset(const ldRazer::vec2s& pos) {
    m_segments.clear();
    m_pos = pos;
    m_posf.set(pos.x, pos.y);
    m_speed = 8;
}

void ldSnakeObj::move(const vec2s& dir) {
    if(canMove(dir)) m_move = dir;
}

void ldSnakeObj::update(float dt) {
    static float step = 0.f;
    step += m_speed*dt;

    if(step > 1.f) {
        m_segments.push_front(m_pos);

        m_pos += m_dir;
        m_dir = m_move;
        step = 0.f;

        m_segments.pop_back();
    }

    auto curr = vec2f(m_pos.x, m_pos.y), next = curr + vec2f(m_dir.x, m_dir.y);
    m_posf = lerp(curr, next, step);
}

vec2f ldSnakeObj::getWorldTranslation() const {
    return m_posf;
}

inline void buildBlock(const vec2s& pos, line2s_arr_t& buffer) {
    buffer.push_back(line2s_t(pos, pos+vec2s(1,0)));
    buffer.push_back(line2s_t(buffer.back().B, pos+vec2s(1,1)));
    buffer.push_back(line2s_t(pos+vec2s(0,1), buffer.back().B));
    buffer.push_back(line2s_t(pos, buffer.back().A));
}

// Remove any duplicate lines (lines that the snake shares with other segments in the snake)
void ldSnakeObj::removeDuplicates(line2s_arr_t& buffer) {
    m_uniqueCache.clear();

    std::sort(buffer.begin(), buffer.end(), [](const line2s_t& A, const line2s_t& B) {
        return A.A < B.A || (A.A == B.A && A.B < B.B);
    });

    for(size_t i = 0, curr = 0; i != buffer.size(); ) {
        while(i != buffer.size() && buffer[curr] == buffer[i]) ++i;
        if(curr + 1 == i) m_uniqueCache.emplace_back(buffer[curr]);
        curr = i;
    }
}

void ldSnakeObj::mergeLines() {
    static uint32_t const red = NO_RED_LASER ? makeARGB(0, 255, 0) : makeARGB(255, 0, 0);
    static uint32_t const green = makeARGB(0, 255, 0);

    m_stripCache.indices.clear();
    m_stripCache.vertices.clear();

    m_stripCache.indices.reserve(m_uniqueCache.size());       // Resize for worst case
    m_stripCache.vertices.reserve(2*m_uniqueCache.size());

    if(!m_uniqueCache.empty()) {
        // Order all the points by y and then x, swap endpoints if A > B
        for (auto &l : m_uniqueCache) if (l.B < l.A) std::swap(l.A, l.B);

        std::sort(m_uniqueCache.begin(), m_uniqueCache.end(), [](const line2s_t &A, const line2s_t &B) {
            return A.A < B.A || (A.A == B.A && A.B < B.B);
        });

        // These are integer lines so we can directly test equality
        std::vector<line2s_arr_t> strips;
        for (size_t i = 0; i != m_uniqueCache.size(); ++i) {
            const auto &line = m_uniqueCache[i];
            bool assigned = false;
            for (auto &strip : strips) {
                if (strip.back().B == line.A) {   // Concatenate to end
                    if (strip.back().dir() == line.dir()) {
                        strip.back().B = line.B;
                        assigned = true;
                        break;
                    } else {
                        strip.push_back(line);
                        assigned = true;
                        break;
                    }
                }
            }

            if (!assigned) strips.push_back({line});
        }

        for(const auto& strip : strips) {
            m_stripCache.indices.emplace_back(ldLineStrip(static_cast<uint32_t>(m_stripCache.vertices.size()), 0, true));
            for(const auto& l : strip) {
                m_stripCache.vertices.emplace_back(ldVertex2f_t{convert(l.A), green});
            }
            m_stripCache.vertices.emplace_back(ldVertex2f_t{convert(strip.back().B), green});
            m_stripCache.indices.back().count = static_cast<uint32_t>(m_stripCache.vertices.size() - m_stripCache.indices.back().start);
        }
    }

    // Draw the head
    vec2f lb, lt, rb, rt;
    if(m_dir.x != 0) {  // Horizontal
        if(m_dir.x == 1) {  // Right
            lb.set(m_pos.x, m_pos.y);
            lt.set(m_pos.x, m_pos.y+1);
            rb.set(m_posf.x+1, m_pos.y);
            rt.set(m_posf.x+1, m_pos.y+1);
        } else {
            rb.set(m_pos.x+1, m_pos.y);
            rt.set(m_pos.x+1, m_pos.y+1);
            lb.set(m_posf.x, m_pos.y);
            lt.set(m_posf.x, m_pos.y+1);
        }                   // Left
    } else {            // Vertical
        if(m_dir.y == 1) {  // Up
            lb.set(m_pos.x, m_pos.y);
            lt.set(m_pos.x, m_posf.y+1);
            rb.set(m_pos.x+1, m_pos.y);
            rt.set(m_pos.x+1, m_posf.y+1);
        } else {            // Down
            lb.set(m_pos.x, m_posf.y);
            lt.set(m_pos.x, m_pos.y+1);
            rb.set(m_pos.x+1, m_posf.y);
            rt.set(m_pos.x+1, m_pos.y+1);
        }
    }

    // Draw the Red head which moves smoothly
    m_stripCache.indices.emplace_back(ldLineStrip(static_cast<uint32_t>(m_stripCache.vertices.size()), 5, false));
    m_stripCache.vertices.emplace_back(ldVertex2f_t{lb, red});
    m_stripCache.vertices.emplace_back(ldVertex2f_t{rb, red});
    m_stripCache.vertices.emplace_back(ldVertex2f_t{rt, red});
    m_stripCache.vertices.emplace_back(ldVertex2f_t{lt, red});
    m_stripCache.vertices.emplace_back(ldVertex2f_t{lb, red});
}

AABB2s_t ldSnakeObj::getHead() const {
    const auto P = m_pos + m_dir;
    return { P, P + vec2s(1, 1) };
}

void ldSnakeObj::buildGeometry() {
    m_segmentCache.clear();
    for(const auto& seg : m_segments) buildBlock(seg, m_segmentCache);
    removeDuplicates(m_segmentCache);
    mergeLines();
}

const line2s_arr_t& ldSnakeObj::getGeometry() const {
    return m_uniqueCache;             // Contains the unique lines
}

void ldSnakeObj::draw(ldMesh2f_t &mesh) {
    buildGeometry();
    auto offset = mesh.vertices.size();
    for(auto& idx : m_stripCache.indices) idx.start += offset;
    std::copy(m_stripCache.indices.begin(), m_stripCache.indices.end(), std::back_inserter(mesh.indices));
    std::copy(m_stripCache.vertices.begin(), m_stripCache.vertices.end(), std::back_inserter(mesh.vertices));
}
