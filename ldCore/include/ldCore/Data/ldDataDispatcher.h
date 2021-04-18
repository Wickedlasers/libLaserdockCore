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

#ifndef LDDATADISPATCHER_H
#define LDDATADISPATCHER_H

#include <memory>

#include <QQmlHelpers>

#include <ldCore/Helpers/ldPropertyObject.h>

class ldBufferManager;
class ldAbstractDataWorker;
class ldHardwareManager;
class ldSimulatorEngine;
class ldHardwareDataWorker;
class ldFilterManager;
#ifdef LASERDOCKLIB_USB_SUPPORT
class ldUsbHardwareManager;
#endif

/** Laserdock data transfer control class. Supports OpenGL simulator and additional custom worker for internal usage */
class LDCORESHARED_EXPORT ldDataDispatcher : public ldPropertyObject
{
    Q_OBJECT

#ifdef LASERDOCKLIB_USB_SUPPORT
    QML_WRITABLE_PROPERTY(bool, isNetwork)
#endif

public:
    /** Constructor/destructor */
    explicit ldDataDispatcher(ldBufferManager *bufferManager,
                              ldHardwareManager *hardwareManager,
                              ldFilterManager* filterManager,
                              QObject *parent = nullptr);
    ~ldDataDispatcher();

    /** If any worker is active */
    bool isActiveTransfer() const;

#ifdef LASERDOCKLIB_USB_SUPPORT
    /** Main data worker */
    ldUsbHardwareManager* usbDataManager() const;
#endif

    /** Simulator core engine class */
    ldSimulatorEngine* simulatorEngine() const;

public slots:
    /** Activate/deactivate data workers */
    void setActiveTransfer(bool active);

signals:
    /** Emitted when data workers state changed */
    void activeChanged(bool active);

private slots:
#ifdef LASERDOCKLIB_USB_SUPPORT
    void onIsNetworkChanged(bool isNetwork);
#endif
private:
    std::unique_ptr<ldSimulatorEngine> m_simulatorEngine;

#ifdef LASERDOCKLIB_USB_SUPPORT
    std::unique_ptr<ldHardwareDataWorker> m_usbDataWorker;
#endif
    std::unique_ptr<ldHardwareDataWorker> m_networkDataWorker;
    ldHardwareDataWorker* m_activeDataWorker=nullptr;
};

#endif // LDDATADISPATCHER_H
