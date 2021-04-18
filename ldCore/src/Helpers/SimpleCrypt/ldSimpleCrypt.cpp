#include <ldCore/Helpers/SimpleCrypt/ldSimpleCrypt.h>

#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFile>
#include <QtCore/QtDebug>
#include <QtCore/QCryptographicHash>

#include <simplecrypt.h>

const QString ldSimpleCrypt::LDS_EXTENSION = ".lds";

ldSimpleCrypt *ldSimpleCrypt::instance()
{
    static ldSimpleCrypt instance;
    return &instance;
}

void ldSimpleCrypt::setKey(quint64 key)
{
    m_crypto->setKey(key);
}

bool ldSimpleCrypt::decryptToFile(const QString &filePath)
{
    QByteArray data = decrypt(filePath);
    if(data.isEmpty())
        return false;

    // save to file
    QString newFilePath = filePath;
    newFilePath.remove(LDS_EXTENSION);

    bool isOk = writeFile(newFilePath, data);
    if(isOk) {
        bool isRemoved = QFile(filePath).remove();
        if(!isRemoved)
            qWarning() << "can't remove" << filePath;
    }

    return isOk;
}


bool ldSimpleCrypt::encryptToFile(const QString &filePath)
{
    qDebug() << __FUNCTION__ << filePath;

    // read file
    QByteArray data = readFile(filePath);
    if(data.isEmpty())
        return false;

    // encode
    QByteArray encodedData = m_crypto->encryptToByteArray(data);
    if (m_crypto->lastError() != SimpleCrypt::ErrorNoError) {
        qWarning() <<  __FUNCTION__ << "Crypt Error:" << m_crypto->lastError();
        return false;
    }

    // save to file
    bool isOk =  writeFile(filePath + LDS_EXTENSION, encodedData);
    if(isOk) {
        bool isRemoved = QFile(filePath).remove();
        if(!isRemoved)
            qWarning() << "can't remove" << filePath;
    }

    return isOk;
}

QByteArray ldSimpleCrypt::decrypt(const QString &filePath)
{
    // read file
    QByteArray data = readFile(filePath);
    if(data.isEmpty())
        return data;

    QByteArray decodedData = m_crypto->decryptToByteArray(data);

    if (m_crypto->lastError() != SimpleCrypt::ErrorNoError) {
        qWarning() << "Crypt Error:" << m_crypto->lastError();
    }

    return decodedData;
}

void ldSimpleCrypt::encryptFolderR(const QString &folder, const QStringList &exceptionFolderList)
{
//    qDebug() << __FUNCTION__ << folder;
    QDirIterator it(folder, QStringList() << "*.svg" << "*.ldva2" << "*.ldva4" << "*.ild", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filePath = it.next();
        QString parentDir = QFileInfo(filePath).dir().dirName();
        if(exceptionFolderList.contains(parentDir))
            continue;

        encryptToFile(filePath);
    }
}

void ldSimpleCrypt::decryptFolderR(const QString &folder)
{
    qDebug() << __FUNCTION__ << folder;
    QDirIterator it(folder, QStringList() << "*.lds", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        decryptToFile(it.next());
    }
}

ldSimpleCrypt::ldSimpleCrypt()
    : m_crypto(new SimpleCrypt(Q_UINT64_C(0x0c2ad4a4acb9f023))) // some random number
{
    m_crypto->setCompressionMode(SimpleCrypt::CompressionNever);
}


ldSimpleCrypt::~ldSimpleCrypt()
{
}


QByteArray ldSimpleCrypt::readFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists()) {
        qWarning() << __FUNCTION__ << ": file does not exist" << filePath;
        return QByteArray();
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << __FUNCTION__ << " can't open file to read";
        return QByteArray();
    }
    QByteArray data = file.readAll();

    return data;
}

bool ldSimpleCrypt::writeFile(const QString &filePath, const QByteArray &encodedData)
{
    QFile encodedFile(filePath);
    if(encodedFile.exists())
        encodedFile.remove();

    if (!encodedFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << __FUNCTION__ << "can't open file to write";
        return false;
    }

    encodedFile.write(encodedData);

    return true;
}

