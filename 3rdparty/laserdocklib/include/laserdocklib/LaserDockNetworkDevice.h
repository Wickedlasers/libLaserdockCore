#ifndef LASERDOCKNETWORKDEVICE_H
#define LASERDOCKNETWORKDEVICE_H

#include <QTimer>
#include <QObject>
#include <memory>
#include <string>
#include <QString>
#include <QUdpSocket>
#include <QElapsedTimer>
#include <laserdocklib/ldSharedQueue.h>
#include <laserdocklib/LaserdockSample.h>

#ifdef ANDROID
class _jobject;
typedef _jobject* jobject;
#endif

class LASERDOCKLIB_EXPORT LaserdockNetworkDevice : public QObject
{
    Q_OBJECT
public:

    enum Status { UNKNOWN,      // unknown state (cube not ready)
                  AUTHENTICATING, // cube has been found, full info received, but we are still authenticating this device
                  INITIALIZED,   // cube has been initialized and authenticated (all info has been read back and defaults have been sent)
                };

    enum ConnectionType {CON_UNKNOWN = 0,           // unknown connection type (not read from cube yet)
                         CON_ETHERNET_SERVER,   // ethernet connection with the cube as the dhcp server
                         CON_WIFI_SERVER,       // wifi connection with the cube as the dhcp server (soft AP)
                         CON_ETHERNET_CLIENT,   // ethernet connection with cube as a client (cube gets assigned an ip address from host)
                         CON_WIFI_CLIENT,        // Wifi connection with cube as a client (cube gets assigned an ip address from host)
                        };

    // static helper functions for the network manager class
    // These allow manager class to broadcast alive messages and
    // check for valid responses from wifi cubes on the network
    static bool RequestDeviceAlive(QUdpSocket& skt);
    static bool DeviceAliveResponseValid(const QByteArray& response_data);
    static bool ConfigDeviceAliveRequestSocket(QUdpSocket& skt);



    void ResetStatus();

    explicit LaserdockNetworkDevice(QString ip_address,QObject *parent = nullptr);
    explicit LaserdockNetworkDevice();

    virtual ~LaserdockNetworkDevice();

    void initialize();

    // legacy functions

    virtual Status status() const;
    virtual std::string get_serial_number() const;
    virtual int8_t get_bus_number() const;
    virtual int8_t get_device_address() const;
    virtual std::string get_device_name() const;
    virtual QString get_ip_address() const;

    virtual bool get_disconnected();
    virtual bool enable_buffer_state_replies();
    virtual bool disable_buffer_state_replies();
    virtual bool enable_output();
    virtual bool disable_output();
    virtual bool get_output(bool * enabled);

    virtual bool dac_rate(uint32_t *rate);
    virtual bool set_dac_rate(uint32_t rate);
    virtual bool max_dac_rate(uint32_t *rate);
    virtual bool min_dac_value(uint32_t *value);
    virtual bool max_dac_value(uint32_t *value);
    virtual bool temperature_degc(int8_t *value);
    virtual bool battery_pct(uint8_t *value);

    virtual bool sample_element_count(uint32_t *count);
    virtual bool iso_packet_sample_count(uint32_t *count);
    virtual bool bulk_packet_sample_count(uint32_t *count);

    virtual bool version_major_number(uint32_t *major);
    virtual bool version_minor_number(uint32_t *minor);

    virtual bool clear_ringbuffer();
    virtual bool ringbuffer_sample_count(uint32_t *count);
    virtual bool ringbuffer_empty_sample_count(uint32_t *count);

    virtual ConnectionType get_connection_type();

    virtual bool runner_mode_enable(bool);
    virtual bool runner_mode_run(bool);
    virtual bool runner_mode_load(LaserdockSample *samples, uint16_t position, uint16_t count);

    virtual bool send(unsigned char * data, uint32_t length);
    virtual bool send_samples(LaserdockSample * samples, uint32_t count);

    virtual bool network_send(unsigned char *data, uint length);
    virtual unsigned char *network_get(unsigned char * data, int length);

    virtual void print() const;

