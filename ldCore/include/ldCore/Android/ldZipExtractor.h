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
    static void staticInit();

    explicit ldZipExtractor(QObject *parent = nullptr);
    ~ldZipExtractor();

    /**
     * @brief init extraction or compression
     * @param zipFilePath on Android should be accessible with absolute path - e.g. in storageDir()/resourceDir()
     * @param dirPath
     */
    void init(const QString &zipFilePath, const QString &dirPath);

    QString zipFilePath() const;


    /**
     * @brief setRemoveDir - remove target dir on extraction, true by default
     * @param isRemoveDir
     */
    void setRemoveDir(bool isRemoveDir);

public slots:
    // start extraction. signal finished(bool) is emitted in the end, property progress is updated
    void startCompression();
    void startExtraction();

signals:
    void finished(bool isOk, const QString &errorMessage = QString());

private:
    QScopedPointer<ldZipExtractorPrivate> m_private;
    QThread m_workerThread;
};

#endif // LDZIPEXTRACTOR_H
