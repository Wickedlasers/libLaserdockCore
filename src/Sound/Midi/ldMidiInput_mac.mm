#include "ldCore/Sound/Midi/ldMidiInput.h"

#include <stdio.h>

#include <QtConcurrent/QtConcurrent>
#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QTimer>
#include <QtMultimedia/QAudioProbe>
#include <QtMultimedia/QAudioRecorder>

#import <Foundation/Foundation.h>

#include "ldCore/Sound/Midi/ldMidiInfo.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-statement-expression"

// midi input funcs and data

//typealias MIDINotifyProc = (UnsafePointer<MIDINotification>, UnsafeMutableRawPointer?) -> Void

void midiNotifyCallback(const MIDINotification *notification, void *context)
{
    ldMidiInput *input = (ldMidiInput*) context;

    switch (notification->messageID) {
    case kMIDIMsgObjectAdded: {
        MIDIObjectAddRemoveNotification *addRemoveNotification = (MIDIObjectAddRemoveNotification*) notification;
        if(addRemoveNotification->childType == kMIDIObjectType_Source) {
            MIDIEndpointRef sourceRef = (MIDIEndpointRef) addRemoveNotification->child;
//            qDebug() << "add" << addRemoveNotification->child << ldMidiInput::getSourceId(sourceRef) << ldMidiInput::getSourceName(sourceRef);
            ldMidiInfo info = ldMidiInput::getInfo(sourceRef);
            if(!info.isValid())
                return;

            if(!input->getDevices().contains(info)) {
                input->addDevice(info);
            }
        }
    }
        break;
    case kMIDIMsgObjectRemoved: {
        MIDIObjectAddRemoveNotification *addRemoveNotification = (MIDIObjectAddRemoveNotification*) notification;
        if(addRemoveNotification->childType == kMIDIObjectType_Source) {
            MIDIEndpointRef sourceRef = (MIDIEndpointRef) addRemoveNotification->child;
//            qDebug() << "remove" << addRemoveNotification->child << ldMidiInput::getSourceId(sourceRef) << ldMidiInput::getSourceName(sourceRef) ;

            ldMidiInfo info = ldMidiInput::getInfo(sourceRef);
            if(!info.isValid())
                return;

            if(input->getDevices().contains(info)) {
                input->removeDevice(info);
            }
        }
    }
        break;
    default:
//        qDebug() << notification->messageID;
        break;
    }

}

