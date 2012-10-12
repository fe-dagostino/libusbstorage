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
#include "FMutexCtrl.h"

USING_NAMESPACE_FED

GENERATE_CLASSINFO( UsbStorageMonitor, FThread )

UsbStorageMonitor::UsbStorageMonitor()
 : FThread( NULL, TP_ABOVE_NORMAL, 10240 ),
   m_pSocket( NULL ),
   m_bExit( false ),
   m_bRaiseEvents( false ),
   m_pUsbStorageMonitorEvents( NULL )
{
}

UsbStorageMonitor::~UsbStorageMonitor()
{
    m_bExit = true;
    FThread::Stop();
    
    if ( m_pSocket != NULL )
    {
      delete m_pSocket;
      m_pSocket = NULL;
    }
}

bool 	UsbStorageMonitor::Start( 
				  const FString&           sDestAddress,
				  WORD                     wDestPort,
				  UsbStorageMonitorEvents* pEvents 
				)
{
  FMutexCtrl _mtxCtrl( m_mtxMonitor );
  
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
  FMutexCtrl _mtxCtrl( m_mtxMonitor );
  
  m_bRaiseEvents = false;
  return true;
}

bool 	UsbStorageMonitor::RequestPartitionRelease( const FString& sMountPoint )
{
  FMutexCtrl _mtxCtrl( m_mtxMonitor );
  
  if ( sMountPoint.IsEmpty() )
    return false;
  
  if ( Connect() == false )
    return false;
  
  UsbMondRequestPartitionRelease        _reqPartRel( (const char*)sMountPoint );
  if ( Send( &_reqPartRel ) == false )
    return false;
  
  return true;
}

VOID	UsbStorageMonitor::Run()
{
  
  while ( !m_bExit )
  {

    if ( Connect() == false )
    {
      // Just avoid loop
      FThread::Sleep( 500 );

      continue;
    }

    struct   timeval timeout  = { 1/*tv.tv_sec*/, 0/*tv.tv_usec*/ }; 
    
    WORD     _wResult = 0;
    FTRY
    {
      _wResult = m_pSocket->Select( FSocket::READY_FOR_READ|FSocket::EXCEPTION_OCCUR|FSocket::TIME_OUT_OCCUR, &timeout );
    }
    FCATCH( FSocketException, ex )
    {
      delete m_pSocket;
      m_pSocket = NULL;
    }

    if (!m_pSocket) 
    {
      if ( m_bRaiseEvents )
	m_pUsbStorageMonitorEvents->OnErrorOccurs();  
      
      // Favorite context switch
      FThread::YieldThread();
      continue;
    }

    if ( _wResult & FSocket::TIME_OUT_OCCUR )
    {
      // Favorite context switch
      FThread::YieldThread();
      continue;
    }
    
    if ( _wResult & FSocket::EXCEPTION_OCCUR )
    {
      delete m_pSocket;
      m_pSocket = NULL;

      // Favorite context switch
      FThread::YieldThread();
      continue;
    }

    if ( _wResult & FSocket::READY_FOR_READ )
    {
      FTRY
      {
	// When ready for read and no data are present this a condition  for 
	// broken connection.
	// If enqueued data are less than proto header most likely incoming
	// is corrupted
	if (
	    ( m_pSocket->Peek() == 0                    ) || 
	    ( m_pSocket->Peek() < sizeof(UsbMondHeader) )
	   )
	{
	  delete m_pSocket;
	  m_pSocket = NULL;
	  
	  // Favorite context switch
	  FThread::YieldThread();	  
	  continue;
	}
	
	UsbMondHeader  _header;
	m_pSocket->Peek( &_header, sizeof(_header) );
	
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
	    
	    m_pSocket->Receive( &_event, _evlen );
	    
	    if ( m_bRaiseEvents )
	    {
	      m_pUsbStorageMonitorEvents->OnDeviceUsbDeviceEvent( _event );
	    }
	  }; break;
	  
	  case eMsgNotifyDiskEvent:
	  {
	    UsbMondNotifyDisk   _event;
	    int                 _evlen = sizeof(_event);
	    
	    m_pSocket->Receive( &_event, _evlen );
	    
	    if ( m_bRaiseEvents )
	    {
	      m_pUsbStorageMonitorEvents->OnDeviceDiskEvent( _event );
	    }
	  }; break;
	    
	  case eMsgNotifyPartitionEvent:
	  {
	    UsbMondNotifyPartition   _event;
	    int                      _evlen = sizeof(_event);
	    
	    m_pSocket->Receive( &_event, _evlen );
	    
	    if ( m_bRaiseEvents )
	    {
	      m_pUsbStorageMonitorEvents->OnDevicePartitionEvent( _event );
	    }
	  }; break;

	  case eMsgNotifyPartitionReleasedEvent:
	  {
	    UsbMondNotifyPartitionReleased  _event;
	    int                             _evlen = sizeof(_event);
	    
	    m_pSocket->Receive( &_event, _evlen );
	    
	    if ( m_bRaiseEvents )
	    {
	      m_pUsbStorageMonitorEvents->OnDevicePartitionReleasedEvent( _event );
	    }
	  }; break;
	  
	  default:
	  {
	  }; break;
	}//switch ( _header.type )
      }
      FCATCH( FSocketException, ex )
      {
	delete m_pSocket;
	m_pSocket = NULL;
      }
    }
    
    // Favorite context switch
    FThread::YieldThread();
  }// while ( !m_bExit )
}


