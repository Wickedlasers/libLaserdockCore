#include "ldUsbConnection.h"

#include <QtCore/QtDebug>
#include <QtAndroidExtras/QAndroidJniEnvironment>

#include <sys/ioctl.h>
#include <linux/usbdevice_fs.h>

#include "hexdump.h"

ldUsbConnection::ldUsbConnection()
{
}

ldUsbConnection::ldUsbConnection(QAndroidJniObject o)
    : m_connection(o)
{
    QAndroidJniObject bufferObject = o.getObjectField("mbuffer", "Ljava/nio/ByteBuffer;");
    //            qDebug() << "bufferObject valid? " << bufferObject.isValid();
    QAndroidJniEnvironment env;
    buffer = (char *)env->GetDirectBufferAddress(bufferObject.object());
    m_fd = m_connection.callMethod<jint>("fd");
    qDebug() << "fd = " << m_fd;
}

void ldUsbConnection::dumpbuffer(){
    log_dumpf("buffer: %s\n", buffer, 0x40, 0x10);
}

void ldUsbConnection::prepare(){
    memset(buffer, 0, 64);
}

bool ldUsbConnection::execute() {
    return m_connection.callMethod<jboolean>("send");
}

bool ldUsbConnection::doIoctl(const usbdevfs_bulktransfer *bulk) const
{
    return ioctl(m_fd, USBDEVFS_BULK, bulk);
}
