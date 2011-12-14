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
CRON=check.cron

echo "Check if user $USER exists"
/bin/id $USER 2>/dev/null
if [ $? -eq 0 ]
then
	echo "User $USER found"
else
	echo "User $USER not found"
	exit -2
fi

#
# Deamon installation
#
echo "Create $PRGR nstallation tree ..."
mkdir /opt/$PRGR
mkdir /opt/$PRGR/bin
echo "Create cfg directory ..."
mkdir /opt/$PRGR/cfg

echo "Copy deamon ..."
cp ./bin/$PRGR   /opt/$PRGR/bin/

echo "Copy deamon ..."
cp ./lib/*   /usr/local/lib/
ldconfig

echo "Copy cron check version ..."
cp ./$CRON /opt/$PRGR/bin/

echo "Change owner for /opt/$PRGR"
chown -R $USER:$USER /opt/$PRGR

#
# Deamon configuration  
#

echo "Copy deamon cfg ..."
cp ./cfg/$CONF /opt/$PRGR/cfg/

echo "Create cfg directory in /etc ..."
mkdir /etc/$PRGR
echo "Create log directory in /var/log ..."
mkdir /var/log/$PRGR


echo "Change owner for /etc/$PRGR"
chown -R $USER:$USER /etc/$PRGR
echo "Change owner for /var/log/$PRGR"
chown -R $USER:$USER /var/log/$PRGR

ln -s /opt/$PRGR/cfg/$CONF /etc/$PRGR/$CONF 

crontab -u $USER ./cron.txt

