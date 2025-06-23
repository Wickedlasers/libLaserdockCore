//
// Created by Guoping Huang on 8/8/16.
//

#ifndef LASERDOCKLIB_LDUSBDEVICEHELPER_H
#define LASERDOCKLIB_LDUSBDEVICEHELPER_H

#include <memory>

#include <jni.h>
#include <string>

#include <laserdocklib/ldAndroidGlobals.h>

class ldUsbDeviceHelper {

public:
    static ldUsbDeviceHelper *getInstance();

    QJniObject getLaserdockDevices();
    QJniObject *openDevice(jobject usbDevice);

    int setupDevice(const QJniObject &usbdevice);

    QJniObject getCmdConnection() const;
    QJniObject getDataConnection() const;

private:
    QJniObject getField(const std::string &fieldName, const std::string &fieldType) const;

    explicit ldUsbDeviceHelper();
    virtual ~ldUsbDeviceHelper();
};


struct JavaUsbConnectionPrivate;

class JavaUsbConnection {
public :
    explicit JavaUsbConnection();
    explicit JavaUsbConnection(QJniObject *);
    ~JavaUsbConnection();

    int getfd() const;

    bool bulk_transfer();

private:
    JavaUsbConnectionPrivate * d;
};


#endif //LASERDOCKLIB_LDUSBDEVICEHELPER_H
