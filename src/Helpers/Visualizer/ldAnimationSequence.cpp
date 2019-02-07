/**
    libLaserdockCore
    Copyright(c) 2018 Wicked Lasers

    This file is part of libLaserdockCore.

    libLaserdockCore is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libLaserdockCore is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libLaserdockCore.  If not, see <https://www.gnu.org/licenses/>.
**/

#include "ldCore/Helpers/Visualizer/ldAnimationSequence.h"

#include "ldCore/ldCore.h"
#include <ldCore/Helpers/Color/ldColorUtil.h>
#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Visualizations/ldVisualizer.h"
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"
#include "ldCore/Helpers/Audio/ldTempoAC.h"
#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/SVG/ldSvgReader.h"


ldAnimationSequenceBezier::ldAnimationSequenceBezier() {
    keyStart.resize(8, 0);
    keyEnd.resize(8, 0);
}


const svgBezierCurvesSequence &ldAnimationSequenceBezier::frames() const
{
    return m_frames;
}

void ldAnimationSequenceBezier::setFrames(const svgBezierCurvesSequence &frames)
{
    m_frames = frames;
}

void ldAnimationSequenceBezier::setRenderAlg(ldAnimationSequenceBezier::RenderAlg renderAlg)
{
    m_renderAlg = renderAlg;
}

void ldAnimationSequenceBezier::drawFrame(ldRendererOpenlase* r, int index) {
    if (index < 0 || (uint) index >= m_frames.size()) return;

    switch (m_renderAlg) {
    case RenderAlg::Bezier:
        drawFrameBezier3(r, index);
        break;
    case RenderAlg::Lines:
        drawFrameLine4(r, index);
        break;
    case RenderAlg::Lights:
        drawFrameLights5(r, index);
        break;
    }
}



static void vert3(ldRendererOpenlase* r, ldShader* s, float xx, float yy, uint32_t cc) {
    float x = xx; float y = yy; uint32_t c = cc;
    if (s != NULL) s->ShaderFunc(&x, &y, &c);
    r->vertex3(x, y, 0, c);
}


void ldAnimationSequenceBezier::drawFrameBezier3(ldRendererOpenlase* r, int index) {


    ldRendererOpenlase* renderer = r;
    svgBezierCurves dataVect = m_frames[index];
    {
        //
        for (const std::vector<ldBezierCurve> &bezierTab : dataVect)
        {
//            if (bezierTab.size() < 2) continue;
            //uint32_t color = ldColorUtil::colorHSV(0, 0, 1);
            uint32_t c = color;
            ldShader* s = _shader1;
            float f = 0;

            renderer->begin(OL_BEZIERSTRIP);
            for (uint i = 0; i<bezierTab.size(); i++)
            {
                const ldBezierCurve &b = bezierTab[i];
                f = (float)i / (bezierTab.size());
                c = (f >= _fstart && f <= _fend) ? (_c1) : (_c2);
                s = (f >= _fstart && f <= _fend) ? (_shader1) : (_shader2);

                vert3(renderer, s, b.start().x, b.start().y, c);
                vert3(renderer, s, b.control1().x, b.control1().y, c);
                vert3(renderer, s, b.control2().x, b.control2().y, c);
            }
            if(!bezierTab.empty()) {
                const ldBezierCurve &b = bezierTab[0];
                vert3(renderer, s, b.start().x, b.start().y, c);
            }
            renderer->end();
        }
    }



}

