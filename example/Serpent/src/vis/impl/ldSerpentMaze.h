//
//  ldSerpentMaze.h
//  LaserdockVisualizer
//
//  Created by Darren Otgaar 31/07/2018
//  Copyright (c) 2018 Wicked Lasers. All rights reserved.
//

#ifndef LASERDOCK_LDSERPENTMAZE_H__
#define LASERDOCK_LDSERPENTMAZE_H__

#include "ldSerpentCommon.h"
#include "ldSnakeObj.h"
#include "ldRazer/ldMesh.h"

/**
 * The Maze class holds the entirety of the map and provides methods for querying intersections and for performing
 * visibility cone queries returning a minimal set of lines to render based on the snake's line of sight.  The snake
 * should move in a rectangle/square approximately 40% - 60% of the screen size.  When the snake is at the border of the
 * rectangle, the rectangle should move in the direction the snake is travelling and should move the viewport of the
 * maze with it.
 *
 * The maze class needs to be able to dump to image for debugging.
 */

// Holds a clipped line and the index to the parent line in m_walls, m_wallsf, and m_visible
using lineidx_arr_t = std::vector<std::pair<ldRazer::line2f_t, size_t>>;

struct pointData {
    ldRazer::vec2f point = {0, 0};
    size_t idx = size_t(-1);
    float theta = std::numeric_limits<float>::signaling_NaN();
    pointData() = default;
    pointData(const ldRazer::vec2f& pPoint, size_t pIdx, float pTheta) : point(pPoint), idx(pIdx), theta(pTheta) { }
};

class ldSerpentMaze {
public:
    ldSerpentMaze();
    ~ldSerpentMaze();

    int16_t wallCount() const { return static_cast<int16_t>(m_walls.size()); }
    ldRazer::vec2s generatePosition(const ldRazer::vec2s& extent=ldRazer::vec2s{1,1}) const;

    void setSnake(ldSnakeObj* snakePtr) { m_snakePtr = snakePtr; }

    bool isInitialised() const { return m_width != 0 && m_height != 0; }
    bool initialise(int16_t width, int16_t height, const ldRazer::AABB2s_arr_t& rooms, const ldRazer::linecol_arr_t& walls);

    bool intersection(const ldRazer::vec2s& pos) const;
    bool intersection(const ldRazer::AABB2s_t& bound) const;
    std::vector<ldRazer::AABB2s_t> query(const ldRazer::vec2s& pos) const;
    std::vector<ldRazer::AABB2s_t> query(const ldRazer::AABB2s_t& bound) const;

    const ldRazer::AABB2s_t& worldBound() const { return m_worldBound; }

    // This method returns the computed line batch containing the visible lines for the maze
    const ldRazer::linecolf_arr_t& renderBatch();
    const ldRazer::linecolf_arr_t& getLineBatch() const { return m_lineBatch; }

    const ldRazer::ldMesh2f_t& getMesh() const { return m_mesh; }
    void draw(ldRazer::ldMesh2f_t& mesh);

protected:
    ldRazer::line2f_t clipToDisc(const ldRazer::disc2f_t& disc, const ldRazer::line2f_t& line);
    void clipByRaycast(const ldRazer::vec2f& centre, float radius, int steps=240);
    lineidx_arr_t clipByVisibility(const ldRazer::vec2f& centre, float radius);

private:
    int16_t m_width = 0;
    int16_t m_height = 0;
    ldSnakeObj* m_snakePtr = nullptr;
    ldRazer::AABB2s_t m_worldBound;
    ldRazer::AABB2s_arr_t m_rooms;
    ldRazer::linecol_arr_t m_walls;
    ldRazer::line2f_arr_t m_wallsf;
    ldRazer::ldMesh2f_t m_mesh;

    // Caches
    ldRazer::linecolf_arr_t m_lineBatch;              // The output of the renderBatch method, cached
    std::vector<pointData> m_workingSet;                // Visibility point data
    std::vector<bool> m_visible;                        // Cull flag per line
    std::vector<size_t> m_openList;                     // The open list of currently intersected lines in clipByVis.
    lineidx_arr_t m_visibilityResult;                   // Cached output of clipByVisibility to avoid realloc
};

#endif //__LASERDOCK_LDSERPENTMAZE_H__
