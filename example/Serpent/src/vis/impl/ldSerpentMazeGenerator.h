//
//  ldSerpentMazeGenerator.h
//  LaserdockVisualizer
//
//  Created by Darren Otgaar 31/07/2018
//  Copyright (c) 2018 Wicked Lasers. All rights reserved.
//

#ifndef LASERDOCK_LDSERPENTMAZEGENERATOR_H__
#define LASERDOCK_LDSERPENTMAZEGENERATOR_H__

#include "ldSerpentMaze.h"

/**
 * The maze generator, allowing specification of several parameters to control generation.  Debug output to image.
 */

class ldSerpentMazeGenerator {
public:
    // Difficulty scale [0.f .. 1.f]
    static std::unique_ptr<ldSerpentMaze> generate(int16_t width, int16_t height, float difficulty);

protected:

};

#endif //__LASERDOCK_LDSERPENTMAZEGENERATOR_H__
