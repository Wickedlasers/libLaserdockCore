#ifndef LASERDOCKLIB_LDUSBCONNECTION
#define LASERDOCKLIB_LDUSBCONNECTION

#include <laserdocklib/ldAndroidGlobals.h>

struct usbdevfs_bulktransfer;

class ldUsbConnection
{
public:
    ldUsbConnection();
    ldUsbConnection(QJniObject o);

    void dumpbuffer();
    void prepare();
    bool execute();

    bool doIoctl(const usbdevfs_bulktransfer *bulk) const;

    char *buffer = nullptr;

private:
    int m_fd = -1;
    QJniObject m_connection;
};

#endif //LASERDOCKLIB_LDUSBCONNECTION
