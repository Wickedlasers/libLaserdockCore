#include "ldCore/Hardware/ldNetworkHardwareManager.h"

#include <QtCore/QtDebug>
#include <QNetworkDatagram>
#include <QtCore/QCoreApplication>
#include <laserdocklib/LaserdockSample.h>
#include "laserdocklib/LaserDockNetworkDevice.h"

#include <ldCore/Filter/ldFilterManager.h>
#include <ldCore/Filter/ldHardwareFilter.h>
#include <ldCore/Hardware/ldNetworkHardware.h>

ldGenerateSecurityRequestCallbackFunc ldNetworkHardwareManager::m_genSecReqCb = nullptr;
ldAuthenticateSecurityResponseCallbackFunc ldNetworkHardwareManager::m_authSecRespCb = nullptr;

ldNetworkHardwareManager::ldNetworkHardwareManager(ldFilterManager *filterManager, QObject *parent)
    : ldAbstractHardwareManager(parent)
    , m_filterManager(filterManager)
{
    moveToThread(&m_managerworkerthread);
    m_managerworkerthread.start();
    QTimer::singleShot(0,this, &ldNetworkHardwareManager::init);
}

ldNetworkHardwareManager::~ldNetworkHardwareManager()
{
    m_managerworkerthread.quit();
    if(!m_managerworkerthread.wait(5000)) {
        qWarning() << __FUNCTION__ <<  "Worker thread wasn't finished";
    }
}

QString ldNetworkHardwareManager::hwType() const
{
    return "Network";
}

void ldNetworkHardwareManager::setGenerateSecurityRequestCb(ldGenerateSecurityRequestCallbackFunc authenticateFunc)
{
    m_genSecReqCb = authenticateFunc;
}

void ldNetworkHardwareManager::setAuthenticateSecurityCb(ldAuthenticateSecurityResponseCallbackFunc checkFunc)
{
    m_authSecRespCb = checkFunc;
}

void ldNetworkHardwareManager::debugAddDevice()
{
    QMutexLocker locker(&m_mutex);
    QString ipAddress = QString("127.0.0.%1").arg(m_networkHardwares.size() + 1);
    std::unique_ptr<LaserdockNetworkDevice> newDevice(new LaserdockNetworkDevice(ipAddress));
    std::unique_ptr<ldNetworkHardware> networkHardware(new ldNetworkHardware(newDevice.release()));
    networkHardware->params().device->setParent(networkHardware.get());
    m_networkHardwares.push_back(std::move(networkHardware)); // move device from init list to the main list
    emit deviceCountChanged(static_cast<uint>(m_networkHardwares.size()));
}

void ldNetworkHardwareManager::debugRemoveDevice()
{
    QMutexLocker locker(&m_mutex);
    if(m_networkHardwares.empty())
        return;
    m_networkHardwares.erase(m_networkHardwares.begin() + m_networkHardwares.size() - 1);
    emit deviceCountChanged(static_cast<uint>(m_networkHardwares.size()));
}

// A network cube device is requesting that we authenticate it
// We should send out a security request from this event
void ldNetworkHardwareManager::DeviceAuthenticateRequest(LaserdockNetworkDevice&device)
{
    QMutexLocker locker(&m_mutex);
    qDebug() << "device @" << device.get_ip_address() << "needs authenticating...";

    QByteArray reqByteArray;
    if(m_genSecReqCb) {
        reqByteArray = m_genSecReqCb();
    } else {
        uint8_t security_req_pkt[]={
            0x01,
            0xe0, 0x2e, 0x00, 0x00, 0x40, 0x9c, 0x00,
            0x00, 0x23, 0x27, 0x08, 0x00, 0x00, 0x00, 0xa1,
            0x21, 0x00, 0x00, 0xea, 0x35, 0x00, 0x00, 0x75,
            0x4f, 0x00, 0x00, 0x90, 0x1f, 0x00, 0x00, 0x40,
            0x39, 0x00, 0x00, 0x9c, 0x6d, 0x00, 0x00, 0xf2,
            0x2d, 0x00, 0x00, 0xa2, 0x6f, 0x00, 0x00, 0x73,
            0xc4
        };

        reqByteArray = QByteArray(reinterpret_cast<const char*>(security_req_pkt),sizeof(security_req_pkt));
    }

   device.SecurityRequest(reqByteArray); // send a security request packet (after which we expect a SecurityResponseReceived event)
}

