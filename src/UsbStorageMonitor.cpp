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


#include "UsbStorageMonitor.h"
#include <FSocket.h>

USING_NAMESPACE_FED

GENERATE_CLASSINFO( UsbStorageMonitor, FThread )

UsbStorageMonitor::UsbStorageMonitor()
 : FThread( NULL, TP_ABOVE_NORMAL, 10240 ),
   m_bExit( false ),
   m_bRaiseEvents( false ),
   m_pUsbStorageMonitorEvents( NULL )
{
}

UsbStorageMonitor::~UsbStorageMonitor()
{
    m_bExit = true;
    FThread::Stop();
}

bool 	UsbStorageMonitor::Start( 
				  const FString&           sDestAddress,
				  WORD                     wDestPort,
				  UsbStorageMonitorEvents* pEvents 
				)
{
  if ( pEvents == NULL )
    return false;

  m_sDestAddress             = sDestAddress;
  m_wDestPort                = wDestPort;
  m_pUsbStorageMonitorEvents = pEvents;
  m_bRaiseEvents             = true;

  if ( !IsRunning() )
  {
    FThread::Start();
  }
  
  return true;
}

bool 	UsbStorageMonitor::Stop()
{
  m_bRaiseEvents = false;
  return true;
}

VOID	UsbStorageMonitor::Run()
{
  FSocket          *pSocket = NULL;
  
  while ( !m_bExit )
  {
    if ( pSocket == NULL )
    {
      // Just avoid loop
      FThread::Sleep( 500 );
      FTRY
      {
	/**
	* Create the socket instance
	*/
	pSocket = new FSocket( AF_INET, SOCK_STREAM, 0 );
	if (!pSocket) 
	{
	  if ( m_bRaiseEvents )
	    m_pUsbStorageMonitorEvents->OnErrorOccurs();  
	  
	  continue;
	}
    
	pSocket->Connect( m_sDestAddress, m_wDestPort ); 
      }
      FCATCH( FSocketException, ex )
      {
	delete pSocket;
	pSocket = NULL;
      }
      
      if (!pSocket) 
      {
	if ( m_bRaiseEvents )
	  m_pUsbStorageMonitorEvents->OnErrorOccurs();  
	
	continue;
      }
    } // if ( udev == NULL )
    

    struct   timeval timeout  = { 1/*tv.tv_sec*/, 0/*tv.tv_usec*/ }; 
    
    WORD _wResult = 0;
    FTRY
    {
      _wResult = pSocket->Select( FSocket::READY_FOR_READ|FSocket::EXCEPTION_OCCUR|FSocket::TIME_OUT_OCCUR, &timeout );
    }
    FCATCH( FSocketException, ex )
    {
      delete pSocket;
      pSocket = NULL;
    }

    if (!pSocket) 
    {
      if ( m_bRaiseEvents )
	m_pUsbStorageMonitorEvents->OnErrorOccurs();  
      
      continue;
    }

    if ( _wResult & FSocket::TIME_OUT_OCCUR )
      continue;

    if ( _wResult & FSocket::EXCEPTION_OCCUR )
    {
      delete pSocket;
      pSocket = NULL;
      
      continue;
    }

    if ( _wResult & FSocket::READY_FOR_READ )
    {
      FTRY
      {
	// If enqueued data are less than proto header most likely incoming
	// is corrupted
	if ( pSocket->Peek() < sizeof(UsbMondHeader) )
	{
	  delete pSocket;
	  pSocket = NULL;
	  
	  continue;
	}
	
	UsbMondHeader  _header;
	pSocket->Peek( &_header, sizeof(_header) );
	
	// Incoming frame is corrupted
	if ( _header.version != 1 )
	{
	}
	
	switch ( _header.type )
	{
	  case eMsgUndefined:
	  {
	  }; break;  

	  case eMsgNotifyUsbDeviceEvent:
	  {
	    UsbMondNotifyUsbDevice   _event;
	    int                      _evlen = sizeof(_event);
	    
	    pSocket->Receive( &_event, _evlen );
	    
	    if ( m_bRaiseEvents )
	    {
	      m_pUsbStorageMonitorEvents->OnDeviceUsbDeviceEvent( _event );
	    }
	  }; break;
	  
	  case eMsgNotifyDiskEvent:
	  {
	    UsbMondNotifyDisk   _event;
	    int                 _evlen = sizeof(_event);
	    
	    pSocket->Receive( &_event, _evlen );
	    
	    if ( m_bRaiseEvents )
	    {
	      m_pUsbStorageMonitorEvents->OnDeviceDiskEvent( _event );
	    }
	  }; break;
	    
	  case eMsgNotifyPartitionEvent:
	  {
	    UsbMondNotifyPartition   _event;
	    int                      _evlen = sizeof(_event);
	    
	    pSocket->Receive( &_event, _evlen );
	    
	    if ( m_bRaiseEvents )
	    {
	      m_pUsbStorageMonitorEvents->OnDevicePartitionEvent( _event );
	    }
	  }; break;
	  
	  default:
	  {
	  }; break;
	}//switch ( _header.type )
      }
      FCATCH( FSocketException, ex )
      {
	delete pSocket;
	pSocket = NULL;
      }
    }
  }// while ( !m_bExit )
}
