// Network Cube class for controlling the new Wifi/LAN lasercube using ESP32 hardware
// History:
//  1/08/20 removed inversion of the y sample data as this was not required now the dev
//          cubes have the wiring for the y galvo corrected.

#include <laserdocklib/LaserDockNetworkDevice.h>
#include <laserdocklib/LaserdockSample.h>
//#include "ldCore/Hardware/ldHardware.h"
#include <QDebug>
#include <QNetworkDatagram>
#include <QtCore/QCoreApplication>
#include <QNetworkInterface>
#include <QVariant>

// commands available over UDP
namespace cmds {
    const uint8_t LASERCUBE_GET_ALIVE =                             0x27;
    const uint8_t LASERCUBE_GET_FULL_INFO =                         0x77;
    const uint8_t LASERCUBE_ENABLE_BUFFER_SIZE_RESPONSE_ON_DATA =   0x78;
    const uint8_t LASERCUBE_CMD_SET_OUTPUT =                        0x80;
    const uint8_t LASERCUBE_CMD_SET_ILDA_RATE =                     0x82;
    const uint8_t LASERCUBE_CMD_CLEAR_RINGBUFFER =                  0x8d;
    const uint8_t LASERCUBE_CMD_GET_RINGBUFFER_EMPTY_SAMPLE_COUNT = 0x8a;
    const uint8_t LASERCUBE_CMD_SET_NV_MODEL_INFO =                 0x97;
    const uint8_t LASERCUBE_SECURITY_CMD_REQUEST =                  0xb0;
    const uint8_t LASERCUBE_SECURITY_CMD_RESPONSE =                 0xb1;
    const uint8_t LASERCUBE_CMD_SET_DAC_BUF_THOLD_LVL =             0xa0;

    // These legacy get commands below have been replaced by
    // the get full info command (x077)
    /*
    const uint8_t LASERCUBE_CMD_GET_OUTPUT =                        0x81;
    const uint8_t LASERCUBE_CMD_GET_ILDA_RATE =                     0x83;
    const uint8_t LASERCUBE_CMD_GET_MAX_ILDA_RATE =                 0x84;
    const uint8_t LASERCUBE_CMD_GET_SAMP_ELEMENT_COUNT =            0x85;
    const uint8_t LASERCUBE_CMD_GET_ISO_PACKET_SAMP_COUNT =         0x86;
    const uint8_t LASERCUBE_CMD_GET_DAC_MIN  =                      0x87;
    const uint8_t LASERCUBE_CMD_GET_DAC_MAX =                       0x88;
    const uint8_t LASERCUBE_CMD_GET_RINGBUFFER_SAMPLE_COUNT =       0x89;
    const uint8_t LASERCUBE_CMD_GET_LASERCUBE_FW_MAJOR_VERSION =    0x8b;
    const uint8_t LASERCUBE_CMD_GET_LASERCUBE_FW_MINOR_VERSION =    0x8c;
    const uint8_t LASERCUBE_CMD_GET_BULK_PACKET_SAMP_COUNT =        0x8e;
    */
}

// values for the get/set output enable command
#define LASERCUBE_CMD_OUTPUT_ENABLE 					0x01
#define LASERCUBE_CMD_OUTPUT_DISABLE 					0x00

// UDP packets on the data port have this id byte as the first byte of the packet for standard uncompressed sample data
#define LASERCUBE_SAMPLE_DATA_ID						0xA9
// UDP packets on the data port have this id byte as the first byte of the packet for compressed sample data
#define LASERCUBE_SAMPLE_DATA_COMPRESSED_ID				0x9A

#define USE_THREAD_BUFFER_SIZE  0

// constants for this class
namespace constants{

    const uint8_t min_protocol_ver = 0;                 // this defines the protocol version of the full info response packet from network cube
    const int alive_port = 45456;                       // this port accepts alive requests, and responds with alive messages only
    const int cmd_port = 45457;                         // this port accepts all the network cube commands
    const int data_port = 45458;                        // this port only accepts sample data to render
    const int comms_timeout_period_ms = 4000;           // how long to wait for a full cube info response pkt before we flag as disconnected
    const int inactive_info_request_period_ms = 250;    // how often to poll for the full cube info status while laser is inactive
    const int active_info_request_period_ms = 2500;    // how often to poll for the full cube info status while laser is active
    const int thread_sample_buffer_size = 8000;         // how big the buffer between threadworker class and our udp class
    const int max_udp_packets = 20;                     // maximum number of UDP packets to send in one go (ESP32 has an internal 20 UDP limit)
    const int max_samples_per_udp_packet = 140;          // max bytes we should ever send in one UDP packet (80 uncompressed samples + 4 byte header)
    const int command_repeat_count = 2;                 // how many times to send out a command over UDP
    const int security_timeout_ms = 1000;               // how long to wait for a security response after a request was sent
}


