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

#ifndef LDSETTINGS_H
#define LDSETTINGS_H

#include <memory>

#include <QtCore/QSettings>

#include <ldCore/ldCore_global.h>

class LDCORESHARED_EXPORT ldSettingsWrapper
{
public:
    static ldSettingsWrapper *instance();

    ~ldSettingsWrapper();

    QSettings *settings();

private:
    ldSettingsWrapper();

    std::unique_ptr<QSettings> m_settings;
};


inline LDCORESHARED_EXPORT QSettings* ldSettings() {
    return ldSettingsWrapper::instance()->settings();
}

#endif // LDSETTINGS_H
