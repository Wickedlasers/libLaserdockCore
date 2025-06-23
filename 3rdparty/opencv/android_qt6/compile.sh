#!/usr/bin/env sh
NDK_ROOT="${1:-${NDK_ROOT}}"
set -x
#if [ ! -d "${WD}/android-cmake" ]; then
#    echo 'Cloning android-cmake'
#    git clone https://github.com/taka-no-me/android-cmake.git
#fi

### ABI setup
#ANDROID_ABI=${ANDROID_ABI:-"armeabi-v7a with NEON"}
#ANDROID_ABI=${ANDROID_ABI:-"arm64-v8a"}
#ANDROID_ABI=${ANDROID_ABI:-"x86"}
ANDROID_ABI=${ANDROID_ABI:-"x86_64"}

### path setup
SCRIPT=$(pwd -P $0)
WD=$SCRIPT
OPENCV_ROOT=${WD}/opencv

BUILD_DIR=$OPENCV_ROOT/platforms/build_android
INSTALL_DIR=${WD}/android_opencv
N_JOBS=${N_JOBS:-15}

if [ "${ANDROID_ABI}" = "armeabi" ]; then
    API_LEVEL=19
else
    API_LEVEL=21
fi

rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"


cmake -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
      -DCMAKE_TOOLCHAIN_FILE="${NDK_ROOT}/build/cmake/android.toolchain.cmake" \
      -DANDROID_NDK="${NDK_ROOT}" \
      -DANDROID_NATIVE_API_LEVEL=${API_LEVEL} \
      -DANDROID_ABI="${ANDROID_ABI}" \
      -D WITH_CUDA=OFF \
      -D WITH_MATLAB=OFF \
-DANDROID_STL=c++_shared \
-D BUILD_ANDROID_EXAMPLES=OFF \
-D BUILD_DOCS=OFF \
-D BUILD_PERF_TESTS=OFF \
-D BUILD_TESTS=OFF \
-D BUILD_JAVA=OFF \
-D BUILD_FAT_JAVA_LIB=OFF \
-D BUILD_ANDROID_PROJECTS=OFF \
-D BUILD_ANDROID_EXAMPLES=OFF \
-DBUILD_SHARED_LIBS=ON \
-DBUILD_opencv_aruco=OFF \
-DBUILD_opencv_bgsegm=OFF \
-DBUILD_opencv_bioinspired=OFF \
-DBUILD_opencv_ccalib=ON \
-DBUILD_opencv_dnn=OFF \
-DBUILD_opencv_dnn_objdetect=OFF \
-DBUILD_opencv_dpm=OFF \
-DBUILD_opencv_face=OFF \
-DBUILD_opencv_features2d=ON \
-DBUILD_opencv_flann=OFF \
-DBUILD_opencv_fuzzy=OFF \
-DBUILD_opencv_hfs=OFF \
-DBUILD_opencv_img_hash=OFF \
-DBUILD_opencv_ml=OFF \
-DBUILD_opencv_line_descriptor=OFF \
-DBUILD_opencv_objdetect=OFF \
-DBUILD_opencv_optflow=OFF \
-DBUILD_opencv_phase_unwrapping=OFF \
-DBUILD_opencv_photo=OFF \
-DBUILD_opencv_plot=OFF \
-DBUILD_opencv_reg=OFF \
-DBUILD_opencv_rgbd=OFF \
-DBUILD_opencv_saliency=OFF \
-DBUILD_opencv_shape=OFF \
-DBUILD_opencv_stereo=OFF \
-DBUILD_opencv_stitching=OFF \
-DBUILD_opencv_structured_light=OFF \
-DBUILD_opencv_superres=OFF \
-DBUILD_opencv_surface_matching=OFF \
-DBUILD_opencv_tracking=OFF \
-DBUILD_opencv_video=OFF \
-DBUILD_opencv_videostab=OFF \
-DBUILD_opencv_xphoto=OFF \
-DBUILD_opencv_xfeatures2d=OFF \
-DBUILD_opencv_ximgproc=OFF \
-DBUILD_opencv_xobjdetect=OFF \
      -DOPENCV_EXTRA_MODULES_PATH="${WD}/opencv_contrib/modules/"  \
      -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}/opencv_install" \
      ../..

make -j${N_JOBS}

# rm and install install folder, and remove build folder
rm -rf "${INSTALL_DIR}/opencv"
make install/strip

cd "${WD}"
rm -rf "${BUILD_DIR}"
