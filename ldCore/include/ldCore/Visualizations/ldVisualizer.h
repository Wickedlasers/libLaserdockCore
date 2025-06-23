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
#include <QtCore/QMutex>
#include <QtCore/QDebug>

#include <QQmlHelpers>

#include <ldCore/Sound/ldSoundData.h>
#include <ldCore/Utilities/ldUtils.h>
#include "ldCore/Shape/ldShape.h"

class ldHardwareBatch;
class ldVisualizationTask;
class ldMusicManager;

/*!
 * \brief The ldVisualizer class - base class for any custom visualization you want to create. See notes for virtual functions for more details
 */
class LDCORESHARED_EXPORT ldVisualizer : public QObject, public ldShape
{
    Q_OBJECT

    QML_READONLY_PROPERTY(bool, isVisActive)
    QML_READONLY_PROPERTY(bool, isVisPaused)
    //! flag for renderer to refresh vis even if it's in a paused state but something is changed inside
    QML_READONLY_PROPERTY(bool, isVisRefreshRequired)

public:
    ldVisualizer(QObject *parent = nullptr);
    ~ldVisualizer();

    /*!
     * \brief visualizerName - vis name for debug in log
     * \return
     */
    virtual QString visualizerName() const;
    /*!
     * \brief info - some general short info that you want to show about this vis
     * \return
     */
    virtual QString info() const;
    /*!
     * \brief targetFPS - FPS that renderer will use
     * \return
     */
    virtual int targetFPS() const { return 45; }

    /*!
     * \brief isValid - is visualizer in a valid state and can be used
     * \return
     */
    virtual bool isValid() const { return true; }

    /*!
     * \brief isMusicAware - flag just to show that visualizer is based on music and some internal optimisations can be used, not really important
     * \return
     */
    Q_INVOKABLE virtual bool isMusicAware() const { return m_isMusicAware; }
    /*!
     * \brief is3d - is it a 3d visualizer and if it can be rotate in 3 axises, not really important flag as well
     * \return
     */
    Q_INVOKABLE virtual bool is3d() const { return m_is3d; }

    /*!
     *  visualizers that can be seeked to a specific frame can override this method, and return true instead,
     * then override the onshoudlstart and pause methods that also receive offset and duration
     */
    virtual bool canSeekPosition() const {return false;}

    /*!
     * \brief clone - clone the visualizer in the same state. should be overrided if you want to use it in timeline, etc. In general it always good to override it
     * \return
     */
    virtual ldVisualizer* clone() const;

    // need hwBatch only for dac rate, probaly it can be done in a better way
    void setHwBatch(ldHardwareBatch *hwBatch);

public slots:
    void start();
    void start(double offset, quint64 durationMs);
    void pause();
    void pause(double offset, quint64 durationMs);
    void stop();

    void updateWith(ldSoundData *pSoundData, float delta);

signals:
    void changed();

protected:
    /*!
     *  Functions that should be overrrided in a child class for a specific logic. You shouldn't block execution of the thread inside these funcions with any QEventLoops to prevent ambivalent state
     */
    virtual void onShouldStart();
    virtual void onShouldPause();
    virtual void onShouldStop();

    // visualizers that can be seeked to a specific frame can override these methods
    virtual void onShouldPause(double /*offset*/, quint64 /*durationMs*/) {onShouldPause();}
    virtual void onShouldStart(double /*offset*/, quint64 /*durationMs*/) {onShouldStart();}


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
    bool m_is3d = false;

    int m_rate = 0;

    mutable QMutex m_mutex;
    bool m_isUseMutex{false};


    ldHardwareBatch *m_hwBatch = nullptr;
};

Q_DECLARE_METATYPE(ldVisualizer*)

#endif // LDVISUALIZER_H
