#!/bin/bash

LIBFREENECT_URL=https://github.com/OpenKinect/libfreenect2
LIBFREENECT_SOURCE=libfreenect2_source

echo "Installing requirements for libfreenect2"

sudo apt-get --yes --force-yes update
sudo apt-get --yes --force-yes install libusb-1.0-0-dev libturbojpeg \
libjpeg-turbo8-dev beignet-dev opencl-headers libva-dev libjpeg-dev openni2-utils

echo "Complete!

Downloading libfreenect2 source"

git clone $LIBFREENECT_URL $LIBFREENECT_SOURCE

echo "Complete!

Building libfreenect2"

cd $LIBFREENECT_SOURCE

cd depends; ./download_debs_trusty.sh

# Install opengl
sudo dpkg -i debs/libglfw3*deb
sudo apt-get --yes --force-yes install -f
sudo apt-get --yes --force-yes install libgl1-mesa-dri-lts-vivid

# Install lib usb
sudo dpkg -i debs/libusb*deb

# Install opencl
printf '\n\n' | sudo apt-add-repository ppa:floe/beignet
sudo apt-get --yes --force-yes update
sudo apt-get --yes --force-yes install beignet-dev
sudo dpkg -i debs/ocl-icd*deb

cd ..


if [ -d build ]; then
	rm -rf build/*
else
	mkdir build
fi

cd build

cmake ..
make -j5
sudo make install

sudo cp ../platform/linux/udev/90-kinect2.rules /etc/udev/rules.d/

echo "Done installing libfreenect2"
