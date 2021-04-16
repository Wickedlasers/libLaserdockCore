#ifndef LASERDOCKLIB_LASERDOCKLIB_H
#define LASERDOCKLIB_LASERDOCKLIB_H

#include <QtCore/qglobal.h>

#ifdef Q_OS_WIN
#if defined(LASERDOCKLIB_LIBRARY)
#define LASERDOCKLIB_EXPORT Q_DECL_EXPORT
#else
#define LASERDOCKLIB_EXPORT Q_DECL_IMPORT
#endif
#else
#define LASERDOCKLIB_EXPORT
#endif



#endif //LASERDOCKLIB_LASERDOCKLIB_H
