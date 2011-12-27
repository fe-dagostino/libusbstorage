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


#ifndef USBSTORAGEMONITOR_H
#define USBSTORAGEMONITOR_H

#include "FThread.h"
#include "FSocket.h"
#include "UsbStorageMonitorEvents.h"

#include <libudev.h>

USING_NAMESPACE_FED

/*
 */
class UsbStorageMonitor : protected FThread
{
  ENABLE_FRTTI( UsbStorageMonitor )
public:
  /***/
  UsbStorageMonitor();
  /***/
  ~UsbStorageMonitor();

  /**
   * Connect StorageMonitor instance to the specified server.
   * @param sDestAddress   server destination address
   * @param wDestPort      server destination port
   * @param pEvents        events implemetation that will be raised from 
   *                       monitor instance.
   * Return value will be true in case of success, false otherwise.
   */
  bool 	Start(
	      const FString&           sDestAddress,
	      WORD                     wDestPort,
	      UsbStorageMonitorEvents* pEvents 
	      );
  /**
   * Stop monitoring.
   * Do not release the connection with the server but just suspend raising
   * at the appliation level.
   * Return true if the function succeded, false otherwise.
   */
  bool 	Stop();
  
  /**
   * Used to unmount the partition. 
   * Unmounting force flush of all disk buffers so it will avoid corrupted
   * data stored on usb support.
   * An event will be dispatched to all connected clients in order to update
   * them that the resources will be no available.
   * Return true if the function succeded, false otherwise.
   */
  bool 	RequestPartitionRelease( const FString& sMountPoint );
  
  
private:
  /**
   */
  VOID     Run();

  /**
   * Connect to the remote server or do nothing if the connection is alredy active.
   * Return true if the function succeded, false otherwise.
   */
  bool     Connect();
  /**
   */
  bool     Send( const UsbMondHeader* pPacket );
  /**
   */
  bool     WaitAnswer( UsbMondHeader* pAnswer, int len, long waitingtime );

private:
  FMutex                    m_mtxMonitor;
  FSocket*                  m_pSocket;

  bool                      m_bExit;
  bool                      m_bRaiseEvents;
  FString                   m_sDestAddress;
  WORD                      m_wDestPort;
  UsbStorageMonitorEvents*  m_pUsbStorageMonitorEvents;
};

#endif // USBSTORAGEMONITOR_H
