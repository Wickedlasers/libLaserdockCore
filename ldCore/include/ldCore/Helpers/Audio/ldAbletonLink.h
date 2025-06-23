#ifndef LDABLETONLINK_H
#define LDABLETONLINK_H

#include "ldCore/ldCore_global.h"
#include <memory>
#include <QtCore/QObject>
#include <QtCore/QTimer>



namespace ableton {
    class Link;
}

class LDCORESHARED_EXPORT ldAbletonLink : public QObject
{
    Q_OBJECT
public:
    explicit ldAbletonLink(QObject *parent = nullptr);
    ~ldAbletonLink();

    bool isLinkAvailable();
    void setActive(bool active);
    float bpm();
    void startPlaying();
    void stopPlaying();
    bool isPlaying() const;
    double beatTime() const;
    double phaseTime(std::chrono::microseconds time,double quantum=1.) const;
    void setTempo(double tempo);
    double quantum() const;
    void setQuantum(double quantum);
    void setBeatOffsetCorrection(int offsetMs);
    bool isStartStopSyncEnabled() const;
    void setStartStopSyncEnabled(bool enabled);


signals:

    // signal event every beat except on the bar (returns which beat of the bar it is on)
    void beatDetected(int beat);
    // signal event every start of a bar (every quantum beats)
    void barDetected();
    // signal indicating a tempo change has occurred
    void bpmUpdated(float bpm);

private:
    float m_currentbpm{120};
    std::unique_ptr<QTimer> m_BeatTimer;
    std::unique_ptr<ableton::Link> m_link;
    double m_Quantum{4.};
    std::chrono::microseconds m_lastBeatTime{0};
    int m_BeatCount{1};
    std::chrono::microseconds now() const;
    int m_beatOffsetCorrectionMs{0};
};

#endif // LDABLETONLINK_H
