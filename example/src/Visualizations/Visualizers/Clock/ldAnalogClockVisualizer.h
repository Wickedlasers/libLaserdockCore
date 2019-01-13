/**
    libLaserdockCore
    Copyright(c) 2018 Wicked Lasers

    This file is part of libLaserdockCore.

    libLaserdockCore is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libLaserdockCore is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libLaserdockCore.  If not, see <https://www.gnu.org/licenses/>.
**/

#ifndef __ldCore__ldAnalogClockVisualizer__
#define __ldCore__ldAnalogClockVisualizer__

#include "ldCore/Visualizations/ldVisualizer.h"
#include "ldCore/Helpers/Maths/ldMaths.h"

class ldClockComplexObject;

class ldAnalogClockVisualizer : public ldVisualizer
{
    Q_OBJECT
public:
    explicit ldAnalogClockVisualizer(QObject *parent = nullptr);
    virtual ~ldAnalogClockVisualizer();

    bool init();

    // ldVisualizer
    
    virtual QString visualizerName() const override { return "Analog"; }
    virtual float targetFPS() const override { return 42; }
    virtual void onShouldStart() override;

protected:
    virtual void draw() override;

private:
    QScopedPointer<ldClockComplexObject> m_clockObj;
};

#endif /*__ldCore__ldAnalogClockVisualizer__*/
