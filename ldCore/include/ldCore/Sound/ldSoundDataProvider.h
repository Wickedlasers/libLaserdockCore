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

#ifndef LDSOUNDDATAPROVIDER_H
#define LDSOUNDDATAPROVIDER_H

#include <memory>

#include <QtCore/QMutex>

#include <ldCore/ldCore_global.h>

#include <ldCore/Sound/ldSoundData.h>

class ldAudioDecoder;
class ldMusicManager;

class LDCORESHARED_EXPORT ldSoundDataProvider : public QObject
{
    Q_OBJECT

public:
    enum class SoundSource {
        SoundInput,
        AudioDecoder
    };


    explicit ldSoundDataProvider(ldMusicManager *musicManager,
                                 ldAudioDecoder *audioDecoder,
                                 QObject *parent = nullptr);
    ~ldSoundDataProvider() override;

    ldSoundData *soundData() const;

    void start();
    void stop();

    void setSoundSource(const SoundSource &source);

    void setSoundLevelGate(int value);
    int soundLevelGate() const;


signals:
    void soundDataReady(ldSoundData *soundData);

private slots:
    void onUpdateAudio(const AudioBlock &block);

    void onUpdateDecoderAudio(const AudioBlock &block);
private:
    QMutex m_mutex;

    ldMusicManager *m_musicManager;
    ldAudioDecoder *m_audioDecoder;

    std::shared_ptr<ldSoundData> m_sounddata;
    std::shared_ptr<ldSoundData> m_decoderSoundData;

    std::atomic<SoundSource> m_soundSource{SoundSource::SoundInput};

    int m_soundLevelGate = -1;

};

#endif // LDSOUNDDATAPROVIDER_H
