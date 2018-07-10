# desktop
set(QT_BASE_DIR "/Path/to/Qt/5.11.0")

#set(LD_ANDROID_BUILD OFF)

if(LD_ANDROID_BUILD)
    # mandatory
    set(QT_ANDROID_SDK_ROOT /Path/to/sdk)
    set(ANDROID_NDK /Path/to/ndk)

    if(NOT DEFINED ANDROID_ABI)
        # You should clear CMake configuration after changing this value
        set(ANDROID_ABI "armeabi-v7a") # currently only "x86" and "armeabi-v7a" values are supported, see ANDROID_TOOLCHAIN_PREFIX variable
    endif()

    # Codesign
    set(QT_ANDROID_SDK_ROOT_VERSION 23.0.0)
    set(ANDROID_KEYSTORE /path/to/key.jks key_name)
    set(ANDROID_STORE_PASSWORD %STORE_PASSWORD%)
    set(ANDROID_KEY_PASSWORD %KEY_PASSWORD%)
endif()