LaserdockNetworkDevice::LaserdockNetworkDevice(QString ip_address,QObject *parent) : QObject(parent)
  ,m_hostaddr(ip_address)
  ,m_max_samples_per_udp_pkt(constants::max_samples_per_udp_packet)
  ,m_ip_address(ip_address)
{
    m_cmdsocket = new QUdpSocket(this);
    m_cmdsocket->bind( constants::cmd_port);
    m_cmdsocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption,5250000);
    m_cmdsocket->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption,5250000);

    connect(m_cmdsocket, &QUdpSocket::readyRead,this, &LaserdockNetworkDevice::readPendingCommandResponses);
   connect(m_cmdsocket, &QUdpSocket::stateChanged,this, [&](QAbstractSocket::SocketState socketState) {
       qDebug() << "socketState" << m_hostaddr << socketState;
   });


    m_datasocket = new QUdpSocket(this);
    m_datasocket->bind( constants::data_port);
    m_datasocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption,5250000);
    m_datasocket->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption,5250000);

    connect(m_datasocket, &QUdpSocket::readyRead,this, &LaserdockNetworkDevice::readPendingDataResponses);
    connect(this,&LaserdockNetworkDevice::SamplesReady,this,&LaserdockNetworkDevice::processSamples);

    connect(qApp, &QCoreApplication::aboutToQuit, this, [&](){
       disable_output();
    });

    connect(this,&LaserdockNetworkDevice::RequestBufferSize,this,[&](){
        uint8_t info_pkt[]={cmds::LASERCUBE_CMD_GET_RINGBUFFER_EMPTY_SAMPLE_COUNT};
        send_command(info_pkt,sizeof(info_pkt),false);
    });


    connect(&m_timer,&QTimer::timeout,this,&LaserdockNetworkDevice::process);

    connect(&m_nocommstimer,&QTimer::timeout,this,[&](){
        // if we are here then there was no comms from network cube after timeout period elapsed
        if (!m_disconnected){
            qDebug() << "*** COMMS LOST WITH NETWORK CUBE @" << m_ip_address << "***";
            m_queue.clear();
            m_disconnected = true;
            emit DeviceDisconnected();
        }
    });

    connect(&m_security_timer,&QTimer::timeout,this,[&](){
        qDebug() << "resending security request";
        m_cmdsocket->writeDatagram(m_security_req_data, m_security_req_data.size(), m_hostaddr, constants::cmd_port);
        m_security_timer.start(constants::security_timeout_ms);
    });


    uint8_t info_pkt[]={cmds::LASERCUBE_GET_FULL_INFO};
    send_command(info_pkt,sizeof(info_pkt),false);
    m_nocommstimer.start(constants::comms_timeout_period_ms); // start comms timeout timer (will be reset when we receive anything from cube)
    m_timer.start(constants::inactive_info_request_period_ms); // periodic timer for requesting full info from the network cube


    /*
    // connect to the initialized event which is triggered when we receive full info packet during device init.
    connect(this,&LaserdockNetworkDevice::DeviceInitialized,[&](){

        // test sending a security request to sha204 device on the remote network cube
       // this sequence was grabbed using wireshark from the USB lasercube
       uint8_t security_req_pkt[]={
           //0xB0, // security request cmd (not needed now as it is added within SecurityRequest() function.
           0x01, // wake option (1= sha204 wake-up, 0 = no wake)
           0xe0, 0x2e, 0x00, 0x00, 0x40, 0x9c, 0x00,
           0x00, 0x23, 0x27, 0x08, 0x00, 0x00, 0x00, 0xa1,
           0x21, 0x00, 0x00, 0xea, 0x35, 0x00, 0x00, 0x75,
           0x4f, 0x00, 0x00, 0x90, 0x1f, 0x00, 0x00, 0x40,
           0x39, 0x00, 0x00, 0x9c, 0x6d, 0x00, 0x00, 0xf2,
           0x2d, 0x00, 0x00, 0xa2, 0x6f, 0x00, 0x00, 0x73,
           0xc4
       };

       QByteArray req(reinterpret_cast<const char*>(security_req_pkt),sizeof(security_req_pkt));
       SecurityRequest(req); // send a security request packet (after which we expect a SecurityResponseReceived event)

    });

    // connect to the security response received event
    // expected RX response bytes (unprogrammed factory default key):
    // 23 5A 8A CA BD B4 10 B2 1C 9D 76 85 04 73 0B 2A 4F 8A 19 CD 22 BB FB 78 37 FC 82 7D 90 4A 3D 4D 0F 71 F2
    // expected RX response bytes (USB lasercube key programmed into sha204):
    // 23 34 8e 0c f3 01 6e 65 22 33 fb 3c 56 5f 32 07 fd 19 f4 1d 09 48 6f 25 4c 20 b2 4f 39 bd cb 20 b9 09 ce
    connect(this,&LaserdockNetworkDevice::SecurityResponseReceived,this,[&](bool success,QByteArray response_data){
        qDebug() << "Security response success:" << success;
        qDebug() << "data:" << response_data.toHex();
    });
*/

}

LaserdockNetworkDevice::~LaserdockNetworkDevice()
{
    // close our open sockets
    m_cmdsocket->close();
    m_datasocket->close();

    // we need a temp socket to send the output disable message before this object is destroyed
    QUdpSocket skt;
    uint8_t en_pkt[]={cmds::LASERCUBE_CMD_SET_OUTPUT,LASERCUBE_CMD_OUTPUT_DISABLE};
    skt.writeDatagram(reinterpret_cast<const char*>(en_pkt), sizeof(en_pkt), m_hostaddr, constants::cmd_port);
    skt.writeDatagram(reinterpret_cast<const char*>(en_pkt), sizeof(en_pkt), m_hostaddr, constants::cmd_port);
}

