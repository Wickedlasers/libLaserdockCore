#pragma once

#include <QtCore/QtGlobal>

#if QT_VERSION >= 0x060000
#include <QtCore/QCoreApplication>
#include <QtCore/QJniEnvironment>
#include <QtCore/QJniObject>
#include <QtCore/private/qandroidextras_p.h>
#include <QtCore/qnativeinterface.h>
#else
#include <QtAndroidExtras/QtAndroid>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QAndroidJniObject>
#endif

#if QT_VERSION < 0x060000
#define QJniObject QAndroidJniObject
#define QJniEnvironment QAndroidJniEnvironment
#define ldAndroidActivityObject QtAndroid::androidActivity().object()
#define ldAndroidPermissionResultGranted QtAndroid::PermissionResult::Granted
#define ldAndroidPermissionResultMap QtAndroid::PermissionResult::Granted
#else
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
#define ldAndroidActivityObject QNativeInterface::QAndroidApplication::context().object();
#else
#define ldAndroidActivityObject QNativeInterface::QAndroidApplication::context()
#endif
#define ldAndroidPermissionResultMap QtAndroidPrivate::PermissionResult::Authorized
#endif
