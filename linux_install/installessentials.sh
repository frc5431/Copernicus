#!/bin/bash

echo "Installing basic components"

sudo apt-get --yes --force-yes update
sudo apt-get --yes --force-yes install build-essential git npm rsync sshpass cmake software-properties-common pkg-config

echo "Installing java 8"

printf '\n\n' | sudo apt-add-repository ppa:webupd8team/java
sudo apt-get  --yes --force-yes update
sudo apt-get --yes --force-yes install oracle-java8-installer

export JAVA_HOME=/usr/lib/jvm/java-8-oracle

echo "Installing gradle"

printf '\n\n' | sudo add-apt-repository ppa:cwchien/gradle
sudo apt-get --yes --force-yes update
sudo apt-get --yes --force-yes install gradle

echo "

Done installing the essential packages"
