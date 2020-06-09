#include <ldCore/Android/ldResourcesExtractor.h>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#endif

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QStandardPaths>
#include <QtCore/QTimer>

#include <ldCore/ldCore.h>
#include <ldCore/Android/ldZipExtractor.h>

ldResourcesExtractor::ldResourcesExtractor(QObject *parent)
    : QObject(parent)
    , m_needExtraction(false)
    , m_progress(0)
    , m_zipExtractor(new ldZipExtractor(this))
{
    connect(m_zipExtractor, &ldZipExtractor::progressChanged, this, &ldResourcesExtractor::update_progress);
    connect(m_zipExtractor, &ldZipExtractor::finished, this, &ldResourcesExtractor::finished);
}

ldResourcesExtractor::~ldResourcesExtractor()
{
}

void ldResourcesExtractor::init(const QString &packageName, int resourcesVersionCode)
{
    QString zipFilePath = findObbFilePath(packageName, resourcesVersionCode);
    if(!zipFilePath.isEmpty())
        m_zipExtractor->init(zipFilePath, ldCore::instance()->resourceDir());

    // check if we need to extract new resources
    checkNeedExtraction(resourcesVersionCode);
}

void ldResourcesExtractor::startExtraction()
{
    qDebug() << __FUNCTION__;

    m_zipExtractor->startExtraction();
}


void ldResourcesExtractor::checkNeedExtraction(int resourcesVersionCode)
{
    int resVersionCode = -1;
    QString resVersionPath = ldCore::instance()->resourceDir() + "/version";
    if(QFile::exists(resVersionPath)) {
        QFile resVersionFile(resVersionPath);
        resVersionFile.open(QIODevice::ReadOnly);
        QByteArray resVersionData = resVersionFile.readAll();
        bool ok;
        int resFileVersionCode = resVersionData.toInt(&ok);
        if(ok) {
            resVersionCode = resFileVersionCode;
        }
    }
    if(resVersionCode == -1
        || resVersionCode < resourcesVersionCode) {
        update_needExtraction(true);
    }
}

QString ldResourcesExtractor::findObbFilePath(const QString &packageName, int resourcesVersionCode)
{
    QString resourcesFilePath;

    QString resourcesFileName = "main." + QString::number(resourcesVersionCode) + "." + packageName + ".obb";

    // 1st try
    // get path to resources file
    QString genericDataLocation = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).first();
    genericDataLocation += "/Android/obb/" + packageName + "/";
    resourcesFilePath = genericDataLocation + resourcesFileName;
    qDebug() << resourcesFilePath;

    bool exists = QFile::exists(resourcesFilePath);

    if(!exists) {
        // 2nd try
        qWarning() << "expected file path doesn't exist, checking other path..." << resourcesFilePath;
        QStringList dataLocations = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
        for(const QString &dataLocation : dataLocations) {
            QString resourcesPath = dataLocation + "/../../../obb/" + packageName + "/" + resourcesFileName;
            exists = QFile::exists(resourcesPath);
            qDebug() << "check " << resourcesPath << QFile::exists(resourcesPath);
            if(exists) {
                resourcesFilePath = resourcesPath;
                break;
            }
        }

#ifdef Q_OS_ANDROID
        // 3rd try
        if(!exists) {
            QAndroidJniObject mediaDir = QAndroidJniObject::callStaticObjectMethod("android/os/Environment", "getExternalStorageDirectory", "()Ljava/io/File;");
            QAndroidJniObject mediaPath = mediaDir.callObjectMethod( "getAbsolutePath", "()Ljava/lang/String;" );
            QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative", "activity", "()Landroid/app/Activity;");
            QAndroidJniObject package = activity.callObjectMethod("getPackageName", "()Ljava/lang/String;");

            QString dataAbsPath = mediaPath.toString() + "/Android/obb/" + package.toString() + "/" + resourcesFileName;

            exists = QFile::exists(dataAbsPath);
            qWarning() << "expected file pathes doesn't exist, checking the last one path..." << dataAbsPath << exists;

            if(exists) {
                resourcesFilePath = dataAbsPath;
            }
        }
#endif
    }

    return resourcesFilePath;
}

#include "ldResourcesExtractor.moc"
