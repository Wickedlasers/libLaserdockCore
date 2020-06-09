#ifndef LDZIPEXTRACTOR_H
#define LDZIPEXTRACTOR_H

#include <QtCore/QObject>
#include <QtCore/QThread>

#include <QQmlHelpers>

#include <ldCore/ldCore_global.h>

class ldZipExtractorPrivate;

class LDCORESHARED_EXPORT ldZipExtractor : public QObject
{
    Q_OBJECT
    QML_READONLY_PROPERTY(int, progress)

public:
    explicit ldZipExtractor(QObject *parent = nullptr);
    ~ldZipExtractor();

    void init(const QString &filePath, const QString &targetDir);

    QString filePath() const;

public slots:
    // start extraction. signal finished(bool) is emitted in the end, property progress is updated
    void startExtraction();

signals:
    void finished(bool isOk, const QString &errorMessage = QString());

private:
    QScopedPointer<ldZipExtractorPrivate> m_private;
    QThread m_workerThread;
};

#endif // LDZIPEXTRACTOR_H