bool UsbStorageMonitor::Connect()
{
  bool       _bRetVal = true;
  FMutexCtrl _mtxCtrl( m_mtxMonitor );
  
  if ( m_pSocket != NULL )
    return _bRetVal;
  
  FTRY
  {
    /**
    * Create the socket instance
    */
    m_pSocket = new FSocket( AF_INET, SOCK_STREAM, 0 );
    if ( m_pSocket == NULL ) 
    {
      if ( m_bRaiseEvents )
	m_pUsbStorageMonitorEvents->OnErrorOccurs();  
      
      _bRetVal = false;
    }
    else
    {
      m_pSocket->Connect( m_sDestAddress, m_wDestPort );
    }
  }
  FCATCH( FSocketException, ex )
  {
    delete m_pSocket;
    m_pSocket = NULL;
    
    _bRetVal = false;
  }

  if ( m_pSocket == NULL ) 
  {
    if ( m_bRaiseEvents )
      m_pUsbStorageMonitorEvents->OnErrorOccurs();  
  }
  
  return _bRetVal;
}

bool UsbStorageMonitor::Send( const UsbMondHeader* pPacket )
{
  if ( pPacket == NULL )
    return false;

  bool       _bRetVal = true;
  FMutexCtrl _mtxCtrl( m_mtxMonitor );

  FTRY
  {
    m_pSocket->Send( pPacket, pPacket->size );
  }
  FCATCH( FSocketException, ex )
  {
    delete m_pSocket;
    m_pSocket = NULL;
    
    _bRetVal = false;
  }
  
  return _bRetVal;
}

bool UsbStorageMonitor::WaitAnswer( UsbMondHeader* pAnswer, int len, long waitingtime )
{
  bool       _bRetVal = false;
  FMutexCtrl _mtxCtrl( m_mtxMonitor );

  struct   timeval timeout  = { waitingtime, 0/*tv.tv_usec*/ }; 
    
  WORD     _wResult = 0;
  FTRY
  {
    _wResult = m_pSocket->Select( FSocket::READY_FOR_READ|FSocket::EXCEPTION_OCCUR|FSocket::TIME_OUT_OCCUR, &timeout );
  }
  FCATCH( FSocketException, ex )
  {
    delete m_pSocket;
    m_pSocket = NULL;
  }

  if ( _wResult & FSocket::READY_FOR_READ )
  {
    FTRY
    {
      // Broken connection condition
      if ( m_pSocket->Peek() == 0 )
      {
      }
      // Corrupted packet condition
      else if ( m_pSocket->Peek() < sizeof(UsbMondHeader) )
      {
      }
      else
      {
	m_pSocket->Receive( pAnswer, len );
	_bRetVal = true;
      }
    }
    FCATCH( FSocketException, ex )
    {
      delete m_pSocket;
      m_pSocket = NULL;
    }
  }
  
  if ( _wResult & FSocket::EXCEPTION_OCCUR )
  {
    delete m_pSocket;
    m_pSocket = NULL;
  }
  
  if ( _wResult & FSocket::TIME_OUT_OCCUR )
  {
  }
  
  return _bRetVal;
}
