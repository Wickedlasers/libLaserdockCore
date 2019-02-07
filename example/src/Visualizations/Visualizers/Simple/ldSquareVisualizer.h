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

//
//  ldSquareVisualizer.h
//  ldCore
//
//  Created by Sergey Gavrushkin on 25/10/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldSquareVisualizer__
#define __ldCore__ldSquareVisualizer__

#include "ldCore/Visualizations/ldVisualizer.h"

class ldSquareVisualizer : public ldVisualizer
{
    Q_OBJECT
public:
    explicit ldSquareVisualizer();
    virtual ~ldSquareVisualizer();

    bool init();

    // ldVisualizer
    virtual QString visualizerName() const override { return "Square"; }

protected:
    // ldVisualizer
    virtual void draw() override;
};

#endif /*__ldCore__ldSquareVisualizer__*/
