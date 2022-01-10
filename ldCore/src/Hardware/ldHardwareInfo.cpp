
#include "ldCore/Hardware/ldHardwareInfo.h"

ldHardwareInfo::ldHardwareInfo(QObject *parent)
    : ldPropertyObject(parent)
    , m_hasValidInfo(false)
    , m_fwMajor(-1)
    , m_fwMinor(-1)
    , m_dacRate(-1)
    , m_maxDacRate(-1)
    , m_batteryPercent(-1)
    , m_modelNumber(-1)
    , m_temperatureDegC(0)
    , m_connectionType(-1)
    , m_bufferSize(-1)
    , m_bufferFree(-1)
    , m_overTemperature(false)
    , m_temperatureWarning(false)
    , m_interlockEnabled(false)
    , m_packetErrors(0)
{
}

ldHardwareInfo::~ldHardwareInfo()
{

}
