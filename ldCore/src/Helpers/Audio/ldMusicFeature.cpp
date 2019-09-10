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

#include "ldCore/Helpers/Audio/ldMusicFeature.h"

#include "ldCore/Helpers/Maths/ldGeometryUtil.h"

static void nb2_class(float in[], float out[]);

MusicFeature1::MusicFeature1() {
    // starting values for stats
    float v[3] = {0.256249f, 0.447609f, 0.273351f};
    statEstimators[0].mean = v[0];
    statEstimators[1].mean = v[1];
    statEstimators[2].mean = v[2];
    statEstimators[3].mean = 0.5;
}

void MusicFeature1::update(const ldSpectrumFrame& /*s*/, ldSoundData* psd) {

    // input values
    float centroid;
    float under80;
    float purity;
    float fuzz;
    float bass;
    float mids;
    float high;

    // calculators

    // ls array
    const int ft = 512;
    float ls[ft];

    float total = 0;
    for (int i = 0; i < ft; i++) {
        float z = psd->GetFFTValueForFrequency(30.0*(i), 0);
        ls[i] = sqrt(z*i);//log(z*(1))/log(1/1000);
        if (ls[i] < 0) ls[i] = 0;
        total += ls[i];
    }
    float average = total / ft;
    clampfp(average, 0, 10);

    // centroid
    centroid = 0;
    for (int i = 0; i < ft; i++) {
        centroid += ls[i]/average * ((float)i) / ft;
    }
    centroid /= ft;
    clampfp(centroid, 0, 10);

    // freq index with 80% mass under
    under80 = 0;
    float u8t = 0;
    for (int i = 0; i < ft; i++) {
        u8t += ls[i];
        if (u8t < 0.8*total) under80 = ((float)i) / ft;
    }
    clampfp(under80, 0, 10);

    // cheap tone purity
    purity = 0;
    for (int i = 0; i < ft; i++) {
        float f = ls[i] / average;
        if (f > 1.2) purity += f;
        //purity += f*f;
    }
    purity /= ft;
    clampfp(purity, 0, 10);
    //purity = sqrtf(purity/ft);
    //purity = 1-(1/(purity+1));

    // cheap fuzz factor
    fuzz = 0;
    for (int i = 1; i < ft-1; i++) {
        float a = ls[i];
        float b = (ls[i-1] + ls[i+1])/2;
        float c = (a-b)/(a+b);
        if (c > 0) fuzz += c*12;
        //fuzz += c*3;
    }
    fuzz /= (ft-2);
    clampfp(fuzz, 0, 1);

    // stat list
    stats[0] = centroid;
    stats[1] = under80;
    stats[2] = purity;
    stats[3] = fuzz;
    stats[4] = bass = psd->GetBass();
    stats[5] = mids = psd->GetMids();
    stats[6] = high = psd->GetHigh();

    for (int i = 0; i < nStat; i++) {
        statEstimators[i].add(stats[i]);
    }

    features[0] = statEstimators[0].mean;
    features[1] = statEstimators[1].mean;
    features[2] = statEstimators[2].mean;
    features[3] = statEstimators[3].mean;
    features[4] = statEstimators[3].variation;
    features[5] = statEstimators[4].variation;
    features[6] = statEstimators[5].variation;
    features[7] = statEstimators[6].variation;

    // fast vs slow, nb w/3 inputs
//    float v[3] = {0.256249, 0.447609, 0.273351};
    float ameans1[3] = {0.4324f, 0.7271f, 0.3776f};
    float astddevs1[3] = {0.0301f, 0.0331f, 0.0505f};
    float ameans2[3] = {0.4707f, 0.7538f, 0.4068f};
    float astddevs2[3] = {0.0256f, 0.0295f, 0.0374f};
    float p1 = 1;
    float p2 = 1;
    for (int i = 0; i < 3; i++) {
        float t;
        t = (features[i] - ameans1[i]) / astddevs1[i];
        t = fabs(t);
        p1 *= (1/(1+t));
        t = (features[i] - ameans2[i]) / astddevs2[i];
        t = fabs(t);
        p2 *= (1/(1+t));
    }
    float rock = 0;
    rock = (p2)/(p1+p2);

    // nb2 classifier
    float o[4];
    nb2_class(features, o);

    // process
    float u = 0;
    for (int i = 0; i < 4; i++) {
        os[i].rho = 0.8f;
        os[i].add(o[i]);
        u+=os[i].mean;
    }
    if (u < 0.01f) u = 0.01f;

    // apply
    statGenreClassical = os[0].mean/u;
    statGenreWorld = os[1].mean/u;
    statGenreRock = os[2].mean/u;
    statGenrePop = os[3].mean/u;
    statMoodFunky = (os[1].mean + os[3].mean) / u;
    statMoodMelodic = (os[0].mean + os[1].mean) / u;
    statMoodFast = rock;

    statGenreClassical2 = o[0]/u;
    statGenreWorld2 = o[1]/u;
    statGenreRock2 = o[2]/u;
    statGenrePop2 = o[3]/u;
    statMoodFunky2 = (o[1] + o[3]) / u;
    statMoodMelodic2 = (o[0] + o[1]) / u;

}