void ldAnimationSequenceBezier::drawFrameLine4(ldRendererOpenlase* r, int index) {

    ldRendererOpenlase* renderer = r;
    const svgBezierCurves &dataVect = m_frames[index];
    {
        //
        for (const std::vector<ldBezierCurve> &bezierTab : dataVect)
        {
//            float alt = 0.0;
            uint32_t c = color;
            ldShader* s = _shader1;
            float f = 0; float ff = 0;

            renderer->begin(OL_LINESTRIP);
            for (uint j = 0; j < bezierTab.size(); j++)
            {

                const ldBezierCurve &b = bezierTab[j];
                //qDebug() << " bezier " << i << " length " << b.length();
                int maxPointsLocal = 10;
                float test = 10.0*b.length();
                if (test < 1.0) test *= 2.0;
                //test /= 2; // faster?
                maxPointsLocal = (int)(test);
                if (maxPointsLocal < 4) maxPointsLocal = 4;

                for (int i = 0; i < maxPointsLocal; i++)
                {
                    ff = (float)i / (maxPointsLocal - 1);
                    f = (j + ff) / (bezierTab.size());
                    c = (f >= _fstart && f <= _fend) ? (_c1) : (_c2);
                    s = (f >= _fstart && f <= _fend) ? (_shader1) : (_shader2);

                    float slope = 1.0*i / (maxPointsLocal - 1);
                    Vec2 p = b.getPoint(slope);

                    //uint32_t color = ldColorUtil::colorHSV(0, 0, 1);
                    //renderer->vertex(x, y, c);
                    vert3(renderer, s, p.x, p.y, c);
                }

            }
            renderer->end();
        }
    }


}

void ldAnimationSequenceBezier::drawFrameLights5(ldRendererOpenlase* r, int index) {

    ldRendererOpenlase* renderer = r;
    const svgBezierCurves &dataVect = m_frames[index];
    {
        //
        int cc = 0;
        int ccc = 8;
        int cccc = 0;
        int ccccc = 4;
        //            cc = (1 - ldCore::instance()->musicManager()->tempoACSlower->phaseReactive) * ccc * ccccc;
        int bc = (int)((1 - ldCore::instance()->musicManager()->tempoTrackerSlow->output()) * ccc * ccccc);

        for (const std::vector<ldBezierCurve> &bezierTab : dataVect)
        {
            //            float alt = 0.0;
            uint32_t c = color;
//            ldShader* s = _shader1;
            float f = 0; float ff = 0;

            cc = bc % ccc;
            cccc = (bc / ccc) % ccccc;



            renderer->begin(OL_LINESTRIP);
            for (uint j = 0; j < bezierTab.size(); j++)
            {

                const ldBezierCurve &b = bezierTab[j];
                //qDebug() << " bezier " << i << " length " << b.length();
                int maxPointsLocal = 10;
                float test = 10.0*b.length();
                if (test < 1.0) test *= 2.0;
                //test /= 2; // faster?
                int x8 = 8; // slower?
                maxPointsLocal = (int)(test*x8);
                if (maxPointsLocal < 2) maxPointsLocal = 2;

                for (int i = 0; i < maxPointsLocal; i++)
                {
                    ff = (float)i / (maxPointsLocal - 1);
                    f = (j + ff) / (bezierTab.size());
                    c = (f >= _fstart && f <= _fend) ? (_c1) : (_c2);
//                    s = (f >= _fstart && f <= _fend) ? (_shader1) : (_shader2);

                    float slope = 1.0*i / (maxPointsLocal - 1);
                    Vec2 p =  b.getPoint(slope);

                    //uint32_t color = ldColorUtil::colorHSV(0, 0, 1);
                    //renderer->vertex(x, y, c);

                    if (cc == 0) {
                        if (cccc == 0) c = 0xff0000;
                        if (cccc == 1) c = 0x00ff00;
                        if (cccc == 2) c = 0xffff00;
                        if (cccc == 3) c = 0x00ff00;
                        //renderer->begin(OL_POINTS);
                        vert3(renderer, 0, p.x, p.y, 0);
                        vert3(renderer, 0, p.x, p.y, c);
                        vert3(renderer, 0, p.x, p.y, 0);
                        //renderer->end();
                        cccc = (cccc + 1) % ccccc;
                    }
                    cc = (cc + 1) % ccc;

                }

            }
            renderer->end();
        }
    }


}

void ldAnimationSequenceBezier::loadDir(const QString &dirPath, const QString &filePrefix, int maskSize, bool isExternal)
{
    m_frames = ldSvgReader::loadSvgSequence(dirPath, isExternal ? ldSvgReader::Type::SvgFrame : ldSvgReader::Type::Dev, 0.01f, filePrefix, maskSize);
    for (int k = 0; k < 8; k++) {
        keyStart[k] = 0;
        keyEnd[k] = m_frames.size() - 1;
    }
}


