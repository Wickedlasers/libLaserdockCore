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

    void setKey(quint64 key);

    bool encrypt(const QString &file);
    QByteArray decrypt(const QString &filePath);

    void encryptFolderR(const QString &folder);

private:
    ldSimpleCrypt();
    ~ldSimpleCrypt();

    QByteArray readFile(const QString &filePath);
    bool writeFile(const QString &filePath, const QByteArray &encodedData);

    std::unique_ptr<SimpleCrypt> m_crypto;
};

#endif // SIMPLECRYPTWRAPPER_H