// generated code

/*
                     Class
Attribute        classical       world        rock  electronic
                    (0.12)      (0.14)      (0.29)      (0.45)
===============================================================*/


static const int inlen = 8;
static const int outlen = 4;

static float wector[inlen*outlen*2] ={
0.4437f,      0.4593f,      0.4544f,      0.4716f,
0.0222f,      0.0371f,      0.0196f,      0.0361f,
 0.744f,      0.7494f,      0.7257f,      0.7404f,
0.0254f,      0.0295f,      0.0267f,      0.0398f,
0.3318f,      0.3555f,      0.4155f,       0.415f,
0.0779f,      0.0886f,      0.0487f,      0.0688f,
 0.507f,      0.5373f,      0.6823f,      0.6184f,
0.1144f,      0.1095f,      0.0859f,      0.1088f,
 0.213f,      0.1998f,      0.1092f,      0.1579f,
0.0988f,      0.1124f,       0.068f,      0.0883f,
0.2932f,      0.3129f,      0.2141f,       0.268f,
0.2143f,      0.2481f,      0.1698f,      0.1731f,
0.6453f,      0.6171f,       0.477f,      0.5209f,
0.1988f,      0.2175f,      0.1969f,      0.1895f,
0.3523f,      0.3524f,      0.2239f,      0.2812f,
0.2293f,       0.276f,      0.1902f,      0.1805f,

};

//static float wector2[inlen*outlen*2] =
//                     {0.4442,      0.4565,      0.4538,      0.4716,
//                     0.0208,      0.0346,       0.019,      0.0361,


//                     0.7455,      0.7477,      0.7254,      0.7404,
//                     0.0246,      0.0296,      0.0268,      0.0398,


//                     0.3238,      0.3512,      0.4148,       0.415,
//                     0.0777,      0.0852,      0.0503,      0.0688,


//                     0.4936,      0.5327,      0.6807,      0.6184,
//                      0.113,      0.1064,      0.0863,      0.1087,


//                     0.2256,      0.2059,      0.1118,      0.1579,
//                     0.0983,      0.1062,      0.0718,      0.0883,


//                     0.2911,       0.322,      0.2152,       0.268,
//                     0.1894,      0.2349,      0.1568,      0.1731,


//                     0.6542,      0.6252,      0.4776,      0.5209,
//                     0.1764,      0.1982,      0.1692,      0.1895,


//                     0.3519,      0.3676,       0.225,      0.2812,
//                     0.2019,      0.2681,      0.1753,      0.1805};

/*


Time taken to build model: 0.02 seconds*/


static float getm(int i, int o) {
    int x = outlen*2*i;
    x = x + o + 0;
    return wector[x];
}
static float gets(int i, int o) {
    int x = outlen*2*i;
    x = x + o + 4;
    return wector[x];
}



