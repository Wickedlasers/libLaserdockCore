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

#include "ldCore/Sound/ldSoundDataProvider.h"

#include <ldCore/ldCore.h>
#include <ldCore/Sound/ldAudioDecoder.h>
#include <ldCore/Sound/ldSoundAnalyzer.h>
#include <ldCore/Sound/ldSoundDeviceManager.h>
#include <ldCore/Visualizations/MusicManager/ldMusicManager.h>

ldSoundDataProvider::ldSoundDataProvider(ldMusicManager *musicManager, ldAudioDecoder *audioDecoder, QObject *parent)
    : QObject(parent)
    , m_musicManager(musicManager)
    , m_audioDecoder(audioDecoder)
{
    qRegisterMetaType<AudioBlock>("AudioBlock");

}

ldSoundDataProvider::~ldSoundDataProvider()
{

}

ldSoundData *ldSoundDataProvider::soundData() const
{
    if(m_soundSource == SoundSource::SoundInput)
        return m_sounddata.get();
    else
        return m_decoderSoundData.get();
}

void ldSoundDataProvider::start()
{
    QMutexLocker lock(&m_mutex);


    if(m_sounddata || m_decoderSoundData)
        return;

    m_sounddata.reset(new ldSoundData());
    m_decoderSoundData.reset(new ldSoundData());
    m_decoderSoundData->setVolumeCorrectionEnabled(false);

    ldSoundAnalyzer * soundAnalyzer = ldCore::instance()->soundDeviceManager()->analyzer();
    ldSoundAnalyzer * audioDecoderAnalyzer = m_audioDecoder->analyzer();
    connect(soundAnalyzer, &ldSoundAnalyzer::audioBlockUpdated, this, &ldSoundDataProvider::onUpdateAudio);
    connect(audioDecoderAnalyzer, &ldSoundAnalyzer::audioBlockUpdated, this, &ldSoundDataProvider::onUpdateDecoderAudio);
}

void ldSoundDataProvider::stop()
{
    QMutexLocker lock(&m_mutex);

    if(!m_sounddata || !m_decoderSoundData)
        return;

    ldSoundAnalyzer * soundAnalyzer = ldCore::instance()->soundDeviceManager()->analyzer();
    ldSoundAnalyzer * audioDecoderAnalyzer = m_audioDecoder->analyzer();
    disconnect(soundAnalyzer, nullptr, this, nullptr);
    disconnect(audioDecoderAnalyzer, nullptr, this, nullptr);

    m_sounddata.reset();
    m_decoderSoundData.reset();
}

void ldSoundDataProvider::setSoundSource(const ldSoundDataProvider::SoundSource &source)
{
    //    QMutexLocker lock(&m_mutex); // deadlock from ldTimelineVisualizer::onShouldStart
    m_soundSource = source;
}

void ldSoundDataProvider::setSoundLevelGate(int value)
{
    m_soundLevelGate = value;
}

int ldSoundDataProvider::soundLevelGate() const
{
    return m_soundLevelGate;
}


void ldSoundDataProvider::onUpdateAudio(const AudioBlock &block)
{
    QMutexLocker lock(&m_mutex);

    // check if running
    if (!m_sounddata)
        return;

    if(m_soundSource == SoundSource::SoundInput) {
        // process block into raw sounddata struct
        m_sounddata->Update(block);

        m_musicManager->setRealSoundLevel(m_sounddata->GetSoundLevel());

        if(m_sounddata->GetSoundLevel() <= m_soundLevelGate) {
            m_sounddata->Update(AudioBlock::EMPTY_AUDIO_BLOCK);
        }

        // update music manager
        m_musicManager->updateWith(m_sounddata, AUDIO_UPDATE_DELTA_S);
    }

    // update visualizer
    if (!m_audioDecoder->get_isActive())
        emit soundDataReady(m_sounddata.get());
}

void ldSoundDataProvider::onUpdateDecoderAudio(const AudioBlock &block)
{
    QMutexLocker lock(&m_mutex);

    // check if running
    if (!m_decoderSoundData)
        return;

    // process block into raw sounddata struct
    m_decoderSoundData->Update(block);

    if(m_soundSource == SoundSource::AudioDecoder) {
        m_musicManager->setRealSoundLevel(m_decoderSoundData->GetSoundLevel());

        // update music manager
        m_musicManager->updateWith(m_decoderSoundData, AUDIO_UPDATE_DELTA_S);
    }

    // update visualizer
    emit soundDataReady(m_decoderSoundData.get());
}

