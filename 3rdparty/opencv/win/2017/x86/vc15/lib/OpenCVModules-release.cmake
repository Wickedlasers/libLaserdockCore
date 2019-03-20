#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "opencv_core" for configuration "Release"
set_property(TARGET opencv_core APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_core PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_core345.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_core345.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS opencv_core )
list(APPEND _IMPORT_CHECK_FILES_FOR_opencv_core "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_core345.lib" "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_core345.dll" )

# Import target "opencv_imgproc" for configuration "Release"
set_property(TARGET opencv_imgproc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_imgproc PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_imgproc345.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_imgproc345.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS opencv_imgproc )
list(APPEND _IMPORT_CHECK_FILES_FOR_opencv_imgproc "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_imgproc345.lib" "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_imgproc345.dll" )

# Import target "opencv_imgcodecs" for configuration "Release"
set_property(TARGET opencv_imgcodecs APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_imgcodecs PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_imgcodecs345.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_imgcodecs345.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS opencv_imgcodecs )
list(APPEND _IMPORT_CHECK_FILES_FOR_opencv_imgcodecs "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_imgcodecs345.lib" "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_imgcodecs345.dll" )

# Import target "opencv_videoio" for configuration "Release"
set_property(TARGET opencv_videoio APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_videoio PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_videoio345.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_videoio345.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS opencv_videoio )
list(APPEND _IMPORT_CHECK_FILES_FOR_opencv_videoio "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_videoio345.lib" "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_videoio345.dll" )

# Import target "opencv_highgui" for configuration "Release"
set_property(TARGET opencv_highgui APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_highgui PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_highgui345.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "Qt5::Core;Qt5::Gui;Qt5::Widgets;Qt5::Test;Qt5::Concurrent"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_highgui345.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS opencv_highgui )
list(APPEND _IMPORT_CHECK_FILES_FOR_opencv_highgui "${_IMPORT_PREFIX}/x86/vc15/lib/opencv_highgui345.lib" "${_IMPORT_PREFIX}/x86/vc15/bin/opencv_highgui345.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
