#include <linux/ioctl.h>
#include <sys/ioctl.h>
//
//// Struct and ioctl define stolen from linux_usbfs.h
//struct usbfs_setinterface {
//    /* keep in sync with usbdevice_fs.h:usbdevfs_setinterface */
//    unsigned int interface;
//    unsigned int altsetting;
//};
//
//#define IOCTL_USBFS_SETINTF _IOR('U', 4, struct usbfs_setinterface)
//
//// Basically the same as linux_usbfs.c
//int fd = 1;
//struct usbfs_setinterface setintf;
//
//setintf.interface = 1;
//setintf.altsetting = 1;
//
//int r = ioctl(fd, IOCTL_USBFS_SETINTF, &setintf);