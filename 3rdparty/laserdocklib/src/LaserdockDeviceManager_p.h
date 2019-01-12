//
// Created by Guoping Huang on 9/6/16.
//

#ifndef LASERDOCKLIB_LASERDOCKDEVICEMANAGERPRIVATE_H
#define LASERDOCKLIB_LASERDOCKDEVICEMANAGERPRIVATE_H

#include <memory>
#include <vector>

class LaserdockDeviceManager;

struct libusb_device;
struct libusb_context;

class LaserdockDevice;

class LaserdockDeviceManagerPrivate {

public:
    LaserdockDeviceManagerPrivate(LaserdockDeviceManager *q_ptr);

    bool initialize_usb();
    bool is_laserdock(libusb_device * device) const;

    std::vector<std::unique_ptr<LaserdockDevice>> get_devices(const std::vector<LaserdockDevice*> &excludedDevices = std::vector<LaserdockDevice*>());

private:
    LaserdockDeviceManager * q;
    libusb_context *m_libusb_ctx;
};


#endif //LASERDOCKLIB_LASERDOCKDEVICEMANAGER_H
