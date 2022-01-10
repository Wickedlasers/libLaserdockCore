//
// Created by otgaard on 2018/08/01.
//

#include "ldRazer/ldRazer.h"
#include "ldSerpentCommon.h"
#include "ldSerpentMazeGenerator.h"

using namespace ldRazer;

void buildSimpleWalls(line2s_arr_t& walls, int16_t width, int16_t height) {
    walls.emplace_back(line2s_t(vec2s(0, 0), vec2s(width, 0)));
    walls.emplace_back(line2s_t(vec2s(width, 0), vec2s(width, height)));
    walls.emplace_back(line2s_t(vec2s(width, height), vec2s(0, height)));
    walls.emplace_back(line2s_t(vec2s(0, height), vec2s(0, 0)));
}

void buildAABB(linecol_arr_t &walls, const AABB2s_t &room) {
    ldRandom<int16_t> rnd(128, 255);
    auto colour = randomColour();
    walls.emplace_back(std::make_pair(line2s_t(room.leftBottom(), room.rightBottom()), colour));
    walls.emplace_back(std::make_pair(line2s_t(room.rightBottom(), room.rightTop()), colour));
    walls.emplace_back(std::make_pair(line2s_t(room.rightTop(), room.leftTop()), colour));
    walls.emplace_back(std::make_pair(line2s_t(room.leftTop(), room.leftBottom()), colour));
}

// Dups to test mergeSegments
void buildAABBDups(linecol_arr_t &walls, const AABB2s_t &room) {
    ldRandom<int16_t> rnd(128, 255);
    auto colour = randomColour();
    walls.emplace_back(std::make_pair(line2s_t(room.leftBottom(), room.leftBottom() + vec2s(room.width()/2, 0)), colour));
    walls.emplace_back(std::make_pair(line2s_t(room.leftBottom() + vec2s(room.width()/2, 0), room.rightBottom()), colour));
    walls.emplace_back(std::make_pair(line2s_t(room.rightBottom(), room.rightTop()), colour));
    walls.emplace_back(std::make_pair(line2s_t(room.rightTop(), room.leftTop()), colour));
    walls.emplace_back(std::make_pair(line2s_t(room.leftTop(), room.leftBottom() + vec2s(0, room.height()/2)), colour));
    walls.emplace_back(std::make_pair(line2s_t(room.leftBottom() + vec2s(0, room.height()/2), room.leftBottom()), colour));
}

int roomDifficultyScale(float difficulty) {
    auto scale = lerp<int>(2, 30, difficulty);
    return scale;
}

AABB2s_arr_t buildSimpleRooms(linecol_arr_t& walls, int16_t width, int16_t height, float difficulty) {
    ldRandom<int16_t> rnd(0);
    buildAABB(walls, AABB2s_t(vec2s(0, 0), vec2s(width, height)));

    std::vector<AABB2s_t> rooms;
    const auto hw = width/3, hh = height/3;

    for(int i = 0; i != roomDifficultyScale(difficulty); ) {
        auto A = rnd.random2(width, height);
        auto B = rnd.random2(width, height);
        if(A.x == 0 || A.x == width || A.y == 0 || A.y == height ||
           B.x == 0 || B.x == width || B.y == 0 || B.y == height) continue;     // Stop bleeding into world bound
        if(A.x > B.x) std::swap(A.x, B.x);
        if(A.y > B.y) std::swap(A.y, B.y);
        AABB2s_t room(A, B);
        if(room.width() == 0 || room.height() == 0 || room.height() > hh || room.width() > hw) continue;

        bool occluded = false;
        for(const auto& r : rooms) {
            occluded = intersection(room, r);
            if(occluded) break;
        }

        if(occluded) continue;
        else {
            rooms.emplace_back(room);
            buildAABB(walls, room);
            i++;
        }
    }

    return rooms;
}

std::unique_ptr<ldSerpentMaze> ldSerpentMazeGenerator::generate(int16_t width, int16_t height, float difficulty) {
    auto maze_ptr = std::unique_ptr<ldSerpentMaze>(new ldSerpentMaze());

    linecol_arr_t walls;
    auto rooms = buildSimpleRooms(walls, width, height, difficulty);
    maze_ptr->initialise(width, height, rooms, walls);
    return maze_ptr;
}

