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

#include <QtCore/QDataStream>
#include <QtCore/QFile>

#include "ldCore/ldCore.h"
#include <ldCore/Helpers/Color/ldColorUtil.h>
#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Shape/ldShader.h"
#include "ldCore/Visualizations/ldVisualizer.h"
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"
#include "ldCore/Helpers/Audio/ldTempoAC.h"
#include "ldCore/Helpers/Audio/ldTempoTracker.h"
#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/SimpleCrypt/ldSimpleCrypt.h"
#include "ldCore/Helpers/SVG/ldSvgReader.h"


ldAnimationSequenceBezier::ldAnimationSequenceBezier() {
    keyStart.resize(8, 0);
    keyEnd.resize(8, 0);
}


const ldBezierPathsSequence &ldAnimationSequenceBezier::frames() const
{
    return m_frames;
}

void ldAnimationSequenceBezier::setFrames(const ldBezierPathsSequence &frames)
{
    m_frames = frames;
}

void ldAnimationSequenceBezier::setRenderAlg(ldAnimationSequenceBezier::RenderAlg renderAlg)
{
    m_renderAlg = renderAlg;
}

ldAnimationSequenceBezier::RenderAlg ldAnimationSequenceBezier::renderAlg() const
{
    return m_renderAlg;
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



static void vert3(ldRendererOpenlase* r, ldShader* s, float x, float y, uint32_t c) {
    if (s) s->ShaderFunc(&x, &y, &c);
    r->vertex3(x, y, 0, c);
}


void ldAnimationSequenceBezier::drawFrameBezier3(ldRendererOpenlase* r, int index) {

    uint32_t c1 = _c1;
    uint32_t c2 = _c2;
    uint32_t c = color;
    ldRendererOpenlase* renderer = r;
    ldBezierPaths dataVect = m_frames[index];
    {
        //
        for (const ldBezierPath &bezierPath : dataVect)
        {
            const std::vector<ldBezierCurve> &curves = bezierPath.data();
            uint32_t col = bezierPath.color();
            if (col!=0) {
                c1 = c2 = col;
            }

            ldShader* s = _shader1;
            float f = 0;

            renderer->begin(OL_BEZIERSTRIP);
            for (uint i = 0; i<curves.size(); i++)
            {
                const ldBezierCurve &b = curves[i];
                f = (float)i / (curves.size());
                c = (f >= _fstart && f <= _fend) ? (c1) : (c2);
                s = (f >= _fstart && f <= _fend) ? (_shader1) : (_shader2);

                vert3(renderer, s, b.start().x, b.start().y, c);
                vert3(renderer, s, b.control1().x, b.control1().y, c);
                vert3(renderer, s, b.control2().x, b.control2().y, c);
            }
            if(!curves.empty()) {
                const ldBezierCurve &b = curves[0];
                vert3(renderer, s, b.start().x, b.start().y, c);
            }
            renderer->end();
        }
    }



}

void ldAnimationSequenceBezier::drawFrameLine4(ldRendererOpenlase* r, int index) {

    ldRendererOpenlase* renderer = r;
    uint32_t c1 = _c1;
    uint32_t c2 = _c2;
    uint32_t c = color;

    const ldBezierPaths &dataVect = m_frames[index];
    {
        //
        for (const ldBezierPath &bezierPath : dataVect)
        {
//            float alt = 0.0;
            const ldGradient &gradient = bezierPath.gradient();
            uint32_t col = bezierPath.color();
            if (col!=0) {
                c1 = c2 = col;
            }


            ldShader* s = _shader1;
            float f = 0; float ff = 0;

            renderer->begin(OL_LINESTRIP);
            for (uint j = 0; j < bezierPath.data().size(); j++)
            {

                const ldBezierCurve &b = bezierPath.data()[j];
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
                    f = (j + ff) / (bezierPath.data().size());
                    c = (f >= _fstart && f <= _fend) ? (c1) : (c2);
                    s = (f >= _fstart && f <= _fend) ? (_shader1) : (_shader2);

                    float slope = 1.0*i / (maxPointsLocal - 1);
                    ldVec2 p = b.getPoint(slope);

                    //uint32_t color = ldColorUtil::colorHSV(0, 0, 1);
                    //renderer->vertex(x, y, c);
                    if(gradient.isValid()) {
                        s = nullptr;
                        c = gradient.getColor(p.x, p.y);
                    }

                    vert3(renderer, s, p.x, p.y, c);
                }

            }
            renderer->end();
        }
    }


}

