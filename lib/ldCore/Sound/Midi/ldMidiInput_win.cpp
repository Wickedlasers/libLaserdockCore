#include "ldMidiInput.h"

#include <QtCore/QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QtMultimedia/QAudioProbe>
#include <QtMultimedia/QAudioRecorder>
#include <QtWidgets/QMessageBox>

#include <ldCore/Sound/ldLoopbackAudioDeviceWorker.h>
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"

#include "ldMidiInfo.h"

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

bool ldMidiInput::openMidi(int n) {
    if(hMidiDevice) {
        stop();
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

QList<ldMidiInfo> ldMidiInput::getDevices() const {
    QList<ldMidiInfo> list;
    // only show default device
    //bool onlydefault = true;
    //if (onlydefault) list.append("LDMIDI default in"); return list;
    UINT nMidiDeviceNum;
    MIDIINCAPS caps;
    nMidiDeviceNum = midiInGetNumDevs();
    if (nMidiDeviceNum == 0) {
        return list;
    }
    for (unsigned int i = 0; i < nMidiDeviceNum; ++i) {
        midiInGetDevCaps(i, &caps, sizeof(MIDIINCAPS));
        QString name;
        name = name.sprintf("%s", caps.szPname);

        // name is unique on Windows
        ldMidiInfo info(i, name);
        list << info;
    }
    return list;
}

int ldMidiInput::getMidiIndex(const ldMidiInfo &info) {

    QList<ldMidiInfo> infos = getDevices();
    for(int i = 0; i < infos.size(); i++) {
        // name is unique on Windows
        if(infos[i].name() == info.name()) {
            return i;
        }
    }

    return -1;
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

    m_isActive = false;
}