    virtual void set_max_udp_samples_per_xfer(uint samples);
    virtual void set_max_udp_packets_per_xfer(uint packets);


public:
    signals:

    // sent out when we lose connection with this device
    void DeviceDisconnected();
    // sent out when cube has been initialized and authenticated OK
    void DeviceReady();
    // sent out when device is ready to be authenticated
    void DeviceNeedsAuthenticating();
    // sent when there is a security request response available
    void SecurityResponseReceived(bool success,QByteArray response_data);

    // new signal events triggered when data changes from this lasercube device
    void DACRateUpdated(uint32_t dacrate);
    void SampleBufferFreeUpdated(uint16_t buffer_free_samples);
    void SampleBufferSizeUpdated(uint16_t buffer_size_samples);
    void BatteryPercentUpdated(uint8_t battery_percent);
    void TemperatureUpdated(int8_t temperature_degc);
    void ConnectionTypeUpdated(ConnectionType connection_type);
    void SerialNumberUpdated(QString serial_num_str);
    void ModelNameUpdated(QString model_name_str);
    void ModelNumberUpdated(uint8_t model_num);
    void UniqueIdUpdated(QString unique_id_str);
    void IPAddressUpdated(QString ip_addr_str);

public slots:
    // perform a security request to this device
    void SecurityRequest(QByteArray request);
    // set this device as authenticated or not.
    void DeviceAuthenticated(bool state);

    // set the Non-Volatile device model info (used at factory only to program model type, number, and region)
    // note: model_str can only be max 24 characters long
    void SetModelInfo(uint8_t region,uint8_t model_num,QString model_str);

private:
    static const uint max_buffered_samples = 140;
    static const uint uncompressed_sample_size_bytes = 10;
    static const uint header_size_bytes = 4;

    std::atomic<uint16_t>               m_buffer_free{0};
    std::atomic<uint16_t>               m_buffer_size{0};
    std::atomic<bool>                   m_disconnected{false};

    char m_dgrambuffer[(max_buffered_samples*uncompressed_sample_size_bytes)+header_size_bytes];

    bool                                m_authenticated{false};
    ldSharedQueue<LaserdockSample>      m_queue;
    QTimer                              m_timer,m_nocommstimer,m_security_timer;
    QByteArray                          m_security_req_data;
    QUdpSocket*                         m_cmdsocket{nullptr};
    QUdpSocket*                         m_datasocket{nullptr};
    QHostAddress                        m_hostaddr;
    QElapsedTimer                      m_elapsed_timer;
    uint                                m_max_samples_per_udp_pkt{80};
    uint                                m_max_udp_packets{20};
    Status                              m_status{UNKNOWN};
    uint8_t                             m_rnum{0};
    uint8_t                             m_frame_cnt{0};

     // network lasercube configuration
    std::string                         m_serialnumber{""};
    QString                             m_ip_address{""};
    QString                             m_model_name{""};
    uint8_t                             m_model_number{0};
    uint8_t                             m_model_region{0};
    bool                                m_outputenabled{false};
    uint32_t                            m_dac_rate{0};
    uint32_t                            m_max_dac_rate{0};
    uint8_t                             m_fw_major{0};
    uint8_t                             m_fw_minor{0};
    uint16_t                            m_dac_range_min{0};
    uint16_t                            m_dac_range_max{0};
    uint16_t                            m_min_dac_value{0};
    uint16_t                            m_max_dac_value{0};
    uint8_t                             m_battery_pct{0};
    int8_t                              m_temperature_degc{0};

    ConnectionType                      m_contype{CON_UNKNOWN};

    void send_command(uint8_t *cmd_data,size_t cmd_length_bytes, bool repeat_enable);
    void processCmdDatagram(QNetworkDatagram &datagram);
    void handleFullInfoPkt(const QByteArray &data );

private :
    signals:

    void SamplesReady();
    void RequestBufferSize();

    private slots:

    void processSamples();
    void readPendingDataResponses();
    void readPendingCommandResponses();
    void process();

};

#endif // LASERDOCKNETWORKDEVICE_H
