#ifndef LDVISDIRECTIONCTRL_H
#define LDVISDIRECTIONCTRL_H

#include <QtCore/QMutex>

#include <QQmlHelpers>

class ldVisDirectionCtrl : public QObject
{
    Q_OBJECT

    QML_READONLY_PROPERTY(bool, isActive)
    Q_PROPERTY (int direction READ get_direction NOTIFY directionChanged)
public:
    enum Direction {
        Right,
        Left,
        LeftRight
    };
    Q_ENUM(Direction)

    static void registerMetaTypes();

    explicit ldVisDirectionCtrl(QObject *parent = nullptr);

public slots:
    void next();
    void reset();

    int get_direction() const;

signals:
    void directionChanged(int direction);

private:
    bool update_direction(int direction);

    mutable QMutex m_mutex;
    int m_direction = Right;
};


#endif // LDVISDIRECTIONCTRL_H
