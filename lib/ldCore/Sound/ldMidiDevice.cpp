#include "ldMidiDevice.h"

#include <QtCore/QtDebug>

#include "Midi/ldMidiInfo.h"
#include "Midi/ldMidiInput.h"

void ldMidiDevice::registerMetaType()
{
    ldMidiInfo::registerMetaType();
}

ldMidiDevice::ldMidiDevice(QObject *parent)
    : ldSoundStubDevice(parent)
    , m_input(new ldMidiInput(this))
{
}


ldMidiDevice::~ldMidiDevice()
{
}

void ldMidiDevice::start(const ldMidiInfo &info) {

    m_input->start(info);

    startStub();
}

void ldMidiDevice::stop() {

    m_input->stop();

    stopStub();
}
