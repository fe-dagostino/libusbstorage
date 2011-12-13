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

cp ../../config/$CONF ./cfg/
cp ../../build/$PRGR  ./bin/

