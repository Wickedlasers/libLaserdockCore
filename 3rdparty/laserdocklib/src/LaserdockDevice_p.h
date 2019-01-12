//
// Created by Guoping Huang on 8/8/16.
//

#ifndef LASERDOCKLIB_LASERDOCKDEVICEPRIVATE_H
#define LASERDOCKLIB_LASERDOCKDEVICEPRIVATE_H

#include <laserdocklib/LaserdockDevice.h>

#ifdef ANDROID
class _jobject;
typedef _jobject* jobject;
#endif

struct libusb_device;

class LaserdockDevice;

class LaserdockDevicePrivate {
public:
    libusb_device * usbdevice;
    struct libusb_device_handle *devh_ctl = nullptr;
    struct libusb_device_handle *devh_data = nullptr;
    int8_t bus_number = -1;
    int8_t device_address = -1;
    LaserdockDevice::Status status = LaserdockDevice::Status::UNKNOWN;

#ifdef ANDROID
    jobject m_jobject;
    LaserdockDevicePrivate(libusb_device * device, jobject jobj, LaserdockDevice * q_ptr);
#endif

    LaserdockDevicePrivate(libusb_device * device, LaserdockDevice * q_ptr);
    virtual ~LaserdockDevicePrivate();

    void initialize();
    void release();
    void print() const;

    std::string get_serial_number();
private:
    LaserdockDevice * q;
};



#endif //LASERDOCKLIB_LASERDOCKDEVICE_H
