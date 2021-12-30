if(WIN32)
    set(OpenCV_DIR "${CMAKE_CURRENT_LIST_DIR}/win")
endif()

if(APPLE)
    if(LD_IOS_BUILD)
        set(OpenCV_DIR "${CMAKE_CURRENT_LIST_DIR}/ios")
    else()
        set(OpenCV_DIR "${CMAKE_CURRENT_LIST_DIR}/mac/share")
    endif()
endif()

if(ANDROID)
    set(OpenCV_DIR "${CMAKE_CURRENT_LIST_DIR}/android/sdk/native/jni")
endif()

mark_as_advanced(OpenCV_DIR)

if(LD_IOS_BUILD)
    set(OPENCV_FRAMEWORK "${OpenCV_DIR}/opencv2.framework")
else()
    list(APPEND CMAKE_PREFIX_PATH ${OpenCV_DIR})
    find_package(OpenCV COMPONENTS opencv_core opencv_imgproc opencv_videoio opencv_highgui opencv_imgcodecs opencv_calib3d opencv_features2d REQUIRED)
endif()

