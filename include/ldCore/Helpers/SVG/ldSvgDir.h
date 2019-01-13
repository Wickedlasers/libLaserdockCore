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

#ifndef __ldCore__ldSvgDir__
#define __ldCore__ldSvgDir__

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include <ldCore/ldCore_global.h>

class LDCORESHARED_EXPORT ldSvgDir
{
public:
    explicit ldSvgDir(const QString &dirPath, const QString filePrefix = "", int maskSize = -1);

    QString filePrefix() const;
    void setFilePrefix(const QString &filePrefix);

    int maskSize() const;
    void setMaskSize(int maskSize);

    QStringList getSvgFiles() const;

    bool copy(const QString &targetDirPath);

private:
    QString m_dirPath;
    QString m_filePrefix;
    int m_maskSize = 1;
};

#endif /*__ldCore__ldSvgDir__*/
