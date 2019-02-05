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

#include <ldCore/Helpers/SVG/ldSvgDir.h>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QtDebug>

ldSvgDir::ldSvgDir(const QString &dirPath, const QString filePrefix, int maskSize)
    : m_dirPath(dirPath)
    , m_filePrefix(filePrefix)
    , m_maskSize(maskSize)
{
    if(filePrefix.isEmpty() || maskSize == -1) {
        QStringList fileList = QDir(m_dirPath).entryList(QStringList("*.svg"), QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
        if(fileList.length() > 0) {
            QFileInfo fileInfo(fileList.first());
            QString baseName = fileInfo.baseName(); // except .svg
            for(int i = baseName.length() - 1; i >= 0; i--) {
                QChar currentChar = baseName[i];
                if(!currentChar.isDigit()) {
                    if(filePrefix.isEmpty()) m_filePrefix = baseName.mid(0, i + 1);
                    if(maskSize == -1) m_maskSize = baseName.length() - i - 1;
                    qDebug() << __FUNCTION__ << m_dirPath << m_filePrefix << m_maskSize;
                    break;
                }
            }
        }
    }
}

QString ldSvgDir::filePrefix() const
{
    return m_filePrefix;
}

int ldSvgDir::maskSize() const
{
    return m_maskSize;
}

void ldSvgDir::setFilePrefix(const QString &filePrefix)
{
    m_filePrefix = filePrefix;
}

void ldSvgDir::setMaskSize(int maskSize)
{
    m_maskSize = maskSize;
}

QStringList ldSvgDir::getSvgFiles() const
{
    QStringList res;

    QStringList files = QDir(m_dirPath).entryList(QDir::Files | QDir::NoDotAndDotDot);
    int size = files.length();

    for (int i=0;i<size+1;i++) {
        QString indexStr = QString::number(i).rightJustified(m_maskSize, '0');
        QString filePath = m_filePrefix + indexStr + ".svg";
        if(QFile::exists(m_dirPath + "/" + filePath))
            res += filePath;
    }

    return res;

}

bool ldSvgDir::copy(const QString &targetDirPath)
{
    QDir().mkpath(targetDirPath);
    QStringList svgFiles = getSvgFiles();
    for(const QString &svgFile : svgFiles) {
        bool ok = QFile::copy(m_dirPath + "/" + svgFile, targetDirPath + "/" + svgFile);
        if(!ok) {
            qWarning() << __FUNCTION__ << "Can't copy " << m_dirPath << svgFile << "to" << targetDirPath;
            QDir(targetDirPath).removeRecursively();
            return false;
        }
    }
    return true;
}
