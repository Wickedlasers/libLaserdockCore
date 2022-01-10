//
// Created by Darren Otgaar on 2018/08/19.
//

#ifndef LASERDOCK_SIM_LDSERPENTFONT_H
#define LASERDOCK_SIM_LDSERPENTFONT_H

#include <QString>
#include <sstream>
#include "ldRazer.h"
#include "ldMesh.h"
#include <iomanip>

// Modelled on the classic Asteroids vector font, each char/digit is 30 x 50

namespace ldRazer
{

    inline QString lexical_cast(const float &value, int precision=2)
    {
        return QString::number(double(value), 'f', precision);
    }

    inline QString lexical_cast(const double &value, int precision=2)
    {
        return QString::number(value, 'f', precision);
    }

    template<typename T>
    inline QString lexical_cast(const T &value, int precision = 2)
    {
        Q_UNUSED(precision);
        return QString::number(value);
    }

    class ldLaserFont
    {
    public:
        int charSpacing;

        ldLaserFont();

        ~ldLaserFont();

        void setScale(float scale)
        {
            m_scale.set(scale, scale);
            auto inv = 1.f / scale;
            m_invScale.set(inv, inv);
        }

        void setScale(const ldRazer::vec2f &scale)
        {
            m_scale = scale;
            m_invScale.set(1.f / scale.x, 1.f / scale.y);
        }

        ldRazer::vec2f getScale() const
        {
            return m_scale;
        }

        ldRazer::vec2f getInvScale() const
        {
            return m_invScale;
        }

        ldRazer::AABB2f_t getStringAABB(const QString &str);

        ldRazer::ldMesh2f_t
        buildString(const ldRazer::vec2f &P, const QString &str, uint32_t colour, bool strict = false);

        size_t buildString(ldRazer::ldMesh2f_t &mesh, const ldRazer::vec2f &P, const QString &str, uint32_t colour,
                           bool strict = false);

    private:
        ldRazer::vec2f m_scale;
        ldRazer::vec2f m_invScale;
    };

}

#endif //LASERDOCK_SIM_LDSERPENTFONT_H