void ldAnimationSequenceBezier::drawFrameLights5(ldRendererOpenlase* r, int index) {

    ldRendererOpenlase* renderer = r;
    const ldBezierPaths &dataVect = m_frames[index];
    {
        //
        int cc = 0;
        int ccc = 8;
        int cccc = 0;
        int ccccc = 4;
        //            cc = (1 - ldCore::instance()->musicManager()->tempoACSlower->phaseReactive) * ccc * ccccc;
        int bc = (int)((1 - ldCore::instance()->musicManager()->tempoTrackerSlow()->output()) * ccc * ccccc);

        for (const ldBezierPath &bezierPath : dataVect)
        {
            //            float alt = 0.0;
            uint32_t c = color;
//            ldShader* s = _shader1;
            float f = 0; float ff = 0;

            cc = bc % ccc;
            cccc = (bc / ccc) % ccccc;



            renderer->begin(OL_LINESTRIP);
            for (uint j = 0; j < bezierPath.data().size(); j++)
            {

                const ldBezierCurve &b = bezierPath.data()[j];
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
                    f = (j + ff) / (bezierPath.data().size());
                    c = (f >= _fstart && f <= _fend) ? (_c1) : (_c2);
//                    s = (f >= _fstart && f <= _fend) ? (_shader1) : (_shader2);

                    float slope = 1.0*i / (maxPointsLocal - 1);
                    ldVec2 p =  b.getPoint(slope);

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
    m_baseFrame = ldSvgReader::loadSvgSequence(dirPath, isExternal ? ldSvgReader::Type::SvgFrame : ldSvgReader::Type::Dev, 0.01f, filePrefix, maskSize);
    m_frames = m_baseFrame.toSequence();
    for (int k = 0; k < 8; k++) {
        keyStart[k] = 0;
        keyEnd[k] = m_frames.size() - 1;
    }
}


void ldAnimationSequenceBezier::load(const QString &filePath)
{
    if(filePath.endsWith("ldva2", Qt::CaseInsensitive)
        || filePath.endsWith("ldva2.lds", Qt::CaseInsensitive)) {
        load2(filePath);
    } else if(filePath.endsWith("ldva4", Qt::CaseInsensitive)
             || filePath.endsWith("ldva4.lds", Qt::CaseInsensitive)) {
        load4(filePath);
    }
}

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
            std::vector<BezierCurve> b;
            b.resize(npoints);
            for (int k = 0; k < npoints; k++) {
                b[k].start = m_frames[i][j].data()[k].start();
                b[k].end = m_frames[i][j].data()[k].end();
                b[k].control1 = m_frames[i][j].data()[k].control1();
                b[k].control2 = m_frames[i][j].data()[k].control2();
            }
            out.writeRawData((char*)b.data(), sizeof(BezierCurve)*npoints);
            if (0) for (int k = 0; k < npoints; k++) {
                //out.writeBytes((char*)frames[i][j].data(), sizeof(BezierCurve)); continue;
                out << m_frames[i][j].data()[k].start().x;// << endl;
                out << m_frames[i][j].data()[k].start().y;// << endl;
                out << m_frames[i][j].data()[k].end().x;// << endl;
                out << m_frames[i][j].data()[k].end().y;// << endl;
                out << m_frames[i][j].data()[k].control1().x;// << endl;
                out << m_frames[i][j].data()[k].control1().y;// << endl;
                out << m_frames[i][j].data()[k].control2().x;// << endl;
                out << m_frames[i][j].data()[k].control2().y;// << endl;
            }
            //out.writeRawData((char*)frames[i][j].data(), sizeof(BezierCurve)*npoints);

        }
    }
    //out << "End" << endl;


    return true;
}


bool ldAnimationSequenceBezier::save4(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << __FUNCTION__ << "Qt file issue" << file.errorString();
        return false;
    }

    QDataStream out(&file);

    char header[] = "LDVA4";
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

    for (const std::vector<ldBezierPath> &paths : m_frames) {
        int ncurves = paths.size();
        out << ncurves;
        for (const ldBezierPath &path : paths) {
            // write color info
            uint32_t pathColor = path.color();
            out << pathColor;

            // write gradient info;
            Gradient gr;
            gr.x1 = path.gradient().x1();
            gr.y1 = path.gradient().y1();
            gr.x2 = path.gradient().x2();
            gr.y2 = path.gradient().y2();

            for(const ldGradientStop &stop : path.gradient().stops()) {
                GradientStop grStop;
                grStop.offset = stop.offset();
                grStop.color = stop.color();
                gr.stops.push_back(grStop);
            }

            out << gr.x1;
            out << gr.y1;
            out << gr.x2;
            out << gr.y2;

            int stopSize = gr.stops.size();
            out << stopSize;
            for(const GradientStop &stop : gr.stops) {
                out << stop.offset;
                out << stop.color;
            }

            // write bezier points info
            int npoints = path.size();
            out << npoints;
            std::vector<BezierCurve> b;
            b.resize(npoints);
            for (int k = 0; k < npoints; k++) {
                b[k].start = path.data()[k].start();
                b[k].end = path.data()[k].end();
                b[k].control1 = path.data()[k].control1();
                b[k].control2 = path.data()[k].control2();
            }
            out.writeRawData((char*)b.data(), sizeof(BezierCurve)*npoints);
        }
    }

    return true;
}

