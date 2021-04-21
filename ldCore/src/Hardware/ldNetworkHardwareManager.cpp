#include "ldCore/Hardware/ldNetworkHardwareManager.h"

#include <QtCore/QtDebug>
#include <QNetworkDatagram>
#include <QtCore/QCoreApplication>
#include <laserdocklib/LaserdockSample.h>
#include "laserdocklib/LaserDockNetworkDevice.h"

#include <ldCore/Filter/ldFilterManager.h>
#include <ldCore/Hardware/ldNetworkHardware.h>

ldNetworkHardwareManager::ldNetworkHardwareManager(ldFilterManager *filterManager, QObject *parent)
    : ldAbstractHardwareManager(parent)
    , m_filterManager(filterManager)
    , m_currentBufferConfig(&m_wifi_server_device_config)
{
    set_isActive(false);
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

ldAbstractHardwareManager::DeviceBufferConfig ldNetworkHardwareManager::getBufferConfig()
{
     //QMutexLocker locker(&m_mutex);
     return *m_currentBufferConfig;
}

void ldNetworkHardwareManager::setGenerateSecurityRequestCb(ldGenerateSecurityRequestCallbackFunc authenticateFunc)
{
    m_genSecReqCb = authenticateFunc;
}

void ldNetworkHardwareManager::setAuthenticateSecurityCb(ldAuthenticateSecurityResponseCallbackFunc checkFunc)
{
    m_authSecRespCb = checkFunc;
}

// A network cube device is requesting that we authenticate it
// We should send out a security request from this event
void ldNetworkHardwareManager::DeviceAuthenticateRequest(LaserdockNetworkDevice&device)
{
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
                                                          bool& success, QByteArray& response_data)
{
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
    std::unique_ptr<LaserdockNetworkDevice> newDevice(new LaserdockNetworkDevice(ip_addr,this));

    auto *device = newDevice.get();

    std::unique_ptr<ldNetworkHardware> networkHardware(new ldNetworkHardware(newDevice.release(),this));
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
   // qDebug() << "someone responded to our ping!!!!!!!!";
    while (m_pingskt->hasPendingDatagrams()) {
            QNetworkDatagram datagram = m_pingskt->receiveDatagram();
            QString ip_addr = datagram.senderAddress().toString();
            const QByteArray& data = datagram.data();
            if (LaserdockNetworkDevice::DeviceAliveResponseValid(data)==true){ // valid lasercube device info response?
                // check if we already have this device in our list
                //qDebug() << "network lasercube with ip:" << ip_addr << "responded to our ping";

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

int ldNetworkHardwareManager::getBufferFullCount() // called from ldthreadeddataworker
{
    //QMutexLocker locker(&m_mutex);

    int fullCount = -1;
    int tmp = 0;

    auto networkHardwareIt = m_networkHardwares.begin();
    while(networkHardwareIt != m_networkHardwares.end()) {
        std::unique_ptr<ldNetworkHardware> &networkHardware = (*networkHardwareIt);
        // we should explicitly activate devices before usage
        if(!networkHardware->isActive()) {
            networkHardwareIt++;
            continue;
        }

        tmp = networkHardware->get_full_count();

         if (tmp==-1){
             networkHardware->ResetStatus(); // reset all hardware init status for this device
             m_initializingnetworkHardwares.push_back(std::move(networkHardware)); // move device back to init list
             networkHardwareIt = m_networkHardwares.erase(networkHardwareIt);
             updateHardwareFilters();
             emit deviceCountChanged(static_cast<uint>(m_networkHardwares.size()));
         } else {
             networkHardwareIt++;
             if (tmp>fullCount) fullCount = tmp; // take the highest buffer used from all connected cubes
         }
    } // end while

    return fullCount;
}

void ldNetworkHardwareManager::sendData(uint startIndex, uint count)
{
    //QMutexLocker locker(&m_mutex);

    if (m_networkHardwares.empty()) {
        return;
    }

    if (m_networkHardwares.size()==1){
       ldNetworkHardware *networkHardware = m_networkHardwares.at(0).get();
       if(networkHardware->isActive()) {
            networkHardware->send_samples(startIndex, count);
       } // end isactive

    }
    else // multiple cubes get data fed to all
    {

        auto networkHardwareIt = m_networkHardwares.begin();
        while(networkHardwareIt != m_networkHardwares.end()) {
            std::unique_ptr<ldNetworkHardware> &networkHardware = (*networkHardwareIt);
            // we should explicitly activate devices before usage
            if(!networkHardware->isActive()) {
                networkHardwareIt++;
                continue;
            }

             bool b = networkHardware->send_samples(startIndex, count);

             if (!b){
                 networkHardware->ResetStatus(); // reset all hardware init status for this device
                 m_initializingnetworkHardwares.push_back(std::move(networkHardware)); // move device back to init list
                 networkHardwareIt = m_networkHardwares.erase(networkHardwareIt);
                 updateHardwareFilters();
                 emit deviceCountChanged(static_cast<uint>(m_networkHardwares.size()));
             } else {
                 networkHardwareIt++;
             }

            /*
            // flip each next sample
            for(uint i = 0; i < count; i++){
                CompressedSample &sample = samples[i];
                sample.x = CompressedSample::flipCoord(sample.x);
            }*/
        }
    } // end else multiple hardware

}


void ldNetworkHardwareManager::setConnectedDevicesActive(bool active)
{
    emit ConnectedDevicesActiveChanged(active);
    /*
    for(std::unique_ptr<ldNetworkHardware> &networkHardware : m_networkHardwares) {
        networkHardware->setActive(active);
    }

    updateCheckTimerState();
    */
}

void ldNetworkHardwareManager::ConnectedDevicesActiveUpdate(bool active)
{
    for(std::unique_ptr<ldNetworkHardware> &networkHardware : m_networkHardwares) {
        networkHardware->setActive(active);
    }

    updateCheckTimerState();
}


uint ldNetworkHardwareManager::deviceCount() const
{
    return static_cast<uint>(m_networkHardwares.size());
}

std::vector<ldHardware*> ldNetworkHardwareManager::devices() const
{
    std::vector<ldHardware*> devices;
    for(const std::unique_ptr<ldNetworkHardware> &hardware : m_networkHardwares)
        devices.push_back(hardware.get());
    return devices;
}


bool ldNetworkHardwareManager::isDeviceActive(int index) const
{
    if(index < 0 || index >= static_cast<int>(m_networkHardwares.size())) {
        return false;
    }

    const std::unique_ptr<ldNetworkHardware> &networkHardware = m_networkHardwares[static_cast<unsigned long>(index)];
    return networkHardware->isActive();
}

void ldNetworkHardwareManager::setDeviceActive(int index, bool active)
{
    if(index < 0 || index >= static_cast<int>(m_networkHardwares.size())) {
        return;
    }

    std::unique_ptr<ldNetworkHardware> &networkHardware = m_networkHardwares[static_cast<unsigned long>(index)];
    networkHardware->setActive(active);

    updateCheckTimerState();
}


bool ldNetworkHardwareManager::hasActiveDevices() const
{
    qDebug() << "ldNetworkHardwareManager::hasActiveDevices()";
    auto it = std::find_if(m_networkHardwares.begin(), m_networkHardwares.end(),
                           [&](const std::unique_ptr<ldNetworkHardware> &networkHardware) {
            return (networkHardware->isActive() && networkHardware->status() == ldHardware::Status::INITIALIZED);
    });

    return it != m_networkHardwares.end();
}

void ldNetworkHardwareManager::networkDeviceDisconnectCheck()
{
    uint oldDeviceCount = static_cast<uint>(m_networkHardwares.size());
    // check for disconnected devices (each device does it's own device disconnect checking in the background)
    auto networkHardwareIt = m_networkHardwares.begin();
    while(networkHardwareIt != m_networkHardwares.end()) {
        std::unique_ptr<ldNetworkHardware> &networkHardware = (*networkHardwareIt);

        if (networkHardware->params().device->get_disconnected()){ // this device has disconnected?
            networkHardware->ResetStatus(); // reset all hardware init status for this device
            m_initializingnetworkHardwares.push_back(std::move(networkHardware)); // move device back to init list
            networkHardwareIt = m_networkHardwares.erase(networkHardwareIt); // remove from the main list
            updateHardwareFilters();
        } else networkHardwareIt++;
    }


    // check if the main list has changed in size due to disconnects, or devices having finished initializing
    if(oldDeviceCount != m_networkHardwares.size()) {
        emit deviceCountChanged(static_cast<uint>(m_networkHardwares.size()));
    }
}

void ldNetworkHardwareManager::networkDeviceCheck()
{
     if (get_isActive()==false) return;
    //qDebug() << "ldNetworkHardwareManager::networkDeviceCheck()";
    //qDebug() << QThread::currentThread();

    //QMutexLocker locker(&m_mutex);

    //qDebug() << "ldNetworkHardwareManager::networkDeviceCheck()";
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
                        m_networkHardwares.push_back(std::move(networkHardware)); // move device from init list to the main list
                        updateHardwareFilters();

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
            networkHardware->ResetStatus(); // reset all hardware init status for this device
            m_initializingnetworkHardwares.push_back(std::move(networkHardware)); // move device back to init list
            networkHardwareIt = m_networkHardwares.erase(networkHardwareIt); // remove from the main list
            updateHardwareFilters();
        } else networkHardwareIt++;

    }


    // check if the main list has changed in size due to disconnects, or devices having finished initializing
    if(oldDeviceCount != m_networkHardwares.size()) {
        emit deviceCountChanged(static_cast<uint>(m_networkHardwares.size()));
    }
}

// Choose the best buffering strategy based on all the currently connected cube interface types.
// One cube may be connected over ethernet as a LAN server, but another may be over Wifi as a client,
// so we must choose a strategy for the buffering, with the least latency without dropouts etc.
void ldNetworkHardwareManager::updateBufferingStrategy(QList<LaserdockNetworkDevice::ConnectionType> &contypes)
{
    if (contypes.size()>0){
        // TODO: for now only use the first device's connection type to set buffering strategy
        // until more testing is performed when more cubes are available
        switch(contypes[0])
        {
            case LaserdockNetworkDevice::ConnectionType::CON_ETHERNET_SERVER :
                m_currentBufferConfig = &m_lan_server_device_config;
                qDebug() << "ldNetworkHardwareManager::updateBufferingStrategy: CON_ETHERNET_SERVER";
            break;

            case LaserdockNetworkDevice::ConnectionType::CON_ETHERNET_CLIENT :
            if (contypes.size()>1){
                m_currentBufferConfig = &m_lan_dual_client_device_config;
                qDebug() << "ldNetworkHardwareManager::updateBufferingStrategy: CON_DUAL_ETHERNET_CLIENT";
            }
            else {
                m_currentBufferConfig = &m_lan_client_device_config;
                qDebug() << "ldNetworkHardwareManager::updateBufferingStrategy: CON_ETHERNET_CLIENT";
            }
            break;

            case LaserdockNetworkDevice::ConnectionType::CON_WIFI_SERVER :
            if (contypes.size()>1){
                 m_currentBufferConfig = &m_wifi_server_client_device_config;
                 qDebug() << "ldNetworkHardwareManager::updateBufferingStrategy: CON_WIFI_SERVER_MULTI_CLIENT";
            } else {
                m_currentBufferConfig = &m_wifi_server_device_config;
                qDebug() << "ldNetworkHardwareManager::updateBufferingStrategy: CON_WIFI_SERVER";
            }
            break;

        case LaserdockNetworkDevice::ConnectionType::CON_WIFI_CLIENT :
            if (contypes.size()>1){
                m_currentBufferConfig = &m_wifi_dual_client_device_config;
                qDebug() << "ldNetworkHardwareManager::updateBufferingStrategy: CON_DUAL_WIFI_CLIENT";
            } else{
                m_currentBufferConfig = &m_wifi_client_device_config;
                qDebug() << "ldNetworkHardwareManager::updateBufferingStrategy: CON_WIFI_CLIENT";
            }

        break;

            // default strategy
            default :
                m_currentBufferConfig = &m_wifi_server_device_config;
                qDebug() << "ldNetworkHardwareManager::updateBufferingStrategy: DEFAULT (CON_WIFI_SERVER)";
        }
    }

}

// check for new devices always when we are not active
// and do not check if active because we don't want to have flickering
void ldNetworkHardwareManager::updateCheckTimerState()
{
    if (get_isActive()==false) return;

    QList<LaserdockNetworkDevice::ConnectionType> cubeConnectionTypes;

    bool isActive = false;
    for(std::unique_ptr<ldNetworkHardware> &networkHardware : m_networkHardwares) {
        if(networkHardware->isActive()) {
            isActive = true;
        }

        // add each device's connection type to a list to check later.
        cubeConnectionTypes <<  networkHardware->params().device->get_connection_type();
    }

    updateBufferingStrategy(cubeConnectionTypes);

    // FIXME: added by Paul - is it necessary here?
    // updateHardwareFilters();

    // update all network devices with udp xfer strategy
    for(std::unique_ptr<ldNetworkHardware> &networkHardware : m_networkHardwares) {
        networkHardware->params().device->set_max_udp_samples_per_xfer(m_currentBufferConfig->max_samples_per_udp_xfer);
        networkHardware->params().device->set_max_udp_packets_per_xfer(m_currentBufferConfig->max_udp_packets_per_xfer);
    }

    if(isActive) {
        QTimer::singleShot(0, m_checkTimer, &QTimer::stop);
    } else {
        QTimer::singleShot(0, m_checkTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
        QTimer::singleShot(0, this, &ldNetworkHardwareManager::networkDeviceCheck);
    }
}

void ldNetworkHardwareManager::updateHardwareFilters()
{
    for(uint i = 0; i < m_networkHardwares.size(); i++) {
        auto &networkHardware = m_networkHardwares[i];
        if(i == 0)
            networkHardware->setFilter(m_filterManager->hardwareFilter());
        else
            networkHardware->setFilter(m_filterManager->hardwareFilter2());
    }
}
void ldNetworkHardwareManager::init()
{
    qDebug() << "ldNetworkHardwareManager::init()"  << QThread::currentThread();

    // FIXME parents
    m_checkTimer = new QTimer();
    m_pingskt = new QUdpSocket();



    connect(this, &ldNetworkHardwareManager::deviceCountChanged, this, &ldNetworkHardwareManager::updateCheckTimerState);
    connect(m_checkTimer, &QTimer::timeout, this, &ldNetworkHardwareManager::networkDeviceCheck);
    connect(this,&ldNetworkHardwareManager::ConnectedDevicesActiveChanged,this,&ldNetworkHardwareManager::ConnectedDevicesActiveUpdate);




    // configure our local socket for talking with any networked lasercubes
    LaserdockNetworkDevice::ConfigDeviceAliveRequestSocket(*m_pingskt);
    // we need to process any datagrams from a responding network lasercube
    connect(m_pingskt, &QUdpSocket::readyRead,this, &ldNetworkHardwareManager::readPendingDeviceResponses);

    m_checkTimer->setInterval(500);

    updateCheckTimerState();

    connect(this,&ldNetworkHardwareManager::isActiveChanged,this,[&](){
        if (get_isActive()) {
            //qDebug() << "ldNetworkHardwareManager::isActive = Yes";
            updateCheckTimerState();
        } else {
            //qDebug() << "ldNetworkHardwareManager::isActive = No";
            QTimer::singleShot(0, m_checkTimer, &QTimer::stop);
        }
    });
}
