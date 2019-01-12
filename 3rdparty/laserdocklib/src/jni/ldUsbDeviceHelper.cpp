#include "ldUsbDeviceHelper.h"

#include <QtAndroid>
#include <QDebug>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>

namespace {
    const char* JAVA_CLASS_NAME = "com/wickedlasers/laserdocklib/LdUsbDeviceHelper";
}

ldUsbDeviceHelper *ldUsbDeviceHelper::getInstance()
{
    static ldUsbDeviceHelper instance;
    return &instance;
}

QAndroidJniObject ldUsbDeviceHelper::getLaserdockDevices()
{
    return QAndroidJniObject::callStaticObjectMethod(JAVA_CLASS_NAME,
                                                                                "getLaserdockDevices",
                                                                                "(Landroid/content/Context;)[Landroid/hardware/usb/UsbDevice;",
                                                                                QtAndroid::androidActivity().object());
}

QAndroidJniObject * ldUsbDeviceHelper::openDevice(jobject usbDevice)
{
    QAndroidJniEnvironment qjniEnv;

    QAndroidJniObject qobj = QAndroidJniObject::callStaticObjectMethod(JAVA_CLASS_NAME,
                                                         "openDevice",
                                                         "(Landroid/content/Context;Landroid/hardware/usb/UsbDevice;)Landroid/hardware/usb/UsbDeviceConnection;",
                                                         QtAndroid::androidActivity().object(),
                                                         usbDevice
                                                        );




    jobject jo = qjniEnv->NewGlobalRef(qobj.object());

    QAndroidJniObject * qaobject = new QAndroidJniObject(jo);
//    QAndroidJniObject * qaobject = new QAndroidJniObject(qobj.object());

    return qaobject;
}

int ldUsbDeviceHelper::setupDevice(const QAndroidJniObject &usbdevice)
{
    return QAndroidJniObject::callStaticMethod<jint>(JAVA_CLASS_NAME,
                                                     "setupDevice",
                                                     "(Landroid/content/Context;Landroid/hardware/usb/UsbDevice;)I",
                                                     QtAndroid::androidActivity().object(),
                                                     usbdevice.object());
}

QAndroidJniObject ldUsbDeviceHelper::getCmdConnection() const
{
    return getField("mCmdConnection", "Lcom/wickedlasers/laserdocklib/LaserdockUsbCmdConnection;");
}

QAndroidJniObject ldUsbDeviceHelper::getDataConnection() const
{
    return getField("mDataConnection", "Lcom/wickedlasers/laserdocklib/LaserdockUsbDataConnection;");
}

QAndroidJniObject ldUsbDeviceHelper::getField(const std::string &fieldName, const std::string &fieldType) const
{
    return QAndroidJniObject::getStaticObjectField(JAVA_CLASS_NAME,
                                                   fieldName.c_str(),
                                                   fieldType.c_str());
}


ldUsbDeviceHelper::ldUsbDeviceHelper()
{
}

ldUsbDeviceHelper::~ldUsbDeviceHelper()
{
}


struct JavaUsbConnectionPrivate {
    JavaUsbConnection * q;
    QAndroidJniObject * qaobj;

    JavaUsbConnectionPrivate(JavaUsbConnection * qptr,  QAndroidJniObject * o) : q(qptr), qaobj(o) {

    }

};

JavaUsbConnection::JavaUsbConnection(QAndroidJniObject * o) : d (new JavaUsbConnectionPrivate(this, o))
{

}

JavaUsbConnection::JavaUsbConnection() {

}


JavaUsbConnection::~JavaUsbConnection(){
    delete d;
}


int JavaUsbConnection::getfd() const {
    return d->qaobj->callMethod<jint>("getFileDescriptor");
}

bool JavaUsbConnection::bulk_transfer() {


//    QAndroidJniObject qobj = QAndroidJniObject::callStaticObjectMethod(USB_DEVICE_HELPER_CLASS_NAME,
//                                                                       "runBulkTransfer",
//                                                                       "(Landroid/hardware/usb/UsbDeviceConnection;)Landroid/hardware/usb/UsbDeviceConnection;",
//                                                                       QtAndroid::androidActivity().object(),
//                                                                       usbDevice
//    );
//

    return d->qaobj->callMethod<jint>("getFileDescriptor");

//    d->qaobj->callMethod<jint>("runBulkTransfer", "(Landroid/hardware/usb/UsbDeviceConnection;I;)I;", d->qaobj->object(), 0);
    return true;
}
