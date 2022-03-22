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


#include "UsbMondProto.h"
#include <string.h>

UsbMondHeader::UsbMondHeader()
{
}

UsbMondHeader::UsbMondHeader(
		  uint16_t version,    
		  uint16_t type,
		  uint16_t reserved,
		  uint16_t size
		)
{
  this->version  = version;
  this->type     = type;
  this->reserved = reserved;
  this->size     = size;
}

/////////////////////////////////////////////////////////////////////

UsbMondNotifyUsbDevice::UsbMondNotifyUsbDevice()
{
}

UsbMondNotifyUsbDevice::UsbMondNotifyUsbDevice(
				      eUsbAction  action,
				      const char* subsystem,
				      const char* devtype,
				      const char* devnode
				    )
  : UsbMondHeader( 1, eMsgNotifyUsbDeviceEvent, 0, sizeof(UsbMondNotifyUsbDevice) )
{
  this->action         = action;

  memset( this->subsystem , '\0', sizeof(this->subsystem ) );
  memset( this->devtype   , '\0', sizeof(this->devtype   ) );
  memset( this->devnode   , '\0', sizeof(this->devnode   ) );

  strncpy( this->subsystem , subsystem , sizeof(this->subsystem ) );
  strncpy( this->devtype   , devtype   , sizeof(this->devtype   ) );
  strncpy( this->devnode   , devnode   , sizeof(this->devnode   ) );
}


////////////////////////////////////////////////////////////////////////////

UsbMondNotifyDisk::UsbMondNotifyDisk()
{
}

UsbMondNotifyDisk::UsbMondNotifyDisk(
				      eUsbAction  action,
				      const char* subsystem,
				      const char* devtype,
				      const char* devnode
				    )
  : UsbMondHeader( 1, eMsgNotifyDiskEvent, 0, sizeof(UsbMondNotifyDisk) )
{
  this->action         = action;

  memset( this->subsystem , '\0', sizeof(this->subsystem ) );
  memset( this->devtype   , '\0', sizeof(this->devtype   ) );
  memset( this->devnode   , '\0', sizeof(this->devnode   ) );

  strncpy( this->subsystem , subsystem , sizeof(this->subsystem ) );
  strncpy( this->devtype   , devtype   , sizeof(this->devtype   ) );
  strncpy( this->devnode   , devnode   , sizeof(this->devnode   ) );
}
		    
/////////////////////////////////////////////////////////////////////

UsbMondNotifyPartition::UsbMondNotifyPartition()
{
}

UsbMondNotifyPartition::UsbMondNotifyPartition(
			  eUsbAction  action,
			  const char* subsystem,
			  const char* devtype,
			  const char* devnode,
			  const char* mountpoint,
			  double      disksize,          // Expressed in GB
			  double      availablesize      // Expressed in GB
                        )
  : UsbMondHeader( 1, eMsgNotifyPartitionEvent, 0, sizeof(UsbMondNotifyPartition) )
{
  this->action         = action;

  memset( this->subsystem , '\0', sizeof(this->subsystem ) );
  memset( this->devtype   , '\0', sizeof(this->devtype   ) );
  memset( this->devnode   , '\0', sizeof(this->devnode   ) );
  memset( this->mountpoint, '\0', sizeof(this->mountpoint) );

  strncpy( this->subsystem , subsystem , sizeof(this->subsystem ) );
  strncpy( this->devtype   , devtype   , sizeof(this->devtype   ) );
  strncpy( this->devnode   , devnode   , sizeof(this->devnode   ) );
  strncpy( this->mountpoint, mountpoint, sizeof(this->mountpoint) );
  this->disksize       = disksize;
  this->availablesize  = availablesize;
}


UsbMondNotifyPartitionReleased::UsbMondNotifyPartitionReleased()
  : UsbMondHeader( 1, eMsgNotifyPartitionReleasedEvent, 0, sizeof(UsbMondNotifyPartitionReleased) )
{
  memset( this->mountpoint, '\0', sizeof(this->mountpoint) );

  this->released = false;
};

UsbMondNotifyPartitionReleased::UsbMondNotifyPartitionReleased( const char* mountpoint, bool released )
  : UsbMondHeader( 1, eMsgNotifyPartitionReleasedEvent, 0, sizeof(UsbMondNotifyPartitionReleased) )
{
  memset( this->mountpoint, '\0', sizeof(this->mountpoint) );

  strncpy( this->mountpoint, mountpoint, sizeof(this->mountpoint) );
  
  this->released = released;
};
  


UsbMondRequestPartitionRelease::UsbMondRequestPartitionRelease( const char* mountpoint )
  : UsbMondHeader( 1, eMsgRequestPartitionRelease, 0, sizeof(UsbMondRequestPartitionRelease) )
{
  memset( this->mountpoint, '\0', sizeof(this->mountpoint) );

  strncpy( this->mountpoint, mountpoint, sizeof(this->mountpoint) );
}

const char* msgtostr( eUsbMessages eMsg )
{
  switch( eMsg )
  {
    case eMsgUndefined:
        return "eMsgUndefined";
    case eMsgNotifyUsbDeviceEvent:
        return "eMsgNotifyUsbDeviceEvent";
    case eMsgNotifyUsbDeviceEventAnswer:
        return "eMsgNotifyUsbDeviceEventAnswer";
    case eMsgNotifyDiskEvent:
        return "eMsgNotifyDiskEvent";
    case eMsgNotifyDiskEventAnswer:
        return "eMsgNotifyDiskEventAnswer";
    case eMsgNotifyPartitionEvent:
        return "eMsgNotifyPartitionEvent";
    case eMsgNotifyPartitionEventAnswer:
        return "eMsgNotifyPartitionEventAnswer";
    case eMsgNotifyPartitionReleasedEvent:
        return "eMsgNotifyPartitionReleasedEvent";
    case eMsgNotifyPartitionReleasedEventAnswer:
        return "eMsgNotifyPartitionReleasedEventAnswer";
    case eMsgRequestPartitionRelease:
        return "eMsgRequestPartitionRelease";
    case eMsgRequestPartitionReleaseAnswer:
        return "eMsgRequestPartitionReleaseAnswer";
  }

  return "UNMANAGED";
}

eUsbAction strtoaction( const char* pAction )
{
  if ( pAction == NULL )
    return eUndefined; 
  
  if ( strcmp( pAction, "add"    ) == 0 )
    return eAdd;
    
  if ( strcmp( pAction, "remove" ) == 0 )
    return eRemove;

  if ( strcmp( pAction, "none"   ) == 0 )
    return eNone;
  
  return eUndefined; 
}
