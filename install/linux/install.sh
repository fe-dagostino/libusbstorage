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

#
# Deamon installation
#
echo "Create $PRGR nstallation tree ..."
if [ ! -d /opt/$PRGR ]
then
        mkdir /opt/$PRGR
fi
if [ ! -d /opt/$PRGR/bin ]
then
        mkdir /opt/$PRGR/bin
fi
if [ ! -d /opt/$PRGR/cfg ]
then
        echo "Create cfg directory ..."
        mkdir /opt/$PRGR/cfg
fi

echo "Signaling exit to previous instance"
pkill $PRGR

echo "Copy dependencies ..."
cp ./lib/*   /usr/local/lib/
ldconfig

echo "Copy deamon ..."
cp ./bin/$PRGR   /opt/$PRGR/bin/

echo "Copy cron check version ..."
cp ./$CRON /opt/$PRGR/bin/

echo "Change owner for /opt/$PRGR"
chown -R $USER:$USER /opt/$PRGR

echo "Make mount points .."
if [ ! -d /mnt/usbkey1 ]
then
	mkdir /mnt/usbkey1
fi
if [ ! -d /mnt/usbkey2 ]
then
	mkdir /mnt/usbkey2
fi

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

