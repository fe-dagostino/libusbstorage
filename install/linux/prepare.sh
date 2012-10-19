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
mkdir doc

#cp -v ../../config/$CONF                     ./cfg/
cp -v ../../build-release/$PRGR              ./bin/
cp -v ../../build-release/$PRGR1             ./bin/
cp -v /usr/local/lib/libfedlibrary-3.0.1.so  ./lib/
tar zcvf doc/html.tar.gz ../../docs/doxy/html
find . -type f |xargs md5sum > md5.txt


