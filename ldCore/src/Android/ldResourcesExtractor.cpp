#include <ldCore/Android/ldResourcesExtractor.h>

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QStandardPaths>
#include <QtCore/QTimer>

#ifdef Q_OS_ANDROID
#include <laserdocklib/ldAndroidGlobals.h>
#endif

#include <ldCore/ldCore.h>
#include <ldCore/Android/ldZipExtractor.h>

ldResourcesExtractor::ldResourcesExtractor(QObject *parent)
    : QObject(parent)
    , m_currentFileIndex(0)
    , m_fileCount(0)
    , m_progress(0)
{
}

ldResourcesExtractor::~ldResourcesExtractor()
{
}

void ldResourcesExtractor::init(const QString &packageName, int mainVersionCode, int patchVersionCode)
{
    int resVersionCode = getExistingResVersionCode();

    auto createZipExtractor = [&](const QString &zipFilePath, bool isRemoveDir) {
        std::unique_ptr<ldZipExtractor> zipExtractor(new ldZipExtractor());
        zipExtractor->setRemoveDir(isRemoveDir);
        connect(zipExtractor.get(), &ldZipExtractor::progressChanged, this, &ldResourcesExtractor::update_progress);
        connect(zipExtractor.get(), &ldZipExtractor::finished, this, &ldResourcesExtractor::onZipExtractorFinished);
        zipExtractor->init(zipFilePath, ldCore::instance()->resourceDir());
        m_zipExtractors.emplace_back(zipExtractor.release());
    };

    if(resVersionCode < mainVersionCode) {
        QString mainZipFilePath = findObbFilePath("main", packageName, mainVersionCode);
        if(mainZipFilePath.isEmpty())
            return;

        createZipExtractor(mainZipFilePath, true);
    }

    if(resVersionCode < patchVersionCode) {
        QString patchZipFilePath = findObbFilePath("patch", packageName, patchVersionCode);
        if(patchZipFilePath.isEmpty())
            return;

        createZipExtractor(patchZipFilePath, false);
    }

    update_fileCount(m_zipExtractors.size());
}

void ldResourcesExtractor::startExtraction()
{
    qDebug() << __FUNCTION__;

    if(m_zipExtractors.empty()) {
        emit finished(false, tr("Nothing to extract"));
        return;
    }

    m_zipExtractors[0]->startExtraction();
}


void ldResourcesExtractor::onZipExtractorFinished(bool isOk, const QString &errorMesssage)
{
    if(!isOk) {
        emit finished(isOk, errorMesssage);
        return;
    }

    update_currentFileIndex(m_currentFileIndex+1);

    if(m_currentFileIndex >= m_zipExtractors.size()) {
        emit finished(isOk, errorMesssage);
        return;
    }

    update_progress(0);
    m_zipExtractors[m_currentFileIndex]->startExtraction();
}

QString ldResourcesExtractor::findObbFilePath(const QString &obbType, const QString &packageName, int resourcesVersionCode)
{
    QString resourcesFilePath;

    QString resourcesFileName = obbType + "." + QString::number(resourcesVersionCode) + "." + packageName + ".obb";

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
            QJniObject mediaDir = QJniObject::callStaticObjectMethod("android/os/Environment", "getExternalStorageDirectory", "()Ljava/io/File;");
            QJniObject mediaPath = mediaDir.callObjectMethod( "getAbsolutePath", "()Ljava/lang/String;" );
#if QT_VERSION >= 0x060000
            QJniObject activity = ldAndroidActivityObject;
            QJniObject package = activity.callObjectMethod("getPackageName", "()Ljava/lang/String;");
            QString androidPackageName = package.toString();
#else
            QJniObject activity = QJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative", "activity", "()Landroid/app/Activity;");
            QJniObject package = activity.callObjectMethod("getPackageName", "()Ljava/lang/String;");
            QString androidPackageName = package.toString();
#endif

            QString dataAbsPath = mediaPath.toString() + "/Android/obb/" + androidPackageName + "/" + resourcesFileName;

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

int ldResourcesExtractor::getExistingResVersionCode()
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

    return resVersionCode;
}