bool LaserdockNetworkDevice::get_interlock_enabled() const
{
    return m_interlock_enabled;
}

bool LaserdockNetworkDevice::get_over_temperature() const
{
   return m_over_temperature;
}

bool LaserdockNetworkDevice::get_temperature_warning() const
{
    return m_temperature_warn;
}


// returns true if we have disconnected from the device this instance is handling
bool LaserdockNetworkDevice::get_disconnected()
{
    return m_disconnected;
}


// Request full info from any network lasercube devices on the local network.
// This message is broadcasted to every node on all valid network interfaces
// A reference to a socket must be specified, and this socket will be the
// same socket that will receive any response messages from any networked
// lasercube devices.
bool LaserdockNetworkDevice::RequestDeviceAlive(QUdpSocket& skt)
{

    uint8_t info_pkt[]={cmds::LASERCUBE_GET_ALIVE};
     const QList<QNetworkInterface> allInterfaces = QNetworkInterface::allInterfaces();
     const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);

     // we scan for valid network interfaces every time this func is called , so any new interfaces are picked up on after laseros has started
     for (const auto& eth : allInterfaces) {
         if(eth.flags().testFlag(QNetworkInterface::IsUp) && eth.flags().testFlag(QNetworkInterface::IsRunning) && !eth.flags().testFlag(QNetworkInterface::IsPointToPoint)) {
             const QList<QNetworkAddressEntry> allEntries = eth.addressEntries();
             for (const auto& entry : allEntries) {
                 if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol  && (entry.ip() != localhost)){
                   // qDebug() <<"ip:" << entry.ip().toString() << "netmask" << entry.netmask().toString() << "broadcast" << entry.broadcast().toString() << "local" << localhost.toString();
                    skt.writeDatagram(reinterpret_cast<const char*>(info_pkt), sizeof(info_pkt), entry.broadcast(), constants::alive_port);
                 }
             }
         }
     }

     return true;
}

// configure the referenced socket to be able to receive device info responses
bool LaserdockNetworkDevice::ConfigDeviceAliveRequestSocket(QUdpSocket& skt)
{
    skt.bind(QHostAddress::AnyIPv4,constants::alive_port);
    skt.setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption,2250000);
    return true;
}

// returns true if the supplied data is a valid network lasercube info response packet
bool LaserdockNetworkDevice::DeviceAliveResponseValid(const QByteArray& response_data)
{
     if (response_data.size()==2){
        if (static_cast<uint8_t>(response_data.at(0))==cmds::LASERCUBE_GET_ALIVE){
            if (response_data.at(1)==0){
                   return true;
            }
        }
     }
    return false;
}

// must be set by the manager class once authentication has been completed
void LaserdockNetworkDevice::DeviceAuthenticated(bool state)
{
    m_authenticated = state;
    if (state==true){
        if (m_status==AUTHENTICATING){
            m_status = INITIALIZED;
            emit DeviceReady();
        }
    }
}

void LaserdockNetworkDevice::SecurityRequest(QByteArray request)
{
    if (request.size()>0){
        request.insert(0,static_cast<int8_t>(cmds::LASERCUBE_SECURITY_CMD_REQUEST));
        m_security_req_data = request; // store in case we need to resend due to timeout
        m_cmdsocket->writeDatagram(m_security_req_data, m_security_req_data.size(), m_hostaddr, constants::cmd_port);
        m_security_timer.start(constants::security_timeout_ms);
    }
}

void LaserdockNetworkDevice::send_command(uint8_t *cmd_data,size_t cmd_length_bytes, bool repeat_enable)
{
    int count;
    if (repeat_enable) count = constants::command_repeat_count; else count = 1;
    for (int i=0;i<count;i++) {
        m_cmdsocket->writeDatagram(reinterpret_cast<const char*>(cmd_data), static_cast<qint64>(cmd_length_bytes), m_hostaddr, constants::cmd_port);
    }
}

// used to put this device back into uninitialized state after connection loss
void LaserdockNetworkDevice::ResetStatus()
{
    if (m_status==INITIALIZED){
        m_status = UNKNOWN;
        m_authenticated = false;
    }
}

void LaserdockNetworkDevice::SetModelInfo(uint8_t region,uint8_t model_num,QString model_str)
{
    const char *str = qPrintable(model_str);
    size_t len = strlen(str)+1; // length of string plus null terminator

    if (len<=25) { // total string size cannot exceed 25 char including null term

        uint8_t cmd_buf[30]; // max size we need if using full model string length

        cmd_buf[0] = cmds::LASERCUBE_CMD_SET_NV_MODEL_INFO;
        cmd_buf[1] = 0xA2;
        cmd_buf[2] = 0x2A;
        cmd_buf[3] = region;
        cmd_buf[4] = model_num;
        strcpy(reinterpret_cast<char*>(&cmd_buf[5]),str);
        len+=5; // add on the 5 bytes at start of packet
        send_command(cmd_buf,len,false);
        qDebug() << "LaserdockNetworkDevice::SetModelInfo cmd sent";
    }
}

