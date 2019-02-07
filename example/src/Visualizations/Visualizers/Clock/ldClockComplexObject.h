//
//  ldClockComplexObject.h
//  LaserdockVisualizer
//
//  Created by Eric Brugère on 12/05/16.
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//

#ifndef __LaserdockVisualizer__ldClockComplexObject__
#define __LaserdockVisualizer__ldClockComplexObject__

#include <QtCore/QObject>
#include <QtCore/QTime>

#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Helpers/Maths/ldMaths.h"

class ldClockComplexObject : public QObject
{
    Q_OBJECT
public:
    ldClockComplexObject();
    ~ldClockComplexObject();

    // call innerDraw with your current m_renderer
    void innerDraw(ldRendererOpenlase* p_renderer, const QTime &time = QTime::currentTime());
    void showAll(bool b_all_visible);

private:
    bool _all_visible;
    svgBezierCurves _svgHour, _svgMin;
    void drawDataBezierAsLinestrip(ldRendererOpenlase* p_renderer, svgBezierCurves &dataVect, float rotation, int color);
};

#endif /*__LaserdockVisualizer__ldClockComplexObject__*/

