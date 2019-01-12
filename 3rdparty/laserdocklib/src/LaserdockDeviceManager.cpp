#include <laserdocklib/LaserdockDeviceManager.h>

#include <cstdio>
#include <vector>

#include <QtCore/QtDebug>

#include <libusb/libusb.h>

#include <laserdocklib/LaserdockDevice.h>

#include "LaserdockDeviceManager_p.h"



#define LASERDOCK_VIN 0x1fc9
#define LASERDOCK_PIN 0x04d8


/// ---------------------------- LaserdockDeviceManager ----------------------------

LaserdockDeviceManager &LaserdockDeviceManager::getInstance()
{
    static LaserdockDeviceManager instance;
    return instance;
}

std::vector<std::unique_ptr<LaserdockDevice> > LaserdockDeviceManager::get_laserdock_devices(const std::vector<LaserdockDevice *> &excludedDevices)
{
    return d->get_devices(excludedDevices);
}

void LaserdockDeviceManager::print_laserdock_devices() {
    std::vector<std::unique_ptr<LaserdockDevice> > devices = get_laserdock_devices();
    for(const std::unique_ptr<LaserdockDevice> &device : devices) {
        device->print();
    }
}

LaserdockDevice *LaserdockDeviceManager::get_next_available_device() {
    std::vector<std::unique_ptr<LaserdockDevice> > devices = get_laserdock_devices();
    if(devices.size() > 0) {
        return devices[0].release();
    } else {
        return NULL;
    }
}

LaserdockDeviceManager::LaserdockDeviceManager()
    : d(new LaserdockDeviceManagerPrivate(this)) {

}

LaserdockDeviceManager::~LaserdockDeviceManager() {
}

/// ---------------------------- LaserdockDeviceManagerPrivate ----------------------------

LaserdockDeviceManagerPrivate::LaserdockDeviceManagerPrivate(LaserdockDeviceManager *q_ptr) : q(q_ptr) {
#ifndef Q_OS_ANDROID
    // android driver works in different way, silent warning
    this->initialize_usb();
#endif
}

bool LaserdockDeviceManagerPrivate::initialize_usb() {
    int rc;
    rc = libusb_init(&m_libusb_ctx);
    if (rc < 0)
    {
        qDebug() << "Error initializing libusb error code:" << rc;
//        fprintf(stderr, "Error initializing libusb: %d\n", /*libusb_error_name(rc)*/rc);
        return false;
    }

    return true;
}

bool LaserdockDeviceManagerPrivate::is_laserdock(libusb_device *device) const {
    struct libusb_device_descriptor device_descriptor;
    int result = libusb_get_device_descriptor(device, &device_descriptor);
    if (result < 0) {
        printf("Failed to get device descriptor!");
    }

    if (LASERDOCK_VIN == device_descriptor.idVendor && LASERDOCK_PIN == device_descriptor.idProduct)
        return true;

    return false;
}