void LaserdockNetworkDevice::process()
{
       // qDebug() << "LaserdockNetworkDevice::process(): requesting full info from network hardware";

    // we periodically request full info back from the network cube in order to update battery levels, temperature etc.
    // we also use this to check for loss of comms with the cube, ensuring we disconnect if we dont here anything after set period.
    uint8_t info_pkt[]={cmds::LASERCUBE_GET_FULL_INFO};
    send_command(info_pkt,sizeof(info_pkt),false);
}

// decode and store full info packet into class vars
void LaserdockNetworkDevice::handleFullInfoPkt(const QByteArray &data)
{
    if (data.size()>=3){
        uint8_t payload_ver_id = static_cast<uint8_t>(data[2]);

        if (data.size()==64 && payload_ver_id==0){ // decode version 0 of the full info packet (allows for future upgrade)
            //qDebug() << "full network lasercube data info RX";
            uint8_t fwmaj,fwmin;

            fwmaj = static_cast<uint8_t>(data[3]);
            fwmin = static_cast<uint8_t>(data[4]);
            if (fwmaj!=m_fw_major) {
                m_fw_major = fwmaj;
                emit FWMajorRevisionUpdated(fwmaj);
            }
            if (fwmin!=m_fw_minor) {
                m_fw_minor = fwmin;
                emit FWMinorRevisionUpdated(fwmin);
            }
            //qDebug().nospace() << "Firmware Ver: " << m_fw_major << "." << m_fw_minor;

            uint8_t flags = static_cast<uint8_t>(data[5]);

            bool oe = (flags & 1) ? true : false;
            bool ilock = false;
            bool twarn = false;
            bool otemp = false;
            uint8_t pkterrors = 0;

            if ((m_fw_major>0) || (m_fw_minor>=13)) {
                ilock = (flags & 2) ? true : false;
                twarn = (flags & 4) ? true : false;
                otemp = (flags & 8) ? true : false;
                pkterrors = (flags>>4) & 0x0f;
            } else { // <=v0.12
                ilock = (flags & 8) ? true : false;
                twarn = (flags & 16) ? true : false;
                otemp = (flags & 32) ? true : false;
            }

            if (oe!=m_outputenabled) {
                m_outputenabled = oe;
                emit OutputEnableUpdated(oe);
            }

            if (ilock != m_interlock_enabled) {
                m_interlock_enabled = ilock;
                emit InterlockEnabledUpdated(ilock);
            }

            if (twarn != m_temperature_warn) {
                m_temperature_warn = twarn;
                emit TemperatureWarningUpdated(twarn);
            }

            if (otemp != m_over_temperature) {
                m_over_temperature = otemp;
                emit OverTemperatureUpdated(otemp);
            }

            if (pkterrors != m_packet_errors) {
                m_packet_errors = pkterrors;
                emit PacketErrorsUpdated(pkterrors);
            }

            //qDebug() << "DAC Enabled:" << m_outputenabled;
            uint32_t dacrate;
            dacrate = static_cast<uint8_t>(data[10]);
            dacrate+= static_cast<uint32_t>(static_cast<uint8_t>(data[11])<<8);
            dacrate+= static_cast<uint32_t>(static_cast<uint8_t>(data[12])<<16);
            dacrate+= static_cast<uint32_t>(static_cast<uint8_t>(data[13])<<24);
            if (dacrate!=m_dac_rate){
                emit DACRateUpdated(dacrate);
                m_dac_rate = dacrate;
            }
            //qDebug() << "DAC rate:" << m_dac_rate;
            uint32_t maxdacrate;
            maxdacrate = static_cast<uint8_t>(data[14]);
            maxdacrate+= static_cast<uint32_t>(static_cast<uint8_t>(data[15])<<8);
            maxdacrate+= static_cast<uint32_t>(static_cast<uint8_t>(data[16])<<16);
            maxdacrate+= static_cast<uint32_t>(static_cast<uint8_t>(data[17])<<24);
            if (m_max_dac_rate!=maxdacrate) {
                m_max_dac_rate = maxdacrate;
                emit MaxDACRateUpdated(maxdacrate);
            }
            //qDebug() << "Max DAC rate:" << m_max_dac_rate;
            uint16_t buffree;
            buffree = static_cast<uint8_t>(data[19]);
            buffree+= static_cast<uint16_t>(static_cast<uint8_t>(data[20])<<8);
            if (buffree!=m_buffer_free){
                emit SampleBufferFreeUpdated(buffree);
                m_buffer_free = buffree;
            }
            //qDebug() << "RX buffer Free:" << buffree;
            uint16_t bufsz;
            bufsz = static_cast<uint8_t>(data[21]);
            bufsz+= static_cast<uint16_t>(static_cast<uint8_t>(data[22])<<8);
            if (bufsz!=m_buffer_size) {
                emit SampleBufferSizeUpdated(bufsz);
                m_buffer_size = bufsz;
            }
            //qDebug() << "RX buffer Size:" << bufsz;

            uint8_t battery_percent = static_cast<uint8_t>(data[23]);
            if (battery_percent!=m_battery_pct){
                emit BatteryPercentUpdated(battery_percent);
                m_battery_pct = battery_percent;
            }

            int8_t temperature = static_cast<int8_t>(data[24]);
            if (temperature!=m_temperature_degc) {
                emit TemperatureUpdated(temperature);
                m_temperature_degc = temperature;
            }

            ConnectionType con = static_cast<ConnectionType>(data[25]+1);
            if (con!=m_contype){
                emit ConnectionTypeUpdated(con);
                m_contype = con;
            }

            QString str;
            str = QString("%1%2%3%4%5%6")
                    .arg( static_cast<uint8_t>(data[26]),2,16)
                    .arg( static_cast<uint8_t>(data[27]),2,16)
                    .arg( static_cast<uint8_t>(data[28]),2,16)
                    .arg( static_cast<uint8_t>(data[29]),2,16)
                    .arg( static_cast<uint8_t>(data[30]),2,16)
                    .arg( static_cast<uint8_t>(data[31]),2,16);

            std::string serstr = str.toUpper().toStdString();
            if (serstr!=m_serialnumber){
                emit SerialNumberUpdated(str);
                m_serialnumber = serstr;
            }

            //qDebug() << QString::fromStdString(m_serialnumber);


            // get network device ip address (does not return client mode ip's at the moment)
            QString ipaddr = QString("%1.%2.%3.%4")
                    .arg( static_cast<uint8_t>(data[32]),0,10)
                    .arg( static_cast<uint8_t>(data[33]),0,10)
                    .arg( static_cast<uint8_t>(data[34]),0,10)
                    .arg( static_cast<uint8_t>(data[35]),0,10);
            //qDebug() << "network device's IP Address:" << str;
            if (ipaddr!=m_ip_address && ipaddr!="0.0.0.0") {
                emit IPAddressUpdated(ipaddr);
                m_ip_address = ipaddr;
            }

            uint8_t modelnum = static_cast<uint8_t>(data[37]);
            if (modelnum!=m_model_number) {
                emit ModelNumberUpdated(modelnum);
                m_model_number = modelnum;
            }

            int pos = 38; // model name starts here in the data array
            QString tmpn;
            while (static_cast<uint8_t>(data[pos])!=0 && pos<data.size()){
                tmpn+=static_cast<char>(data[pos]);
                pos++;
            }

            if (tmpn!=m_model_name){
                emit ModelNameUpdated(tmpn);
                m_model_name = tmpn;
            }
            //qDebug() << "remote device model: " << m_model_name;

            if (m_status==UNKNOWN){
                m_status = AUTHENTICATING;
                emit DeviceNeedsAuthenticating();
            }

            /*
            if (m_status!=INITIALIZED){
                m_status = INITIALIZED; // we have received full info so we can go live
                emit DeviceInitialized();
            }*/

            m_disconnected = false;
            m_nocommstimer.start(constants::comms_timeout_period_ms); // restart comms timeout

        }
    }

}


