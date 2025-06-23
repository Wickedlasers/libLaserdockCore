#include "ldUsbDeviceHelper.h"

#include <QtCore/QDebug>


namespace {
    const char* JAVA_CLASS_NAME = "com/wickedlasers/laserdocklib/LdUsbDeviceHelper";
}

ldUsbDeviceHelper *ldUsbDeviceHelper::getInstance()
{
    static ldUsbDeviceHelper instance;
    return &instance;
}

QJniObject ldUsbDeviceHelper::getLaserdockDevices()
{
    return QJniObject::callStaticObjectMethod(JAVA_CLASS_NAME,
                                                                                "getLaserdockDevices",
                                                                                "(Landroid/content/Context;)[Landroid/hardware/usb/UsbDevice;",
                                                                                ldAndroidActivityObject);
}

QJniObject * ldUsbDeviceHelper::openDevice(jobject usbDevice)
{
    QJniEnvironment qjniEnv;

    QJniObject qobj = QJniObject::callStaticObjectMethod(JAVA_CLASS_NAME,
                                                         "openDevice",
                                                         "(Landroid/content/Context;Landroid/hardware/usb/UsbDevice;)Landroid/hardware/usb/UsbDeviceConnection;",
                                                         ldAndroidActivityObject,
                                                         usbDevice
                                                        );




    jobject jo = qjniEnv->NewGlobalRef(qobj.object());

    QJniObject * qaobject = new QJniObject(jo);
//    QJniObject * qaobject = new QJniObject(qobj.object());

    return qaobject;
}

int ldUsbDeviceHelper::setupDevice(const QJniObject &usbdevice)
{
    return QJniObject::callStaticMethod<jint>(JAVA_CLASS_NAME,
                                                     "setupDevice",
                                                     "(Landroid/content/Context;Landroid/hardware/usb/UsbDevice;)I",
                                                     ldAndroidActivityObject,
                                                     usbdevice.object());
}

QJniObject ldUsbDeviceHelper::getCmdConnection() const
{
    return getField("mCmdConnection", "Lcom/wickedlasers/laserdocklib/LaserdockUsbCmdConnection;");
}

QJniObject ldUsbDeviceHelper::getDataConnection() const
{
    return getField("mDataConnection", "Lcom/wickedlasers/laserdocklib/LaserdockUsbDataConnection;");
}

QJniObject ldUsbDeviceHelper::getField(const std::string &fieldName, const std::string &fieldType) const
{
    return QJniObject::getStaticObjectField(JAVA_CLASS_NAME,
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
    QJniObject * qaobj;

    JavaUsbConnectionPrivate(JavaUsbConnection * qptr,  QJniObject * o) : q(qptr), qaobj(o) {

    }

};

JavaUsbConnection::JavaUsbConnection(QJniObject * o) : d (new JavaUsbConnectionPrivate(this, o))
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


//    QJniObject qobj = QJniObject::callStaticObjectMethod(USB_DEVICE_HELPER_CLASS_NAME,
//                                                                       "runBulkTransfer",
//                                                                       "(Landroid/hardware/usb/UsbDeviceConnection;)Landroid/hardware/usb/UsbDeviceConnection;",
//                                                                       QtAndroidPrivate::activity(),
//                                                                       usbDevice
//    );
//

    return d->qaobj->callMethod<jint>("getFileDescriptor");

//    d->qaobj->callMethod<jint>("runBulkTransfer", "(Landroid/hardware/usb/UsbDeviceConnection;I;)I;", d->qaobj->object(), 0);
    return true;
}
