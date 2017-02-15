#!/bin/bash

echo "Installing openni"
printf '\n\n' | sudo apt-add-repository ppa:deb-rob/ros-trusty
sudo apt-get --yes --force-yes update
sudo apt-get --yes --force-yes install libopenni2-dev
echo "Done installing openni"
