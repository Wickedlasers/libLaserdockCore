//
//  ldSerpentLine.h
//  LaserdockVisualizer
//
//  Created by Darren Otgaar on 31/07/2018
//  Copyright (c) 2018 Wicked Lasers. All rights reserved.
//

#ifndef LASERDOCK_LDSERPENTRAY_H__
#define LASERDOCK_LDSERPENTRAY_H__

#include "ldRazerVec2.h"

namespace ldRazer
{

    template<typename T>
    struct ldRay
    {
        using type = T;
        using vector_t = ldRazerVec2<T>;

        ldRay(const vector_t &P, const vector_t &dir) : origin(P), dir(dir)
        {
            assert(dir.isUnit());
        }

        vector_t origin;
        vector_t dir;
    };

}

#endif //__LASERDOCK_LDSERPENTLINE_H__
