//
//  ldArrowVisualizer.cpp
//  LaserdockVisualizer
//
//  Created by MEO 05/04/2017 - with code from Pong by Sergey Gavrushkin on 12/09/16.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.


#include "ldArrowVisualizer.h"


#include <QFile>
#include <QTextStream>

#include <ldCore/Helpers/Color/ldColorUtil.h>
#include <ldCore/Helpers/Maths/ldMaths.h>

// ldArrowVisualizer
ldArrowVisualizer::ldArrowVisualizer()
    : ldLuaGameVisualizer("arrow", "ldArrowVisualizer.lua")
{
    setPosition(ccp(1, 1));
}

ldArrowVisualizer::~ldArrowVisualizer()
{
}

void ldArrowVisualizer::moveY(double y)
{
    QMutexLocker lock(&m_mutex);

    if(y < -0.2) {
        //callLuaFunction("OnKeyUp", false);
        //callLuaFunction("OnKeyDown", true);
    } else if(y > 0.2) {
        //callLuaFunction("OnKeyDown", false);
        //callLuaFunction("OnKeyUp", true);
    } else {
        //callLuaFunction("OnKeyDown", false);
        //callLuaFunction("OnKeyUp", false);
    }
}

void ldArrowVisualizer::moveX(double x)
{
    QMutexLocker lock(&m_mutex);

    if(x < -0.2) {
        callLuaFunction("OnKeyLeft", true);
        callLuaFunction("OnKeyRight", false);
    } else if(x > 0.2) {
        callLuaFunction("OnKeyRight", true);
        callLuaFunction("OnKeyLeft", false);
    } else {
        callLuaFunction("OnKeyRight", false);
        callLuaFunction("OnKeyLeft", false);
    }
}

void ldArrowVisualizer::keyLeft(bool keyPress) {
    QMutexLocker lock(&m_mutex);

    callLuaFunction("OnKeyLeft", keyPress);
}

void ldArrowVisualizer::keyRight(bool keyPress) {
    QMutexLocker lock(&m_mutex);

    callLuaFunction("OnKeyRight", keyPress);
}

// draw
void ldArrowVisualizer::draw() {
    QMutexLocker lock(&m_mutex);

    callLuaFunction("OnDraw");
}

void ldArrowVisualizer::onGameReset()
{
    callLuaFunction("OnReset");
}

void ldArrowVisualizer::onGamePause()
{
    callLuaFunction("OnStop");
}

void ldArrowVisualizer::onGamePlay()
{
    callLuaFunction("OnStart");
}

