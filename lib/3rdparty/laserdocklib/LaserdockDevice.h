//
// Created by Guoping Huang on 8/8/16.
//

#ifndef LASERDOCKLIB_LASERDOCKDEVICE_H
#define LASERDOCKLIB_LASERDOCKDEVICE_H

#include <memory>
#include <string>

#ifdef _WIN32
#define LASERDOCKLIB_EXPORT __declspec(dllexport)
#else
#define LASERDOCKLIB_EXPORT
#endif

uint16_t LASERDOCKLIB_EXPORT float_to_laserdock_xy(float var);
uint16_t LASERDOCKLIB_EXPORT laserdock_sample_flip(uint16_t);

struct LaserdockSample
{
    uint16_t rg;      //lower byte is red, top byte is green
    uint16_t b;       //lower byte is blue
    uint16_t x;
    uint16_t y;
};



#ifdef ANDROID

class _jobject;
typedef _jobject* jobject;
#endif

struct libusb_device;
class LaserdockDevicePrivate;

class LASERDOCKLIB_EXPORT LaserdockDevice {

public:

    enum Status { UNKNOWN, INITIALIZED };

    explicit LaserdockDevice(libusb_device *usbdevice);
    explicit LaserdockDevice();

    virtual ~LaserdockDevice();

    void initialize();

    virtual Status status() const;

    virtual std::string get_serial_number() const;
    virtual int8_t get_bus_number() const;
    virtual int8_t get_device_address() const;
    virtual std::string get_device_name() const;

    virtual bool enable_output();
    virtual bool disable_output();
    virtual bool get_output(bool * enabled);

    virtual bool dac_rate(uint32_t *rate);
    virtual bool set_dac_rate(uint32_t rate);
    virtual bool max_dac_rate(uint32_t *rate);
    virtual bool min_dac_value(uint32_t *value);
    virtual bool max_dac_value(uint32_t *value);

    virtual bool sample_element_count(uint32_t *count);
    virtual bool iso_packet_sample_count(uint32_t *count);
    virtual bool bulk_packet_sample_count(uint32_t *count);

    virtual bool version_major_number(uint32_t *major);
    virtual bool version_minor_number(uint32_t *minor);

    virtual bool clear_ringbuffer();
    virtual bool ringbuffer_sample_count(uint32_t *count);
    virtual bool ringbuffer_empty_sample_count(uint32_t *count);

    virtual bool runner_mode_enable(bool);
    virtual bool runner_mode_run(bool);
    virtual bool runner_mode_load(LaserdockSample *samples, uint16_t position, uint16_t count);

    virtual bool send(unsigned char * data, uint32_t length);
    virtual bool send_samples(LaserdockSample * samples, uint32_t count);

    virtual bool usb_send(unsigned char *data, int length);
    virtual unsigned char *usb_get(unsigned char * data, int length);

    virtual void print() const;

private:
    std::unique_ptr<LaserdockDevicePrivate> d;
};

#endif //LASERDOCKLIB_LASERDOCKDEVICE_H
