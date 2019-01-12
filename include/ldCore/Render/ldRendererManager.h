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

#ifndef LDRENDERERMANAGER_H
#define LDRENDERERMANAGER_H
#include "ldCore/ldCore_global.h"

#include <QtCore/QObject>
#include <QtCore/QList>

class ldAbstractRenderer;

/** Renderer type enum */
enum ldRendererType {
    UNINITIALIZED,
    OPENLASE
};

/** Renderer manager. Currently only Openlase is supported */
class LDCORESHARED_EXPORT ldRendererManager : public QObject
{
    Q_OBJECT
public:
    explicit ldRendererManager(QObject *parent = 0);

    ldAbstractRenderer* getRenderer(ldRendererType type);

private:
    QList<ldAbstractRenderer*> m_renderers;
};

#endif // LDRENDERERMANAGER_H
