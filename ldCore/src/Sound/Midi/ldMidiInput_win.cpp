#include "ldCore/Sound/Midi/ldMidiInput.h"

#include <QtCore/QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QtMultimedia/QAudioProbe>
#include <QtMultimedia/QAudioRecorder>
#include <QtWidgets/QMessageBox>

#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"

#include "ldCore/Sound/Midi/ldMidiInfo.h"

#include <SDKDDKVer.h>
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <mmsystem.h>
//#pragma comment(lib, "winmm.lib")

#include <qtimer.h>
// midi input funcs and data

static void CALLBACK midi_callback(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
static void printmsg(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);


/////////////////////////
// internal funcs

bool ldMidiInput::openMidi(const ldMidiInfo &info) {
    if(hMidiDevice) {
        stop();
    }

    int n = -1;
    QList<ldMidiInfo> infos = getDevices();
    for(int i = 0; i < infos.size(); i++) {
        // name is unique on Windows
        if(infos[i].name() == info.name()) {
           n = i;
           break;
        }
    }
    if(n == -1) {
       qWarning() << "Can't find midi port" << info.id() << info.name();
       return false;
    }

    DWORD nMidiPort = n;
    UINT nMidiDeviceNum;
    MMRESULT rv;
        
    nMidiDeviceNum = midiInGetNumDevs();
    if (nMidiDeviceNum == 0) {
        fprintf(stderr, "midiInGetNumDevs() return 0...");
        return false;
    }

    rv = midiInOpen(&hMidiDevice, nMidiPort, (DWORD)(void*)midi_callback, (DWORD_PTR) this, CALLBACK_FUNCTION);
    if (rv != MMSYSERR_NOERROR) {
        fprintf(stderr, "midiInOpen() failed...rv=%d", rv);
        return false;
    }

    rv = midiInStart(hMidiDevice);

    if (rv != MMSYSERR_NOERROR) {
        fprintf(stderr, "midiInStart() failed...rv=%d", rv);
        return false;
    }

    m_openedInfo = info;
    return true;
}


static void CALLBACK midi_callback(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
        
//    printmsg(hMidiIn, wMsg, dwInstance, dwParam1, dwParam2);

    // ignore all but data
    if (wMsg != MIM_DATA) return;

    // first three bytes
    unsigned char b1 = 0;
    unsigned char b2 = 0;
    unsigned char b3 = 0;
    b1 = (dwParam1 >> 0) & 0xff;
    b2 = (dwParam1 >> 8) & 0xff;
    b3 = (dwParam1 >> 16) & 0xff;

    // split byte one
    int b1a = b1 >> 4;


    bool isValidNote = false;
    bool isValidCCMessage = false;
    // extract values for note
    ldMidiNote e;
    ldMidiCCMessage message;
    // extract values for cc message

    // check for onset or offset
    if (b1a == 9) {
        isValidNote = true;
        e.onset = true;
        e.note = b2;
        e.velocity = b3;
    }
    else if (b1a == 8) {
        isValidNote = true;
        e.note = b2;
    } else if(b1a = 11) {
        message.faderNumber = b2;
        message.value = b3;
        isValidCCMessage = true;
    }

    ldMidiInput *input = (ldMidiInput*) dwInstance;
    Q_ASSERT(input);
    // save as event
    if(isValidNote) {
        // send
        emit input->noteReceived(e);
    }

    if(isValidCCMessage) {
        emit input->messageReceived(message);
    }

}

static void printmsg(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {

    //qDebug() << wMsg << " " << dwParam1 << " " << dwParam2;
    switch (wMsg) {
    case MIM_OPEN:
        printf("wMsg=MIM_OPEN\n");
        break;
    case MIM_CLOSE:
        printf("wMsg=MIM_CLOSE\n");
        break;
    case MIM_DATA:
        printf("wMsg=MIM_DATA, dwInstance=%08x, dwParam1=%08x, dwParam2=%08x\n", dwInstance, dwParam1, dwParam2);
        {QString s; s.sprintf("wMsg=MIM_DATA, dwInstance=%08x, dwParam1=%08x, dwParam2=%08x\n", dwInstance, dwParam1, dwParam2); qDebug() << s; }
        break;
    case MIM_LONGDATA:
        printf("wMsg=MIM_LONGDATA\n");
        break;
    case MIM_ERROR:
        printf("wMsg=MIM_ERROR\n");
        break;
    case MIM_LONGERROR:
        printf("wMsg=MIM_LONGERROR\n");
        break;
    case MIM_MOREDATA:
        printf("wMsg=MIM_MOREDATA\n");
        break;
    default:
        printf("wMsg = unknown\n");
        break;
    }
}


/////////////////

QList<ldMidiInfo> ldMidiInput::getDevices() {
    return m_infos;
}

ldMidiInput::~ldMidiInput()
{

}

void ldMidiInput::init()
{
    m_infos = getCurrentDevices();

    m_timer.setInterval(2000);
    connect(&m_timer, &QTimer::timeout, this, &ldMidiInput::checkDevices);
    m_timer.start();
}

void ldMidiInput::stop() {
    if (hMidiDevice) {
        MMRESULT res = midiInStop(hMidiDevice);
        if(res != MMSYSERR_NOERROR ) {
            qWarning() << "midiInStop error" << res;
        }
        res = midiInClose(hMidiDevice);
        if(res != MMSYSERR_NOERROR ) {
            qWarning() << "midiInClose error" << res;
        }
        hMidiDevice = NULL;
    }

    m_openedInfo = ldMidiInfo();

    m_isActive = false;
}

void ldMidiInput::checkDevices()
{
    QList<ldMidiInfo> currentDevices = getCurrentDevices();

    // check for removed
    auto it = m_infos.begin();
    while(it != m_infos.end()) {
        auto currentDevicesIt = std::find_if(currentDevices.begin(), currentDevices.end(), [&](const ldMidiInfo &currentDevice) {
            return currentDevice.name() == it->name();
        });
        if(currentDevicesIt == currentDevices.end()) {
            ldMidiInfo info = *it;
            if(info == m_openedInfo)
                stop();

            it = m_infos.erase(it);
            emit deviceRemoved(info);
        } else {
            it++;
        }
    }

    // check for new devices
    for(const ldMidiInfo &currentDevice : currentDevices) {
        it = std::find_if(m_infos.begin(), m_infos.end(), [&](const ldMidiInfo &info) {
            return info.name() == currentDevice.name();
        });
        if(it == m_infos.end()) {
            m_infos.push_back(currentDevice);

            emit deviceAdded(currentDevice);
        }
    }
}

QList<ldMidiInfo> ldMidiInput::getCurrentDevices()
{
    QList<ldMidiInfo> res;
    MIDIINCAPS caps;
    UINT nMidiDeviceNum = midiInGetNumDevs();
    for (unsigned int i = 0; i < nMidiDeviceNum; ++i) {
        midiInGetDevCaps(i, &caps, sizeof(MIDIINCAPS));
        QString name;
        name = name.sprintf("%s", caps.szPname);

        // name is unique on Windows
        ldMidiInfo info(i, name);
        res << info;
    }

    return res;
}