// internal funcs
//        typealias MIDIReadProc = (UnsafePointer<MIDIPacketList>, UnsafeMutableRawPointer?, UnsafeMutableRawPointer?) -> Void
void midiInputCallback (const MIDIPacketList *list,
                        void */*procRef*/,
                        void *srcRef)
{
    bool continueSysEx = false;
    UInt16 nBytes;
    const MIDIPacket *packet = &list->packet[0];
    const uint SYSEX_LENGTH = 1024;
    unsigned char sysExMessage[SYSEX_LENGTH];
    unsigned int sysExLength = 0;

    for (unsigned int i = 0; i < list->numPackets; i++) {
        nBytes = packet->length;

        if (continueSysEx) {
            unsigned int lengthToCopy = MIN (nBytes, SYSEX_LENGTH - sysExLength);
            // Copy the message into our SysEx message buffer,
            // making sure not to overrun the buffer
            memcpy(sysExMessage + sysExLength, packet->data, lengthToCopy);
            sysExLength += lengthToCopy;

            // Check if the last byte is SysEx End.
            continueSysEx = (packet->data[nBytes - 1] == 0xF7);

            if (!continueSysEx || sysExLength == SYSEX_LENGTH) {
                // We would process the SysEx message here, as it is we're just ignoring it

                sysExLength = 0;
            }
        } else {

            UInt16 iByte, size;

            iByte = 0;
            while (iByte < nBytes) {
                size = 0;

                // First byte should be status
                unsigned char status = packet->data[iByte];
                if (status < 0xC0) {
                    size = 3;
                } else if (status < 0xE0) {
                    size = 2;
                } else if (status < 0xF0) {
                    size = 3;
                } else if (status == 0xF0) {
                    // MIDI SysEx then we copy the rest of the message into the SysEx message buffer
                    unsigned int lengthLeftInMessage = nBytes - iByte;
                    unsigned int lengthToCopy = MIN (lengthLeftInMessage, SYSEX_LENGTH);

                    memcpy(sysExMessage + sysExLength, packet->data, lengthToCopy);
                    sysExLength += lengthToCopy;

                    size = 0;
                    iByte = nBytes;

                    // Check whether the message at the end is the end of the SysEx
                    continueSysEx = (packet->data[nBytes - 1] != 0xF7);
                } else if (status < 0xF3) {
                    size = 3;
                } else if (status == 0xF3) {
                    size = 2;
                } else {
                    size = 1;
                }

//                unsigned char messageType = status & 0xF0;
//                unsigned char messageChannel = status & 0xF;


                ldMidiNote e;
                bool isValidNote = false;

                ldMidiCCMessage message;
                bool isValidMessage = false;

                switch (status & 0xF0) {
                    case 0x80:
                        //NSLog(@"Note off: %d, %d", packet->data[iByte + 1], packet->data[iByte + 2]);
                        {
                            // save as event
                            e.note = packet->data[iByte + 1];
                            e.onset = false;
                            e.velocity = packet->data[iByte + 2];
                            isValidNote = true;
                        }
                        break;

                    case 0x90:
                        //NSLog(@"Note on: %d, %d", packet->data[iByte + 1], packet->data[iByte + 2]);
                        {
                            // save as event
                            e.note = packet->data[iByte + 1];
                            e.onset = true;
                            e.velocity = packet->data[iByte + 2];
                            isValidNote = true;
                        }
                        break;

                    case 0xA0:
                        //NSLog(@"Aftertouch: %d, %d", packet->data[iByte + 1], packet->data[iByte + 2]);
                        break;

                    case 0xB0:
                        // fader on AKAI APC mini
                        message.faderNumber = packet->data[iByte + 1];
                        message.value = packet->data[iByte + 2];
                        isValidMessage = true;
                        //NSLog(@"Control message: %d, %d", packet->data[iByte + 1], packet->data[iByte + 2]);
                        break;

                    case 0xC0:
                        //NSLog(@"Program change: %d", packet->data[iByte + 1]);
                        break;

                    case 0xD0:
                        //NSLog(@"Change aftertouch: %d", packet->data[iByte + 1]);
                        break;

                    case 0xE0:
                        //NSLog(@"Pitch wheel: %d, %d", packet->data[iByte + 1], packet->data[iByte + 2]);
                        break;

                    default:
                        //NSLog(@"Some other message");
                        break;
                }


                ldMidiInput *input = static_cast<ldMidiInput*>(srcRef);
                Q_ASSERT(input);
               if(isValidNote) {
                // send
                   emit input->noteReceived(e);
               }
               if(isValidMessage) {
                   emit input->messageReceived(message);
               }


                iByte += size;
            }
        }

        packet = MIDIPacketNext(packet);
    }
}

ldMidiInput::~ldMidiInput()
{

    if (m_midiClient != 0) {
        MIDIClientDispose(m_midiClient);
        m_midiClient = 0;
    }
}

void ldMidiInput::init()
{
    // get all midi sources
    ItemCount sourceCount = MIDIGetNumberOfSources();
//    qDebug() << "midi devices ct is " << sourceCount;
    for (ItemCount i = 0 ; i < sourceCount ; i++) {
        MIDIEndpointRef source = MIDIGetSource(i);
        if (source != 0) {

            ldMidiInfo info = getInfo(source);
            if(!info.isValid())
                continue;

            m_infos.append(info);
        }
    }

    OSStatus result = MIDIClientCreate(CFSTR("MIDI client"), midiNotifyCallback, this, &m_midiClient);
    if (result != noErr) {
//NSLog(@"Error creating MIDI client: %s - %s",
//GetMacOSStatusErrorString(result),
//        GetMacOSStatusCommentString(result));
        qDebug() << "midi client create fail" << result;
    }
}