// we handle the security response back from a network cube device here
// we must inform the device whether the authentication was successful or not
// by updating the device using <DeviceAuthenticated> func after checking the response data.
void ldNetworkHardwareManager::DeviceAuthenticateResponse(LaserdockNetworkDevice &device,
                                                          bool& /*success*/, QByteArray& response_data)
{
    QMutexLocker locker(&m_mutex);
    if(!m_authSecRespCb) {
        // for now we will just tell the device that it was authenticated OK.
        device.DeviceAuthenticated(true);
        return;
    }

    if(m_authSecRespCb(response_data)) {
      qDebug() << "Device Authenication Successful.";
      device.DeviceAuthenticated(true);
    } else {
      qDebug() << "Device Authenication Failed.";
      device.DeviceAuthenticated(false);
    }
}

// This handles adding of a new network cube to our manager
// we also connect to any important events from the new device
void ldNetworkHardwareManager::AddNetworkDevice(QString& ip_addr)
{
    QMutexLocker locker(&m_mutex);
    std::unique_ptr<LaserdockNetworkDevice> newDevice(new LaserdockNetworkDevice(ip_addr,this));

    auto *device = newDevice.get();

    std::unique_ptr<ldNetworkHardware> networkHardware(new ldNetworkHardware(newDevice.release(),this));

    connect(networkHardware.get(),&ldHardware::enabledChanged,this,&ldAbstractHardwareManager::deviceEnabledStateChanged);


      // add to list of hardwares that are being initialized
    m_initializingnetworkHardwares.push_back(std::move(networkHardware));

    // connect to all the events we need to handle from this new device

    // we need to handle authentication when the device requests it
    connect(device,&LaserdockNetworkDevice::DeviceNeedsAuthenticating,this,[this,device](){
        DeviceAuthenticateRequest(*device);
    });

    connect(device,&LaserdockNetworkDevice::DeviceReady,this,[device](){
        qDebug() << "device @" << device->get_ip_address() << "Ready.";
    });

    connect(device,&LaserdockNetworkDevice::DeviceDisconnected,this,[this,device](){
        qDebug() << "device @" << device->get_ip_address() << "disconnected.";
        networkDeviceDisconnectCheck();
    });

    // we need to check any authentication response from the cube, and update cube with authentication result
    connect(device,&LaserdockNetworkDevice::SecurityResponseReceived,this,[this,device](bool success,QByteArray response_data){
        DeviceAuthenticateResponse(*device,success,response_data);
    });


}

// When the manager sends out a ping using a global broadcast message, any cubes on the
// network will respond with an alive response.  These responses are checked here, and
// any new devices are added, initialized, and authenticated.
void ldNetworkHardwareManager::readPendingDeviceResponses()
{
     QMutexLocker locker(&m_mutex);
   // qDebug() << "someone responded to our ping!!!!!!!!";
    while (m_pingskt->hasPendingDatagrams()) {
            QNetworkDatagram datagram = m_pingskt->receiveDatagram();
            QString ip_addr = datagram.senderAddress().toString();
            const QByteArray& data = datagram.data();
            if (LaserdockNetworkDevice::DeviceAliveResponseValid(data)==true){ // valid lasercube device info response?
                // check if we already have this device in our list
               // qDebug() << "network lasercube with ip:" << ip_addr << "responded to our ping";

                bool new_device = true;

                // check currently initializing hardware list
                for (size_t cnt = 0;cnt<m_initializingnetworkHardwares.size();cnt++){
                    QString ip = m_initializingnetworkHardwares.at(cnt)->params().device->get_ip_address();
                    if (ip==ip_addr){ // this device is already in our list so ignore
                        new_device = false;
                        break;
                    }
                }

                // check finished initializing list
                for (size_t cnt = 0;cnt<m_networkHardwares.size();cnt++){
                    QString ip = m_networkHardwares.at(cnt)->params().device->get_ip_address();
                    if (ip==ip_addr){ // this device is already in our list so ignore
                        new_device = false;
                        break;
                    }
                }

                if (new_device){
                    qDebug() << "new network lasercube found with ip:" << ip_addr;
                    AddNetworkDevice(ip_addr);
                }


            }
            else {
                   // qDebug() << "unknown device with ip:" << ip_addr << "responded to our ping";
                }

        }
}

