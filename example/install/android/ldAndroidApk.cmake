set(LD_QML_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/res/qml)

option(LD_ANDROID_INSTALL "Deploy android" ON)

if(LD_ANDROID_INSTALL)
    set(LD_ANDROID_INSTALL_ARG "INSTALL")
else()
    set(LD_ANDROID_INSTALL_ARG "")
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION 0)

set(QT_ANDROID_NDK_ROOT ${ANDROID_NDK})

include(${CMAKE_SOURCE_DIR}/android/AddQtAndroidApk/AddQtAndroidApk.cmake)
add_qt_android_apk(${PROJECT_NAME}_apk ${PROJECT_NAME}
    JAVA_SOURCES ${CMAKE_SOURCE_DIR}/3rdparty/laserdocklib/java
    NAME "LaserdockExample"
    ANDROID_RESOURCES_PATH ${CMAKE_SOURCE_DIR}/android/res
    PACKAGE_NAME ${LD_EXAMPLE_PACKAGE_NAME}
    ANDROID_MIN_API 21 # 5.0
    ANDROID_TARGET_API 28 # 9.0 Oreo support
    DEPENDS openlase
    DEPENDS ldCore
    DEPENDS laserdocklib
    DEPENDS quazip
    DEPENDS ${CMAKE_SOURCE_DIR}/3rdparty/laserdocklib/3rdparty/libusb/lib/android/${ANDROID_ABI}/libusb1.0.so
    QML_ROOT_PATH ${LD_QML_ROOT_PATH}
    #  For release deployment
    VERSION_CODE 1
    KEYSTORE ${CMAKE_SOURCE_DIR}/example/install/android/libLaserdockCore.jks libLaserdockCore
    KEYSTORE_PASSWORD libLaserdockCore
    KEY_PASSWORD libLaserdockCore
    ${LD_ANDROID_INSTALL_ARG}
    )


# create obb
include(${CMAKE_SOURCE_DIR}/android/AddQtAndroidObb.cmake)
if(LD_ANDROID_INSTALL)
    set(LD_DEPLOY_OBB "DEPLOY_OBB")
else()
    set(LD_DEPLOY_OBB "")
endif()

add_qt_android_obb(${PROJECT_NAME}_obb ${PROJECT_NAME}
    RESOURCES_VERSION_CODE ${LD_EXAMPLE_RESOURCES_VERSION_CODE}
    PACKAGE_NAME ${LD_EXAMPLE_PACKAGE_NAME}
    RES_DIRS ${SOURCE_RESOURCES_PATH};${SOURCE_LDCORE_RESOURCES_PATH}
    ${LD_DEPLOY_OBB}
    )


