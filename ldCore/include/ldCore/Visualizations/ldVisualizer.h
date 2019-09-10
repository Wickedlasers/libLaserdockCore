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


/*

Subclass ldVisualizer to create a drawing system for the laser.

Access drawing utility through member m_renderer (ldRendererOpenlase*)
Access sound response utility through member m_musicManager (ldMusicManager*)

Override the following functions for a basic render loop:

    virtual void draw() override;

This function is called every time a new frame needs to be drawn. The implementation is responsible for
drawing the frame using the m_renderer object (ldRendererOpenlase). Render parameters and fame modes
should be set here as well if any non-default values are required.

    virtual void updateWith(ldSoundData *pSoundData, float delta);

This function is called when new audio data is available and processed. Audio data is available by accessing
pSoundData (ldSoundData) for raw data and m_musicManager (ldMusicManager) for advanced algorithms.
m_musicManager values are updated once per call to updateWith(), but can also be accessed directly in draw().

Timing notes:

For draw(), the real-time delay between subsequent calls (and display of corresponding frames) is dependant
on frame sizes and and external settings. The function m_renderer->getLastFrameDeltaSeconds() can be called
in draw() to get the timing for each frame.

updateWith() is called on a fixed timer, every AUDIO_UPDATE_DELTA_S seconds.

Other useful functions:

override getTargetFPS() to enforce a hard maximum limit on the draw rate. Default value is 30 frames per second.
Range is 10-100, recommended 30-60. Can be used along with render params (which provides a soft minimum limit)
to stabilize frame rates when drawing complex scenes.

override getVisualizerName() for name used by the app.

*/


#ifndef LDVISUALIZER_H
#define LDVISUALIZER_H

#include <QtCore/QObject>
#include <QtCore/QDebug>

#include <ldCore/Sound/ldSoundData.h>
#include <ldCore/Utilities/ldUtils.h>
#include "ldCore/Shape/ldShape.h"

class ldVisualizationTask;
class ldMusicManager;

class LDCORESHARED_EXPORT ldVisualizer : public QObject, public ldShape
{
    Q_OBJECT

public:
    ldVisualizer(QObject *parent = nullptr);
    ~ldVisualizer();

public:
    virtual QString visualizerName() const;
    virtual float targetFPS() const { return 30; }

    bool init();

    void start();
    void stop();

    virtual void updateWith(ldSoundData *pSoundData, float delta);

    virtual bool isValid() const { return true; }

    Q_INVOKABLE virtual bool isMusicAware() const { return m_isMusicAware; }
    Q_INVOKABLE virtual bool isRepeatAvailable() const { return false; }

protected:
    virtual void onShouldStart();
    virtual void onShouldStop();

    /*!
     * \brief clearBuffer
     */
    virtual void clearBuffer(){}

    /*!
     * \brief onUpdate
     * \a delta is a delay time.
     */
    virtual void onUpdate(ldSoundData * /*pSoundData*/, float /*delta*/){}

    /*!
     * \brief draw
     */
    virtual void draw() override;

    ldMusicManager* m_musicManager;

    bool m_isMusicAware = true; // most of visualizers are music aware

    int m_rate = 0;

    mutable QMutex m_mutex;
};

Q_DECLARE_METATYPE(ldVisualizer*)

#endif // LDVISUALIZER_H
