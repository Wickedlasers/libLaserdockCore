#ifndef LDRESOURCESEXTRACTOR_H
#define LDRESOURCESEXTRACTOR_H

#include <QtCore/QObject>
#include <QtCore/QThread>

#include <QQmlHelpers>

#include <ldCore/ldCore_global.h>

class ldResourcesExtractorPrivate;

class LDCORESHARED_EXPORT ldResourcesExtractor : public QObject
{
    Q_OBJECT
    QML_READONLY_PROPERTY(bool, needExtraction)
    QML_READONLY_PROPERTY(int, progress)

public:
    explicit ldResourcesExtractor(QObject *parent = 0);
    ~ldResourcesExtractor();

public slots:
    void startExtraction();

signals:
    void finished();

private:
    QScopedPointer<ldResourcesExtractorPrivate> d_ptr;
    QThread m_workerThread;
};

#endif // LDRESOURCESEXTRACTOR_H