#include <cmath>
/*
double phi(double x)
{
    // constants
    double a1 =  0.254829592;
    double a2 = -0.284496736;
    double a3 =  1.421413741;
    double a4 = -1.453152027;
    double a5 =  1.061405429;
    double p  =  0.3275911;

    // Save the sign of x
    int sign = 1;
    if (x < 0)
        sign = -1;
    x = fabs(x)/sqrt(2.0);

    // A&S formula 7.1.26
    double t = 1.0/(1.0 + p*x);
    double y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);

    return 0.5*(1.0 + sign*y);
}
double erf(double x)
{
 double y = 1.0 / ( 1.0 + 0.3275911 * x);
 return 1 - (((((
        + 1.061405429  * y
        - 1.453152027) * y
        + 1.421413741) * y
        - 0.284496736) * y
        + 0.254829592) * y)
        * exp (-x * x);
}
static double GetBellCurvePoint(double Percentage, double Midpoint = 0)
{
    if (Percentage > Midpoint) {
        Percentage = 1 - Percentage;
        return 1 - ((Percentage - ((1 - Percentage) * Percentage)) * (1 / (1 - Midpoint)));
    } else {
        return (Percentage - ((1 - Percentage) * Percentage)) * (1 / Midpoint);
    }
}*/

static float pdf_gaussian(float x, float m = 0, float s = 1) {return ( 1 / ( s * sqrt(2*M_PI) ) ) * exp( -0.5 * pow( (x-m)/s, 2.0 ) );}


static void nb2_class(float in[], float out[]) {

    float prior[outlen];
    prior[0] = log(0.2);
    prior[1] = prior[0];
    prior[2] = log(0.3);
    prior[3] = prior[2];
//    float g = 1.0/1000.0f;
    for (int i = 0; i < outlen; i++) {
        float p = 1;
        float l = prior[i];
        for (int j = 0; j < inlen; j++) {
            float d = in[j] - getm(j, i);
            float z = d / gets(j, i);
            z = fabs(z);
            if (z > 5) z = 5;
            //p *= 1/(1+z);
            //p *= phi(z);
            //p *= 1-erf(abs(z));
            //p *= GetBellCurvePoint(abs(z));
            p *= pdf_gaussian(in[j], getm(j, i), gets(j, i));
            //p *= pdf_gaussian()
            l += logf(pdf_gaussian(z));
        }
        //out[i] = p;
        out[i] = expf(l);
        if (!(out[i] >= 0)) out[i] = 0;
        if (!(out[i] <= 1)) out[i] = 1;
    }

    float total = 0;
    for (int i = 0; i < outlen; i++) {
        total += out[i];
    }
    //if (total < g) total = g;
    for (int i = 0; i < outlen; i++) {
        out[i] /= total;
        if (!(out[i] >= 0)) out[i] = 0;
        if (!(out[i] <= 1)) out[i] = 1;
    }


    //////////////////


/*
    static int ac = 0; ac++; ac %= 30*5; if (!ac){
        //if (rock > 0.66) qDebug() << "Genre : fast";
        //else if (rock < 0.33) qDebug() << "Genre : slow";
        //else qDebug() << "Genre : ????";
        //qDebug() << "funk factor: " << funkEstimator.mean;

        //qDebug() << "funk factor: " << funkEstimator.mean;
   }

    char* fileName = "mmt3.txt";
    //char* cat = "classical";
    char* cat = "rock";
    static int snapshots = 0;//1000
    float interval = 3;

    static float timer = 0;
    timer += AUDIO_UPDATE_DELTA_S;
    if (timer > interval) {
        timer -= interval;
        if (snapshots >= 1) {
            snapshots--;
            QString string;
            for (int i = 0; i < nFeat; i++) {
                string.append(QString::number(features[i]));
                string.append(", ");
            }
            string.append(cat);
            string.append("\r\n");
            string.append("\0");
            QFile file(fileName);
            file.open(QIODevice::Append);
            QTextStream stream(&file);
            stream << string;
            qDebug() << string;
            file.close();
        }

    }*/

}
