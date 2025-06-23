#include <ldCore/Android/ldZipExtractor.h>

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTimer>

#include <quazip/JlCompress.h>

// ------------------------- ldZipExtractorPrivate ----------------------

class ldZipExtractorPrivate : public QObject
{
    Q_OBJECT
public:
    explicit ldZipExtractorPrivate(QObject *parent = nullptr);

    void init(const QString &zipFilePath, const QString &dirPath);
    void compressDir();
    void extractDir();

    QString zipFilePath() const;

    void setRemoveDir(bool isRemoveDir);

signals:
    void progress(int progress); // 0..1
    void finished(bool ok, const QString &errorMessage = QString());

private:
    QString m_zipFilePath;
    QString m_dirPath;

    bool m_isRemoveDir = true;
};

ldZipExtractorPrivate::ldZipExtractorPrivate(QObject *parent)
    : QObject(parent)
{
}

void ldZipExtractorPrivate::init(const QString &zipFilePath, const QString &dirPath)
{
    m_zipFilePath = zipFilePath;
    m_dirPath = dirPath;
}

void ldZipExtractorPrivate::compressDir()
{
    qDebug() << __FUNCTION__ << m_zipFilePath << m_dirPath;

    if(m_zipFilePath.isEmpty() || m_dirPath.isEmpty()) {
        qWarning() << "Zip extractor is not inited" << m_zipFilePath << m_dirPath;
        emit finished(false, tr("Zip file %1 doesn't exist").arg(m_zipFilePath));
        return;
    }

    if(!QFile::exists(m_dirPath)) {
        qWarning() << "Target dir file doesn't exist" << m_dirPath;
        emit finished(false, tr("Target dir %1 doesn't exist").arg(m_dirPath));
        return;
    }


    if(QFile::exists(m_zipFilePath)) {
        if(!QFile::remove(m_zipFilePath)) {
            qWarning() << "Can't remove zip file" << m_zipFilePath;
            emit finished(false, tr("Can't remove zip file %1").arg(m_zipFilePath));
            return;
        }
    }

    JlCompress::compressDir(m_zipFilePath, m_dirPath, true);

    emit finished(true);
}

void ldZipExtractorPrivate::extractDir()
{
    qDebug() << __FUNCTION__ << m_zipFilePath << m_dirPath;

    if(m_zipFilePath.isEmpty() || m_dirPath.isEmpty()) {
        qWarning() << "Zip extractor is not inited" << m_zipFilePath << m_dirPath;
        emit finished(false, tr("Zip file %1 doesn't exist").arg(m_zipFilePath));
        return;
    }

    if(!QFile::exists(m_zipFilePath)) {
        qWarning() << "Zip file doesn't exist" << m_zipFilePath;
        emit finished(false, tr("Zip file %1 doesn't exist").arg(m_zipFilePath));
        return;
    }

    QStringList fileList = JlCompress::getFileList(m_zipFilePath);

    if(m_isRemoveDir && !QDir(m_dirPath).removeRecursively()) {
        qWarning() << "Can't remove target dir" << m_dirPath;
        emit finished(false, tr("Can't remove target dir %1").arg(m_dirPath));
        return;
    }
    if(!QDir().mkpath(m_dirPath)) {
        qWarning() << "Can't create target dir" << m_dirPath;
        emit finished(false, tr("Can't create target dir %1").arg(m_dirPath));
        return;
    }

    QuaZip zip(m_zipFilePath);
    if(!zip.open(QuaZip::mdUnzip)) {
        qWarning() << "Zip file can't be opened" << m_zipFilePath;
        emit finished(false, tr("Zip file %1 can't be opened").arg(m_zipFilePath));
        return;
    }

    QDir directory(m_dirPath);
    QStringList extracted;
    if (!zip.goToFirstFile()) {
        qWarning() << "Zip file is empty";
        emit finished(false, tr("Zip file %1 is empty").arg(m_zipFilePath));
        return;
    }
    int i = 0;
    do {
        QString name = zip.getCurrentFileName();
        QString absFilePath = directory.absoluteFilePath(name);
        if (!JlCompress::extractFile(&zip, "", absFilePath)) {
            JlCompress::removeFile(extracted);
            qWarning() << "Zip file can't be extracted" << name << absFilePath << extracted;
            emit finished(false, tr("Zip file %1 can't be extracted: %2").arg(m_zipFilePath).arg(name));
            return;
        }
        extracted.append(absFilePath);

        i++;

        emit progress(static_cast<int>(i * 100.0 / fileList.size()));
    } while (zip.goToNextFile());

    // goToNextFile can be finished with error
    if(zip.getZipError()!=0) {
        JlCompress::removeFile(extracted);
        emit finished(false, tr("Zip file %1 can't be extracted: %2").arg(m_zipFilePath).arg(QString::number(i)));
        return;
    }

    zip.close();

    emit finished(true);
}

QString ldZipExtractorPrivate::zipFilePath() const
{
    return m_zipFilePath;
}

void ldZipExtractorPrivate::setRemoveDir(bool isRemoveDir)
{
    m_isRemoveDir = isRemoveDir;
}

// ------------------------- ldZipExtractor ----------------------

void ldZipExtractor::staticInit()
{
    QuaZip::setDefaultFileNameCodec(QTextCodec::codecForName("UTF-8"));
}

ldZipExtractor::ldZipExtractor(QObject *parent)
    : QObject(parent)
    , m_progress(0)
    , m_private(new ldZipExtractorPrivate())
{
    m_private->moveToThread(&m_workerThread);
    m_workerThread.start();

    connect(m_private.data(), &ldZipExtractorPrivate::progress, this, &ldZipExtractor::update_progress);
    connect(m_private.data(), &ldZipExtractorPrivate::finished, this, &ldZipExtractor::finished);
}

ldZipExtractor::~ldZipExtractor()
{
    m_workerThread.quit();
    if(!m_workerThread.wait()) {
        qWarning() << "ldZipExtractor m_worker_thread wasn't finished correctly";
    }
}

void ldZipExtractor::init(const QString &zipFilePath, const QString &dirPath)
{
    m_private->init(zipFilePath, dirPath);
}

QString ldZipExtractor::zipFilePath() const
{
    return m_private->zipFilePath();
}

void ldZipExtractor::setRemoveDir(bool isRemoveDir)
{
    m_private->setRemoveDir(isRemoveDir);
}

void ldZipExtractor::startCompression()
{
    QTimer::singleShot(0, m_private.data(), &ldZipExtractorPrivate::compressDir);
}

void ldZipExtractor::startExtraction()
{
    QTimer::singleShot(0, m_private.data(), &ldZipExtractorPrivate::extractDir);
}

#include "ldZipExtractor.moc"
