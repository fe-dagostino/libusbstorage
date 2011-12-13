/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Francesco Emanuele D'Agostino <fedagostino@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef __USBMOND_PROTO_H__
#define __USBMOND_PROTO_H__

#include <stdint.h>

enum eUsbMessages
{
  eMsgUndefined,
  eMsgNotifyUsbDeviceEvent,
  eMsgNotifyUsbDeviceEventAnswer,
  eMsgNotifyDiskEvent,
  eMsgNotifyDiskEventAnswer,
  eMsgNotifyPartitionEvent,
  eMsgNotifyPartitionEventAnswer
};

struct UsbMondHeader
{
  UsbMondHeader();
  UsbMondHeader(
		  uint16_t version,    
		  uint16_t type,
		  uint16_t reserved,
		  uint16_t size
		);
		
  uint16_t version;    // Prtocol version
  uint16_t type;
  uint16_t reserved;
  uint16_t size;
};

enum eUsbAction
{
  eUndefined,
  eNone,      //is not associated with a particular event
  eAdd,       //for attach events
  eRemove     //for detach events
};

#define   DEV_PROPS_MAX_LEN 200


struct UsbMondNotifyUsbDevice : public UsbMondHeader
{
  UsbMondNotifyUsbDevice();
  UsbMondNotifyUsbDevice(
		      eUsbAction  action,
		      const char* subsystem,
		      const char* devtype,
		      const char* devnode
		    );
  eUsbAction    action;
  char 		subsystem[DEV_PROPS_MAX_LEN];
  char 		devtype[DEV_PROPS_MAX_LEN];
  char 		devnode[DEV_PROPS_MAX_LEN];
};

struct UsbMondNotifyDisk : public UsbMondHeader
{
  UsbMondNotifyDisk();
  UsbMondNotifyDisk(
		      eUsbAction  action,
		      const char* subsystem,
		      const char* devtype,
		      const char* devnode
		    );
  eUsbAction    action;
  char 		subsystem[DEV_PROPS_MAX_LEN];
  char 		devtype[DEV_PROPS_MAX_LEN];
  char 		devnode[DEV_PROPS_MAX_LEN];
};

struct UsbMondNotifyPartition : public UsbMondHeader
{
  UsbMondNotifyPartition();
  UsbMondNotifyPartition(
			  eUsbAction  action,
			  const char* subsystem,
			  const char* devtype,
			  const char* devnode,
			  const char* mountpoint,
			  double      disksize,          // Expressed in GB
			  double      availablesize      // Expressed in GB
                        );
  eUsbAction    action;
  char 		subsystem[DEV_PROPS_MAX_LEN];
  char 		devtype[DEV_PROPS_MAX_LEN];
  char 		devnode[DEV_PROPS_MAX_LEN];
  char		mountpoint[DEV_PROPS_MAX_LEN];
  double        disksize;                        // Expressed in GB
  double        availablesize;                   // Expressed in GB
};

eUsbAction strtoaction( const char* );

#endif //__USBMOND_PROTO_H__
