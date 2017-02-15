#!/bin/bash

echo "Welcome to the perception dependency installer
Created by David Smerkous for Titan Academy Robotics Vision
Date: Mon, Jan 23, 2017
Team: 5431
Today: `date`

Starting..."

BASE=installessentials.sh
BOOST=installboost.sh
OPENCV=installopencv.sh
OPENNI=installopenni.sh
FREENECT=installlibfreenect2.sh
NTCORE=installntcore.sh
PERCEPTION_PATH=$(dirname "$0")


if [[ $# -eq 1 ]]; then
	case $1 in
		"--help") echo "Usage: $0 [Path to install directory]"; exit 1;;
		*) PERCEPTION_PATH="$1";;
	esac
	echo "Using defined path: $1"
else
	echo "Using current directory for installation: $PERCEPTION_PATH"
fi

TOTAL_INSTALL=6

print_progress() {
	printf "\n\n\n\nTotal progress: $1/$TOTAL_INSTALL\n\n\n\n"
}

perception_install() {
	$PERCEPTION_PATH/$1
}

print_progress 0
perception_install $BASE
print_progress 1
perception_install $BOOST
print_progress 2
perception_install $OPENCV
print_progress 3
perception_install $OPENNI
print_progress 4
perception_install $FREENECT
print_progress 5
perception_install $NTCORE
print_progress 6

echo "

Done installing perception!"
