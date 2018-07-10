/**
    libLaserdockCore
    Copyright(c) 2018 Wicked Lasers

    This file is part of libLaserdockCore.

    libLaserdockCore is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libLaserdockCore is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libLaserdockCore.  If not, see <https://www.gnu.org/licenses/>.
**/

#ifndef LDABSTRACTGAME_H
#define LDABSTRACTGAME_H

#include <QQmlHelpers>
#include <QMap>

#include "ldCore/ldCore_global.h"

#ifdef LD_USE_ANDROID_LAYOUT
#define TOUCH_KEY_SUPPORT //
#define ALWAYS_PLAY_STATE // no game over
#endif


class QKeyEvent;
class ldAbstractGameVisualizer;

/**
 * @brief The ldAbstractGame class - base class for each mini-app.
 */
class LDCORESHARED_EXPORT ldAbstractGame : public QObject
{
    Q_OBJECT

    QML_CONSTANT_PROPERTY(QString, id)
    QML_CONSTANT_PROPERTY(QString, title)

    QML_WRITABLE_PROPERTY(bool, isActive)

    Q_PROPERTY(QStringList levelList READ get_levelList CONSTANT)

    Q_PROPERTY(QStringList hotkeys READ get_hotkeys CONSTANT)
#ifdef TOUCH_KEY_SUPPORT
    Q_PROPERTY(QStringList touchHotkeys READ get_touchHotkeys CONSTANT)
#endif

    QML_WRITABLE_PROPERTY(int, levelIndex)
    QML_WRITABLE_PROPERTY(bool, isPlaying)
    QML_WRITABLE_PROPERTY(bool, isPaused)

public:
#ifdef TOUCH_KEY_SUPPORT
    enum TouchKey {
        Left,
        Right,
        Up,
        Down,
        A,
        B,
        X,
        Y
    };
    Q_ENUM(TouchKey)

    static void registerMetaTypes();
#endif

    explicit ldAbstractGame(const QString &id, const QString &title, QObject *parent = 0);
    virtual ~ldAbstractGame();

    QStringList get_levelList() const;
    QStringList get_hotkeys() const;
#ifdef TOUCH_KEY_SUPPORT
    QStringList get_touchHotkeys() const;
#endif

public slots:
    void play();

    void pause();

    /** @brief reset - Reset game to initial state */
    void reset();

    /** @brief toggle - toggle play/paus */
    void toggle();

    /** @brief setComplexity - game complexity */
    void setComplexity(float speed);

    /** @brief setSoundEnabled - enable/disable sound if supported */
    void setSoundEnabled(bool enabled);

    /** @brief setSoundLevel - set sound level if supported */
    void setSoundLevel(int soundLevel);

#ifdef TOUCH_KEY_SUPPORT
    void sendKeyEvent(Qt::Key key, bool isPressed);
    int getKey(int key);
#endif

protected:
    void initTouchHotkeys();

    virtual bool eventFilter(QObject *obj, QEvent *ev) override;

    virtual void activate();
    virtual void deactivate();

    QStringList m_levelList;
    QStringList m_hotkeys;

#ifdef TOUCH_KEY_SUPPORT
    QMap<TouchKey, Qt::Key> m_keyMap;
    QStringList m_touchHotkeys;
#endif

private:
    virtual bool handleKeyEvent(QKeyEvent *keyEvent) = 0;
    virtual ldAbstractGameVisualizer *getGameVisualizer() const = 0;

private slots:
    void onActiveChanged(bool isActive);
    void onLevelIndexChanged(int currentIndex);
};

#endif // LDABSTRACTGAME_H