#include <qfile.h>
void ldAnimationSequenceBezier::load(const QString &filePath) {


    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "error::ldSvgReader::loadSvg: file does not exist" << filePath;
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "error::ldSvgReader::loadSvg: Qt file issue";
        return;
    }

    //QByteArray blob = file.readAll();
    QTextStream in(&file);

    QString line;

    line = in.readLine(); //out << "LDV Animation File" << endl;
    line = in.readLine(); //out << "Render Alg" << endl;
    line = in.readLine(); //out << renderAlg << endl;
    m_renderAlg = RenderAlg(line.toInt());
    line = in.readLine(); //out << "Frame Count" << endl;
    line = in.readLine(); //out << nframes << endl;
    int nframes = line.toInt();
    m_frames.resize(nframes);

    line = in.readLine(); //out << "Key Frame Index start end" << endl;
    for (int i = 0; i < 8; i++) {
        line = in.readLine(); //out << keyStart[i] << endl;
        keyStart[i] = line.toInt();
        line = in.readLine(); //out << keyEnd[i] << endl;
        keyEnd[i] = line.toInt();
    }

    line = in.readLine(); //out << "Frame Data" << endl;
    for (int i = 0; i < nframes; i++) {
        line = in.readLine(); //out << "Frame " << i << endl;
        line = in.readLine(); //out << "Curve Count" << endl;
        line = in.readLine(); //out << ncurves << endl;
        int ncurves = line.toInt();
        m_frames[i].resize(ncurves);
        for (int j = 0; j < ncurves; j++) {
            line = in.readLine(); //out << "Curve " << j << endl;
            line = in.readLine(); //out << "Point Count" << endl;
            line = in.readLine(); //out << npoints << endl;
            int npoints = line.toInt();
            m_frames[i][j].resize(npoints);
            for (int k = 0; k < npoints; k++) {
                float sx = in.readLine().toFloat();
                float sy = in.readLine().toFloat();
                float ex = in.readLine().toFloat();
                float ey = in.readLine().toFloat();
                float c1x = in.readLine().toFloat();
                float c1y = in.readLine().toFloat();
                float c2x = in.readLine().toFloat();
                float c2y = in.readLine().toFloat();
                m_frames[i][j][k].setStart(Vec2(sx, sy));
                m_frames[i][j][k].setEnd(Vec2(ex, ey));
                m_frames[i][j][k].setControl1(Vec2(c1x, c1y));
                m_frames[i][j][k].setControl2(Vec2(c2x, c2y));
            }
        }
    }
    line = in.readLine(); //out << "End" << endl;


}

void ldAnimationSequenceBezier::save(const QString &filePath) {


    QFile file(filePath);
    //if (!file.exists()) {
    //    qDebug() << "error::ldSvgReader::loadSvg: file does not exist" << str;
    //    return;
    //}
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "error::ldSvgReader::loadSvg: Qt file issue";
        QFileDevice::FileError error = file.error();
        qDebug() << error << file.errorString();

        return;
    }

    //QByteArray blob = file.readAll();
    //QTextStream in(&file);
    QTextStream out(&file);

    out << "LDV Animation File" << endl;

    out << "Render Alg" << endl;
    out << m_renderAlg << endl;

    int nframes = m_frames.size();
    out << "Frame Count" << endl;
    out << nframes << endl;

    out << "Key Frame Index start end" << endl;
    for (int i = 0; i < 8; i++) {
        out << keyStart[i] << endl;
        out << keyEnd[i] << endl;
    }

    out << "Frame Data" << endl;
    for (int i = 0; i < nframes; i++) {
        out << "Frame " << i << endl;
        int ncurves = m_frames[i].size();
        out << "Curve Count " << endl;
        out << ncurves << endl;
        for (int j = 0; j < ncurves; j++) {
            out << "Curve " << j << endl;
            int npoints = m_frames[i][j].size();
            out << "Point Count" << endl;
            out << npoints << endl;
            for (int k = 0; k < npoints; k++) {
                out << m_frames[i][j][k].start().x << endl;
                out << m_frames[i][j][k].start().y << endl;
                out << m_frames[i][j][k].end().x << endl;
                out << m_frames[i][j][k].end().y << endl;
                out << m_frames[i][j][k].control1().x << endl;
                out << m_frames[i][j][k].control1().y << endl;
                out << m_frames[i][j][k].control2().x << endl;
                out << m_frames[i][j][k].control2().y << endl;
            }
        }
    }
    out << "End" << endl;


}

