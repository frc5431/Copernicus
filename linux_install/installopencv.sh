#!/bin/bash

OPENCV_URL=https://github.com/opencv/opencv/archive/3.2.0.zip
OPENCV_ZIP=opencvsource.zip
OPENCV_SOURCE=opencv_source

echo "Installing prerequisets for OpenCV"

sudo apt-get --yes --force-yes install libjpeg8-dev \
libtiff5-dev libjasper-dev libpng12-dev libavcodec-dev \
libavformat-dev libswscale-dev libv4l-dev libxvidcore-dev \
libx264-dev libgtk-3-dev libatlas-base-dev gfortran \
python2.7-dev python3.5-dev gcc-multilib jasper zip unzip v4l-utils

echo "Complete!

Downloading opencv source"

if [ ! -f $OPENCV_ZIP ]; then
	wget -O $OPENCV_ZIP $OPENCV_URL
else
        echo "Opencv source already installed here!"
fi

if [ $? -ne 0 ]; then
        echo "Failed to download the opencv source"
fi

echo "Complete!

Unzipping opencv"

if [ -d $OPENCV_SOURCE ]; then
        rm -rf $OPENCV_SOURCE/*
else
        mkdir $OPENCV_SOURCE
fi

unzip $OPENCV_ZIP -d $OPENCV_SOURCE

if [ $? -ne 0 ]; then
        echo "Failed to unzip the opencv source!"
fi

echo "Complete!

Building opencv"

cd $OPENCV_SOURCE/*/

if [ -d build ]; then
	rm -rf build/*
else
	mkdir build
fi

cd build
cmake .. -DWITH_OPENNI2=ON -DWITH_OPENMP=ON -DWITH_OPENGL=ON \
-DWITH_OPENCL=ON -DWITH_JPEG=ON -DWITH_JASPER=ON -DBUILD_ZLIB=ON \
-DBUILD_TESTS=ON -DWITH_VA=ON -DWITH_VTK=ON -DWITH_PNG=ON

make
sudo make install

cd ../../..

echo "

Done installing opencv!"
