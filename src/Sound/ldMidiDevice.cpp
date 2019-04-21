#include "ldMidiDevice.h"

#include <QtCore/QtDebug>

#include "ldCore/Sound/Midi/ldMidiInfo.h"
#include "ldCore/Sound/Midi/ldMidiInput.h"

void ldMidiDevice::registerMetaType()
{
    ldMidiInfo::registerMetaType();
}

ldMidiDevice::ldMidiDevice(QObject *parent)
    : ldSoundStubDevice(parent)
    , m_input(new ldMidiInput(this))
{
    qDebug() << __FUNCTION__;
}


ldMidiDevice::~ldMidiDevice()
{
}

QList<ldMidiInfo> ldMidiDevice::getDevices() const
{
    return m_input->getDevices();
}

void ldMidiDevice::start(const ldMidiInfo &info)
{
    ldSoundStubDevice::start();

    QMutexLocker lock(&m_mutex);
    m_input->start(info);
}

void ldMidiDevice::stop() {
    ldSoundStubDevice::stop();

    QMutexLocker lock(&m_mutex);
    m_input->stop();
}

ldMidiInput *ldMidiDevice::input() const
{
    return m_input;
}
