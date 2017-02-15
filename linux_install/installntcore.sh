#!/bin/bash

NTCORE_SOURCE=ntcore_source

echo "Installing requirements for ntcore"

sudo apt-get --yes --force-yes install gcc-multilib g++-multilib

echo "Downloading ntcore source"

git clone https://github.com/wpilibsuite/ntcore $NTCORE_SOURCE

echo "Complete!

Building ntcore"

cd $NTCORE_SOURCE

./gradlew build -PskipArm
./gradlew ntcoreZip -PskipArm
./gradlew ntcoreSourceZip -PskipArm

if [ -d build ]; then
	rm -rf build/*
else
	mkdir build
fi

cd build

cmake ..
make -j5
sudo make install

cd ../..

echo "Done installing ntcore!"
