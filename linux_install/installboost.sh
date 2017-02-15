#!/bin/bash

BOOST_DOWNLOAD="https://sourceforge.net/projects/boost/files/boost/1.63.0/boost_1_63_0.tar.bz2/download"
BOOST_SOURCE=$(pwd)/boost_source
BOOST_TAR=boostsource.tar.bz2

echo "Downloading Boost 1.63.0..."

if [ ! -f $BOOST_TAR ]; then
	wget -O $BOOST_TAR $BOOST_DOWNLOAD
else
	echo "Boost source already installed here!"
fi

if [ $? -ne 0 ]; then
	echo "Failed to download the boost source"
fi


echo "Complete!


Untarring Boost..."

if [ -d $BOOST_SOURCE ]; then
	rm -rf $BOOST_SOURCE/*
else
	mkdir $BOOST_SOURCE
fi

tar --bzip2 -xf $BOOST_TAR -C $BOOST_SOURCE --strip-components 1

if [ $? -ne 0 ]; then
	echo "Failed to untar the boost source!"
fi

echo "Complete!

Building boost..."

cd $BOOST_SOURCE

sudo chmod 777 bootstrap.sh
./bootstrap.sh
sudo ./b2 install

cd ..

echo "Complete!

Done installing boost!"

