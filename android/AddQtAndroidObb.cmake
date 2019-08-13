cmake_minimum_required(VERSION 3.0)

include(CMakeParseArguments)

# define a macro to create an Android APK target
#
# example:
# add_qt_android_apk(my_app_apk my_app
#     RESOURCES_VERSION_CODE "1"
#     PACKAGE_NAME "org.example.app"
#     RES_DIRS "${CMAKE_CURRENT_LIST_DIR}/res1";${CMAKE_CURRENT_LIST_DIR}/res2"
#     DEPLOY_OBB # android only, optional, adb sync
#)
#

macro(add_qt_android_obb TARGET SOURCE_TARGET)
    # parse the macro arguments
    cmake_parse_arguments(ARG "DEPLOY_OBB" "RESOURCES_VERSION_CODE;PACKAGE_NAME" "RES_DIRS"  ${ARGN})

    set(ANDROID_OBB_DIR ${CMAKE_BINARY_DIR})
    set(ANDROID_OBB_FILE main.${ARG_RESOURCES_VERSION_CODE}.${ARG_PACKAGE_NAME}.obb)
    set(ANDROID_OBB_FILE_PATH ${ANDROID_OBB_DIR}/${ANDROID_OBB_FILE})

    set(ANDROID_RESOURCES_TEMP_DIR ${CMAKE_BINARY_DIR}/android_resExt)


    # Create obb file command
    add_custom_command(OUTPUT ${ANDROID_OBB_FILE_PATH}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ANDROID_OBB_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ANDROID_RESOURCES_TEMP_DIR}
        COMMAND echo ${ARG_RESOURCES_VERSION_CODE} > ${ANDROID_RESOURCES_TEMP_DIR}/version
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ARG_RES_DIRS} ${ANDROID_RESOURCES_TEMP_DIR}
        COMMAND cd ${ANDROID_RESOURCES_TEMP_DIR} && zip -r -X ${ANDROID_OBB_FILE} *
        COMMAND ${CMAKE_COMMAND} -E rename ${ANDROID_RESOURCES_TEMP_DIR}/${ANDROID_OBB_FILE} ${ANDROID_OBB_FILE_PATH}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${ANDROID_RESOURCES_TEMP_DIR}
        COMMENT "Creating obb file for android with all required resources"
        )

    # create_obb target
    add_custom_target(create_obb ALL
        DEPENDS ${ANDROID_OBB_FILE_PATH}
        )

    # optional deploy_obb target
    if(ANDROID)
        # make sure that the Android toolchain is used
        # find the Android SDK
        if(NOT QT_ANDROID_SDK_ROOT)
            set(QT_ANDROID_SDK_ROOT $ENV{ANDROID_SDK})
            if(NOT QT_ANDROID_SDK_ROOT)
                message(FATAL_ERROR "Could not find the Android SDK. Please set either the ANDROID_SDK environment variable, or the QT_ANDROID_SDK_ROOT CMake variable to the root directory of the Android SDK")
            endif()
        endif()
        string(REPLACE "\\" "/" QT_ANDROID_SDK_ROOT ${QT_ANDROID_SDK_ROOT}) # androiddeployqt doesn't like backslashes in paths
        message(STATUS "Found Android SDK: ${QT_ANDROID_SDK_ROOT}")

        # deploy_obb target
        if(ARG_DEPLOY_OBB)
            set(DEPLOY_OBB_ARG "ALL")
        else()
            set(DEPLOY_OBB_ARG "")
        endif()
        add_custom_target(deploy_obb ${DEPLOY_OBB_ARG}
            COMMAND ${QT_ANDROID_SDK_ROOT}/platform-tools/adb push --sync ${ANDROID_OBB_FILE_PATH} /sdcard/Android/obb/${ARG_PACKAGE_NAME}/${ANDROID_OBB_FILE}
            DEPENDS create_obb
            COMMENT "Deploy obb")
    endif()

endmacro()
