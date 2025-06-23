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

#include "ldCore/Filter/ldHardwareFilter.h"

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include "ldCore/ldCore.h"
#include "ldCore/Filter/ldDeadzoneFilter.h"
#include "ldCore/Filter/ldFilterManager.h"
#include "ldCore/Filter/ldProjectionUtil.h"
#include "ldCore/Data/ldFrameBuffer.h"

// simple moving average template class
template <typename T, typename Total, size_t N>
class Moving_Average
{
  public:
    Moving_Average& operator()(T sample)
    {
        if (num_samples_ < N)
        {
            samples_[num_samples_++] = sample;
            total_ += sample;
        }
        else
        {
            T& oldest = samples_[num_samples_++ % N];
            if (num_samples_== 0) { // wraparound detect (wraps at 2^64 or 2^32 )
                samples_[num_samples_++] = sample;
                total_ = sample;
            } else {
                total_ += sample - oldest;
                oldest = sample;
            }
        }
        return *this;
    }

  operator double() const { return total_ / std::min(num_samples_, N); }

  private:
    T samples_[N];
    size_t num_samples_{0};
    Total total_{0};
};

class ldLaserPowerLimiter : public QObject
{
 Q_OBJECT
public:
    ldLaserPowerLimiter(QObject *parent = nullptr) : QObject(parent)
    , m_powerLimiterFilter(new ldPowerFilter)
    , m_powerAverager(new Moving_Average<float,double,power_limiter_num_samples>)

    {
        connect(this,&ldLaserPowerLimiter::deviceActiveUpdate,this,&ldLaserPowerLimiter::onDeviceActiveUpdated);
        connect(this,&ldLaserPowerLimiter::deviceTemperatureUpdate,this,&ldLaserPowerLimiter::onDeviceTemperatureUpdated);
        connect(this,&QObject::destroyed,this,[&](){
            if (m_timer) {
                disconnect(m_timer.get(),&QTimer::timeout,this,&ldLaserPowerLimiter::onUpdateAverager);
                m_timer->stop();
                m_timer.reset();
            }
        });
    }

    ~ldLaserPowerLimiter() {
    }

    void process(ldVertex &input)
    {
        m_powerLimiterFilter->process(input);
    }

signals :
    void deviceTemperatureUpdate(int tempDegC);
    void deviceActiveUpdate(bool isActive);
public slots:

    void onDeviceTemperatureUpdated(int tempDegC) {
        m_lastTemperatureDegC = tempDegC;
        //qDebug() << "filter laser power, temp=" << tempDegC;
    }

    void onUpdateAverager() {
        //qDebug() << __FUNCTION__ << "timer";
        m_tempAveragedDegC =  m_powerAverager->operator ()(m_lastTemperatureDegC);
        //qDebug() << "averaged temp =" << m_tempAveragedDegC;
        if (m_tempAveragedDegC>=power_limit_start_temp) {
            const float range = power_limit_end_temp - power_limit_start_temp;
            float result = (m_tempAveragedDegC-power_limit_start_temp)/range;
            float new_power_scale_factor = 1.0f - (std::min(result,1.0f) * (1.0f-power_limit_scale_factor));
            //qDebug() << "laser power limit =" << new_power_scale_factor;
            m_powerLimiterFilter.get()->m_brightness = new_power_scale_factor;
        } else {
            m_powerLimiterFilter.get()->m_brightness = 1.0f;
        }
    }

    void onDeviceActiveUpdated(bool isActive) {
        if (isActive==m_isActive) return;

        m_isActive = isActive;
        if (isActive) {
            m_timer.reset(new QTimer());
            connect(m_timer.get(),&QTimer::timeout,this,&ldLaserPowerLimiter::onUpdateAverager);
            m_timer->setInterval(update_period_ms);
            m_timer->start();
        } else {
            disconnect(m_timer.get(),&QTimer::timeout,this,&ldLaserPowerLimiter::onUpdateAverager);
            m_timer->stop();
            m_timer.reset();
        }
    }

private:
    const int update_period_ms = 100; // how often to pass current temperature into rolling averager
    const float power_limit_scale_factor = 0.5f; // amount of laser power reduction when at the end temp
    const float power_limit_start_temp = 60.0f; // laser power will start to be reduced from this temperature
    const float power_limit_end_temp = 75.0f; // laser power will be reduced to its minimum when this temperature is reached (laser cut-off temp is 76 Deg C in firmware).
    static const size_t power_limiter_num_samples = 150; // number of samples in rolling averager

    std::unique_ptr<ldPowerFilter> m_powerLimiterFilter;
    std::unique_ptr<Moving_Average<float,double,power_limiter_num_samples>> m_powerAverager;

