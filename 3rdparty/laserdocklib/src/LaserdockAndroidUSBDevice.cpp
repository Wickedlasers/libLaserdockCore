//
// Created by Guoping Huang on 12/27/17.
//

#include "LaserdockAndroidUSBDevice.h"

#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>

#include <QtAndroid>
#include <QtCore/qdebug.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/usbdevice_fs.h>
#include <string.h>
#include "hexdump.h"
#include <QThread>

class LaserdockAndroidDevice;


#define USB_DIR_IN 0x80

#include "jni/ldUsbConnection.h"
#include "jni/ldUsbDeviceHelper.h"

//struct usbfs_setinterface {
//    /* keep in sync with usbdevice_fs.h:usbdevfs_setinterface */
//    unsigned int interface;
//    unsigned int altsetting;
//};
//
//struct usbdevfs_bulktransfer {
//    unsigned int ep;
//    unsigned int len;
//    unsigned int timeout; /* in milliseconds */
//    void __user *data;
//};
//
//struct usbdevfs_iso_packet_desc {
//    unsigned int length;
//    unsigned int actual_length;
//    unsigned int status;
//};
//
//struct usbdevfs_urb {
//    unsigned char type;
//    unsigned char endpoint;
//    int status;
//    unsigned int flags;
//    void __user *buffer;
//    int buffer_length;
//    int actual_length;
//    int start_frame;
//    union {
//        int number_of_packets;	/* Only used for isoc urbs */
//        unsigned int stream_id;	/* Only used with bulk streams */
//    };
//    int error_count;
//    unsigned int signr;	/* signal to be sent on completion,
//				  or 0 if none should be sent. */
//    void __user *usercontext;
//    struct usbdevfs_iso_packet_desc iso_frame_desc[0];
//};

//
//#define IOCTL_USBFS_SETINTF _IOR('U', 4, struct usbfs_setinterface)
//#define USBDEVFS_BULK       _IOWR('U', 2, struct usbdevfs_bulktransfer)


struct LaserdockAndroidUSBDevicePrivate {

    LaserdockAndroidDevice * q;
    QAndroidJniObject usbdevice;
    ldUsbConnection cmdConnection;
    ldUsbConnection dataConnection;
    unsigned char security_resp_buffer[64] = {};
    LaserdockDevice::Status status = LaserdockDevice::Status::UNKNOWN;


    LaserdockAndroidUSBDevicePrivate(LaserdockAndroidDevice * qptr, QAndroidJniObject device) : q(qptr), usbdevice (device)
    {
        this->initialize();
    }

    QString deviceName() {
        return usbdevice.callObjectMethod("getDeviceName", "()Ljava/lang/String;").toString();
    }


    void initialize() {

        int res = ldUsbDeviceHelper::getInstance()->setupDevice(usbdevice);
        if(res < 0) {
            qWarning() << "Can't setup device";
            return;
        }

        QAndroidJniObject cmd = ldUsbDeviceHelper::getInstance()->getCmdConnection();
        cmdConnection = ldUsbConnection(cmd);

        QAndroidJniObject data = ldUsbDeviceHelper::getInstance()->getDataConnection();
        dataConnection = ldUsbConnection(data);

        status = LaserdockDevice::Status::INITIALIZED;
    }


    ~LaserdockAndroidUSBDevicePrivate(){}
};



LaserdockAndroidDevice::LaserdockAndroidDevice(QAndroidJniObject device) : LaserdockDevice(), d(new LaserdockAndroidUSBDevicePrivate(this, device)) {

}

LaserdockDevice::Status LaserdockAndroidDevice::status() const
{
    return d->status;
}

std::string LaserdockAndroidDevice::get_device_name() const
{
    return d->deviceName().toStdString();
}

std::string LaserdockAndroidDevice::get_serial_number() const {
    return "unavailable";
}


bool LaserdockAndroidDevice::version_major_number(uint32_t *major){
    d->cmdConnection.prepare();
    d->cmdConnection.buffer[0] = 0x8b;
    if(!d->cmdConnection.execute())
        return false;

    int *v = (int*)(&d->cmdConnection.buffer[2]);
    *major = *v;

    return true;
}

bool LaserdockAndroidDevice::version_minor_number(uint32_t *minor) {
    d->cmdConnection.prepare();
    d->cmdConnection.buffer[0] = 0x8c;
    if(!d->cmdConnection.execute())
        return false;

    int *v = (int*)(&d->cmdConnection.buffer[2]);
    *minor = *v;

    return true;
}

bool LaserdockAndroidDevice::clear_ringbuffer() {
    d->cmdConnection.prepare();
    d->cmdConnection.buffer[0] = 0x8d;
    if(!d->cmdConnection.execute())
        return false;

    return true;
}

bool LaserdockAndroidDevice::bulk_packet_sample_count(uint32_t *count) {
    d->cmdConnection.prepare();
    d->cmdConnection.buffer[0] = 0x8e;
    if(!d->cmdConnection.execute())
        return false;

    int *v = (int*)(&d->cmdConnection.buffer[2]);
    *count = *v;

    return true;
}


bool LaserdockAndroidDevice::max_dac_rate(uint32_t *rate){
    d->cmdConnection.prepare();
    d->cmdConnection.buffer[0] = 0x84;

    if(!d->cmdConnection.execute())
        return false;

    int *v = (int*)(&d->cmdConnection.buffer[2]);
    *rate = *v;

    return true;
}


