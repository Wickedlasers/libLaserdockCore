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
    virtual QString hwType() const override;
    virtual QString address() const override;

    virtual int getDacRate() const override;
    virtual int getMaximumDacRate() const override;
    virtual void setDacRate(int rate) const override;
    virtual void setDacBufferTHold(int level) const override;

    void send_security_request(QByteArray request);
    void get_security_response(QByteArray &response);

    const device_params &params() const;

    virtual bool send_samples(uint startIndex, uint count) override;
    bool send_samples(LaserdockSample *samples, unsigned int size);

    virtual int get_full_count() override;
    void initialize();
    void ResetStatus();

    virtual void setActive(bool active) override;


signals:
    void deviceDisconnected();

private:
   QScopedPointer<ldNetworkHardwarePrivate> d_ptr;
   Q_DECLARE_PRIVATE(ldNetworkHardware)
};

#endif // LDNETWORKHARDWARE_H
