#ifndef LDRESOURCESEXTRACTOR_H
#define LDRESOURCESEXTRACTOR_H

#include <QQmlHelpers>

#include <ldCore/ldCore_global.h>

class ldZipExtractor;

// class to extract resources from obb file on android
class LDCORESHARED_EXPORT ldResourcesExtractor : public QObject
{
    Q_OBJECT
    QML_READONLY_PROPERTY(int, currentFileIndex)
    QML_READONLY_PROPERTY(int, fileCount)
    QML_READONLY_PROPERTY(int, progress)

public:
    explicit ldResourcesExtractor(QObject *parent = nullptr);
    ~ldResourcesExtractor();

    // init state of current resource package. needExtraction flag is updated
    void init(const QString &packageName, int mainVersionCode, int patchVersionCode);

public slots:
    // start extraction. signal finished(bool) is emitted in the end, property progress is updated
    // see log message for error description
    void startExtraction();

signals:
    void finished(bool isOk, const QString &errorMesssage = QString());

private:
    void onZipExtractorFinished(bool isOk, const QString &errorMesssage = QString());

    QString findObbFilePath(const QString &obbType, const QString &packageName, int resourcesVersionCode);
    int getExistingResVersionCode();

    std::vector<std::unique_ptr<ldZipExtractor>> m_zipExtractors;
};

#endif // LDRESOURCESEXTRACTOR_H
