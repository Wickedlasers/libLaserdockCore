#ifndef LDMIDIINPUT_H
#define LDMIDIINPUT_H

#include <QtCore/QObject>
#include <QtCore/QTimer>

#ifdef Q_OS_MAC
#include <CoreMIDI/CoreMIDI.h>
#endif
#ifdef Q_OS_WIN
#include <Windows.h>
#include <mmeapi.h>
#undef max
#undef min
#endif

#include "ldMidiInfo.h"
#include "ldMidiTypes.h"

class LDCORESHARED_EXPORT ldMidiInput : public QObject
{
    Q_OBJECT
public:
    explicit ldMidiInput(QObject *parent);
    ~ldMidiInput();
        
    QList<ldMidiInfo> getDevices();

    bool isActive() const;

    void init();

#ifdef Q_OS_MAC
    static ldMidiInfo getInfo(MIDIEndpointRef source);
    static SInt32 getSourceId(MIDIEndpointRef source);
    static QString getSourceName(MIDIEndpointRef source);
#endif

public slots:
    void start(const ldMidiInfo &info);
    void stop();

signals:
    void noteReceived(const ldMidiNote &note);
    void messageReceived(const ldMidiCCMessage &message);

    void deviceAdded(const ldMidiInfo &info);
    void deviceRemoved(const ldMidiInfo &info);

#ifdef Q_OS_WIN
private slots:
    void checkDevices();
#endif

private:
    void addDevice(const ldMidiInfo &info);
    void removeDevice(const ldMidiInfo &info);

    bool openMidi(const ldMidiInfo &info);


#ifdef Q_OS_MAC
    MIDIClientRef m_midiClient = 0;
    MIDIPortRef m_inputPort = 0;
    MIDIEndpointRef m_sourceEndPoint = 0;

    friend void midiNotifyCallback(const MIDINotification *notification, void *context);
#elif defined(Q_OS_WIN)

    QList<ldMidiInfo> getCurrentDevices();
    QTimer m_timer;
    HMIDIIN hMidiDevice = NULL;
    ldMidiInfo m_openedInfo;
#endif

    QList<ldMidiInfo> m_infos;
    bool m_isActive = false;

};

#endif //LDMIDIINPUT_H

