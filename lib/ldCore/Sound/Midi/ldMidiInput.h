#ifndef LDMIDIINPUT_H
#define LDMIDIINPUT_H

#include <QtCore/QObject>

#ifdef Q_OS_MAC
#include <CoreMIDI/CoreMIDI.h>
#endif
#ifdef Q_OS_WIN
#include <Windows.h>
#include <mmeapi.h>
#undef max
#undef min
#endif
class ldMidiInfo;

#include "ldMidiTypes.h"

class LDCORESHARED_EXPORT ldMidiInput : public QObject
{
    Q_OBJECT
public:
    explicit ldMidiInput(QObject *parent);
    ~ldMidiInput();
        
    static QList<ldMidiInfo> getDevices();

    bool isActive() const;

public slots:
    void start(const ldMidiInfo &info);
    void stop();

signals:
    void noteReceived(const ldMidiNote &note);
    void messageReceived(const ldMidiCCMessage &message);

private:
    bool openMidi(const ldMidiInfo &info);

#ifdef Q_OS_MAC
    MIDIClientRef m_midiClient = 0;
    MIDIPortRef m_inputPort = 0;
    MIDIEndpointRef m_sourceEndPoint = 0;
#elif defined(Q_OS_WIN)
    HMIDIIN hMidiDevice = NULL;
#endif

    bool m_isActive = false;

};

#endif //LDMIDIINPUT_H

