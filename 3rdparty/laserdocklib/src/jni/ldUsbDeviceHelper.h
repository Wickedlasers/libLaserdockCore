//
// Created by Guoping Huang on 8/8/16.
//

#ifndef LASERDOCKLIB_LDUSBDEVICEHELPER_H
#define LASERDOCKLIB_LDUSBDEVICEHELPER_H

#include <memory>

#include <jni.h>
#include <string>

class QAndroidJniObject;

class ldUsbDeviceHelper {

public:
    static ldUsbDeviceHelper *getInstance();

    QAndroidJniObject getLaserdockDevices();
    QAndroidJniObject *openDevice(jobject usbDevice);

    int setupDevice(const QAndroidJniObject &usbdevice);

    QAndroidJniObject getCmdConnection() const;
    QAndroidJniObject getDataConnection() const;

private:
    QAndroidJniObject getField(const std::string &fieldName, const std::string &fieldType) const;

    explicit ldUsbDeviceHelper();
    virtual ~ldUsbDeviceHelper();
};


struct JavaUsbConnectionPrivate;

class JavaUsbConnection {
public :
    explicit JavaUsbConnection();
    explicit JavaUsbConnection(QAndroidJniObject *);
    ~JavaUsbConnection();

    int getfd() const;

    bool bulk_transfer();

private:
    JavaUsbConnectionPrivate * d;
};


#endif //LASERDOCKLIB_LDUSBDEVICEHELPER_H
