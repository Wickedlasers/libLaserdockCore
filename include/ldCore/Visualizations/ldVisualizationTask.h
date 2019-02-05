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

#include <ldCore/Task/ldAbstractTask.h>
#include <ldCore/Sound/ldSoundData.h>

class ldLogoLaserdock;

class ldRendererOpenlase;
class ldVisualizer;

class LDCORESHARED_EXPORT ldVisualizationTask : public ldAbstractTask
{
    Q_OBJECT

    Q_PROPERTY(ldVisualizer* currentVisualizer READ getCurrentVisualizer WRITE setCurrentVisualizer NOTIFY currentVisualizerChanged)

public:
    typedef struct {
        bool renderOpenlase = true;
        ldFrameBuffer * buffer;
        quint64 delta;
    } RenderState;

    explicit ldVisualizationTask(QObject *parent = 0);
    ~ldVisualizationTask();

    // ldAbstractTask
    void start() override;
    void stop() override;
    void update(quint64 delta, ldFrameBuffer * buf) override;

    RenderState *renderState();
    ldVisualizer *getCurrentVisualizer() const;

public slots:
    void setCurrentVisualizer(ldVisualizer *visualizer = nullptr);
    void setTempVisualizer(ldVisualizer *visualizer = nullptr);

    void setSoundLevelGate(int value);
    int soundLevelGate() const;

signals:
    void currentVisualizerChanged(ldVisualizer *visualizer);

private slots:
    void onUpdateAudio(const AudioBlock& block);
    void onUpdateDecoderAudio(const AudioBlock& block);

private:
    ldVisualizer *getActiveVis() const;
    void startVisualizer(ldVisualizer *v);

    mutable QMutex m_mutex;

    ldVisualizer* m_currentVisualizer;
    ldVisualizer* m_tempVisualizer;
    std::shared_ptr<ldSoundData> m_sounddata;
    std::shared_ptr<ldSoundData> m_decoderSoundData;
    RenderState m_renderstate;

    std::unique_ptr<ldLogoLaserdock> m_logo;

    ldRendererOpenlase * m_openlase;

    int m_soundLevelGate = 0;
};

#endif // LDVISUALIZATIONTASK_H