    std::unique_ptr<QTimer> m_timer;
    int m_lastTemperatureDegC{0};
    double m_tempAveragedDegC{0};
    bool m_isActive{false};
};

ldHardwareFilter::ldHardwareFilter(ldScaleFilter *globalScaleFilter, QObject *parent)
    : QObject(parent)
    , m_galvoAverager(new Moving_Average<float,double,galvo_limiter_num_samples>)
    , m_borderFilter(new ldDeadzoneFilter())
    , m_colorFilter(new ldColorFilter())
    , m_colorCurveFilter(new ldColorCurveFilter())
    , m_deadzoneFilter(new ldDeadzoneFilter())
    , m_flipFilter(new ldFlipFilter())
    , m_powerFilter(new ldPowerFilter)
    , m_projectionBasic(new ldProjectionBasic)
    , m_rotateFilter(new ldRotateFilter)
    , m_scaleFilter(new ldScaleFilter())
    , m_shiftFilter(new ldShiftFilter(std::vector<ldScaleFilter*>{m_scaleFilter.get(), globalScaleFilter }))
    , m_ttlFilter(new ldTtlFilter())
    , m_laserPowerFilter(new ldLaserPowerLimiter())
{
    qRegisterMetaType<ldVertexFrame>("ldVertexFrame");

    m_colorCurveFilter->m_enabled = true;

    connect(this,&ldHardwareFilter::deviceTemperatureUpdated,m_laserPowerFilter.get(),&ldLaserPowerLimiter::deviceTemperatureUpdate);
    connect(this,&ldHardwareFilter::activeChanged,m_laserPowerFilter.get(),&ldLaserPowerLimiter::deviceActiveUpdate);

    // set up deadzone with default zone
    m_deadzoneFilter->resetToDefault();
    // set up border
    ldDeadzoneFilter::Deadzone deadzone;
    deadzone.setAttenuation(1);
    deadzone.setWidth(1.01f);
    deadzone.setHeight(1.01f);
    deadzone.moveLeft(0);
    deadzone.moveTop(0);
    m_borderFilter->add(deadzone);
    m_borderFilter->setEnabled(true);
    m_borderFilter->setReverse(true);

    // data for offset correction
    // init old[] buffer with zeroes
    ldVertex t;
    t.x() = t.y() = 0;
    t.clearColor();
    for (int i = 0; i < OFFSET_MAX; i++) old[i] = t;


    m_lastFrame.resize(ldFrameBuffer::FRAMEBUFFER_CAPACITY);


}

ldHardwareFilter::~ldHardwareFilter()
{
}

void ldHardwareFilter::setActive(bool isActive)
{
//    qDebug() << "######################### active = " << isActive;
    if (isActive!=m_isActive) {
        m_isActive = isActive;
        emit activeChanged(isActive);
    }
}

ldColorFilter *ldHardwareFilter::colorFilter() const
{
    return m_colorFilter.get();
}

ldColorCurveFilter *ldHardwareFilter::baseColorCurveFilter() const
{
    return m_colorCurveFilter.get();
}

void ldHardwareFilter::processFrame(const ldVertexFrame &frame)
{
    m_lastFrame = frame;

    for(ldVertex &v : m_lastFrame.frame()) {
        ldCore::instance()->filterManager()->dataFilter()->processFilter(v);
        processFrameV(v);
    }


    // before putting it into buffer we have to make minor polish
    for(ldVertex &v : m_lastFrame.frame())
        processSafeLaserOutput(v);

    m_deadzoneFilter->processFrame(m_lastFrame.frame());

    emit frameProcessed(m_lastFrame);
}

void ldHardwareFilter::processVertex(ldVertex &v)
{
    processSafeLaserOutput(v);
    m_deadzoneFilter->processFilter(v);
}

