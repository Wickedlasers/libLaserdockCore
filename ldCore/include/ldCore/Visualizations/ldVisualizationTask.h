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

#ifndef LDVISUALIZATIONTASK_H
#define LDVISUALIZATIONTASK_H

#include <memory>

#include <QtCore/QMutex>

#include <ldCore/Task/ldAbstractTask.h>

class ldLogoLaserdock;
class ldHardwareBatch;
class ldRendererOpenlase;
class ldSoundData;
class ldSoundDataProvider;
class ldVisualizer;

class LDCORESHARED_EXPORT ldVisualizationTask : public ldAbstractTask
{
    Q_OBJECT

public:
    typedef struct RenderStateStruct {
        bool renderOpenlase = true;
        ldFrameBuffer * buffer = nullptr;
        quint64 delta = 0;
    } RenderState;

    explicit ldVisualizationTask(ldSoundDataProvider *soundDataProvider,
                                 ldHardwareBatch *hwBatch,
                                 QObject *parent = nullptr);
    ~ldVisualizationTask() override;

    // ldAbstractTask
    void start() override;
    void stop() override;

    void update(quint64 delta, ldFrameBuffer * buf) override;

    RenderState *renderState();

    ldVisualizer *getBaseVisualizer() const;

    void setRotX(float x);
    void setRotY(float y);
    void setRotZ(float z);

public slots:
    void setVisualizer(ldVisualizer *visualizer = nullptr, int priority = 0);

    void setIsShowLogo(bool showLogo);

    ldRendererOpenlase* get_openlase();

signals:
    void baseVisualizerChanged(ldVisualizer *visualizer);
    void currentFpsChanged(int fps);

private slots:
    void onSoundDataReady(ldSoundData *soundData);

private:
    ldVisualizer *getActiveVis() const;
    void startVisualizer(ldVisualizer *v);

    mutable QRecursiveMutex m_mutex;

    ldSoundDataProvider *m_soundDataProvider;
    ldHardwareBatch *m_hwBatch;

    QMap<int, ldVisualizer*> m_visualizers;
    RenderState m_renderstate;
    int m_current_fps{-1};
    float m_fps_avg{0};
    int m_fps_cnt{0};

    std::unique_ptr<ldLogoLaserdock> m_logo;

    ldRendererOpenlase * m_openlase;

    bool m_isShowLogo = false;


    float m_rotX = 0;
    float m_rotY = 0;
    float m_rotZ = 0;
};

#endif // LDVISUALIZATIONTASK_H