void ldNetworkHardwareManager::removeDeviceFromActiveList(std::unique_ptr<ldNetworkHardware>&networkHardware)
{
    QMutexLocker locker(&m_mutex);
    networkHardware->ResetStatus(); // reset all hardware init status for this device
    networkHardware->filter()->setActive(false);
    disconnect(networkHardware->params().device,&LaserdockNetworkDevice::TemperatureUpdated,networkHardware->filter(),&ldHardwareFilter::deviceTemperatureUpdated);
    m_initializingnetworkHardwares.push_back(std::move(networkHardware)); // move device back to init list
}

void ldNetworkHardwareManager::moveDeviceToActiveList(std::unique_ptr<ldNetworkHardware>&networkHardware)
{
    QMutexLocker locker(&m_mutex);

    QString newSN = QString::fromStdString(networkHardware->params().device->get_serial_number());
    auto networkHardwareIt = m_networkHardwares.begin();
    while(networkHardwareIt != m_networkHardwares.end()) {
        std::unique_ptr<ldNetworkHardware> &existingNetworkHardware = (*networkHardwareIt);
        QString sn = QString::fromStdString(existingNetworkHardware->params().device->get_serial_number());

        if (sn==newSN) {
            qDebug() << "network harware device SN:" << newSN << "already exists, not added to active list";
            return;
        }
        networkHardwareIt++;
    }

    networkHardware->setFilter(m_filterManager->getFilterById(networkHardware->id()));
    networkHardware->filter()->setActive(true);

    // ensure hardware filter for this device gets all temperature updates,  and send the current temperature to hardware filter immediately.
    connect(networkHardware->params().device,&LaserdockNetworkDevice::TemperatureUpdated,networkHardware->filter(),&ldHardwareFilter::deviceTemperatureUpdated);    
    int8_t current_temperature;
    if (networkHardware->params().device->temperature_degc(&current_temperature)) {
        emit networkHardware->filter()->deviceTemperatureUpdated(current_temperature);
    }

    m_networkHardwares.push_back(std::move(networkHardware)); // move device from init list to the main list

    emit deviceCountChanged(static_cast<uint>(m_networkHardwares.size()));
}



void ldNetworkHardwareManager::setConnectedDevicesActive(bool active)
{
    emit ConnectedDevicesActiveChanged(active);
}

void ldNetworkHardwareManager::ConnectedDevicesActiveUpdate(bool active)
{
    QMutexLocker locker(&m_mutex);
    for(std::unique_ptr<ldNetworkHardware> &networkHardware : m_networkHardwares) {
        networkHardware->setActive(active);
    }
}


uint ldNetworkHardwareManager::deviceCount() const
{
    return static_cast<uint>(m_networkHardwares.size());
}

std::vector<ldHardware*> ldNetworkHardwareManager::devices() const
{
    QMutexLocker locker(&m_mutex);
    std::vector<ldHardware*> devices;
    for(const std::unique_ptr<ldNetworkHardware> &hardware : m_networkHardwares)
        devices.push_back(hardware.get());
    return devices;
}


void ldNetworkHardwareManager::networkDeviceDisconnectCheck()
{
    QMutexLocker locker(&m_mutex);
    uint oldDeviceCount = static_cast<uint>(m_networkHardwares.size());
    // check for disconnected devices (each device does it's own device disconnect checking in the background)
    auto networkHardwareIt = m_networkHardwares.begin();
    while(networkHardwareIt != m_networkHardwares.end()) {
        std::unique_ptr<ldNetworkHardware> &networkHardware = (*networkHardwareIt);

        if (networkHardware->params().device->get_disconnected()){ // this device has disconnected?
            removeDeviceFromActiveList(networkHardware);
            networkHardwareIt = m_networkHardwares.erase(networkHardwareIt); // remove from the main list
        } else networkHardwareIt++;
    }


    // check if the main list has changed in size due to disconnects, or devices having finished initializing
    if(oldDeviceCount != m_networkHardwares.size()) {
        emit deviceCountChanged(static_cast<uint>(m_networkHardwares.size()));
    }
}

