#ifndef LDRESOURCESEXTRACTOR_H
#define LDRESOURCESEXTRACTOR_H

#include <QtCore/QObject>
#include <QtCore/QThread>

#include <QQmlHelpers>

#include <ldCore/ldCore_global.h>

class ldResourcesExtractorPrivate;

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
    void finished(bool isOk);

private:
    QScopedPointer<ldResourcesExtractorPrivate> m_private;
    QThread m_workerThread;
};

#endif // LDRESOURCESEXTRACTOR_H
