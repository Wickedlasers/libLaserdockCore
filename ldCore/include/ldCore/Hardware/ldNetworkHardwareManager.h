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

typedef QByteArray (*ldGenerateSecurityRequestCallbackFunc)();
typedef bool (*ldAuthenticateSecurityResponseCallbackFunc)(const QByteArray &resData);

class LDCORESHARED_EXPORT ldNetworkHardwareManager : public ldAbstractHardwareManager
{
    Q_OBJECT

public:
    explicit ldNetworkHardwareManager(ldFilterManager *filterManager, QObject *parent = nullptr);
    ~ldNetworkHardwareManager() override;

    virtual QString hwType() const override;
    virtual QString managerName() const override { return "Network Hardware Manager"; }

    virtual void setConnectedDevicesActive(bool active) override;

    virtual uint deviceCount() const override;
    virtual std::vector<ldHardware*> devices() const override;

    virtual void deviceCheck() override;


    static void setGenerateSecurityRequestCb(ldGenerateSecurityRequestCallbackFunc authenticateFunc);
    static void setAuthenticateSecurityCb(ldAuthenticateSecurityResponseCallbackFunc checkFunc);

    virtual void debugAddDevice() override;
    virtual void debugRemoveDevice() override;

public slots:
    virtual void removeDevice(const QString &hwId)  override;

private:
    signals:
        void ConnectedDevicesActiveChanged(bool active);

private slots:
    void init();
    void ConnectedDevicesActiveUpdate(bool active);
    void networkDeviceDisconnectCheck();
    void readPendingDeviceResponses(); // handles incoming responses to our pings for devices on the network
    void DeviceAuthenticateRequest(LaserdockNetworkDevice &device);
    void DeviceAuthenticateResponse(LaserdockNetworkDevice &device,bool&success, QByteArray& response_data);
private:
    void AddNetworkDevice(QString& ip_addr);
    void removeDeviceFromActiveList(std::unique_ptr<ldNetworkHardware>&networkHardware);
    void moveDeviceToActiveList(std::unique_ptr<ldNetworkHardware>&networkHardware);

    mutable QRecursiveMutex m_mutex;

    ldFilterManager *m_filterManager;
    QThread m_managerworkerthread;
    QUdpSocket *m_pingskt{nullptr};

    static ldGenerateSecurityRequestCallbackFunc m_genSecReqCb;
    static ldAuthenticateSecurityResponseCallbackFunc m_authSecRespCb;

    std::vector<std::unique_ptr<ldNetworkHardware> > m_initializingnetworkHardwares; // somewhere to store new hardware while it is being initialized
    std::vector<std::unique_ptr<ldNetworkHardware> > m_networkHardwares; // once a network hardware has succesully initialized, it is stored here
};

#endif // LDNETWORKHARDWAREMANAGER_H
