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

#include "ldCore/ldCore_global.h"

#include "ldGamepad.h"
#include "ldGamepadCtrl.h"

class QKeyEvent;

class ldAbstractGameVisualizer;

/**
 * The ldAbstractGame class - base class for each game class
 */
class LDCORESHARED_EXPORT ldAbstractGame : public QObject
{
    Q_OBJECT

    /** each game has id and title properties */
    QML_CONSTANT_PROPERTY(QString, id)
    QML_CONSTANT_PROPERTY(QString, title)

    /** optional virtual gamepad support */
    QML_CONSTANT_PROPERTY(ldGamepadCtrl*, gamepadCtrl)

    /** activate game */
    QML_WRITABLE_PROPERTY(bool, isActive)

    /** number of levels - initialize in a child constructor */
    Q_PROPERTY(QStringList levelList READ get_levelList CONSTANT)

    /** Optional QStringList of key descriptions - first is key, second is description */
    Q_PROPERTY(QStringList keyDescriptions READ get_keyDescriptions CONSTANT)

    /** current level index */
    QML_WRITABLE_PROPERTY(int, levelIndex)

    /**
       Game can have several states:
        isPlaying && !isPaused = playing
        isPlaying && isPaused = paused
        !isPlaying = finished
        */
    QML_WRITABLE_PROPERTY(bool, isPlaying)
    QML_WRITABLE_PROPERTY(bool, isPaused)

public:
    static void registerMetaTypes();

    /** Constructor/destructor */
    explicit ldAbstractGame(const QString &id, const QString &title, QObject *parent = 0);
    virtual ~ldAbstractGame();

    /** Properties READ functions */
    QStringList get_levelList() const;
    QStringList get_keyDescriptions() const;

    /** QObject */
    virtual bool eventFilter(QObject *obj, QEvent *ev) override;

    ldAbstractGameVisualizer *getVisualizer() const;

    void moveX(double x);
    void moveY(double y);

    void moveRightX(double x);
    void moveRightY(double y);

public slots:
    void play();

    void pause();

    /** Reset game to initial state */
    void reset();

    /** Toggle play/pause */
    void toggle();

    /** Game complexity level */
    void setComplexity(float speed);

    /** Enable/disable sound if supported */
    void setSoundEnabled(bool enabled);

    /** Set sound level (if sound is enabled) */
    void setSoundLevel(int soundLevel);

protected:
    virtual void activate();
    virtual void deactivate();

    QStringList m_levelList;
    QStringList m_keyDescriptions;

private:
    virtual bool handleKeyEvent(QKeyEvent *keyEvent) = 0;
    virtual ldAbstractGameVisualizer *getGameVisualizer() const = 0;

private slots:
    void onActiveChanged(bool isActive);
    void onLevelIndexChanged(int currentIndex);
};

#endif // LDABSTRACTGAME_H