#include "qdatastream.h"

struct BezierCurve2 {
    Vec2 start;
    Vec2 end;
    Vec2 control1;
    Vec2 control2;
};

bool ldAnimationSequenceBezier::save2(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "error::ldSvgReader::loadSvg: Qt file issue";
        qDebug() << file.errorString();
        return false;
    }

    QDataStream out(&file);



    //out << "LDV Animation File" << endl;
    //5 byte header
    char header[] = "LDVA2";
    //file.write(header, 5);
    out.writeRawData(header, 5);

    out << m_renderAlg;

    int nframes = m_frames.size();
    out << nframes;

    for (int i = 0; i < 8; i++) {
        out << keyStart[i];
        out << keyEnd[i];
    }

    out << sourceFPS;
    out << sourceBPM;

   // out << "Frame Data" << endl;
    for (int i = 0; i < nframes; i++) {
        //out << "Frame " << i << endl;
        int ncurves = m_frames[i].size();
        //out << "Curve Count " << endl;
        out << ncurves;// << endl;
        for (int j = 0; j < ncurves; j++) {
            //out << "Curve " << j << endl;
            int npoints = m_frames[i][j].size();
            //out << "Point Count" << endl;
            out << npoints;// << endl;
            std::vector<BezierCurve2> b;
            b.resize(npoints);
            for (int k = 0; k < npoints; k++) {
                b[k].start = m_frames[i][j][k].start();
                b[k].end = m_frames[i][j][k].end();
                b[k].control1 = m_frames[i][j][k].control1();
                b[k].control2 = m_frames[i][j][k].control2();
            }
            out.writeRawData((char*)b.data(), sizeof(BezierCurve2)*npoints);
            if (0) for (int k = 0; k < npoints; k++) {
                //out.writeBytes((char*)frames[i][j].data(), sizeof(BezierCurve)); continue;
                out << m_frames[i][j][k].start().x;// << endl;
                out << m_frames[i][j][k].start().y;// << endl;
                out << m_frames[i][j][k].end().x;// << endl;
                out << m_frames[i][j][k].end().y;// << endl;
                out << m_frames[i][j][k].control1().x;// << endl;
                out << m_frames[i][j][k].control1().y;// << endl;
                out << m_frames[i][j][k].control2().x;// << endl;
                out << m_frames[i][j][k].control2().y;// << endl;
            }
            //out.writeRawData((char*)frames[i][j].data(), sizeof(BezierCurve)*npoints);

        }
    }
    //out << "End" << endl;


    return true;
}



bool ldAnimationSequenceBezier::load2(const QString &filePath) {
    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "error::ldSvgReader::loadSvg: file does not exist" << filePath;
        qDebug() << file.errorString();
        return false;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "error::ldSvgReader::loadSvg: Qt file issue";
        qDebug() << file.errorString();
        return false;
    }

    //QByteArray blob = file.readAll();
    //QTextStream in(&file);
    //QB out(&file);
    QDataStream in(&file);

    //out << "LDV Animation File" << endl;
    //5 byte header
//    char header[] = "LDVA2";
    char res[5];
