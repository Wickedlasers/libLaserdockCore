set(LD_QML_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/res/qml)

option(LD_ANDROID_INSTALL "Deploy android" ON)

if(LD_ANDROID_INSTALL)
    set(LD_ANDROID_INSTALL_ARG "INSTALL")
else()
    set(LD_ANDROID_INSTALL_ARG "")
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION 0)

set(LDCORE_DIR "${CMAKE_SOURCE_DIR}")
set(OPENCV_LIBDIR ${LDCORE_DIR}/3rdparty/opencv/android/sdk/native/libs/${ANDROID_ABI})
get_target_property(LIBUSB_SOURCE_DIR libusb SOURCE_DIR)
get_target_property(LASERDOCKLIB_SOURCE_DIR laserdocklib SOURCE_DIR)

set(QT_ANDROID_NDK_ROOT ${ANDROID_NDK})

include(${CMAKE_SOURCE_DIR}/android/AddQtAndroidApk/AddQtAndroidApk.cmake)
add_qt_android_apk(${PROJECT_NAME}_apk ${PROJECT_NAME}
    JAVA_SOURCES ${LASERDOCKLIB_SOURCE_DIR}/java
    NAME "LaserdockExample"
    ANDROID_RESOURCES_PATH ${LDCORE_DIR}/android/res
    PACKAGE_NAME ${LD_EXAMPLE_PACKAGE_NAME}
    ANDROID_MIN_API ${ANDROID_NATIVE_API_LEVEL}
    ANDROID_TARGET_API ${ANDROID_TARGET_API_LEVEL}
    DEPENDS openlase
    DEPENDS ldCore
    DEPENDS laserdocklib
    DEPENDS quazip
    DEPENDS ${LIBUSB_SOURCE_DIR}/lib/android/${ANDROID_ABI}/libusb1.0.so
#    DEPENDS ${OPENCV_LIBDIR}/libopencv_core.so
#    DEPENDS ${OPENCV_LIBDIR}/libopencv_imgcodecs.so
#    DEPENDS ${OPENCV_LIBDIR}/libopencv_imgproc.so
#    DEPENDS ${OPENCV_LIBDIR}/libopencv_highgui.so
#    DEPENDS ${OPENCV_LIBDIR}/libopencv_videoio.so
    QML_ROOT_PATH ${LD_QML_ROOT_PATH}
    #  For release deployment
    VERSION_CODE 1
    KEYSTORE ${LDCORE_DIR}/example/install/android/libLaserdockCore.jks libLaserdockCore
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