void LaserdockNetworkDevice::readPendingCommandResponses()
{
    //qDebug() << "CMD response RX.\r\n";
    while (m_cmdsocket->hasPendingDatagrams()) {
            m_nocommstimer.start(constants::comms_timeout_period_ms); // restart comms timeout
            QNetworkDatagram datagram = m_cmdsocket->receiveDatagram();
            processCmdDatagram(datagram);
        }
}

// process incoming command response messages
void LaserdockNetworkDevice::processCmdDatagram(QNetworkDatagram &datagram)
{
    //qDebug() << "CMD Response:" << datagram.data().toHex();

    QByteArray data = datagram.data();

    if (data.length()<2){ // need at least 2 byte response
        if (data.length()>0) qDebug() << "CMD:" << static_cast<uint8_t>(data[0]) << "Failed!";
        else qDebug() << "invalid CMD RX response detected! (length<2)";
    }
    else if (data.at(1)!=0x00){ // 2nd byte is the result byte
        qDebug() << "CMD:" << static_cast<uint8_t>(data[0]) << "Failed!";
        return;
    }
    else // command was succesfull
    switch(static_cast<uint8_t>(data.at(0))){
        case cmds::LASERCUBE_GET_FULL_INFO :
            handleFullInfoPkt(datagram.data()); // process full info result payload
        break;
        case cmds::LASERCUBE_CMD_GET_RINGBUFFER_EMPTY_SAMPLE_COUNT:
            if (data.size()==4){
                uint16_t tmp;
                tmp = static_cast<uint8_t>(data[2]);
                tmp+= static_cast<uint16_t>(static_cast<uint8_t>(data[3])<<8);
                m_buffer_free = tmp;
                m_disconnected = false;
                //qDebug() << "remote buffer free:" << m_buffer_free;
                //processSamples();
            }
        break;

        case cmds::LASERCUBE_SECURITY_CMD_RESPONSE :
        {
            m_security_timer.stop(); // stop timeout as we have received security response
            bool ok = (data.at(1)==0x00 && data.at(2)==0x00) ? true : false;
                data.remove(0,3); // remove 0xb1 command and success bytes
                //qDebug() << "security response data:" << data.toHex();
                emit SecurityResponseReceived(ok,data);
        }
        break;

        case cmds::LASERCUBE_SECURITY_CMD_REQUEST :
        {
            uint8_t security_resp_pkt[]={
                0xB1, // security response cmd
            };
            // we have succesfully received an ack to the security request, so get the security response
            m_cmdsocket->writeDatagram(reinterpret_cast<const char *>(security_resp_pkt), sizeof(security_resp_pkt), m_hostaddr, constants::cmd_port);
        }
        break;
    }
}

