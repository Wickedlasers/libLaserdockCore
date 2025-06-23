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

#include <QtCore/QHash>
#include <QtCore/QSettings>

#include <ldCore/ldCore_global.h>

class LDCORESHARED_EXPORT ldSettingsWrapper
{
public:
    static ldSettingsWrapper *instance();

    ~ldSettingsWrapper();

    QSettings *settings();

    // QSettings
    void setValue(const QString &key, const QVariant &value);
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    bool contains(const QString &key) const;
    void remove(const QString &key);

    int beginReadArray(const QString &prefix);
    void beginWriteArray(const QString &prefix, int size = -1);
    void setArrayIndex(int i);
    void endArray();

    void beginGroup(const QString &prefix);
    void endGroup();

    QStringList allKeys() const;
    QStringList childGroups() const;

    void sync();

    // rename some inner group preserving all it's childer groups and values
    void renameGroup(const QStringList &pathToGroup,
                     const QString &newGroupName);

private:    
    ldSettingsWrapper();
    QString getAbsoluteKey(const QString &key) const;

    std::unique_ptr<QSettings> m_settings;
    mutable QHash<QString, QVariant> m_dirtyCache;
    mutable QHash<QString, QVariant> m_cache;
    bool m_isArrayReadWriting = false;
};


inline LDCORESHARED_EXPORT auto ldSettings() {
    return ldSettingsWrapper::instance();
}

#endif // LDSETTINGS_H
