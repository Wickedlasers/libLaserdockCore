#include "ldCore/Helpers/ldLzrHelper.h"

#include <QtGui/QColor>
#include <QtCore/QtDebug>

#include <ldCore/Render/ldRendererOpenlase.h>

lzr::FrameList ldLzrHelper::convertOpenlaseToLzr(const std::vector<std::vector<OLPoint> > &data)
{
    const int EMPTY_POINT_COUNT = 5;

    lzr::FrameList result;
    lzr::Frame frame;

    for(const std::vector<OLPoint> &pointVec : data) {
        // add empty points on line start
        lzr::Point emptyPoint(0.0, 0,0, 0, 0, 0);
        if(!pointVec.empty()) {
            emptyPoint.x = static_cast<double>(pointVec[0].x);
            emptyPoint.y = static_cast<double>(pointVec[0].y);
        }
        for(int i = 0; i < EMPTY_POINT_COUNT; i++) {
            frame.push_back(emptyPoint);
        }

        // add data
        for(const OLPoint &p : pointVec) {
            lzr::Point lzrPoint;
            lzrPoint.x = static_cast<double>(p.x);
            lzrPoint.y = static_cast<double>(p.y);
            QColor color = QColor::fromRgb(p.color);
            lzrPoint.r = static_cast<uint8_t>(color.red());
            lzrPoint.g = static_cast<uint8_t>(color.green());
            lzrPoint.b = static_cast<uint8_t>(color.blue());
            lzrPoint.i = (p.color == 0) ? 0 : 1;

            frame.push_back(lzrPoint);
        }

        // add empty points on line end
        if(!pointVec.empty()) {
            emptyPoint.x = static_cast<double>(pointVec[pointVec.size() - 1].x);
            emptyPoint.y = static_cast<double>(pointVec[pointVec.size() - 1].y);
        }
        for(int i = 0; i < EMPTY_POINT_COUNT; i++) {
            frame.push_back(emptyPoint);
        }
    }

    result.push_back(frame);
    return result;
}

std::vector<std::vector<OLPoint> > ldLzrHelper::convertLzrToOpenlase(const lzr::FrameList &lzrFrameList)
{
    std::vector<std::vector<OLPoint> > result;

    for(const lzr::Frame &lzrFrame : lzrFrameList) {
        std::vector<OLPoint> libolFrame;

        for(const lzr::Point &lzrPoint : lzrFrame) {
            // convert lzr point to libol
            OLPoint libolPoint;
            libolPoint.x = static_cast<float>(lzrPoint.x);
            libolPoint.y = static_cast<float>(lzrPoint.y);
            libolPoint.z = 0.f;
            libolPoint.color = lzrPoint.is_blanked() ? 0 : QColor(lzrPoint.r, lzrPoint.g, lzrPoint.b).rgb();

            // ilda can have multiple same points in a raw - skip them
            // on some files this adjustment cause blank output, disable it
//            if(!libolFrame.empty() && libolPoint == libolFrame.back()) {
//                continue;
//            }

            // add
            libolFrame.push_back(libolPoint);
        }

        // add frame to frame list
        result.push_back(libolFrame);
    }
    return result;
}

lzr::FrameList ldLzrHelper::readIldaFile(const QString &fileName)
{
    lzr::FrameList frameList;

    lzr::ILDA* ilda = lzr::ilda_open(fileName.toLatin1().constData(), "r");
    if(ilda == nullptr) {
        qWarning() << "Can't open ILDA file" << fileName;
        return frameList;
    }

    // check projector count
    size_t pCount = lzr::ilda_projector_count(ilda);
    if(pCount == 0) {
        qWarning() << "No projector info" << fileName;
        lzr::ilda_close(ilda);
        return frameList;
    }

    // read from first project
    int res = lzr::ilda_read(ilda, 0, frameList);
    if(res != 0) {
        QString ilda_error = QString::fromLatin1(lzr::ilda_error(ilda));
        // empty error can be in case if file has no end header with 0 number of records
        // example - immigrant_song-LD.ILD
        if(ilda_error != "") {
            qWarning() << "Warning loading ILD file: " << fileName << res << frameList.size() << ilda_error;
        }
    }

    // close
    lzr::ilda_close(ilda);

    return frameList;
}

bool ldLzrHelper::writeIldaFile(const QString &filePath, lzr::FrameList &frameList, const QString &name)
{
    lzr::ILDA* ilda = lzr::ilda_open(qPrintable(filePath), "w");
    if(ilda == nullptr) {
        qWarning() << "Can't open ILDA file for write" << filePath;
        return false;
    }

    bool isOk = false;
    int res = lzr::ilda_write(ilda, 0, frameList, qPrintable(name), qPrintable("Wicked Lasers Inc."));
    if(res == LZR_SUCCESS)  {
        qDebug() << "ILDA file was saved " << filePath;
        isOk = true;
    } else {
        qWarning() << "WARNING: ILDA file was not saved " << res << filePath << frameList.size();
    }
    lzr::ilda_close(ilda);

    return isOk;
}