//    char c;
    //file.write(header, 5);
    //in.readBytes(&res, 5);
    in.readRawData(res, 5);
    // todo: compare header[] with res[] to verify version

    int renderAlgInt;
    in >> renderAlgInt;
    m_renderAlg = RenderAlg(renderAlgInt);

    int nframes; in >> nframes;
    m_frames.resize(nframes);

    for (int i = 0; i < 8; i++) {
        in >> keyStart[i];
        in >> keyEnd[i];
    }

    in >> sourceFPS;
    in >> sourceBPM;

    for (int i = 0; i < nframes; i++) {
        //line = in.readLine(); //out << "Frame " << i << endl;
        //line = in.readLine(); //out << "Curve Count" << endl;
        //line = in.readLine(); //out << ncurves << endl;
        //int ncurves = line.toInt();
        int ncurves; in >> ncurves;
        m_frames[i].resize(ncurves);
        for (int j = 0; j < ncurves; j++) {
            //line = in.readLine(); //out << "Curve " << j << endl;
            //line = in.readLine(); //out << "Point Count" << endl;
            //line = in.readLine(); //out << npoints << endl;
            //int npoints = line.toInt();
            int npoints; in >> npoints;
            m_frames[i][j].resize(npoints);

            std::vector<BezierCurve2> b;
            b.resize(npoints);
            in.readRawData((char*)b.data(), sizeof(BezierCurve2)*npoints);
            for (int k = 0; k < npoints; k++) {
                m_frames[i][j][k].setStart(b[k].start);
                m_frames[i][j][k].setEnd(b[k].end);
                m_frames[i][j][k].setControl1(b[k].control1);
                m_frames[i][j][k].setControl2(b[k].control2);
            }

            //in.readRawData((char*)frames[i][j].data(), sizeof(BezierCurve2)*npoints);
            if (0) for (int k = 0; k < npoints; k++) {
                float sx, sy, ex, ey, c1x, c1y, c2x, c2y;
                in >> sx;
                in >> sy;
                in >> ex;
                in >> ey;
                in >> c1x;
                in >> c1y;
                in >> c2x;
                in >> c2y;

                m_frames[i][j][k].setStart(Vec2(sx, sy));
                m_frames[i][j][k].setEnd(Vec2(ex, ey));
                m_frames[i][j][k].setControl1(Vec2(c1x, c1y));
                m_frames[i][j][k].setControl2(Vec2(c2x, c2y));
            }
        }
    }
    //line = in.readLine(); //out << "End" << endl;

    return true;
}


void ldAnimationSequenceBezier::save3(const QString &filePath) {


    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "error::ldSvgReader::loadSvg: Qt file issue";
        //        QFileDevice::FileError error = file.error();
        qDebug() << file.errorString();
        return;
    }

    QDataStream out(&file);
    //5 byte header
    char header[] = "LDVA3";
    out.writeRawData(header, 5);

    out << m_renderAlg;

    int nframes = m_frames.size();
    out << nframes;

    for (int i = 0; i < 8; i++) {
        out << keyStart[i];
        out << keyEnd[i];
    }

    out << sourceFPS;
    out << sourceBPM;

    for (int i = 0; i < nframes; i++) {
        int ncurves = m_frames[i].size();
        out << ncurves;// << endl;
        for (int j = 0; j < ncurves; j++) {
            int npoints = m_frames[i][j].size();
            out.writeRawData((char*)m_frames[i][j].data(), sizeof(ldBezierCurve)*npoints);
        }
    }



}



void ldAnimationSequenceBezier::load3(const QString &filePath) {


    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "error::ldSvgReader::loadSvg: file does not exist" << filePath;
        //        QFileDevice::FileError error = file.error();
        qDebug() << file.errorString();
        return;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "error::ldSvgReader::loadSvg: Qt file issue";
        //        QFileDevice::FileError error = file.error();
        qDebug() << file.errorString();
        return;
    }

    QDataStream in(&file);

    //5 byte header
    char res[5];
    in.readRawData(res, 5);
    // todo: compare header[] with res[] to verify version

    int renderAlgInt;
    in >> renderAlgInt;
    m_renderAlg = RenderAlg(renderAlgInt);

    int nframes; in >> nframes;
    m_frames.resize(nframes);

    for (int i = 0; i < 8; i++) {
        in >> keyStart[i];
        in >> keyEnd[i];
    }

    in >> sourceFPS;
    in >> sourceBPM;

    for (int i = 0; i < nframes; i++) {
        int ncurves; in >> ncurves;
        m_frames[i].resize(ncurves);
        for (int j = 0; j < ncurves; j++) {
            int npoints; in >> npoints;
            m_frames[i][j].resize(npoints);
            in.readRawData((char*)m_frames[i][j].data(), sizeof(ldBezierCurve)*npoints);
        }
    }

}

