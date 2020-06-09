#ifndef LDRESOURCESEXTRACTOR_H
#define LDRESOURCESEXTRACTOR_H

#include <QQmlHelpers>

#include <ldCore/ldCore_global.h>

class ldZipExtractor;

// class to extract resources from obb file on android
class LDCORESHARED_EXPORT ldResourcesExtractor : public QObject
{
    Q_OBJECT
    QML_READONLY_PROPERTY(bool, needExtraction)
    QML_READONLY_PROPERTY(int, progress)

public:
    explicit ldResourcesExtractor(QObject *parent = nullptr);
    ~ldResourcesExtractor();

    // init state of current resource package. needExtraction flag is updated
    void init(const QString &packageName, int resourcesVersionCode);

public slots:
    // start extraction. signal finished(bool) is emitted in the end, property progress is updated
    // see log message for error description
    void startExtraction();

signals:
    void finished(bool isOk, const QString &errorMesssage = QString());

private:
    void checkNeedExtraction(int resourcesVersionCode);
    QString findObbFilePath(const QString &packageName, int resourcesVersionCode);

    ldZipExtractor *m_zipExtractor;
};

#endif // LDRESOURCESEXTRACTOR_H
