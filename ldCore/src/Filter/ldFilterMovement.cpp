//
//  ldFilterMovement.cpp
//
//  Created by Eric Brugère on 01/14/22.
//  Copyright (c) 2022 Wicked Lasers. All rights reserved.
//
#include <ldCore/Filter/ldFilterMovement.h>

#include <ldCore/Helpers/Maths/ldMaths.h>

#include <ldCore/Helpers/Maths/ldFilterMath.h>

using namespace ldFilterMath;

#define DEBUG_VERBOSE 0

// ldFilterMovement
ldFilterMovement::ldFilterMovement()
    : ldFilter()
    , LD_INIT_MIN_MAX_PROPERTY(horizontal, 0, -1, 1)
    , LD_INIT_MIN_MAX_PROPERTY(vertical, 0, -1, 1)
    , LD_INIT_MIN_MAX_PROPERTY(rotateX, 0, -1, 1)
    , LD_INIT_MIN_MAX_PROPERTY(rotateY, 0, -1, 1)
    , LD_INIT_MIN_MAX_PROPERTY(rotateZ, 0, -1, 1)
    , m_mirror(false)
    , m_isMovementFilterActive(false)
    , m_decay_x(0)
    , m_decay_y(0)
    , m_decay_rx(0)
    , m_decay_ry(0)
    , m_decay_rz(0)
{
    connect(this,&ldFilterMovement::horizontalChanged,[&](){update_isMovementFilterActive(checkIsMovementFilterActive());});
    connect(this,&ldFilterMovement::verticalChanged,[&](){update_isMovementFilterActive(checkIsMovementFilterActive());});
    connect(this,&ldFilterMovement::rotateXChanged,[&](){update_isMovementFilterActive(checkIsMovementFilterActive());});
    connect(this,&ldFilterMovement::rotateYChanged,[&](){update_isMovementFilterActive(checkIsMovementFilterActive());});
    connect(this,&ldFilterMovement::rotateZChanged,[&](){update_isMovementFilterActive(checkIsMovementFilterActive());});
    connect(this,&ldFilterMovement::mirrorChanged,[&](){update_isMovementFilterActive(checkIsMovementFilterActive());});
}

bool ldFilterMovement::checkIsMovementFilterActive()
{
    if (!m_enabled) return false;

    if(get_horizontal() == 0
        && get_vertical() == 0
        && get_rotateX() == 0
        && get_rotateY() == 0
        && get_rotateZ() == 0
        ) return false;
    else return true;
}

// process
void ldFilterMovement::process(ldVertex &v)
{
    if(!checkIsMovementFilterActive())
        return;

    float delta = 1.0f/30000.0f;
    m_decay_x += delta*m_horizontal*10.0f;
    m_decay_x = ldMaths::periodIntervalKeeper(m_decay_x, -10.0f, 10.0f);
    m_decay_y += delta*m_vertical*10.0f;
    m_decay_y = ldMaths::periodIntervalKeeper(m_decay_y, -10.0f, 10.0f);
    m_decay_rx += delta*m_rotateX*1.0f;
    m_decay_rx = ldMaths::periodIntervalKeeper(m_decay_rx, -1.0f, 1.0f);
    m_decay_ry += delta*m_rotateY*1.0f;
    m_decay_ry = ldMaths::periodIntervalKeeper(m_decay_ry, -1.0f, 1.0f);
    m_decay_rz += delta*m_rotateZ*1.0f;
    m_decay_rz = ldMaths::periodIntervalKeeper(m_decay_rz, -1.0f, 1.0f);

    // particular cases to recenter at speed 0 & also helps the user to have a zero value on the slider...
    float eps = 0.01f;
    bool xOff = false;
    bool yOff = false;
    bool rotateIt = false;
    if (fabs(m_horizontal)<eps) {
        m_decay_x=0;
        xOff = true;
    }
    if (fabs(m_vertical)<eps) {
        m_decay_y=0;
        yOff = true;
    }
    if (fabs(m_rotateX)<eps) {
        m_decay_rx=0;
    } else rotateIt = true;
    if (fabs(m_rotateY)<eps) {
        m_decay_ry=0;
    } else rotateIt = true;
    if (fabs(m_rotateZ)<eps) {
        m_decay_rz=0;
    } else rotateIt = true;
    //
    float x2 = ldMaths::periodIntervalKeeper(v.x() + m_decay_x, -1.0f, 1.0f);
    float y2 = ldMaths::periodIntervalKeeper(v.y() + m_decay_y, -1.0f, 1.0f);

    //
    float ceil = 0.65f;

    // correction for translated hidden points
    if (fabs(x2)>ceil && !xOff) {
        float sign = (x2>0)?1.0f:-1.0f;
        x2 = sign * ceil * (1.0f - (fabs(x2)-ceil)/(1.0f-ceil));
        if (!m_mirror) { v.r() = v.g() = v.b() = 0; }
    }
    if (fabs(y2)>ceil && !yOff) {
        float sign = (y2>0)?1.0f:-1.0f;
        y2 = sign * ceil * (1.0f - (fabs(y2)-ceil)/(1.0f-ceil));
        if (!m_mirror) { v.r() = v.g() = v.b() = 0; }
    }
    // qDebug()<<" x2"<<x2;

    if (rotateIt) {
        // rotation angle
        float z = 0;
        rott(x2, y2, m_decay_rz);
        rott(y2, z, m_decay_rx);
        rott(x2, z, m_decay_ry);

        // perspective applly
        Perspective per;
        per.fadeuselinear = false;
        per.fadeuseradius = false;
        per.zoffset = 3.0f;
        per.xymult = 3.0f;
        per.fadebasedist = 3.0f;

        //
        per.apply(v, x2, y2, z);
    } else {
        v.x() = x2;
        v.y() = y2;
    }

}
