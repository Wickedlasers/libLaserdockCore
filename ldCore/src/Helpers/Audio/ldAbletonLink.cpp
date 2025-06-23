#include "ldCore/Helpers/Audio/ldAbletonLink.h"
#include <QtCore/QtDebug>

#if defined(__APPLE__)
#define LINK_PLATFORM_MACOSX 1
#elif defined(_WIN32)
#define LINK_PLATFORM_WINDOWS 1
//#elif defined(__linux__)
//#define LINK_PLATFORM_LINUX 1
#else
// for unsupported platforms, just create dummy ableton link class that does nothing
#define DUMMY_ABLETON_LINK
#endif

#ifndef DUMMY_ABLETON_LINK
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wshadow"
#elif __GNUC__
#endif
    #include <ableton/Link.hpp>

#ifdef __clang__
#pragma clang diagnostic pop
#elif __GNUC__
//#pragma GCC diagnostic pop
#endif

#else
namespace ableton {
    class Link {
    public:
        Link() {}
        ~Link() {}
    };
}
#endif

ldAbletonLink::ldAbletonLink(QObject *parent) : QObject(parent)
  , m_BeatTimer(new QTimer(this))
  #ifndef DUMMY_ABLETON_LINK
  , m_link(new ableton::Link(120.0))
  # else
  , m_link(nullptr)
  #endif
  , m_Quantum(4.)
{

    //qDebug() << __FUNCTION__;

    #ifndef DUMMY_ABLETON_LINK
        m_link->setNumPeersCallback([](const std::size_t numPeers){
            qDebug() << "ableton peers:" << numPeers;
        });

        m_link->setTempoCallback([this](const double bpm){
            qDebug() << "ableton tempo changed:" << bpm;
            emit bpmUpdated(static_cast<float>(bpm));
        });

        m_link->setStartStopCallback([](const bool isPlaying){
            qDebug() << "ableton playing:" << isPlaying;
        });

        m_link->enableStartStopSync(false);
        //m_link->enable(true);
        m_lastBeatTime = now();


        m_BeatTimer->setInterval(10);
        connect(m_BeatTimer.get(),&QTimer::timeout,this,[this]{            
            if (phaseTime(now(),m_Quantum)<phaseTime(m_lastBeatTime,m_Quantum)) {
                //qDebug() << "start of bar";
                m_BeatCount = 1;
                emit barDetected();
            }
            else
            if (phaseTime(now())<phaseTime(m_lastBeatTime)) {
                m_BeatCount++;
                //qDebug() << "beat" << m_BeatCount;
                emit beatDetected(m_BeatCount);
            }

            m_lastBeatTime = now();
        });

        connect(this,&ldAbletonLink::bpmUpdated,[this](float bpm){
            m_currentbpm = bpm;
            //qDebug() << "ableton tempo changed:" << bpm;
        });

        m_BeatTimer->start();
    #endif
}

ldAbletonLink::~ldAbletonLink()
{
    #ifndef DUMMY_ABLETON_LINK
        m_link->enable(false);
    #endif
}

std::chrono::microseconds ldAbletonLink::now() const {
    #ifndef DUMMY_ABLETON_LINK
    return m_link->clock().micros() + std::chrono::milliseconds(m_beatOffsetCorrectionMs);
    #else
        return std::chrono::microseconds(0);
    #endif
}

void ldAbletonLink::setActive(bool active)
{
    #ifndef DUMMY_ABLETON_LINK
        m_link->enable(active);
    #else
        Q_UNUSED(active)
    #endif
}

float ldAbletonLink::bpm()
{
    return m_currentbpm;
}



bool ldAbletonLink::isLinkAvailable()
{
    return (m_link!=nullptr) ? true : false;
}

void ldAbletonLink::startPlaying()
{
    #ifndef DUMMY_ABLETON_LINK
        auto sessionState = m_link->captureAppSessionState();
        sessionState.setIsPlayingAndRequestBeatAtTime(true, now(), 0., m_Quantum);
        m_link->commitAppSessionState(sessionState);
    #endif
}

void ldAbletonLink::stopPlaying()
{
    #ifndef DUMMY_ABLETON_LINK
        auto sessionState = m_link->captureAppSessionState();
        sessionState.setIsPlaying(false, now());
        m_link->commitAppSessionState(sessionState);
    #endif
}

bool ldAbletonLink::isPlaying() const
{
    #ifndef DUMMY_ABLETON_LINK
        return m_link->captureAppSessionState().isPlaying();
    #else
        return false;
    #endif
}

double ldAbletonLink::beatTime() const
{
    #ifndef DUMMY_ABLETON_LINK
        auto sessionState = m_link->captureAppSessionState();
        return sessionState.beatAtTime(now(), m_Quantum);
    #else
        return 0.0;
    #endif
}

double ldAbletonLink::phaseTime(std::chrono::microseconds time, double quantum) const
{
    #ifndef DUMMY_ABLETON_LINK
        auto sessionState = m_link->captureAppSessionState();
        return sessionState.phaseAtTime(time, quantum);
    #else
        Q_UNUSED(time)
        Q_UNUSED(quantum)
        return 0.0;
    #endif
}

void ldAbletonLink::setTempo(double tempo)
{
    #ifndef DUMMY_ABLETON_LINK
        auto sessionState = m_link->captureAppSessionState();
        sessionState.setTempo(tempo, now());
        m_link->commitAppSessionState(sessionState);
    #else
        Q_UNUSED(tempo)
    #endif
}

double ldAbletonLink::quantum() const
{
    return m_Quantum;
}

void ldAbletonLink::setQuantum(double quantum)
{
    if (quantum>=2. && quantum<=16.) {
        m_Quantum = quantum;
    }
}

bool ldAbletonLink::isStartStopSyncEnabled() const
{
    #ifndef DUMMY_ABLETON_LINK
        return m_link->isStartStopSyncEnabled();
    #else
        return false;
    #endif
}

void ldAbletonLink::setStartStopSyncEnabled(bool enabled)
{
    #ifndef DUMMY_ABLETON_LINK
        m_link->enableStartStopSync(enabled);
    #else
        Q_UNUSED(enabled)
    #endif
}

void ldAbletonLink::setBeatOffsetCorrection(int offsetMs)
{
    m_beatOffsetCorrectionMs = offsetMs;
}
