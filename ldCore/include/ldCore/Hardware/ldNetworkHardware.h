#ifndef LDNETWORKHARDWARE_H
#define LDNETWORKHARDWARE_H

#include "ldHardware.h"

class LaserdockNetworkDevice;
struct LaserdockSample;
class ldNetworkHardwarePrivate;

class LDCORESHARED_EXPORT ldNetworkHardware : public ldHardware {

    Q_OBJECT

public:
    struct device_params {

        uint32_t ilda_rate = 30000;


        LaserdockNetworkDevice *device = nullptr;
    };

    ldNetworkHardware(LaserdockNetworkDevice *device, QObject *parent = nullptr);
    ~ldNetworkHardware();

    virtual QString id() const override;

    void send_security_request(QByteArray request);
    void get_security_response(QByteArray &response);

    const device_params &params() const;

    bool send_samples(uint startIndex, uint count);
    bool send_samples(LaserdockSample *samples, unsigned int size);

    int  get_full_count();
    void initialize();
    void ResetStatus();

    void setActive(bool active);


signals:
    void deviceDisconnected();

private:
   QScopedPointer<ldNetworkHardwarePrivate> d_ptr;
   Q_DECLARE_PRIVATE(ldNetworkHardware)
};

#endif // LDNETWORKHARDWARE_H
