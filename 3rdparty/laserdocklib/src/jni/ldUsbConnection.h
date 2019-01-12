#ifndef LASERDOCKLIB_LDUSBCONNECTION
#define LASERDOCKLIB_LDUSBCONNECTION

#include <QAndroidJniObject>

struct usbdevfs_bulktransfer;

class ldUsbConnection
{
public:
    ldUsbConnection();
    ldUsbConnection(QAndroidJniObject o);

    void dumpbuffer();
    void prepare();
    bool execute();

    bool doIoctl(const usbdevfs_bulktransfer *bulk) const;

    char *buffer = nullptr;

private:
    int m_fd = -1;
    QAndroidJniObject m_connection;
};

#endif //LASERDOCKLIB_LDUSBCONNECTION