// Process the incoming data responses to every data sample packet we have sent over the network.
// These are just returning the buffer free status from the network cube device.
void LaserdockNetworkDevice::readPendingDataResponses()
{
    //qDebug() << "DATA response RX";
    uint16_t tmp = 0;
    while (m_datasocket->hasPendingDatagrams()) {
            m_nocommstimer.start(constants::comms_timeout_period_ms); // restart comms timeout
            QNetworkDatagram datagram = m_datasocket->receiveDatagram();
            const QByteArray& data = datagram.data();
            if (data.length()==4){ // we are only interested in the ring buffer status response from network lasercube
                if (static_cast<uint8_t>(data[0]) == cmds::LASERCUBE_CMD_GET_RINGBUFFER_EMPTY_SAMPLE_COUNT){
                    tmp = static_cast<uint8_t>(data[2]);
                    tmp+= static_cast<uint16_t>(static_cast<uint8_t>(data[3])<<8);
                    m_buffer_free = tmp; // update buffer free status of this device
                }
            }
    }

    //qDebug() << "free:" << m_buffer_free;
}




void LaserdockNetworkDevice::initialize()
{

}

bool LaserdockNetworkDevice::enable_output() {
    qDebug() << "LaserdockNetworkDevice::OutputEnableUpdate-> device output enabled";
    uint8_t en_pkt[]={cmds::LASERCUBE_CMD_SET_OUTPUT,LASERCUBE_CMD_OUTPUT_ENABLE};
    send_command(en_pkt,sizeof(en_pkt),true);
    m_outputenabled = true;
    m_queue.clear();
    m_timer.start(constants::active_info_request_period_ms); // periodic timer for requesting full info from the network cube

    return true;
}

bool LaserdockNetworkDevice::disable_output() {
    qDebug() << "LaserdockNetworkDevice::OutputEnableUpdate-> device output disabled";
    uint8_t en_pkt[]={cmds::LASERCUBE_CMD_SET_OUTPUT,LASERCUBE_CMD_OUTPUT_DISABLE};
    send_command(en_pkt,sizeof(en_pkt),true);
    m_outputenabled = false;
    m_queue.clear();
    m_timer.start(constants::inactive_info_request_period_ms); // periodic timer for requesting full info from the network cube
    return true;
}



bool LaserdockNetworkDevice::get_output(bool *enabled) {
    if (enabled!=nullptr) *enabled = m_outputenabled;
    return true;
}


LaserdockNetworkDevice::Status LaserdockNetworkDevice::status() const {
   // qDebug() << "LaserdockNetworkDevice::status()";
    return m_status;
}

std::string LaserdockNetworkDevice::get_serial_number() const
{
    return m_serialnumber;
    //return lddev_private->get_serial_number();
}

std::string LaserdockNetworkDevice::get_device_name() const
{
    return "NetworkLaserCube";
}

QString LaserdockNetworkDevice::get_ip_address() const
{
    return m_ip_address;
}


int8_t LaserdockNetworkDevice::get_bus_number() const
{
    return -1;
}

int8_t LaserdockNetworkDevice::get_device_address() const
{
    return -1;
}



void LaserdockNetworkDevice::print() const
{
    //lddev_private->print();
}

bool LaserdockNetworkDevice::temperature_degc(int8_t *value)
{
    if (value!=nullptr){
        *value = m_temperature_degc;
        return true;
    }
    else return false;
}

bool LaserdockNetworkDevice::battery_pct(uint8_t *value)
{
    if (value!=nullptr){
        *value = m_battery_pct;
        return true;
    }
    else return false;
}



bool LaserdockNetworkDevice::dac_rate(uint32_t *rate) {

    if (rate!=nullptr){
        *rate = m_dac_rate;
        return true;
    } else return false;
}

bool LaserdockNetworkDevice::enable_buffer_state_replies()
{
    // enable buffer size responses every time the network cube receives a data packet
    uint8_t en_bresp[]={cmds::LASERCUBE_ENABLE_BUFFER_SIZE_RESPONSE_ON_DATA,0x01};
    send_command(en_bresp,sizeof(en_bresp),true);
    return true;
}

bool LaserdockNetworkDevice::disable_buffer_state_replies()
{
    // disable buffer size responses every time the network cube receives a data packet
    uint8_t en_bresp[]={cmds::LASERCUBE_ENABLE_BUFFER_SIZE_RESPONSE_ON_DATA,0x00};
    send_command(en_bresp,sizeof(en_bresp),true);
    return true;
}

bool LaserdockNetworkDevice::set_dac_rate(uint32_t rate) {
    //emit DACRateUpdated(rate);

    qDebug() << "################## idla rate set to:" << rate << "#######################";
    uint8_t dac_pkt[5];
    dac_pkt[0] = cmds::LASERCUBE_CMD_SET_ILDA_RATE;
    dac_pkt[1] = rate&0xff;
    dac_pkt[2] = (rate>>8)&0xff;
    dac_pkt[3] = (rate>>16)&0xff;
    dac_pkt[4] = (rate>>24)&0xff;

    send_command(dac_pkt,sizeof(dac_pkt),true);
    return true;
}

