# opencv 3.4.8
export PATH="/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/Applications/CMake.app/Contents/bin"

mkdir build
cd build

cmake -DCMAKE_OSX_DEPLOYMENT_TARGET=10.12 \
        -DWITH_AVFOUNDATION=OFF \
        -DWITH_QT=OFF -DWITH_QTKIT=OFF \
        -DWITH_AVFOUNDATION=ON \
        -DWITH_FFMPEG=OFF \
        -DBUILD_SHARED_LIBS=ON \
        -DOPENCV_GENERATE_PKGCONFIG=OFF \
        -DBUILD_opencv_python2=OFF \
        -DBUILD_opencv_apps=OFF \
        -DBUILD_opencv_aruco=OFF \
        -DBUILD_opencv_bgsegm=OFF \
        -DBUILD_opencv_bioinspired=OFF \
        -DBUILD_opencv_calib3d=OFF \
        -DBUILD_opencv_ccalib=OFF \
        -DBUILD_opencv_dnn=OFF \
        -DBUILD_opencv_dnn_objdetect=OFF \
        -DBUILD_opencv_dpm=OFF \
        -DBUILD_opencv_face=OFF \
        -DBUILD_opencv_features2d=OFF \
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
        -DBUILD_opencv_ts=OFF \
        -DBUILD_opencv_video=OFF \
        -DBUILD_opencv_videostab=OFF \
        -DBUILD_opencv_xphoto=OFF \
        -DBUILD_opencv_xfeatures2d=OFF \
        -DBUILD_opencv_ximgproc=OFF \
        -DBUILD_opencv_xobjdetect=OFF \
        -DCMAKE_INSTALL_PREFIX=/Users/ncuxer/work/wl/libs/opencv/bin/ \
        ..

make -j 7

make install

cd ..
