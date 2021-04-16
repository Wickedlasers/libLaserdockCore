#ifndef LDNETWORKHARDWAREMANAGER_H
#define LDNETWORKHARDWAREMANAGER_H

#include <memory>

#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QUdpSocket>

#include <laserdocklib/LaserDockNetworkDevice.h>

#include "ldAbstractHardwareManager.h"
#include <QThread>

class ldFilterManager;
class ldNetworkHardware;

typedef bool (*ldNetworkAuthenticateCallbackFunc)(ldNetworkHardware *device);

class LDCORESHARED_EXPORT ldNetworkHardwareManager : public ldAbstractHardwareManager
{
    Q_OBJECT

public:
    explicit ldNetworkHardwareManager(ldFilterManager *filterManager, QObject *parent = nullptr);
    ~ldNetworkHardwareManager() override;

    int getBufferFullCount() override;

    bool hasActiveDevices() const override;

    bool isDeviceActive(int index) const;
    void setDeviceActive(int index, bool active);

    void sendData(uint startIndex, uint count) override;

    virtual void setConnectedDevicesActive(bool active) override;

    virtual uint deviceCount() const override;
    virtual std::vector<ldHardware*> devices() const override;

    virtual DeviceBufferConfig getBufferConfig()  override;

public slots:


private:
    signals:
        void ConnectedDevicesActiveChanged(bool active);

private slots:
    void init();
    void ConnectedDevicesActiveUpdate(bool active);
    void networkDeviceDisconnectCheck();
    void networkDeviceCheck();
    void readPendingDeviceResponses(); // handles incoming responses to our pings for devices on the network
    void DeviceAuthenticateRequest(LaserdockNetworkDevice &device);
    void DeviceAuthenticateResponse(LaserdockNetworkDevice &device,bool&success, QByteArray& response_data);
private:
    void updateCheckTimerState();
    void updateBufferingStrategy(QList<LaserdockNetworkDevice::ConnectionType> &contypes);
    void updateHardwareFilters();
    void AddNetworkDevice(QString& ip_addr);

    mutable QMutex m_mutex;

    ldFilterManager *m_filterManager;
    QThread m_managerworkerthread;
    QTimer *m_checkTimer;
    QUdpSocket *m_pingskt;

    std::vector<std::unique_ptr<ldNetworkHardware> > m_initializingnetworkHardwares; // somewhere to store new hardware while it is being initialized
    std::vector<std::unique_ptr<ldNetworkHardware> > m_networkHardwares; // once a network hardware has succesully initialized, it is stored here

#if defined(Q_OS_ANDROID)
      const DeviceBufferConfig m_wifi_server_device_config{300,2400,12,4,2,140,10}; // buffering strategy for a wifi server network cube
      const DeviceBufferConfig m_wifi_server_client_device_config{600,2500,12,5,10,140,10}; // buffering strategy for a wifi server/client(s) network cube
      const DeviceBufferConfig m_wifi_client_device_config{300,3000,12,2,2,100,20}; // buffering strategy for a wifi client network cube
      const DeviceBufferConfig m_wifi_dual_client_device_config{300,4000,12,2,2,100,10}; // buffering strategy for a wifi client network cube
      const DeviceBufferConfig m_lan_server_device_config{300,1800,12,4,2,140,10}; // buffering strategy for ethernet server network cube
      const DeviceBufferConfig m_lan_client_device_config{300,2400,12,4,2,140,10}; // buffering strategy for ethernet client network cube
      const DeviceBufferConfig m_lan_dual_client_device_config{300,2400,12,4,2,140,10}; // buffering strategy for ethernet client network cube
#elif defined(Q_OS_IOS)
    const DeviceBufferConfig m_wifi_server_device_config{300,3000,12,8,8,140,20}; // buffering strategy for a wifi server network cube
    const DeviceBufferConfig m_wifi_server_client_device_config{700,2800,12,10,10,140,10}; // buffering strategy for a wifi server network cube
    const DeviceBufferConfig m_wifi_client_device_config{300,2400,12,4,2,140,10}; // buffering strategy for a wifi client network cube
    const DeviceBufferConfig m_wifi_dual_client_device_config{300,2400,12,4,2,140,10}; // buffering strategy for a wifi client network cube
    const DeviceBufferConfig m_lan_server_device_config{300,1800,12,4,2,140,10}; // buffering strategy for ethernet server network cube
    const DeviceBufferConfig m_lan_client_device_config{300,2400,12,4,2,140,10}; // buffering strategy for ethernet client network cube
    const DeviceBufferConfig m_lan_dual_client_device_config{300,2400,12,4,2,140,10}; // buffering strategy for ethernet client network cube
#else

    const DeviceBufferConfig m_wifi_server_device_config{600,2000,12,4,10,140,20}; // buffering strategy for a wifi server network cube
    const DeviceBufferConfig m_wifi_server_client_device_config{600,2500,12,5,10,140,10}; // buffering strategy for a wifi server/client(s) network cube
    const DeviceBufferConfig m_wifi_client_device_config{700,1800,12,6,4,80,20}; // buffering strategy for a wifi client network cube
    const DeviceBufferConfig m_wifi_dual_client_device_config{700,1500,12,6,8,140,10}; // buffering strategy for 2x wifi client network cube
    const DeviceBufferConfig m_lan_server_device_config{700,1800,12,6,4,80,20}; // buffering strategy for ethernet server network cube
    const DeviceBufferConfig m_lan_client_device_config{700,1800,12,6,4,80,20}; // buffering strategy for ethernet client network cube
    const DeviceBufferConfig m_lan_dual_client_device_config{700,1800,12,6,4,140,10}; // buffering strategy for ethernet client network cube
#endif

    const DeviceBufferConfig* m_currentBufferConfig{nullptr}; // points to the currently selected device buffering configuration
};

#endif // LDNETWORKHARDWAREMANAGER_H
