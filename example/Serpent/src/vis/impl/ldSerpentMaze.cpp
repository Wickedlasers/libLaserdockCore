//
// Created by otgaard on 2018/08/01.
//

#include <list>
#include "ldSerpentMaze.h"
#include "ldRazer/ldQueries.h"

using namespace ldRazer;

ldSerpentMaze::ldSerpentMaze() = default;
ldSerpentMaze::~ldSerpentMaze() = default;

// Generate a starting position within the maze
vec2s ldSerpentMaze::generatePosition(const vec2s& extent) const {
    ldRandom<int16_t> rnd(0);

    vec2s pos;
    AABB2s_t bound;
    bool intersects = false;

    do {
        pos = rnd.random2(m_width, m_height);
        bound = AABB2s_t{pos - extent, pos + extent};
        for(const auto& room: m_rooms) {
            intersects = open_intersection(bound, room);
            if(intersects) break;
        }
    } while(intersects && !m_rooms.empty());

    return pos;
}

bool ldSerpentMaze::initialise(int16_t width, int16_t height, const AABB2s_arr_t& rooms, const linecol_arr_t& walls) {
    m_width = width;
    m_height = height;
    m_rooms = rooms;
    m_walls = walls;
    m_wallsf.resize(walls.size());
    m_visible.resize(walls.size());
    m_worldBound.setCorners(vec2s(0, 0), vec2s(width, height));
    std::transform(m_walls.begin(), m_walls.end(), m_wallsf.begin(), [](const linecol_arr_t::value_type& l) {
        return convert(l.first);
    });
    return true;
}

bool ldSerpentMaze::intersection(const vec2s& pos) const {
    for(const auto& room : m_rooms) {
        if(::intersection(room, pos)) return true;
    }
    return false;
}

bool ldSerpentMaze::intersection(const AABB2s_t& bound) const {
    if(!::open_intersection(worldBound(), bound)) return true;

    for(const auto& room : m_rooms) {
        if(::open_intersection(room, bound)) return true;
    }
    return false;
}

std::vector<AABB2s_t> ldSerpentMaze::query(const vec2s& pos) const {
    std::vector<AABB2s_t> result;
    for(const auto& room : m_rooms) {
        if(::intersection(room, pos)) result.emplace_back(room);
    }
    return result;
}

std::vector<AABB2s_t> ldSerpentMaze::query(const AABB2s_t& bound) const {
    std::vector<AABB2s_t> result;
    for(const auto& room : m_rooms) {
        if(::open_intersection(room, bound)) result.emplace_back(room);
    }
    return result;
}

line2f_t ldSerpentMaze::clipToDisc(const disc2f_t &disc, const line2f_t &line) {
    float parms[2];
    auto iP = ::intersection(disc, line, parms);
    if(iP == 2) {
        const auto d = line.dir();
        return {line.A + parms[0]*d, line.A + parms[1]*d};
    } else if(iP == 1) {
        return line;        // The whole line is visible, coincident with a view ray
    }

    return {vec2f(0, 0), vec2f(0, 0)};
}

const linecolf_arr_t& ldSerpentMaze::renderBatch() {
    m_lineBatch.clear();
    if(!m_snakePtr) return m_lineBatch;

    // Clear the visible array
    m_visible.clear();
    m_visible.resize(m_walls.size(), true);

    // Create the circular viewport
    const auto centre = m_snakePtr->getWorldTranslation() + vec2f{.5f, .5f};
    const auto radius = VIEWPORT_SIZE/2.8f - 1.f;
    const disc2f_t disc(centre, radius);

    const auto dim = float(std::max(m_width, m_height));
    auto lines = clipByVisibility(centre, dim*dim);
    for(auto& l : lines) {
        // Clip to disc
        auto line = clipToDisc(disc, l.first);
        if(line.isDegenerate()) continue;
        m_lineBatch.emplace_back(std::make_pair(line, m_walls[l.second].second));
    }

    // Merge into linestrips
    m_mesh.indices.clear();
    m_mesh.vertices.clear();

    mergeSegments(m_lineBatch, m_mesh, true);
    return m_lineBatch;
}

