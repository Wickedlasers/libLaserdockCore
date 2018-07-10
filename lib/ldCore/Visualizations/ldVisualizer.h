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
    virtual const char* getInternalName() { return __FILE__; }
    virtual QString previewFilename() const { return "";}

    virtual const char* visualizerName();
    virtual float targetFPS() { return 30; }

    ldVisualizationTask * task() const {
        return m_task;
    }

    bool init();

    void start();
    void stop();

    //used to pass in parent vtask so the visualizer has a handle of it.
    void prepare(ldVisualizationTask * task = NULL);

    virtual void updateWith(ldSoundData *pSoundData, float delta);

    QMap<QString, QVariant> const & getParameters(){
        return m_parameters;
    }

    virtual bool isValid() const { return true; }

    Q_INVOKABLE virtual bool isMusicAware() const { return m_isMusicAware; }
    Q_INVOKABLE virtual bool isRepeatAvailable() const { return false; }

public slots:
    void setParameters(QMap<QString, QVariant> const & parameters){
        m_parameters = parameters;
    }

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
    virtual void draw(void);

    QMap<QString, QVariant> m_parameters;
    ldVisualizationTask * m_task;
    ldMusicManager* m_musicManager;

    bool m_isMusicAware = true; // most of visualizers are music aware

    int m_rate = 0;

    QMutex m_mutex;
};

Q_DECLARE_METATYPE(ldVisualizer*)

#endif // LDVISUALIZER_H
