#include "ldCore/Sound/Midi/ldMidiInput.h"

ldMidiInput::~ldMidiInput()
{
}

void ldMidiInput::init()
{
}

bool ldMidiInput::openMidi(const ldMidiInfo & /*info*/) {

    return false;
}

void ldMidiInput::stop() {
}

/////////////////

QList<ldMidiInfo> ldMidiInput::getDevices()
{
    return m_infos;
}

void ldMidiInput::addDevice(const ldMidiInfo &info)
{
    m_infos.push_back(info);
    emit deviceAdded(info);
}

void ldMidiInput::removeDevice(const ldMidiInfo &info)
{
    m_infos.removeAll(info);
    emit deviceRemoved(info);
}