bool LaserdockNetworkDevice::set_dac_buffer_thold_lvl(uint32_t level)
{
    if (level >= m_buffer_size) return false; // can't exceed available buffer space

    // model 10 (ultra mk2) onwards now support this command
    // Also going forward, mk1 cube with firmware > 1.23 will have this
    if (m_model_number >= 10 || m_fw_major > 1 || m_fw_minor > 23) {
        qDebug() << "################## Network Cube Buffer Level THold:" << level << "##################";
    //
        uint8_t dac_pkt[5];
        dac_pkt[0] = cmds::LASERCUBE_CMD_SET_DAC_BUF_THOLD_LVL;
        dac_pkt[1] = level & 0xff;
        dac_pkt[2] = (level >> 8) & 0xff;
        dac_pkt[3] = (level >> 16) & 0xff;
        dac_pkt[4] = (level >> 24) & 0xff;

        send_command(dac_pkt,sizeof(dac_pkt),true);
    }
    return true;
}

bool LaserdockNetworkDevice::max_dac_rate(uint32_t *rate) {
    if (rate!=nullptr){
        *rate = m_max_dac_rate;
        return true;
    } else return false;
}

bool LaserdockNetworkDevice::min_dac_value(uint32_t *value) {
    if (value!=nullptr){
        *value = m_min_dac_value;
        return true;
    } else return false;
}

bool LaserdockNetworkDevice::max_dac_value(uint32_t *value) {
    if (value!=nullptr){
        *value = m_max_dac_value;
        return true;
    } else return false;
}


bool LaserdockNetworkDevice::sample_element_count(uint32_t *count) {
    Q_UNUSED(count)
    return false;//guint32(lddev_private->devh_ctl, 0X85, count);
}

bool LaserdockNetworkDevice::iso_packet_sample_count(uint32_t *count) {
     Q_UNUSED(count)
    return false;//guint32(lddev_private->devh_ctl, 0x86, count);
}

bool LaserdockNetworkDevice::bulk_packet_sample_count(uint32_t *count) {
     Q_UNUSED(count)
    return false;//guint32(lddev_private->devh_ctl, 0x8E, count);
}

bool LaserdockNetworkDevice::version_major_number(uint32_t *major) {
    if (major!=nullptr){
        *major = m_fw_major;
        return true;
    } else return false;
}

bool LaserdockNetworkDevice::version_minor_number(uint32_t *minor) {
    if (minor!=nullptr){
        *minor = m_fw_minor;
        return true;
    } else return false;
}


bool LaserdockNetworkDevice::ringbuffer_sample_count(uint32_t *count) {
    if (m_disconnected) return false;

    if (count!=nullptr){
    #if ( USE_THREAD_BUFFER_SIZE==0)
        *count = m_buffer_size;
    #else
        *count = constants::thread_sample_buffer_size;
     #endif
        return true;
    }
    else return false;
}

bool LaserdockNetworkDevice::ringbuffer_empty_sample_count(uint32_t *count) {
    if (m_disconnected) return false;

    if (count!=nullptr){
#if ( USE_THREAD_BUFFER_SIZE==0)
        *count = m_buffer_free;
#else
        *count = constants::thread_sample_buffer_size-m_queue.size();
#endif
        return true;
    }
    else return false;
}

bool LaserdockNetworkDevice::send(unsigned char *data, uint32_t length) {

    uint8_t* ptr = reinterpret_cast<uint8_t*>(malloc(length+1));
    if (ptr){
        ptr[0] = LASERCUBE_SAMPLE_DATA_ID;
        memcpy(ptr+1,data,length);
        m_datasocket->writeDatagram(reinterpret_cast<const char *>(ptr), sizeof(length+1), m_hostaddr, constants::data_port);
        free(ptr);
        return true;
    } else
        return false;
}

bool LaserdockNetworkDevice::network_send(unsigned char *data, uint length)
{
    m_datasocket->writeDatagram(reinterpret_cast<const char *>(data), length, m_hostaddr, constants::data_port);
    return true;
}

unsigned char *LaserdockNetworkDevice::network_get(unsigned char * data, int length)
{
    Q_UNUSED(data)
    Q_UNUSED(length)
    return nullptr;
}

void LaserdockNetworkDevice::set_max_udp_samples_per_xfer(uint samples)
{
    if (samples<=max_buffered_samples){
        m_max_samples_per_udp_pkt = samples;
        //qDebug() << "max sample size per UDP Xfer:" << samples;
    }
}
void LaserdockNetworkDevice::set_max_udp_packets_per_xfer(uint packets)
{
    if (packets<=constants::max_udp_packets){
        m_max_udp_packets = packets;
        qDebug() << "max packets per UDP Xfer:" << packets;
    }
}

