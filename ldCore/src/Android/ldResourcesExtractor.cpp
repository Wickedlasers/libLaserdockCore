#include <ldCore/Android/ldResourcesExtractor.h>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#endif

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QStandardPaths>
#include <QtCore/QTimer>

#include <quazip/JlCompress.h>

#include <ldCore/ldCore.h>

class ldResourcesExtractorPrivate : public QObject
{
    Q_OBJECT
public:
    explicit ldResourcesExtractorPrivate(QObject *parent = nullptr);

    void init(const QString &packageName, int resourcesVersionCode);

    void extractDir();

signals:
    void progress(int progress); // 0..1
    void finished(bool ok);

private:
    QString m_resourcesFile;

};

ldResourcesExtractorPrivate::ldResourcesExtractorPrivate(QObject *parent)
    : QObject(parent)
{
}

void ldResourcesExtractorPrivate::init(const QString &packageName, int resourcesVersionCode)
{
    QString resourcesFileName = "main." + QString::number(resourcesVersionCode) + "." + packageName + ".obb";

    // 1st try
    // get path to resources file
    QString genericDataLocation = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).first();
    genericDataLocation += "/Android/obb/" + packageName + "/";
    m_resourcesFile = genericDataLocation + resourcesFileName;
    qDebug() << m_resourcesFile;

    bool exists = QFile::exists(m_resourcesFile);

    if(!exists) {
        // 2nd try
        qWarning() << "expected file path doesn't exist, checking other path..." << m_resourcesFile;
        QStringList dataLocations = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
        for(const QString &dataLocation : dataLocations) {
            QString resourcesPath = dataLocation + "/../../../obb/" + packageName + "/" + resourcesFileName;
            exists = QFile::exists(resourcesPath);
            qDebug() << "check " << resourcesPath << QFile::exists(resourcesPath);
            if(exists) {
                m_resourcesFile = resourcesPath;
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
                m_resourcesFile = dataAbsPath;
            }
        }
#endif
    }
}

void ldResourcesExtractorPrivate::extractDir()
{
    qDebug() << __FUNCTION__ << m_resourcesFile << ldCore::instance()->resourceDir();

    if(m_resourcesFile.isEmpty() || !QFile::exists(m_resourcesFile)) {
        qWarning() << "Zip file doesn't exist";
        emit finished(false);
        return;
    }

    QStringList fileList = JlCompress::getFileList(m_resourcesFile);

    QDir(ldCore::instance()->resourceDir()).removeRecursively();
    QDir().mkpath(ldCore::instance()->resourceDir());

    QuaZip zip(m_resourcesFile);
    if(!zip.open(QuaZip::mdUnzip)) {
        qWarning() << "Zip file can't be opened";
        emit finished(false);
        return;
    }

    QDir directory(ldCore::instance()->resourceDir());
    QStringList extracted;
    if (!zip.goToFirstFile()) {
        qWarning() << "Zip file is empty";
        emit finished(false);
        return;
    }
    int i = 0;
    do {
        QString name = zip.getCurrentFileName();
        QString absFilePath = directory.absoluteFilePath(name);
        if (!JlCompress::extractFile(&zip, "", absFilePath)) {
            JlCompress::removeFile(extracted);
            qWarning() << "Zip file can't be removed" << name << absFilePath << extracted;
            emit finished(false);
            return;
        }
        extracted.append(absFilePath);

        i++;

        emit progress(static_cast<int>(i * 100.0 / fileList.size()));
    } while (zip.goToNextFile());

    zip.close();

    if(zip.getZipError()!=0) {
        JlCompress::removeFile(extracted);
        extracted =  QStringList();
    }

    qDebug() << "finished";
    emit finished(true);
}


ldResourcesExtractor::ldResourcesExtractor(QObject *parent)
    : QObject(parent)
    , m_needExtraction(false)
    , m_progress(0)
    , m_private(new ldResourcesExtractorPrivate())
{
    m_private->moveToThread(&m_workerThread);
    m_workerThread.start();
}

ldResourcesExtractor::~ldResourcesExtractor()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

void ldResourcesExtractor::init(const QString &packageName, int resourcesVersionCode)
{
    m_private->init(packageName, resourcesVersionCode);

    connect(m_private.data(), &ldResourcesExtractorPrivate::progress, this, &ldResourcesExtractor::update_progress);
    connect(m_private.data(), &ldResourcesExtractorPrivate::finished, this, &ldResourcesExtractor::finished);

    // check if we need to extract new resources
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

void ldResourcesExtractor::startExtraction()
{
    qDebug() << __FUNCTION__;

    QTimer::singleShot(0, m_private.data(), &ldResourcesExtractorPrivate::extractDir);
}


#include "ldResourcesExtractor.moc"