void ldHardwareFilter::processFrameV(ldVertex &v)
{
    bool mode_disable_scale = ldCore::instance()->filterManager()->dataFilter()->frameModes & FRAME_MODE_SKIP_TRANSFORM;

    if (!mode_disable_scale) {
        m_flipFilter->processFilter(v);
        m_scaleFilter->processFilter(v);
        m_rotateFilter->processFilter(v);
        m_shiftFilter->processFilter(v);
    }

    bool mode_disable_colorcorrection = ldCore::instance()->filterManager()->dataFilter()->frameModes & FRAME_MODE_DISABLE_COLOR_CORRECTION;
    if (!mode_disable_colorcorrection) {
        // ttl filter should be before color adjustments
        m_ttlFilter->processFilter(v);
        m_colorCurveFilter->processFilter(v);
        m_colorFilter->processFilter(v);
    }

    // keystone
    bool mode_disable_projection = ldCore::instance()->filterManager()->dataFilter()->frameModes & FRAME_MODE_SKIP_TRANSFORM;
    if (!mode_disable_projection) {
        if(!m_projectionBasic->isNullTransform()) {
            // map
            float x = v.x();
            float y = v.y();
            // correct for size
            x *= 1.0f/m_projectionBasic->maxdim();
            y *= 1.0f/m_projectionBasic->maxdim();
            // transform
            m_projectionBasic->transform(x, y);
            v.x() = x;
            v.y() = y;
        }
#ifdef LD_CORE_ENABLE_OPENCV
        // keystone per corner
        if (!m_projectionBasic->isNullKeystoneTransform()) {
            ldVec2 pt = m_projectionBasic->applyCornerKeystone(v.x(), v.y());
            v.x() = pt.x;
            v.y() = pt.y;
        }
#endif
    }

    if(!mode_disable_colorcorrection)
        m_powerFilter->processFilter(v);

    // borders and clamp
    m_borderFilter->processFilter(v);
    v.x() = std::min<float>(std::max<float>(v.x(), -1), 1);
    v.y() = std::min<float>(std::max<float>(v.y(), -1), 1);
}


void ldHardwareFilter::resetFilter()
{
    m_deadzoneFilter->resetFilter();
}

ldVertexFrame &ldHardwareFilter::lastFrame()
{
    return m_lastFrame;
}

ldDeadzoneFilter *ldHardwareFilter::deadzone() const
{
    return m_deadzoneFilter.get();
}

ldFlipFilter *ldHardwareFilter::flipFilter()
{
    return m_flipFilter.get();
}

ldPowerFilter *ldHardwareFilter::powerFilter() const
{
    return m_powerFilter.get();
}

ldRotateFilter *ldHardwareFilter::rotateFilter() const
{
    return m_rotateFilter.get();
}


ldScaleFilter *ldHardwareFilter::scaleFilter() const
{
    return m_scaleFilter.get();
}

ldShiftFilter *ldHardwareFilter::shiftFilter() const
{
    return m_shiftFilter.get();
}

ldTtlFilter *ldHardwareFilter::ttlFilter() const
{
    return m_ttlFilter.get();
}

void ldHardwareFilter::setKeystoneX(float keystoneX)
{
    m_projectionBasic->setPitch(keystoneX);
}

void ldHardwareFilter::setKeystoneY(float keystoneY)
{
    m_projectionBasic->setYaw(keystoneY);
}

#ifdef LD_CORE_KEYSTONE_CORRECTION
void ldHardwareFilter::setTopLeftXKeystone(float topLeftXValue)
{
    m_projectionBasic->setTopLeftXKeystone(topLeftXValue);
}
void ldHardwareFilter::setTopLeftYKeystone(float topLeftYValue)
{
    m_projectionBasic->setTopLeftYKeystone(topLeftYValue);
}

void ldHardwareFilter::setTopRightXKeystone(float topRightXValue)
{
    m_projectionBasic->setTopRightXKeystone(topRightXValue);
}
void ldHardwareFilter::setTopRightYKeystone(float topRightYValue)
{
    m_projectionBasic->setTopRightYKeystone(topRightYValue);
}

void ldHardwareFilter::setBottomLeftXKeystone(float bottomLeftXValue)
{
    m_projectionBasic->setBottomLeftXKeystone(bottomLeftXValue);
}
void ldHardwareFilter::setBottomLeftYKeystone(float bottomLeftYValue)
{
    m_projectionBasic->setBottomLeftYKeystone(bottomLeftYValue);
}

void ldHardwareFilter::setBottomRightXKeystone(float bottomRightXValue)
{
    m_projectionBasic->setBottomRightXKeystone(bottomRightXValue);
}
void ldHardwareFilter::setBottomRightYKeystone(float bottomRightYValue)
{
    m_projectionBasic->setBottomRightYKeystone(bottomRightYValue);
}
#endif

void ldHardwareFilter::setOffset(int offset)
{
    m_offset = offset;
    // perform offset
    if (m_offset < -OFFSET_MAX + 1) m_offset = -OFFSET_MAX + 1;
    if (m_offset >  OFFSET_MAX - 1) m_offset = OFFSET_MAX - 1;
}