void LaserdockNetworkDevice::processSamples()
{
    char*dptr = &m_dgrambuffer[4];
    int udp_count = 0;
    uint sample_count = 0;

    unsigned int count = static_cast<unsigned int>(m_queue.size());


    quint64 samples_gone = m_elapsed_timer.elapsed() * 30; // get num ms since last process and convert to num samples that will have been rendered in cube since last time.
    if (samples_gone>0){
        m_elapsed_timer.start();
        uint16_t bufsz = m_buffer_size;

        if (samples_gone>bufsz) {
            m_buffer_free = bufsz;
        } else {
            m_buffer_free+=samples_gone; // we can pre-calc how many samples were rendered by the cube since the last time we sent samples
            if (m_buffer_free>bufsz) m_buffer_free = bufsz;
           // uint16_t s = samples_gone;
           // if (s>0) qDebug() << "samples gone since last call:" << s;
        }
    }


    if (count>m_buffer_free) count = m_buffer_free;

    if (count>(m_max_samples_per_udp_pkt* constants::max_udp_packets)){
        count = (m_max_samples_per_udp_pkt* constants::max_udp_packets);
    }





    m_dgrambuffer[0] = static_cast<char>(LASERCUBE_SAMPLE_DATA_ID); // sample data packet id
    m_dgrambuffer[1] = 0x00;

    for (unsigned int cnt=0;cnt < count;cnt++){
        LaserdockSample s = m_queue.front();
        m_queue.pop_front();

        // convert from compressed usb sample to uncompressed UDP sample
        uint16_t x = s.x;
        uint16_t y = s.y;
        uint16_t b = static_cast<uint16_t>(s.b<<4);
        uint16_t r = static_cast<uint16_t>((s.rg & 0xff)<<4);
        uint16_t g = static_cast<uint16_t>(((s.rg>>8) & 0xff)<<4);
        // pack into buffer in the correct order
        *dptr++ = static_cast<char>(x&0xff);
        *dptr++ = static_cast<char>((x>>8)&0xff);
        *dptr++ = static_cast<char>(y&0xff);
        *dptr++ = static_cast<char>((y>>8)&0xff);
        *dptr++ = static_cast<char>(r&0xff);
        *dptr++ = static_cast<char>((r>>8)&0xff);
        *dptr++ = static_cast<char>(g&0xff);
        *dptr++ = static_cast<char>((g>>8)&0xff);
        *dptr++ = static_cast<char>(b&0xff);
        *dptr++ = static_cast<char>((b>>8)&0xff);

        sample_count++;
        if (sample_count == m_max_samples_per_udp_pkt){
             m_dgrambuffer[2] = static_cast<char>(m_rnum++);
             m_dgrambuffer[3] = static_cast<char>(m_frame_cnt);

             this->network_send(reinterpret_cast<uint8_t*>(m_dgrambuffer),(sample_count*10)+4);
             dptr = &m_dgrambuffer[4];
             sample_count = 0;
             udp_count++;
        }
    }

     // forward any remaining samples
    if (sample_count>0){
        //qDebug() << "remain samples:" << sample_count;
         m_dgrambuffer[2] = static_cast<char>(m_rnum++);
         m_dgrambuffer[3] = static_cast<char>(m_frame_cnt);
         this->network_send(reinterpret_cast<uint8_t*>(m_dgrambuffer),(sample_count*10)+4);
    }

    m_buffer_free-=count; // update remote buffer free since we know how many samples we just sent
    m_frame_cnt++;
}


LaserdockNetworkDevice::ConnectionType  LaserdockNetworkDevice::get_connection_type()
{
    return m_contype;
}

bool LaserdockNetworkDevice::send_samples(LaserdockSample *samples, uint32_t count)
{

    if (m_queue.size()>constants::thread_sample_buffer_size){
        qDebug() << "thread queue exceeded limit, clearing queue"   ;
        m_queue.clear();
    }

    if (m_disconnected){
        qDebug() << "disconnected";
        return false;
    }

    if (samples==nullptr && count==0){ // not sending samples so must be requesting buffer size due to cutoff
        //qDebug() << "request buffer size";
        emit RequestBufferSize();

    }
    else {

        while(count){
            m_queue.push_back(*samples);
            count--;
            samples++;
        }

        // we do a pre-update of the buffer free, which will be corrected on the next UDP read of the real buffer free
        //if (m_buffer_free>=count) m_buffer_free-=static_cast<uint16_t>(count); else m_buffer_free = 0;

        // we cannot send over UDP here as this function is being called from another thread, so use QT signal to flag
        // there are samples in our thread safe queue to forward to the lasercube.
        emit SamplesReady();
    }

    return true;
}

bool LaserdockNetworkDevice::clear_ringbuffer()
{
    uint8_t clr_pkt[]={cmds::LASERCUBE_CMD_CLEAR_RINGBUFFER};
    send_command(clr_pkt,sizeof(clr_pkt),true);
    return true;
}

// ???? what's this?
bool LaserdockNetworkDevice::runner_mode_enable(bool v)
{
    Q_UNUSED(v)
    return false;
}

// ???? what's this?
bool LaserdockNetworkDevice::runner_mode_run(bool)
{
    return false;
}

// ???? what's this?
bool LaserdockNetworkDevice::runner_mode_load(LaserdockSample *samples, uint16_t position, uint16_t count)
{
    Q_UNUSED(samples)
    Q_UNUSED(position)
    Q_UNUSED(count)
    return false;
}
