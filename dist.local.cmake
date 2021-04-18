# desktop
set(QT_BASE_DIR "/Path/to/Qt/5.14.0")

if(LD_ANDROID_BUILD)
    # mandatory
    set(QT_ANDROID_SDK_ROOT /Path/to/sdk)
    set(ANDROID_NDK /Path/to/ndk) # r21

    if(NOT DEFINED ANDROID_ABI)
        # You should clear CMake configuration after changing this value
        set(ANDROID_ABI "arm64-v8a" CACHE STRING "") # currently only "arm64-v8a" and "armeabi-v7a" values are supported
    endif()
endif()

if(LD_IOS_BUILD)
    set(LD_APPLE_DEVELOPMENT_ID "XXXXXXXXX" CACHE STRING "" FORCE)
endif()
