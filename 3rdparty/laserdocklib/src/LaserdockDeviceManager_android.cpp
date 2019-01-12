#include <laserdocklib/LaserdockDeviceManager.h>

#include <cstdio>
#include <vector>

#include <jni.h>

#include <QtDebug>
#include <QtAndroid>
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>

#include <laserdocklib/LaserdockDevice.h>

#include "LaserdockAndroidUSBDevice.h"
#include "LaserdockDeviceManager_p.h"
#include "jni/ldUsbDeviceHelper.h"

std::vector<std::unique_ptr<LaserdockDevice> > LaserdockDeviceManagerPrivate::get_devices(const std::vector<LaserdockDevice*> &excludedDevices ) {
    std::vector<std::unique_ptr<LaserdockDevice>> laserdockDevices;
    if(excludedDevices.size()> 0) return laserdockDevices;

//    qDebug() << "getting usb devices...";


    // get laserdock devices
    QAndroidJniObject usbDevicesJni = ldUsbDeviceHelper::getInstance()->getLaserdockDevices();
    jobjectArray objectArray = usbDevicesJni.object<jobjectArray>();

    QAndroidJniEnvironment qjniEnv;
    int length = qjniEnv->GetArrayLength(objectArray);
    for(int i = 0; i < length; i++) {
        jobject jobj = qjniEnv->GetObjectArrayElement(objectArray, i);
        // get device name
        QAndroidJniObject qObj(jobj);
        QAndroidJniObject jDeviceName = qObj.callObjectMethod("getDeviceName", "()Ljava/lang/String;");
        std::string devicename = jDeviceName.toString().toStdString();

//        qDebug() << QString::fromStdString(devicename);

        // exclude devices
        auto it = std::find_if(excludedDevices.begin(), excludedDevices.end(), [&](LaserdockDevice *excludedDevice) {
//            qDebug() << excludedDevice;
//            qDebug() << "excluded device name: " << QString::fromStdString(excludedDevice->get_device_name());
            return excludedDevice->get_device_name() == devicename;
        });

        if(it != excludedDevices.end()) {
            continue;
        }


        std::unique_ptr<LaserdockDevice> d(new LaserdockAndroidDevice(qObj));
        if(d->status() == LaserdockDevice::Status::INITIALIZED) {
            qDebug() << QString::fromStdString(d->get_device_name());
            laserdockDevices.push_back(std::move(d));
        }

    }
    return laserdockDevices;
}


//        auto device = LaserdockAndroidDevice(device);
//qDebug() << device.deviceName();
