
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
    , m_temperatureDegC(-255)
    , m_connectionType(-1)
    , m_bufferSize(-1)
    , m_bufferFree(-1)
    , m_overTemperature(-1)
    , m_temperatureWarning(-1)
    , m_interlockEnabled(-1)
    , m_packetErrors(-1)
{
}

ldHardwareInfo::~ldHardwareInfo()
{

}
