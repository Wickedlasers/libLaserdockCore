if(WIN32)
    if(MSVC_VERSION STREQUAL "1900")
        set(OpenCV_DIR "${CMAKE_CURRENT_LIST_DIR}/win/2015")
    else()
        set(OpenCV_DIR "${CMAKE_CURRENT_LIST_DIR}/win/2017")
    endif()
endif()

if(APPLE)
    set(OpenCV_DIR "${CMAKE_CURRENT_LIST_DIR}/mac/share")
endif()

if(ANDROID)
    set(OpenCV_DIR "${CMAKE_CURRENT_LIST_DIR}/android/sdk/native/jni")
endif()

mark_as_advanced(OpenCV_DIR)
set(CMAKE_PREFIX_PATH ${OpenCV_DIR})
find_package(OpenCV COMPONENTS opencv_core opencv_imgproc opencv_videoio opencv_highgui opencv_imgcodecs REQUIRED)