void ldAnimationSequenceBezier::autoscale() {

    float xmin = 0;
    float xmax = 0;
    float ymin = 0;
    float ymax = 0;

#define XY(x, y) {\
        xmin = MIN(xmin, x);\
        ymin = MIN(ymin, y);\
        xmax = MAX(xmax, x);\
        ymax = MAX(ymax, y);\
    }

    for (uint index = 0; index < m_frames.size(); index++)
    {
        svgBezierCurves dataVect = m_frames[index];
        for (uint p = 0; p < dataVect.size(); p++)
        {
            std::vector<ldBezierCurve> bezierTab = dataVect.at(p);
//            if (bezierTab.size() < 2) continue;
//            uint32_t color = ldColorUtil::colorHSV(0, 0, 1);

            //
            for (uint i = 0; i < bezierTab.size(); i++)
            {
                ldBezierCurve b = bezierTab.at(i);
                XY(b.start().x, b.start().y);
                XY(b.control1().x, b.control1().y);
                XY(b.control2().x, b.control2().y);
                if (m_renderAlg == RenderAlg::Lines) XY(b.end().x, b.end().y);
            }
            //BezierCurve b = bezierTab.at(0);
            //XY(b.start.x, b.start.y);

        }
    }

    // target dimensions
    float x1 = -1;
    float x2 = 1;
    float y1 = -1;
    float y2 = 1;

    //math
    //:xr(xmin) = x1
    //:xr(xmax) = x2
    //let xr(t) = f1 + f2*t
    //f1 + f2*xmin = x1
    //f1 + f2*xmax = x2
    //f1 = x1 - f2*xmin
    //(x1 - f2*xmin) + f2*xmax = x2
    //-f2*xmin + f2*xmax = x2 - x1
    //(xmax - xmin)*f2 = (x2-x1)
    // |f2 = (x2-x1)/(xmax - xmin);
    //f1 + f2*xmin = x1
    //f1 = -f2*xmin + x1
    // |f1 = -(x2-x1)/(xmax - xmin)*xmin + x1;

    float f1x = x1 - xmin*(x2 - x1) / (xmax - xmin);
    float f2x = (x2 - x1) / (xmax - xmin);
    float f1y = y1 - ymin*(y2 - y1) / (ymax - ymin);
    float f2y = (y2 - y1) / (ymax - ymin);

    // maintain aspect
    if (f2x > f2y) {
        f1x = f1x * f2y / f2x;
        f2x = f2y;
    }
    if (f2y > f2x) {
        f1y = f1y * f2x / f2y;
        f2y = f2x;
    }



#define XY2(bc, getter, setter) {\
    Vec2 tmp = bc.getter(); \
    tmp *= Vec2(f2x, f2y); \
    tmp += Vec2(f1x, f1y); \
    bc.setter(tmp); \
    }

    for (uint index = 0; index < m_frames.size(); index++)
    {
        svgBezierCurves dataVect = m_frames[index];
        //
        for (uint p = 0; p < dataVect.size(); p++)
        {
            std::vector<ldBezierCurve> bezierTab = dataVect[p];
//            if (bezierTab.size() < 2) continue;
//            uint32_t color = ldColorUtil::colorHSV(0, 0, 1);

            //
            for (uint i = 0; i < bezierTab.size(); i++)
            {
                ldBezierCurve b = bezierTab[i];

                XY2(b, start, setStart);
                XY2(b, control1, setControl1);
                XY2(b, control2, setControl2);
                if (m_renderAlg == RenderAlg::Lines) XY2(b, end, setEnd);
                m_frames[index][p][i] = b;
            }
//            BezierCurve b = bezierTab.at(0);
            //XY(b.start.x, b.start.y);

        }
    }


}

