#ifndef LDSIMPLECRYPT_H
#define LDSIMPLECRYPT_H

#include <memory>

#include <QtCore/QByteArray>

#include <ldCore/ldCore_global.h>

class SimpleCrypt;

class LDCORESHARED_EXPORT ldSimpleCrypt
{
public:
    static const QString LDS_EXTENSION;

    static ldSimpleCrypt *instance();

    // these 2 functions are added to handle ".lds" extension smoothly in case when you need it
    static QString completeBaseName(const QString &path);
    static QString suffix(const QString &path);

    void setKey(quint64 key);

    bool decryptToFile(const QString &filePath);
    bool encryptToFile(const QString &file);

    QByteArray decrypt(const QString &filePath);

    void encryptFolderR(const QString &folder, const QStringList &exceptionFolderList);
    void decryptFolderR(const QString &folder);

private:
    ldSimpleCrypt();
    ~ldSimpleCrypt();

    QByteArray readFile(const QString &filePath);
    bool writeFile(const QString &filePath, const QByteArray &encodedData);

    std::unique_ptr<SimpleCrypt> m_crypto;
};

#endif // SIMPLECRYPTWRAPPER_H