void ldHardwareFilter::processSafeLaserOutput(ldVertex &v)
{
    bool mode_disable_overscan = ldCore::instance()->filterManager()->dataFilter()->frameModes & FRAME_MODE_UNSAFE_OVERSCAN;
    bool mode_disable_underscan = ldCore::instance()->filterManager()->dataFilter()->frameModes & FRAME_MODE_UNSAFE_UNDERSCAN;

    m_laserPowerFilter->process(v); // perform laser power limiting based on temperature

    // new overscan protection which will shrink the image in order to limit the galvo power.
    // This algorithm uses a rolling average and still allows fast transients, but overall averages the galvo power usage.
     if (!mode_disable_overscan) {
         const float average_threshold = 0.04f;
         const float minimum_scale_factor = 0.25f;
         const float correction_factor = 0.00003f;

         float tx = v.x() * m_scalelimiter;
         float ty = v.y() * m_scalelimiter;
         float dx = fabs(tx - m_lastX1);
         float dy = fabs(ty - m_lastY1);
         float avg = m_galvoAverager->operator ()(dx+dy);
         // The values in the statement below were obtained by imperical means, by adjusting until the
         // measured galvo power usage was within acceptable limits.
         if (avg>=average_threshold && m_scalelimiter>=minimum_scale_factor) m_scalelimiter-=correction_factor;
            else if (m_scalelimiter<1.0f)  {m_scalelimiter+=correction_factor;if (m_scalelimiter>1.0f) m_scalelimiter=1.0f;}

         v.x() = m_lastX1 = tx;
         v.y() = m_lastY1 = ty;
     }


    if (!galvo_libre) {
        /*
        // scan protection - old version left in for reference
        // overscan
        // quiet algorithm
        if (!mode_disable_overscan) {
            float maxDistance = overscan_speed; // (per point)
            float tx = v.x();
            float ty = v.y();
            float dx = tx - m_lastX1;
            float dy = ty - m_lastY1;
            float dist = sqrtf(dx*dx+dy*dy);
            if (!alternate_maxspeed) {
                // algorithm with a max 2d distance.  has fewer visual artifacts but is more limiting
                float mult = 1.0;
                if (dist > maxDistance) mult = maxDistance / dist;
                tx = m_lastX1 + dx*mult;
                ty = m_lastY1 + dy*mult;
            } else {
                // alternate algorithm treats x/y galvos independantly
                if (dx > maxDistance) dx = maxDistance;
                if (dx < -maxDistance) dx = -maxDistance;
                if (dy > maxDistance) dy = maxDistance;
                if (dy < -maxDistance) dy = -maxDistance;
                tx = m_lastX1 + dx;
                ty = m_lastY1 + dy;
            }
            v.x() = m_lastX1 = tx;
            v.y() = m_lastY1 = ty;

        }*/

        // underscan
        // restless algorithm
        // if laser is too slow it will draw a circle
        bool underscan = true;
        // skip if it's a black point
        if (v.isBlank()) underscan = false;
        if (underscan && !mode_disable_underscan)
        {
            float dx = v.x() - m_lastX2;
            float dy = v.y() - m_lastY2;
            float d = sqrtf(dx*dx+dy*dy);
            if (d > underscan_speed) {
                d = 1;
                m_lastX2 = v.x();
                m_lastY2 = v.y();
            } else {
                d = 0;
            }
            //if (v.b() == 0 && v.g() == 0 && v.b() == 0) d = 1;
            // with these constants, algo takes about 2ms of constant laser to activate
            m_averaged = 0.99*m_averaged + 0.01*d;
            if (d == 1) m_averaged = 0.98*m_averaged + 0.02*d;
            float r = 0;
            float mad = 0.4f; // at least 40% of points must be under
            if (d == 0 && m_averaged < mad) r = 0.125 * (mad-m_averaged)/mad;


            if (r > 0) {
                // move laser in a circle and fade out
                m_turns += 1.0/64.0;
                m_turns -= (int) m_turns;
                // set color
                float ff = fminf(fmaxf(1.0,((mad-m_averaged)/mad)*2-1), 1);
                v.r() *= 1-ff;
                v.g() *= 1-ff;
                v.b() *= 1-ff;
            }
        }
    }


    //
    // color/pos alignment offset
    // save in old buffer in praparation for performing offset
    for (int i = OFFSET_MAX - 1; i >= 1; i--)
        old[i] = old[i - 1];
    old[0] = v;

    // convert signed offset value to array indexes coloroffset and posoffset
    int co = 0;
    int po = 0;
    if (m_offset > 0) co = m_offset;
    if (m_offset < 0) po = -m_offset;
    // take values from old buffer depending on offsets
    for (int i = 0; i < ldVertex::COLOR_COUNT; i++) v.color[i] = old[co].color[i];
    for (int i = 0; i < ldVertex::POS_COUNT; i++) v.position[i] = old[po].position[i];
}

#include "ldCore/Filter/ldHardwareFilter.moc"