void ldAnimationSequenceBezier::removeblank() {

    int nremoved = 0;
    for (uint index = 0; index < m_frames.size(); index++)
    {
        bool remove = true;
        svgBezierCurves dataVect = m_frames[index];
        for (uint p = 0; p < dataVect.size(); p++)
        {
//            std::vector<ldBezierCurve> bezierTab = dataVect.at(p);
//            if (bezierTab.size() < 2) continue;
            remove = false;
        }

        if (remove) {
            for (uint i = 0; i < 8; i++) {
                if (keyStart[i] >= 0 && (uint) keyStart[i] >= index) keyStart[i]--;
                if (keyEnd[i] >= 0 && (uint) keyEnd[i] >= index) keyEnd[i]--;
            }
            m_frames.erase(m_frames.begin() + index);
            index--;
            nremoved++;
        }
    }
    qDebug() << "removed frames ct " << nremoved;

}

void ldAnimationSequenceBezier::drawFrameBezier3x(ldRendererOpenlase* r, int index, float fstart, float fend, uint32_t c1, uint32_t c2) {

    if (index < 0 || (uint) index >= m_frames.size()) return;

        ldRendererOpenlase* renderer = r;
        svgBezierCurves dataVect = m_frames[index];
        {
            //
            for (const std::vector<ldBezierCurve> &bezierTab : dataVect)
            {
//                if (bezierTab.size() < 2) continue;
                //uint32_t color = ldColorUtil::colorHSV(0, 0, 1);
                uint32_t c = color;
                float f = 0;

                renderer->begin(OL_BEZIERSTRIP);
                for (uint i = 0; i<bezierTab.size(); i++)
                {
                    f = (float)i / (bezierTab.size());
                    c = (f >= fstart && f <= fend) ? (c1) : (c2);
                    const ldBezierCurve &b = bezierTab[i];
                    renderer->vertex3(b.start().x, b.start().y, 0, c);
                    renderer->vertex3(b.control1().x, b.control1().y, 0, c);
                    renderer->vertex3(b.control2().x, b.control2().y, 0, c);
                }
                f = 1;
                c = (f >= fstart && f <= fend) ? (c1) : (c2);
                if(!bezierTab.empty()) {
                    const ldBezierCurve &b = bezierTab[0];
                    renderer->vertex3(b.start().x, b.start().y, 0, c);
                }
                renderer->end();
            }
        }

}

void ldAnimationSequenceBezier::scale(float xs, float ys) {

#define XY3(bc,getter,setter) { \
    Vec2 tmp = bc.getter(); \
    tmp *= Vec2(xs, ys); \
    bc.setter(tmp); \
}

    for (svgBezierCurves &dataVect : m_frames) {
        for (std::vector<ldBezierCurve> &bezierTab : dataVect) {
//            if (bezierTab.size() < 2) continue;
            for (ldBezierCurve &b : bezierTab) {
                XY3(b, start, setStart);
                XY3(b, control1, setControl1);
                XY3(b, control2, setControl2);
                if (m_renderAlg == RenderAlg::Lines) XY3(b, end, setEnd);
            }
        }
    }

}

void ldAnimationSequenceBezier::move(float xt, float yt) {

#define XY4(bc,getter,setter) { \
    Vec2 tmp = bc.getter(); \
    tmp += Vec2(xt, yt); \
    bc.setter(tmp); \
}

    for (svgBezierCurves &dataVect : m_frames) {
        for (std::vector<ldBezierCurve> &bezierTab : dataVect) {
//            if (bezierTab.size() < 2) continue;
            for (ldBezierCurve &b : bezierTab) {
                XY4(b, start, setStart);
                XY4(b, control1, setControl1);
                XY4(b, control2, setControl2);
                if (m_renderAlg == RenderAlg::Lines) XY4(b, end, setEnd);
            }
        }
    }

}