void ldNetworkHardwareManager::deviceCheck()
{
    QMutexLocker locker(&m_mutex);

     if(!m_pingskt) {
         QTimer::singleShot(0, this, &ldNetworkHardwareManager::deviceCheck);
         return;
     }
    // qDebug() << "ldNetworkHardwareManager::networkDeviceCheck()";
    //qDebug() << QThread::currentThread();

    //QMutexLocker locker(&m_mutex);

    // qDebug() << __FUNCTION__;
    uint oldDeviceCount = static_cast<uint>(m_networkHardwares.size());

    // periodically we send out a ping for any lasercube devices on the network
    LaserdockNetworkDevice::RequestDeviceAlive(*m_pingskt);

    // Check for any network hardware that has been found but not yet initialized.
    // The hardware is not immediately placed in the m_networkHardwares anymore, as this caused issues
    // due to the initialization of network hardware not able to be completed immediately over
    // UDP (unlike usb hardware that has blocking calls to libusb to perform init immediately).
    // Placing hardware in the main hardware list straight away caused the enable button to
    // become active before the network cube had been fully initialized.
    // The hardware now sits in an initializing list until such time as the hardware has completed
    // initialization, at which point it is xfered to the main hardware list.
    // If the hardware does not init after a set period, it is then removed from the init list until
    // the hardware is detected again on the next device check.

    if (m_initializingnetworkHardwares.size()>0){ // any network devices that are still initializing?
        //qDebug() << m_initializingnetworkHardwares.size() << "devices yet to initialize";
        auto initnetworkHardwareIt = m_initializingnetworkHardwares.begin();
        while(initnetworkHardwareIt != m_initializingnetworkHardwares.end()) {
            std::unique_ptr<ldNetworkHardware> &networkHardware = (*initnetworkHardwareIt);


            // check if the network device has finished initializing
            // we must check that laserdocknetworkdevice is initialized before we init the ldnetworkhardware device
            if (networkHardware->params().device->status() == LaserdockNetworkDevice::Status::INITIALIZED){
                    if (networkHardware->status() == ldHardware::Status::INITIALIZED){
                        // check authentication if available
                        qDebug() << "network h/w device initialised OK.";
                        moveDeviceToActiveList(networkHardware);
                        initnetworkHardwareIt = m_initializingnetworkHardwares.erase(initnetworkHardwareIt); // remove from init list
                        continue;

                    } else { // device not init yet, so send init sequence
                        networkHardware->initialize();
                    }
            }

            initnetworkHardwareIt++;
        } // end while
    }


    // check for disconnected devices (each device does it's own device disconnect checking in the background)
    auto networkHardwareIt = m_networkHardwares.begin();
    while(networkHardwareIt != m_networkHardwares.end()) {
        std::unique_ptr<ldNetworkHardware> &networkHardware = (*networkHardwareIt);

        // do not check active devices
        //if(networkHardware->isActive()) {
        //    networkHardwareIt++;
        //    continue;
        //}

        if (networkHardware->params().device->get_disconnected()){ // this device has disconnected?
            removeDeviceFromActiveList(networkHardware);
            networkHardwareIt = m_networkHardwares.erase(networkHardwareIt); // remove from the main lis
        } else networkHardwareIt++;

    }


    // check if the main list has changed in size due to disconnects, or devices having finished initializing
    if(oldDeviceCount != m_networkHardwares.size()) {
        emit deviceCountChanged(static_cast<uint>(m_networkHardwares.size()));
    }
}

void ldNetworkHardwareManager::removeDevice(const QString &hwId)
{
    QMutexLocker locker(&m_mutex);

    auto networkHardwareIt = m_networkHardwares.begin();
    while(networkHardwareIt != m_networkHardwares.end()) {
        std::unique_ptr<ldNetworkHardware> &networkHardware = (*networkHardwareIt);
        if (networkHardware->id() == hwId){
            removeDeviceFromActiveList(networkHardware);
            networkHardwareIt = m_networkHardwares.erase(networkHardwareIt); // remove from the main lis
            emit deviceCountChanged(static_cast<uint>(m_networkHardwares.size()));
            return;
        } else networkHardwareIt++;
    }
}

// check for new devices always when we are not active
// and do not check if active because we don't want to have flickering

void ldNetworkHardwareManager::init()
{
    QMutexLocker locker(&m_mutex);

    qDebug() << "ldNetworkHardwareManager::init()" ;

    // FIXME parents
    m_pingskt = new QUdpSocket();

    connect(this,&ldNetworkHardwareManager::ConnectedDevicesActiveChanged,this,&ldNetworkHardwareManager::ConnectedDevicesActiveUpdate);

    // configure our local socket for talking with any networked lasercubes
    LaserdockNetworkDevice::ConfigDeviceAliveRequestSocket(*m_pingskt);
    // we need to process any datagrams from a responding network lasercube
    connect(m_pingskt, &QUdpSocket::readyRead,this, &ldNetworkHardwareManager::readPendingDeviceResponses);
}
