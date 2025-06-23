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
    sync();
}

QSettings *ldSettingsWrapper::settings()
{
    return m_settings.get();
}

void ldSettingsWrapper::setValue(const QString &key, const QVariant &value)
{
    if(m_isArrayReadWriting) {
      m_settings->setValue(key, value);
      return;
    }

    QString absoluteKey = getAbsoluteKey(key);

    m_cache.insert(absoluteKey, value);
    m_dirtyCache.insert(absoluteKey, value);

// on iOS and Android settings are not stored, just sync them each time
#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
    sync();
#endif
}

QVariant ldSettingsWrapper::value(const QString &key, const QVariant &defaultValue) const
{
    if(m_isArrayReadWriting)
      return m_settings->value(key, defaultValue);

    QString absoluteKey = getAbsoluteKey(key);

    if(m_cache.contains(absoluteKey))
      return m_cache[absoluteKey];

    m_cache.insert(absoluteKey, m_settings->value(key, defaultValue));

    return m_cache[absoluteKey];
}

bool ldSettingsWrapper::contains(const QString &key) const
{
    if(m_isArrayReadWriting)
      return m_settings->contains(key);

    QString absoluteKey = getAbsoluteKey(key);

    if(m_cache.contains(absoluteKey))
      return true;

    return m_settings->contains(absoluteKey);
}

void ldSettingsWrapper::remove(const QString &key)
{
    if(m_isArrayReadWriting) {
      m_settings->remove(key);
      return;
    }

    QString absoluteKey = getAbsoluteKey(key);

    m_cache.remove(absoluteKey);
    auto it = m_cache.begin();
    while(it != m_cache.end()) {
      QString eachKey = it.key();
      if(eachKey.startsWith(absoluteKey)) {
          it = m_cache.erase(it);
      } else {
          it++;
      }
    }

    m_dirtyCache.remove(absoluteKey);
    it = m_dirtyCache.begin();
    while(it != m_dirtyCache.end()) {
      QString eachKey = it.key();
      if(eachKey.startsWith(absoluteKey)) {
          it = m_dirtyCache.erase(it);
      } else {
          it++;
      }
    }

    m_settings->remove(key);
}

int ldSettingsWrapper::beginReadArray(const QString &prefix)
{
    m_isArrayReadWriting = true;
    return m_settings->beginReadArray(prefix);
}

void ldSettingsWrapper::beginWriteArray(const QString &prefix, int size)
{
    m_isArrayReadWriting = true;
    m_settings->beginWriteArray(prefix, size);
}

void ldSettingsWrapper::endArray()
{
    m_isArrayReadWriting = false;
    return m_settings->endArray();
}

void ldSettingsWrapper::beginGroup(const QString &prefix)
{
    m_settings->beginGroup(prefix);
}

void ldSettingsWrapper::endGroup()
{
    m_settings->endGroup();
}

QStringList ldSettingsWrapper::allKeys() const
{
    return m_settings->allKeys();
}

QStringList ldSettingsWrapper::childGroups() const
{
    return m_settings->childGroups();
}

void ldSettingsWrapper::sync()
{
    for(const QString &key : m_dirtyCache.keys())
      m_settings->setValue(key, m_dirtyCache[key]);

    m_settings->sync();

    m_dirtyCache.clear();
}

void ldSettingsWrapper::renameGroup(const QStringList &pathToGroup, const QString &newGroupName)
{
    if(pathToGroup.isEmpty())
        return;

    QVariantList allValues;

    // dive in into the group
    for(int i = 0; i < pathToGroup.size(); i++) {
        QString eachGroup = pathToGroup[i];
        beginGroup(eachGroup);
    }

    // store old values
    QStringList keys = allKeys();
    for(const QString &key : keys) {
        allValues.push_back(value(key));
    }
    endGroup();

    // remove old group
    remove(pathToGroup[pathToGroup.size() - 1]);

    // store new values
    beginGroup(newGroupName);
    Q_ASSERT(allValues.length() == keys.length());
    for(int i = 0; i < keys.length(); i++) {
        setValue(keys[i], allValues[i]);
    }

    for(int i = 0; i < pathToGroup.size(); i++)
        endGroup();
}

void ldSettingsWrapper::setArrayIndex(int i)
{
    m_settings->setArrayIndex(i);
}

ldSettingsWrapper::ldSettingsWrapper()
{
    QString storageLocation = ldCore::instance()->storageDir();
    QDir().mkpath(storageLocation);
    m_settings.reset(new QSettings(storageLocation + "/config.ini", QSettings::IniFormat));

    qDebug() << __FUNCTION__ << m_settings->group();
}

QString ldSettingsWrapper::getAbsoluteKey(const QString &key) const
{
    return m_settings->group().isEmpty()
               ? key
               : m_settings->group() + "/" + key
        ;
}
