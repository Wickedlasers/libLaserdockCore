// ldImageHelper.cpp
//  Created by Eric Brugère on 8/feb/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.

#include "ldCore/Helpers/Image/ldImageHelper.h"

#include <QtCore/QDebug>
#include <QtGui/QImage>

// IplImage2QImage
//QImage ldImageHelper::IplImage2QImage(IplImage *iplImg)
//{
//    int h = iplImg->height;
//    int w = iplImg->width;
//    int channels = iplImg->nChannels;
//    QImage qimg(w, h, QImage::Format_ARGB32);
//    char *data = iplImg->imageData;

//    for (int y = 0; y < h; y++, data += iplImg->widthStep) {
//        for (int x = 0; x < w; x++) {
//            char r = 0, g = 0, b = 0, a = 0;
//            if (channels == 1) {
//                r = data[x * channels];
//                g = data[x * channels];
//                b = data[x * channels];
//            } else if (channels == 3 || channels == 4) {
//                r = data[x * channels + 2];
//                g = data[x * channels + 1];
//                b = data[x * channels];
//            }

//            if (channels == 4) {
//                a = data[x * channels + 3];
//                qimg.setPixel(x, y, qRgba(r, g, b, a));
//            } else {
//                qimg.setPixel(x, y, qRgb(r, g, b));
//            }
//        }
//    }

//    return qimg;
//}



struct RGB {
    uchar b, g, r;
};

QImage ldImageHelper::QImageFromMat(const cv::Mat &img) {
    switch (img.type()) {
    case CV_8UC1: return QImage(img.data, img.cols, img.rows, img.step, QImage::Format_Indexed8).copy();
    case CV_8UC3: return QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888).rgbSwapped().copy();
    case CV_8UC4: return QImage(img.data, img.cols, img.rows, img.step, QImage::Format_ARGB32).copy();
    }

    // just try another one...
    return QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888).rgbSwapped().copy();

    // old code, left just for reference
//    IplImage iplImage(img);
//    return IplImage2QImage(&iplImage);
}

cv::Mat ldImageHelper::matFromQImage(const QImage &image) {
    //cv::Mat mat(image.height(), image.width(), CV_8UC3, image.bytesPerLine());
    //cv::Mat mat(image.height(), image.width(), CV_8UC3,
    //cv::Mat mat()
    cv::Mat mat(image.height(), image.width(), CV_8UC3);
    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.cols; j++) {
            RGB& rgb = mat.ptr<RGB>(i)[j];
            QRgb qrgb = image.pixel(QPoint(j, i));
            rgb.b = (qrgb >> 0) & 0xff;
            rgb.g = (qrgb >> 8) & 0xff;
            rgb.r = (qrgb >> 16) & 0xff;
            if (((qrgb >> 24) & 0xff) == 0) rgb.r = rgb.g = rgb.b = 255;
        }
    }
    return mat;
}

cv::Mat ldImageHelper::resizeFitInSquare(const cv::Mat& _input, int size) {
    int neww = size;
    int newh = size;
    int w = _input.cols;
    int h = _input.rows;
    if (w > h) {
        newh = size * h / w;
    }
    else {
        neww = size * w / h;
    }
    cv::Mat ret;
#ifdef Q_OS_ANROID
    // do NEAREST resize on Android as it is requires less CPU
    cv::resize(_input, ret, cv::Size(neww, newh), 0, 0, cv::InterpolationFlags::INTER_NEAREST);
#else
    cv::resize(_input, ret, cv::Size(neww, newh), 0, 0, cv::InterpolationFlags::INTER_CUBIC);
#endif
    return ret;
}

QImage ldImageHelper::resizeLaserImage(const QImage &src, int size) {
    if(src.isNull())
        return src;

    // algorithm for scaling preview images more smoothly
    // tries to keep lines from disappearing when scaling down
    // and generate a consistent appearance across resolutions

    // setup
    cv::Mat i = ldImageHelper::matFromQImage(src);
    int oldsize = MAX(src.width(), src.height());
    float ratio = ((float)oldsize / size);

    // dilate
    float dilation_f = ((ratio + 1.5f) - 1) / 2;
    int dilation_size = (int)roundf(dilation_f);
    float dilation_extra = dilation_f - dilation_size;
    if (dilation_size > 0) {
        cv::Mat element_dilation = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(dilation_size, dilation_size), cv::Point(dilation_size/2, dilation_size/2));
        cv::dilate(i, i, element_dilation);
    }

    // blur
    cv::Size blur_size(1 + 1, 1 + 1);
    cv::blur(i, i, blur_size);

    // lighten
    float lighten = 1.5f + dilation_extra/2;
    i = (1.0f + lighten) * i;

    // resize
    //i = img_resizeFitInSquare(i, size);
    //cv::resize(i, i, cv::Size(size, size), 0, 0, cv::InterpolationFlags::INTER_CUBIC);
    cv::resize(i, i, cv::Size(size, size), 0, 0, cv::InterpolationFlags::INTER_LINEAR);
    //cv::resize(i, i, cv::Size(size, size), 0, 0, cv::InterpolationFlags::INTER_AREA);

    // blur again
    //cv::Size blur_size(1 + 1, 1 + 1);
    //cv::blur(i, i, blur_size);

    // convert and return
    QImage dst = ldImageHelper::QImageFromMat(i);
    return dst;
}
