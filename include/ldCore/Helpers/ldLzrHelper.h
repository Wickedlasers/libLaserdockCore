#ifndef LDLZRHELPER_H
#define LDLZRHELPER_H

#include <QtCore/QObject>

#include "ldCore/ldCore_global.h"

namespace lzr {
    class Frame;
    typedef std::vector<Frame> FrameList;
}

struct OLPoint;

namespace ldLzrHelper
{
    LDCORESHARED_EXPORT lzr::FrameList convertOpenlaseToLzr(const  std::vector<std::vector<OLPoint> >  &data);
    LDCORESHARED_EXPORT std::vector<std::vector<OLPoint> > convertLzrToOpenlase(const lzr::FrameList &lzrFrameList);

    LDCORESHARED_EXPORT lzr::FrameList readIldaFile(const QString &fileName);
    LDCORESHARED_EXPORT bool writeIldaFile(const QString &filePath, lzr::FrameList &frameList, const QString &name);
}

#endif // LDLZRHELPER_H
