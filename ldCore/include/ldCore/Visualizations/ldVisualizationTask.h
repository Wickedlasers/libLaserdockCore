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
#include <ldCore/Sound/ldSoundData.h>

class ldAudioDecoder;
class ldLogoLaserdock;
class ldMusicManager;
class ldRendererOpenlase;
class ldVisualizer;

class LDCORESHARED_EXPORT ldVisualizationTask : public ldAbstractTask
{
    Q_OBJECT

public:
    enum class SoundSource {
        SoundInput,
        AudioDecoder
    };

    typedef struct {
        bool renderOpenlase = true;
        ldFrameBuffer * buffer = nullptr;
        quint64 delta = 0;
    } RenderState;

    explicit ldVisualizationTask(ldMusicManager *musicManager,
                                 ldAudioDecoder *audioDecoder,
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

    void setSoundLevelGate(int value);
    int soundLevelGate() const;

    void setIsShowLogo(bool showLogo);

    void setSoundSource(const ldVisualizationTask::SoundSource &source);

signals:
    void baseVisualizerChanged(ldVisualizer *visualizer);

private slots:
    void onUpdateAudio(const AudioBlock& block);
    void onUpdateDecoderAudio(const AudioBlock& block);

private:
    ldVisualizer *getActiveVis() const;
    void startVisualizer(ldVisualizer *v);

    mutable QMutex m_mutex;

    ldMusicManager *m_musicManager;
    ldAudioDecoder *m_audioDecoder;

    QMap<int, ldVisualizer*> m_visualizers;
    std::shared_ptr<ldSoundData> m_sounddata;
    std::shared_ptr<ldSoundData> m_decoderSoundData;
    RenderState m_renderstate;

    std::unique_ptr<ldLogoLaserdock> m_logo;

    ldRendererOpenlase * m_openlase;

    int m_soundLevelGate = 0;
    bool m_isShowLogo = true;

    SoundSource m_soundSource = SoundSource::SoundInput;

    float m_rotX = 0;
    float m_rotY = 0;
    float m_rotZ = 0;
};

#endif // LDVISUALIZATIONTASK_H


