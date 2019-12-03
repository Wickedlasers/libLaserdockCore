#include <ldCore/Settings/ldSettings.h>

#include <QtCore/QDir>
#include <QtCore/QtDebug>

#include <ldCore/ldCore.h>

ldSettingsWrapper *ldSettingsWrapper::instance()
{
    static ldSettingsWrapper instance;
    return &instance;
}

ldSettingsWrapper::~ldSettingsWrapper()
{
}

QSettings *ldSettingsWrapper::settings()
{
    return m_settings.get();
}

ldSettingsWrapper::ldSettingsWrapper()
{
    QString storageLocation = ldCore::instance()->storageDir();
    QDir().mkpath(storageLocation);
    m_settings.reset(new QSettings(storageLocation + "/config.ini", QSettings::IniFormat));
}
