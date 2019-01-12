#include <laserdocklib/LaserdockDeviceManager.h>

#include <algorithm>

#include <libusb/libusb.h>

#include <laserdocklib/LaserdockDevice.h>

#include "LaserdockDeviceManager_p.h"

std::vector<std::unique_ptr<LaserdockDevice> > LaserdockDeviceManagerPrivate::get_devices(const std::vector<LaserdockDevice *> &excludedDevices) {
    std::vector<std::unique_ptr<LaserdockDevice>> laserdockDevices;

    libusb_device **libusb_device_list;
    ssize_t cnt = libusb_get_device_list(NULL, &libusb_device_list);
    ssize_t i = 0;

    if (cnt < 0) {
        fprintf(stderr, "Error finding USB device\n");
        libusb_free_device_list(libusb_device_list, 1); // probably not necessary
        return laserdockDevices;
    }

    for (i = 0; i < cnt; i++) {
        libusb_device *libusb_device = libusb_device_list[i];
        if (is_laserdock(libusb_device)) {
            std::unique_ptr<LaserdockDevice> d(new LaserdockDevice(libusb_device));

            // exclude devices
            auto it = std::find_if(excludedDevices.begin(), excludedDevices.end(), [&](LaserdockDevice *excludedDevice) {
                    return excludedDevice->get_bus_number() == d->get_bus_number()
                    && excludedDevice->get_device_address() == d->get_device_address();
            });
            if(it != excludedDevices.end()) {
                continue;
            }

            // on Windows libusb return laserdock device 2 times at the first run
            // one of handles is invalid
            d->initialize();
            if(d->status() == LaserdockDevice::Status::INITIALIZED)
                laserdockDevices.push_back(std::move(d));
        }
    }

    libusb_free_device_list(libusb_device_list, cnt);
    return laserdockDevices;
}
