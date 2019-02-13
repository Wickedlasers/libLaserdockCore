#ifndef LDIMAGEHELPER_H
#define LDIMAGEHELPER_H

#ifdef Q_OS_LINUX
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#else // Q_OS_LINUX
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#endif

#include <openlase/libol.h>

#include <ldCore/Utilities/ldBasicDataStructures.h>
#include <ldCore/ldCore_global.h>

class QImage;

class LDCORESHARED_EXPORT ldImageHelper
{
public:
    static QImage IplImage2QImage(IplImage *iplImg);
    static QImage QImageFromMat(const cv::Mat &_input);
    static cv::Mat matFromQImage(const QImage &image);
    static cv::Mat resizeFitInSquare(const cv::Mat& _input, int size);
    static QImage drawLaserImageFromPoints(const std::vector<OLPoint> &, int size, float scale = 1);
    static QImage drawLaserImageFromVerts(const std::vector<Vertex> &, int size, float scale = 1);
    static QImage resizeLaserImage(const QImage &src, int size);

};

#endif // LDIMAGEHELPER_H
