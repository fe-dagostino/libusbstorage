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
PRGR1=usbdiskmon
CONF=usbmond.cfg

mkdir cfg
mkdir bin
mkdir lib

cp -v ../../config/$CONF                     ./cfg/
cp -v ../../build-release/$PRGR              ./bin/
cp -v ../../build-release/$PRGR1             ./bin/
cp -v /usr/local/lib/libfedlibrary-3.0.0.so  ./lib/

