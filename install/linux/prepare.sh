#!/bin/bash

#
# Check if running user is root
#
if [ "$(id -u)" != "0" ]
then
	echo "This script must be run as root" 1>&2
	exit -1
fi

USER=root
PRGR=usbmond
CONF=usbmond.cfg

mkdir cfg
mkdir bin
mkdir lib

cp ../../config/$CONF ./cfg/
cp ../../build/$PRGR  ./bin/
cp /usr/local/lib/fedlibrary-3.0.0.so  ./lib/

