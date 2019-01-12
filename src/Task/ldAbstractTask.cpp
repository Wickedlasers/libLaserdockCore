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

#include "ldCore/Task/ldAbstractTask.h"

/*!
  \class ldAbstractTask
  \brief Abstract class for all task classes.
  \inmodule rendeirng

  All tasks that want be run by the Laserdock task system needs to a subclass of this abstract class.
  This class only defines an interface for task and nothing more.

  The interface defined is extremely simple.
  \l ldAbstractTask::initialize() is used to initialize the task class to an known state right after being
  loaded into the task manager. \l ldAbstractTask::start() is called right before the task about to be
  executed. \l ldAbstractTask::stop() is called right before the task is about to be stopped.
  \l ldAbstractTask::update(quint64,ldFrameBuffer*) is the function that is executed every time
  when a task needs to updated. Implement your animation code in this function.
*/

/*!
    \brief ldAbstractTask::ldAbstractTask

    constructor, the passed paramenter parent sets the parent of this class, which is used for memory manangement purposes.
*/

ldAbstractTask::ldAbstractTask(QObject *parent) :
    QObject(parent)
{

}

/*!
    \fn ldAbstractTask::initialize()
    \brief initialize the task class, to be overrided by subclass. default implementation is empty.

    The reason for an initialize function is that sometimes intialization in constructor is not possible.
    Some variables are simply not ready in the constructor.
*/

/*!
    \fn ldAbstractTask::reset()
    \brief Resets task to a known state same as right after initialize. Default implementation is empty.
*/

/*!
    \fn ldAbstractTask::start()
    \brief Gets called before a task is started. Default implementation is empty.
*/


/*!
    \fn ldAbstractTask::stop()
    \brief Gets called before a task is stopped. Default implementation is empty.
*/


/*!
    \fn ldAbstractTask::update(quint64 delta, ldFrameBuffer * buffer)
    \brief Gets called before a task needs to be updated. Default implementation is empty.

    This function is the workhorse of the task class. Animation is updated in this function.
    When the task needs to be updated, this function is called by the task manager to update the task.
    The arguments passed in where the time since last update,  quint64 delta and the buffer to which the updated
    data is written into.
*/
