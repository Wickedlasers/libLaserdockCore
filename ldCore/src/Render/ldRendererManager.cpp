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

#include "ldCore/Render/ldRendererManager.h"

#include <QtCore/QDebug>

#include "ldCore/Render/ldAbstractRenderer.h"
#include "ldCore/Render/ldRendererOpenlase.h"

/*!
  \class ldRendererManager
  \brief Manager for all renderers which are used to render graphics.
  \inmodule rendering

*/

ldRendererManager::ldRendererManager(QObject *parent) :
    QObject(parent),
    m_renderers(QList<ldAbstractRenderer *>())
{
    qDebug() << __FUNCTION__;
}

ldAbstractRenderer * ldRendererManager::getRenderer(ldRendererType type){
    //currently we only support OPENLASE as renderer, more can be added later
    if(type != OPENLASE) return NULL;
    ldAbstractRenderer * found = NULL;

    //search for the renderer and return it if found
    QListIterator<ldAbstractRenderer *> i(m_renderers);
    while(i.hasNext()){
        found = i.next();
        if(found->type() == type)
            return found;
    }

    //can't find the renderer, instantiate one
    found = new ldRendererOpenlase;
    m_renderers.append(found);

    return found;
}


