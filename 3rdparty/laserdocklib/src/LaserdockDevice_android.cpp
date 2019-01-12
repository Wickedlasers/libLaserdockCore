#include "LaserdockDevice.h"
#include "LaserdockDevice_p.h"

#include <QtDebug>

#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "libusb/libusb.h"

#include "JavaUsbDeviceHelper.h"

// Struct and ioctl define stolen from linux_usbfs.h
struct usbfs_setinterface {
  /* keep in sync with usbdevice_fs.h:usbdevfs_setinterface */
  unsigned int interface;
  unsigned int altsetting;
};
#define IOCTL_USBFS_SETINTF _IOR('U', 4, struct usbfs_setinterface)

//LaserdockDevice::LaserdockDevice(libusb_device * usbdevice, jobject obj) : d(new LaserdockDevicePrivate(usbdevice, obj, this))
//{
//    d->initialize();
//}

void LaserdockDevicePrivate::initialize() {
    int r = 0;

    // open device with UsbDevice from Java code
    jint fd0 = JavaUsbDeviceHelper::getInstance()->openDevice(m_jobject);
    if(fd0 == -1) {
        qWarning() << "fd0 invalid";
        return;
    }

    // create libusb mapping
//    r = libusb_open2(this->usbdevice, &this->devh_ctl, fd0);
//    if (r != 0)  {
//        qWarning() << "fd0 libusb_open2 failed";
//        return;
//    }

    // open device with UsbDevice from Java code
    jint fd1 = JavaUsbDeviceHelper::getInstance()->openDevice(m_jobject);
    if(fd1 == -1) {
        qWarning() << "fd1 invalid";
        return;
    }

    // create libusb mapping
//    r = libusb_open2(this->usbdevice, &this->devh_data, fd1);
//    if (r != 0)  {
//        qWarning() << "fd1 libusb_open2 failed";
//        return;
//    }

    // trick to set alt interface without java call UsbDevice.setInterface (API 21+ restriction)
    struct usbfs_setinterface setintf;
    setintf.interface = 1;
    setintf.altsetting = 1;
    r = ioctl(fd1, IOCTL_USBFS_SETINTF, &setintf);
    if (r == -1) {
        qWarning() << "fd1 ioctl failed" << errno;
        return;
    }

    qDebug() << "Open device" << fd0 << fd1;

    this->status = LaserdockDevice::Status::INITIALIZED;
}