ldMidiInfo ldMidiInput::getInfo(MIDIEndpointRef source)
{
    SInt32 id = getSourceId(source);
    QString name = getSourceName(source);

    if (id == -1 || name.isEmpty()) {
        return ldMidiInfo();
    }

    return ldMidiInfo(id, name);
}


bool ldMidiInput::openMidi(const ldMidiInfo &info) {

    if(!m_midiClient)
        return false;

    int n = -1;
    QList<ldMidiInfo> infos = ldMidiInput::getDevices();
    for(int i = 0; i < infos.size(); i++) {
        // id is unique on mac
        if(infos[i].id() == info.id()) {
            n = i;
            break;
        }
    }
    if(n == -1) {
       qWarning() << "Can't find midi port" << info.id() << info.name();
       return false;
    }


    OSStatus result = MIDIInputPortCreate(m_midiClient, CFSTR("Input"), midiInputCallback, NULL, &m_inputPort);
    if (result != noErr) {
//NSLog(@"Error creating MIDI client: %s - %s",
//GetMacOSStatusErrorString(result),
//        GetMacOSStatusCommentString(result));
        qDebug() << "midi port create fail";
        return false;

    }


    // get all midi sources
    ItemCount sourceCount = MIDIGetNumberOfSources();
    qDebug() << "midi devices ct is " << sourceCount;

   // specific one
    m_sourceEndPoint = MIDIGetSource(n);
    if (m_sourceEndPoint == 0) {
        qDebug() << "midi endpoint fail";
        return false;
    }


    result = MIDIPortConnectSource(m_inputPort, m_sourceEndPoint, this);
    if (noErr != result) {
        qDebug() << "midi connect fail";
        return false;
    }


    return true;
}

void ldMidiInput::stop() {
    if(!m_isActive) {
        return;
    }

    m_isActive = false;

    if (m_sourceEndPoint != 0 && m_inputPort != 0) {
        qDebug() << "midi input close" << getSourceName(m_sourceEndPoint);

        MIDIPortDisconnectSource(m_inputPort, m_sourceEndPoint);
        m_sourceEndPoint = 0;
    }

    if (m_inputPort != 0) {
        MIDIPortDispose(m_inputPort);
        m_inputPort = 0;
    }
}

/////////////////

QList<ldMidiInfo> ldMidiInput::getDevices()
{
    return m_infos;
}

void ldMidiInput::addDevice(const ldMidiInfo &info)
{
//    qDebug() << __FUNCTION__ << info.name();
    m_infos.push_back(info);
    emit deviceAdded(info);
}

void ldMidiInput::removeDevice(const ldMidiInfo &info)
{
//    qDebug() << __FUNCTION__ << info.name();
    if (m_sourceEndPoint) {
        if(getSourceId(m_sourceEndPoint) == info.id())
            stop();
    }

    m_infos.removeAll(info);
    emit deviceRemoved(info);
}

SInt32 ldMidiInput::getSourceId(MIDIEndpointRef source)
{
    SInt32 id = -1;
    OSStatus status = MIDIObjectGetIntegerProperty(source, kMIDIPropertyUniqueID, &id);
    if (status != noErr) {
        id = -1;
        qWarning() << "MIDIObjectGetIntegerProperty kMIDIPropertyUniqueID" << status;
//                continue;
    }
    return id;
}

QString ldMidiInput::getSourceName(MIDIEndpointRef source)
{
    CFStringRef str = nil;
    OSStatus status = MIDIObjectGetStringProperty(source, kMIDIPropertyName, &str);
    if (status != noErr) {
        qWarning() << "MIDIObjectGetStringProperty kMIDIPropertyDisplayName" << status;
        return QString();
    }

    QString res =  QString::fromCFString(str);
    CFRelease(str);

    return res;
}