bool ldAnimationSequenceBezier::isGradient() const
{
    return m_isGradient;
}

bool ldAnimationSequenceBezier::isValid(const QString &filePath) const
{
    return filePath.endsWith("ldva2", Qt::CaseInsensitive)
           || filePath.endsWith("ldva2.lds", Qt::CaseInsensitive)
           || filePath.endsWith("ldva4", Qt::CaseInsensitive)
           || filePath.endsWith("ldva4.lds", Qt::CaseInsensitive);
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
        ldBezierPaths dataVect = m_frames[index];
        for (uint p = 0; p < dataVect.size(); p++)
        {
            std::vector<ldBezierCurve> bezierTab = dataVect.at(p).data();
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

    if(m_baseFrame.data().size() > 0){
        const ldRect &explicitRect =  m_baseFrame.data().front().explicitRect();
        if(!explicitRect.isNull()) {
//            qDebug() << xmin << xmax << ymin << ymax;
//            qDebug() << explicitRect.left() << explicitRect.right() << explicitRect.bottom() << explicitRect.top();

            if(xmin < explicitRect.left()) xmin = explicitRect.left();
            if(xmax > explicitRect.right()) xmax = explicitRect.right();
            if(ymin < explicitRect.bottom()) ymin = explicitRect.bottom();
            if(ymax > explicitRect.top()) ymax = explicitRect.top();
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


    for (uint index = 0; index < m_frames.size(); index++)
    {
        const ldBezierPaths &dataVect = m_frames[index];
        for (uint p = 0; p < dataVect.size(); p++)
        {
            ldBezierPath path = dataVect[p];
            path.scale(f2x, f2y);
            path.translate(ldVec2(f1x, f1y));
            m_frames[index][p] = path;
        }
    }


}

void ldAnimationSequenceBezier::removeblank() {

    int nremoved = 0;
    for (uint index = 0; index < m_frames.size(); index++)
    {
        bool remove = true;
        ldBezierPaths dataVect = m_frames[index];
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

void ldAnimationSequenceBezier::scale(float xs, float ys) {
    for (ldBezierPaths &paths : m_frames) {
        for (ldBezierPath &path : paths) {
            path.scale(xs, ys);
        }
    }

}

void ldAnimationSequenceBezier::move(float xt, float yt) {
    for (ldBezierPaths &paths : m_frames) {
        for (ldBezierPath &path : paths) {
            path.translate(ldVec2(xt, yt));
        }
    }

}

void ldAnimationSequenceBezier::drawFrameBezier3x(ldRendererOpenlase* r, int index, float fstart, float fend, uint32_t c1, uint32_t c2) {

    if (index < 0 || (uint) index >= m_frames.size()) return;

        ldRendererOpenlase* renderer = r;
        ldBezierPaths dataVect = m_frames[index];
        {
            //
            for (const ldBezierPath &bezierPath : dataVect)
            {
//                if (bezierTab.size() < 2) continue;
                //uint32_t color = ldColorUtil::colorHSV(0, 0, 1);
                uint32_t c = color;
                float f = 0;

                renderer->begin(OL_BEZIERSTRIP);
                for (uint i = 0; i<bezierPath.size(); i++)
                {
                    f = (float)i / (bezierPath.size());
                    c = (f >= fstart && f <= fend) ? (c1) : (c2);
                    const ldBezierCurve &b = bezierPath.data()[i];
                    renderer->vertex3(b.start().x, b.start().y, 0, c);
                    renderer->vertex3(b.control1().x, b.control1().y, 0, c);
                    renderer->vertex3(b.control2().x, b.control2().y, 0, c);
                }
                f = 1;
                c = (f >= fstart && f <= fend) ? (c1) : (c2);
                if(!bezierPath.empty()) {
                    const ldBezierCurve &b = bezierPath.data()[0];
                    renderer->vertex3(b.start().x, b.start().y, 0, c);
                }
                renderer->end();
            }
        }

}



bool ldAnimationSequenceBezier::load2(const QString &filePath) {
    QByteArray data = readFile(filePath);

    //QByteArray blob = file.readAll();
    //QTextStream in(&file);
    //QB out(&file);
    QDataStream in(data);

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
            ldBezierPath &path = m_frames[i][j];
            //line = in.readLine(); //out << "Curve " << j << endl;
            //line = in.readLine(); //out << "Point Count" << endl;
            //line = in.readLine(); //out << npoints << endl;
            //int npoints = line.toInt();
            int npoints; in >> npoints;

            std::vector<BezierCurve> b;
            b.resize(npoints);
            in.readRawData((char*)b.data(), sizeof(BezierCurve)*npoints);
            for (int k = 0; k < npoints; k++) {
                ldBezierCurve curve;
                curve.setStart(b[k].start);
                curve.setEnd(b[k].end);
                curve.setControl1(b[k].control1);
                curve.setControl2(b[k].control2);
                path.add(curve);
            }

            //in.readRawData((char*)frames[i][j].data(), sizeof(BezierCurve2)*npoints);
            if (0) for (int k = 0; k < npoints; k++) {
                    ldBezierCurve curve;
                    float sx, sy, ex, ey, c1x, c1y, c2x, c2y;
                    in >> sx;
                    in >> sy;
                    in >> ex;
                    in >> ey;
                    in >> c1x;
                    in >> c1y;
                    in >> c2x;
                    in >> c2y;

                    curve.setStart(ldVec2(sx, sy));
                    curve.setEnd(ldVec2(ex, ey));
                    curve.setControl1(ldVec2(c1x, c1y));
                    curve.setControl2(ldVec2(c2x, c2y));
                    path.add(curve);
                }
        }
    }
    //line = in.readLine(); //out << "End" << endl;

    return true;
}

bool ldAnimationSequenceBezier::load4(const QString &filePath)
{
    QByteArray data = readFile(filePath);

    QDataStream in(data);

    char res[5];
    in.readRawData(res, 5);

    int renderAlgInt;
    in >> renderAlgInt;
    m_renderAlg = RenderAlg(renderAlgInt);

    int nframes; in >> nframes;
    Q_ASSERT_X(nframes >= 0, "ldAnimationSequenceBezier", "nframes is negative!");

    m_frames.resize(nframes);

    for (int i = 0; i < 8; i++) {
        in >> keyStart[i];
        in >> keyEnd[i];
    }

    in >> sourceFPS;
    in >> sourceBPM;

    for (int i = 0; i < nframes; i++) {
        int ncurves;
        in >> ncurves;
        m_frames[i].resize(ncurves);
        for (int j = 0; j < ncurves; j++) {
            ldBezierPath &path = m_frames[i][j];
            // read color
            uint32_t pathColor;
            in >> pathColor;
            path.setColor(pathColor);

            // read gradient
            Gradient gr;
            in >> gr.x1;
            in >> gr.y1;
            in >> gr.x2;
            in >> gr.y2;

            int grCount;
            in >> grCount;
            for(int k = 0; k < grCount; k++) {
                GradientStop stop;
                in >> stop.offset;
                in >> stop.color;
                gr.stops.push_back(stop);
            }

            ldGradient bGr;
            bGr.setX1(gr.x1);
            bGr.setY1(gr.y1);
            bGr.setX2(gr.x2);
            bGr.setY2(gr.y2);
            for(const GradientStop &stop : gr.stops) {
                ldGradientStop bStop(stop.offset, stop.color);
                bGr.addStop(bStop);
                m_isGradient = true;
            }
            path.setGradient(bGr);


            // read bezier curves
            int npoints;
            in >> npoints;
            std::vector<BezierCurve> b;
            b.resize(npoints);
            in.readRawData((char*)b.data(), sizeof(BezierCurve)*npoints);
            for (int k = 0; k < npoints; k++) {
                ldBezierCurve curve;
                curve.setStart(b[k].start);
                curve.setEnd(b[k].end);
                curve.setControl1(b[k].control1);
                curve.setControl2(b[k].control2);
                path.add(curve);
            }
        }
    }

    return true;
}


QByteArray ldAnimationSequenceBezier::readFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists()) {
        QString securedFilePath = filePath + ldSimpleCrypt::LDS_EXTENSION;
        if(QFile::exists(securedFilePath)) {
            return ldSimpleCrypt::instance()->decrypt(securedFilePath);
        }

        qWarning() << "error::ldSvgReader::loadSvg: file does not exist" << filePath << file.errorString();
        return QByteArray();
    }

    if(filePath.endsWith(ldSimpleCrypt::LDS_EXTENSION)) {
        return ldSimpleCrypt::instance()->decrypt(filePath);
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "error::ldSvgReader::loadSvg: Qt file issue" << file.errorString();
        return QByteArray();
    }

    return file.readAll();
}


