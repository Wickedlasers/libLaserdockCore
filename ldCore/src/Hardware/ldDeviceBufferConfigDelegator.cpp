#include "ldCore/Hardware/ldDeviceBufferConfigDelegator.h"

#include <QtCore/QtDebug>

#include <ldCore/Hardware/ldNetworkHardware.h>

ldDeviceBufferConfigDelegator::ldDeviceBufferConfigDelegator(QObject *parent)
    : QObject(parent)
    , m_currentBufferConfig(&m_wifi_server_device_config)
{
}

ldDeviceBufferConfigDelegator::~ldDeviceBufferConfigDelegator()
{
}

ldDeviceBufferConfigDelegator::DeviceBufferConfig ldDeviceBufferConfigDelegator::getBufferConfig() const
{
    return *m_currentBufferConfig;
}

void ldDeviceBufferConfigDelegator::updateBufferingStrategy(std::vector<ldHardware *> devices)
{
    QList<LaserdockNetworkDevice::ConnectionType> cubeConnectionTypes;

    for(ldHardware *hw : devices) {
        ldNetworkHardware *networkHardware = qobject_cast<ldNetworkHardware*>(hw);
        // add each device's connection type to a list to check later.
        if(networkHardware)
            cubeConnectionTypes <<  networkHardware->params().device->get_connection_type();
    }

    bool hasNetworkHws = cubeConnectionTypes.size() > 0;
    bool onlyUsbHws = cubeConnectionTypes.size() == 0 && devices.size() > 0;

    if(hasNetworkHws) {
        updateNetworkBufferingStrategy(cubeConnectionTypes);

        // update all network devices with udp xfer strategy
        for(ldHardware *hw : devices) {
            ldNetworkHardware *networkHardware = qobject_cast<ldNetworkHardware*>(hw);
            if(!networkHardware)
                continue;
            networkHardware->params().device->set_max_udp_samples_per_xfer(m_currentBufferConfig->max_samples_per_udp_xfer);
            networkHardware->params().device->set_max_udp_packets_per_xfer(m_currentBufferConfig->max_udp_packets_per_xfer);
        }
    }

    if(onlyUsbHws)
        m_currentBufferConfig = &m_usb_device_config;
 }


// Choose the best buffering strategy based on all the currently connected cube interface types.
// One cube may be connected over ethernet as a LAN server, but another may be over Wifi as a client,
// so we must choose a strategy for the buffering, with the least latency without dropouts etc.
void ldDeviceBufferConfigDelegator::updateNetworkBufferingStrategy(QList<LaserdockNetworkDevice::ConnectionType> &contypes)
{
    if (contypes.size()>0){
        // TODO: for now only use the first device's connection type to set buffering strategy
        // until more testing is performed when more cubes are available
        switch(contypes[0])
        {
            case LaserdockNetworkDevice::ConnectionType::CON_ETHERNET_SERVER :
                m_currentBufferConfig = &m_lan_server_device_config;
                qDebug() << "ldDeviceBufferConfigDelegator::updateBufferingStrategy: CON_ETHERNET_SERVER";
            break;

            case LaserdockNetworkDevice::ConnectionType::CON_ETHERNET_CLIENT :
            if (contypes.size()>1){
                m_currentBufferConfig = &m_lan_dual_client_device_config;
                qDebug() << "ldDeviceBufferConfigDelegator::updateBufferingStrategy: CON_DUAL_ETHERNET_CLIENT";
            }
            else {
                m_currentBufferConfig = &m_lan_client_device_config;
                qDebug() << "ldDeviceBufferConfigDelegator::updateBufferingStrategy: CON_ETHERNET_CLIENT";
            }
            break;

            case LaserdockNetworkDevice::ConnectionType::CON_WIFI_SERVER :
            if (contypes.size()>1){
                 m_currentBufferConfig = &m_wifi_server_client_device_config;
                 qDebug() << "ldDeviceBufferConfigDelegator::updateBufferingStrategy: CON_WIFI_SERVER_MULTI_CLIENT";
            } else {
                m_currentBufferConfig = &m_wifi_server_device_config;
                qDebug() << "ldDeviceBufferConfigDelegator::updateBufferingStrategy: CON_WIFI_SERVER";
            }
            break;

        case LaserdockNetworkDevice::ConnectionType::CON_WIFI_CLIENT :
            if (contypes.size()>1){
                m_currentBufferConfig = &m_wifi_dual_client_device_config;
                qDebug() << "ldDeviceBufferConfigDelegator::updateBufferingStrategy: CON_DUAL_WIFI_CLIENT";
            } else{
                m_currentBufferConfig = &m_wifi_client_device_config;
                qDebug() << "ldDeviceBufferConfigDelegator::updateBufferingStrategy: CON_WIFI_CLIENT";
            }

        break;

            // default strategy
            default :
                m_currentBufferConfig = &m_wifi_server_device_config;
                qDebug() << "ldDeviceBufferConfigDelegator::updateBufferingStrategy: DEFAULT (CON_WIFI_SERVER)";
        }
    }
}

