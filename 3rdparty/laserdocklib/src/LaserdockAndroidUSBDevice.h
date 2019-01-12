//
// Created by Guoping Huang on 12/27/17.
//

#ifndef LASERDOCKLIB_LASERDOCKANDROIDUSBDEVICE_H
#define LASERDOCKLIB_LASERDOCKANDROIDUSBDEVICE_H

#include <QAndroidJniObject>

#include <laserdocklib/LaserdockDevice.h>

struct LaserdockAndroidUSBDevicePrivate;

class LaserdockAndroidDevice : public LaserdockDevice {

public:
    explicit LaserdockAndroidDevice(QAndroidJniObject usbdevice);

    virtual Status status() const;

    std::string get_device_name() const;

    std::string get_serial_number() const;

    bool enable_output();
    bool disable_output();
    bool get_output(bool * enabled);

    bool dac_rate(uint32_t *rate);
    bool set_dac_rate(uint32_t rate);
    bool max_dac_rate(uint32_t *rate);
    bool min_dac_value(uint32_t *value);
    bool max_dac_value(uint32_t *value);

    bool sample_element_count(uint32_t * /*count*/){ return true; }
    bool iso_packet_sample_count(uint32_t * /*count*/){ return true; }
    bool bulk_packet_sample_count(uint32_t *count);

    bool version_major_number(uint32_t *major);
    bool version_minor_number(uint32_t *minor);

    bool clear_ringbuffer();
    bool ringbuffer_sample_count(uint32_t *count);
    bool ringbuffer_empty_sample_count(uint32_t *count);

    bool runner_mode_enable(bool){ return true; }
    bool runner_mode_run(bool){ return true; }
    bool runner_mode_load(LaserdockSample * /*samples*/, uint16_t /*position*/, uint16_t /*count*/){ return true; }

    bool send(unsigned char * data, uint32_t length);

    bool usb_send(unsigned char *data, int length);
    unsigned char *usb_get(unsigned char * data, int length);

    void print() const { }

private:
    LaserdockAndroidUSBDevicePrivate *d;
};



#endif //LASERDOCKLIB_LASERDOCKANDROIDUSBDEVICE_H
