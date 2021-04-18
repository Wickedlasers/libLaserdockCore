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

#include <ldCore/ldCore_global.h>

class ldBezierCurveObject;

class QImage;

class LDCORESHARED_EXPORT ldImageHelper
{
public:
//    static QImage IplImage2QImage(IplImage *iplImg);
    static QImage QImageFromMat(const cv::Mat &_input);
    static cv::Mat matFromQImage(const QImage &image);
    static cv::Mat resizeFitInSquare(const cv::Mat& _input, int size);
    static QImage resizeLaserImage(const QImage &src, int size);

};

#endif // LDIMAGEHELPER_H