void ldSerpentMaze::clipByRaycast(const vec2f &centre, float radius, int steps) {
    const auto dtheta = 2.f*float(M_PI)/steps;

    for(int t = 0; t != steps; ++t) {
        auto line = line2f_t(centre, centre + polarToCartesian(t*dtheta, radius+2.f));
        auto idx = ::query(line, m_wallsf);
        if(idx != size_t(-1)) m_visible[idx] = true;
    }
}

lineidx_arr_t ldSerpentMaze::clipByVisibility(const vec2f& centre, float radius) {
    const auto epsilon = .0001f;
    const auto delta_theta = 10.f*std::numeric_limits<float>::epsilon();

    if(m_workingSet.size() < m_wallsf.size()*2) m_workingSet.resize(m_wallsf.size()*2);

    for(size_t i = 0, end = m_wallsf.size(); i != end; ++i) {
        const auto& ln = m_wallsf[i];
        const size_t idx = 2*i;
        m_workingSet[idx].idx = i;
        m_workingSet[idx].point = ln.A;
        m_workingSet[idx].theta = computeAngle(normalise(ln.A - centre), epsilon);
        m_workingSet[idx+1].idx = i;
        m_workingSet[idx+1].point = ln.B;
        m_workingSet[idx+1].theta = computeAngle(normalise(ln.B - centre), epsilon);
    }

    std::sort(m_workingSet.begin(), m_workingSet.end(), [](const pointData& A, const pointData& B) {
        return A.theta < B.theta;
    });

    if(m_workingSet.empty()) return {};

    const auto start_ray = line2f_t(centre, centre + polarToCartesian(0.f, radius));
    size_t curr_nearest = ::query(start_ray, m_wallsf);
    if(curr_nearest == size_t(-1)) return {};

    m_openList.clear();

    // Add all lines that intersect the starting line to open
    auto startingSet = ::queryAll(line2f_t(centre, centre + polarToCartesian(0.f, radius)), m_wallsf);
    std::copy(startingSet.begin(), startingSet.end(), std::back_inserter(m_openList));
    vec2f points[2];
    auto count = ::intersection(m_wallsf[curr_nearest], start_ray, points);
    if(count == 0) return {};
    auto start_pos = points[0];
    const auto origin_pos = points[0];

    m_visibilityResult.clear();

    for(size_t idx = 0; idx != m_workingSet.size(); ++idx) {
        const auto& P = m_workingSet[idx];

        auto ray_o = line2f_t(centre, centre + polarToCartesian(P.theta, radius));
        auto ray_d = line2f_t(centre, centre + polarToCartesian(P.theta + delta_theta, radius));

        auto it = std::find(m_openList.begin(), m_openList.end(), P.idx);
        if(it == m_openList.end()) m_openList.emplace_back(P.idx);
        else                       m_openList.erase(it);

        auto prev_nearest = curr_nearest;
        curr_nearest = ::query(ray_d, m_wallsf);
        if(prev_nearest != curr_nearest) {
            // Close the line and add it to the output
            count = ::intersection(m_wallsf[prev_nearest], ray_o, points);
            if(count == 0) {
                m_visibilityResult.emplace_back(
                        std::make_pair(
                                line2f_t(start_pos, projectToSegment(m_wallsf[prev_nearest], P.point)),
                                prev_nearest));
            } else {
                m_visibilityResult.emplace_back(
                        std::make_pair(
                                line2f_t(start_pos, points[0]),
                                prev_nearest));
            }

            count = ::intersection(m_wallsf[curr_nearest], ray_o, points);
            if(count == 0) start_pos = projectToSegment(m_wallsf[curr_nearest], P.point);
            else           start_pos = points[0];
        }
    }

    // Close the loop
    m_visibilityResult.emplace_back(std::make_pair(line2f_t(start_pos, origin_pos), curr_nearest));
    return m_visibilityResult;
}

void ldSerpentMaze::draw(ldMesh2f_t &mesh) {
    renderBatch();
    mesh += m_mesh;
}