bool LaserdockAndroidDevice::min_dac_value(uint32_t *value){
    d->cmdConnection.prepare();
    d->cmdConnection.buffer[0] = 0x87;
    if(!d->cmdConnection.execute())
        return false;

    int *v = (int*)(&d->cmdConnection.buffer[2]);
    *value = *v;

    return true;
}

bool LaserdockAndroidDevice::max_dac_value(uint32_t *value){
    d->cmdConnection.prepare();
    d->cmdConnection.buffer[0] = 0x88;
    if(!d->cmdConnection.execute())
         return false;

    int *v = (int*)(&d->cmdConnection.buffer[2]);
    *value = *v;

    return true;
}


bool LaserdockAndroidDevice::ringbuffer_sample_count(uint32_t *count){
    d->cmdConnection.prepare();
    d->cmdConnection.buffer[0] = 0x89;
    if(!d->cmdConnection.execute())
        return false;


    int *v = (int*)(&d->cmdConnection.buffer[2]);
    *count = *v;
    return true;
}

bool LaserdockAndroidDevice::ringbuffer_empty_sample_count(uint32_t *count){
//    *count = 768;
//    return true;
////    qDebug() << "empty ringbuffer : " << *count;
    d->cmdConnection.prepare();
    d->cmdConnection.buffer[0] = 0x8a;
    if(!d->cmdConnection.execute())
        return false;

    int *v = (int*)(&d->cmdConnection.buffer[2]);
    *count = *v;

    return true;
}


bool LaserdockAndroidDevice::enable_output(){
    d->cmdConnection.prepare();
    d->cmdConnection.buffer[0] = 0x80;
    d->cmdConnection.buffer[1] = 0x01;
    return d->cmdConnection.execute();
}

bool LaserdockAndroidDevice::disable_output(){
    d->cmdConnection.prepare();
    d->cmdConnection.buffer[0] = 0x80;
    d->cmdConnection.buffer[1] = 0x00;
    return d->cmdConnection.execute();
}


bool LaserdockAndroidDevice::get_output(bool * enabled){
    d->cmdConnection.prepare();
    d->cmdConnection.buffer[0] = 0x81;
//    d->cmdConnection.dumpbuffer();
    if(!d->cmdConnection.execute())
        return false;
//    d->cmdConnection.dumpbuffer();

    char isEnabled = d->cmdConnection.buffer[2];

    *enabled = (isEnabled == 0x01 ? true : false);

    return true;
}

bool LaserdockAndroidDevice::set_dac_rate(uint32_t rate) {
    d->cmdConnection.prepare();
    d->cmdConnection.buffer[0] = 0x82;
    int * pint = (int*)&d->cmdConnection.buffer[1];
    *pint = rate;
//    d->cmdConnection.dumpbuffer();
    if(!d->cmdConnection.execute())
        return false;
//    d->cmdConnection.dumpbuffer();
    return true;
}

bool LaserdockAndroidDevice::dac_rate(uint32_t *rate) {
    d->cmdConnection.prepare();
    d->cmdConnection.buffer[0] = 0x83;
//    d->cmdConnection.dumpbuffer();
    if(!d->cmdConnection.execute())
        return false;

//    d->cmdConnection.dumpbuffer();
    int *v = (int*)(&d->cmdConnection.buffer[2]);
    *rate = *v;
    return true;
}

bool LaserdockAndroidDevice::send(unsigned char * data, uint32_t length){
    struct usbdevfs_bulktransfer  bulk;

    memset(&bulk, 0, sizeof(bulk));
    bulk.ep = 3;
    bulk.len = length;
    bulk.data = data;
    bulk.timeout = 0;

    return d->dataConnection.doIoctl(&bulk);
}


bool LaserdockAndroidDevice::usb_send(unsigned char *data, int length){
//    log_dumpf("security send: %s\n", data, 0x40, 0x10);

    usbdevfs_bulktransfer bulk;
    memset(&bulk, 0, sizeof(bulk));
    bulk.ep = 1;
    bulk.len = length;
    bulk.data = data;
    bulk.timeout = 0;

    bool r = d->cmdConnection.doIoctl(&bulk);

    return r;
}

unsigned char * LaserdockAndroidDevice::usb_get(unsigned char * data, int length){
//    qDebug() << "security data length =>" << length;
//    log_dumpf("security ress: %s\n", data, 0x40, 0x10);
    struct usbdevfs_bulktransfer  bulk;
    memset(&bulk, 0, sizeof(bulk));
    bulk.ep = 1;
    bulk.len = length;
    bulk.data = data;
    bulk.timeout = 0;
    bool r =  d->cmdConnection.doIoctl(&bulk);

//    qDebug() << "usb_send => " << r;

    memset(&bulk, 0, sizeof(bulk));
    memset(d->security_resp_buffer, 0, 64);
    bulk.ep = 1 | USB_DIR_IN;
    bulk.len = 64;
    bulk.data = d->security_resp_buffer;
    bulk.timeout = 0;

    r = d->cmdConnection.doIoctl(&bulk);
//    log_dumpf("security resp: %s\n", d->security_resp_buffer, 0x40, 0x10);
//    qDebug() << "usb response=> " << r;

    return d->security_resp_buffer;
}
