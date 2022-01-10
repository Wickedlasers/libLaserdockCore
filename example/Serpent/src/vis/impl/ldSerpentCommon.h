//
// Created by Darren Otgaar on 2018/08/01.
//

// NOTE: This is temporary code which will be removed during development

#ifndef LASERDOCK_SIM_LDSERPENTCOMMON_HPP
#define LASERDOCK_SIM_LDSERPENTCOMMON_HPP

#include "ldRazer/ldRazer.h"

// Constant Definitions for Snake, Maze, and World
const size_t MAX_MAZE_LINES = 200;          // I have no idea really
const size_t MAX_PICKUP_LINES = 20;         // Keep it small... (including radar)
const size_t MAX_SNAKE_LEN = 50;
const size_t MAX_SNAKE_LINES = 4*MAX_SNAKE_LEN;
const uint16_t SCREEN_DIM = 210;            // Measured by pixels in on-screen display
const uint16_t BLOCK_SIZE = 4;              // Size of one unit block in game in pixels
const uint16_t VIEWPORT_SIZE = SCREEN_DIM/BLOCK_SIZE;
const size_t SCREEN_SCALE = 4;              // Magnification scale for displaying on a monitor
const size_t SCREEN_SIZE = SCREEN_DIM * SCREEN_SCALE;
const size_t WORLD_SCALE = SCREEN_SCALE * BLOCK_SIZE;
const size_t MAP_SIZE = 2048;
const size_t SCRATCH_LINES = 800;           // Menus, animation, etc.

enum SoundEvent {
    SE_PICKUP_TARGET,
    SE_PICKUP_SHRINK,
    SE_PICKUP_SLOW,
    SE_PICKUP_EXTRALIFE,
    SE_SNAKE_DIE,
    SE_LEVEL_COMPLETED,
    SE_GAME_START
};

using playSoundFnc = std::function<void(const SoundEvent&)>;

#endif //LASERDOCK_SIM_LDSERPENTCOMMON_HPP
