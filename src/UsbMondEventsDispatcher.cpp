/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) <year>  <name of author>

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

#include "UsbMondEventsDispatcher.h"

#include "FMutexCtrl.h"
#include "FChannel.h"
#include "IThreadFactory.h"
#include "FChannelFactory.h"
#include "UsbMondConfig.h"

#include "LOGGING/FLogger.h"

#include <sys/mount.h>

NAMESPACE_FED_BEGIN
NAMESPACE_LOGGING_BEGIN


/**
 *
 */
class UsbMondEventsDispatcher::UsbMondChannel : protected FChannel, virtual protected IChannelReaderEvents
{
public:
  /***/
  UsbMondChannel( IConnection* pIConnection, IThreadFactory* pThreadFactory )
    : FChannel( pIConnection, pThreadFactory )
  {
  }

  /***/
  virtual ~UsbMondChannel()
  {
  }

  BOOL Write( const UsbMondHeader& rMsg )
  {
    BOOL bRetVal = FALSE;
    FTRY
    {
      if ( GetConnection() != NULL )
      {
	bRetVal = GetConnection()->Write( FData(	
					&rMsg, 
					(DWORD)rMsg.size
				      ) 
			      );
      }
    }
    FCATCH( FChannelException, ex ) 
    {
#ifdef _TRACING_EXCEPTION_CATCH
      if ( FLogger::IsValid() )
      {
	FLogMailbox* _pLogMbx = FLogger::GetInstance().GetGlobalMailbox();
	
	if ( _pLogMbx->TestLogMessageFlag( FLogMessage::MT_TRACE_EXCEPTION ) )
	{
	  FLogMessage* pLogMessage = new FLogMessage( _pLogMbx->GetName(), FLogMessage::MT_CATCH_EXCEPTION, "UsbMondChannel", "Write", ex.GetMessage() );
	  _pLogMbx->Write(pLogMessage); 
	}
      }
#endif
    }
    
    if ( bRetVal == FALSE )
    {
      Finalize();
    }
    
    return bRetVal;
  }
  
// IChannelReaderEvents implementation
protected:
  /***/
  VOID	OnReaderStarted()
  {
  }

  /***/
  VOID	OnDataReceived( const VOID* pData, DWORD dwDataLen )
  {
    if ( dwDataLen < sizeof(UsbMondHeader) )
    {
      //@todo
      return;
    }
    
    const UsbMondHeader* pUsbMondHeader = (const UsbMondHeader*)pData;
    
    switch ( pUsbMondHeader->type )
    {
      case eMsgRequestPartitionRelease:
      {
	const UsbMondRequestPartitionRelease* pReqPartRel = (const UsbMondRequestPartitionRelease*)pData;
	
        if ( FLogger::IsValid() )
        {
          FLogMailbox* _pLogMbx = FLogger::GetInstance().GetGlobalMailbox();

          if ( _pLogMbx->TestLogMessageFlag( FLogMessage::MT_LOGGING_INFO ) )
          {
            FString      sMsg( 0, "SYSCALL umount(%s)",  pReqPartRel->mountpoint );
            FLogMessage* pLogMessage = new FLogMessage( _pLogMbx->GetName(), FLogMessage::MT_LOGGING_INFO, "UsbMondChannel", "OnDataReceived", sMsg );
            _pLogMbx->Write(pLogMessage);
          }
        }

        // The sync() function shall cause all information in memory that updates 
        // file systems to be scheduled for writing out to all file systems. 
	sync();

	int _iRetVal = umount2( pReqPartRel->mountpoint, MNT_DETACH );

        if ( FLogger::IsValid() )
        {
          FLogMailbox* _pLogMbx = FLogger::GetInstance().GetGlobalMailbox();

          if ( _pLogMbx->TestLogMessageFlag( FLogMessage::MT_LOGGING_INFO ) )
          {
            FString      sMsg;
            if ( _iRetVal >= 0 )
              sMsg.Format( 0, "SYSCALL umount(%s) RETVAL[%d]",  pReqPartRel->mountpoint, _iRetVal );
            else
              sMsg.Format( 0, "SYSCALL umount(%s) RETVAL[%d] ERRNO[%d]",  pReqPartRel->mountpoint, _iRetVal, errno );

            FLogMessage* pLogMessage = new FLogMessage( _pLogMbx->GetName(), FLogMessage::MT_LOGGING_INFO, "UsbMondChannel", "OnDataReceived", sMsg );
            _pLogMbx->Write(pLogMessage);
          }
        }

	
	UsbMondNotifyPartitionReleased        notifyParRel( pReqPartRel->mountpoint, (_iRetVal==0));
	
	// Dispatch event to all connected clients.
	UsbMondEventsDispatcher::GetInstance().Dispatch( notifyParRel );
	
      }; break;
      default:
      {
      }; break;
    }
  }

  /**
  * By default, in presence of errors, return value is TRUE and 
  * instruct the channel to finalize the connection and to release 
  * memory resources.
  */
  BOOL	OnReaderError( ChannelError eError )
  { 
    return TRUE;
  };

  /***/
  VOID	OnReadTimeOut()
  {
  }

  /***/
  VOID	OnStopReader()
  {
  }

/* FChannelimplementation */
protected:
  /***/
  IChannelReaderEvents*   GetReaderEvents() { return this; };
  /***/
  IChannelWriterEvents*   GetWriterEvents() { return NULL; };

  friend class UsbMondEventsDispatcher::UsbMondChannelFactory;
};

// Threads coming from RCI runs in concurrency with main thread that is set 
// to with TP_CRITICAL priority. Creating RCI thread with the same priority
// will force the scheduler to provide the same cpu time occupation avoiding
// RCI threads from waiting long time before to be served.
class UsbMondEventsDispatcher::UsbMondThreadFactory : public IThreadFactory
{
public:
  FThread*	Create( IRunnable* pRunnable )
  {
    return new FThread( NULL, pRunnable, FThread::TP_CRITICAL, 1024 );
  }
};

class UsbMondEventsDispatcher::UsbMondChannelFactory : public FChannelFactory
{
public:
	UsbMondChannelFactory()
	{
	}

	~UsbMondChannelFactory()
	{
	}

protected:
	FChannel*		CreateChannel( IConnection* pIConnection ) const
	{
		UsbMondChannel* pUsbMondChannel = new UsbMondChannel( pIConnection, new UsbMondThreadFactory() );
		
		if ( pUsbMondChannel != NULL )
		{
		  LOG_INFO( "Start Reader on a new Channel", CreateChannel() )
		  pUsbMondChannel->StartReader( pUsbMondChannel );
		}
		
		return pUsbMondChannel;
	}

};

///////////////////////////////////////////////////////////

GENERATE_CLASSINFO( UsbMondEventsDispatcher, FSingleton )
IMPLEMENT_SINGLETON( UsbMondEventsDispatcher )


VOID   UsbMondEventsDispatcher::OnInitialize()
{
  IConnectionFactory*    _pIConnectionFactory = new FTcpConnectionFactory();

  IConnectionListener*   _pConnectionListener = new FTcpConnectionListener( 
							_pIConnectionFactory, 
							UsbMondConfig::GetInstance().GetMonitorServerAddress( NULL ), 
							UsbMondConfig::GetInstance().GetMonitorServerPort( NULL ), 
							TRUE 
						    );
  UsbMondChannelFactory* _pChannelFactory     = new UsbMondChannelFactory();

  m_server.AddListener( _pConnectionListener, _pChannelFactory );
}

VOID   UsbMondEventsDispatcher::OnFinalize()
{
}
  
  
BOOL    UsbMondEventsDispatcher::Dispatch( const UsbMondHeader& rMsg )
{
  FMutexCtrl _mtxCtrl( m_mtxDispatch );

  LOG_INFO( FString( 0, "Dispatching Message [%d] Len [%d]", rMsg.type, rMsg.size ), Dispatch() )

  FChannelCollector::Iterator _iter            = m_server.GetChannelCollector().Begin();
  UsbMondChannel*             _pUsbMondChannel = NULL;

  while ( (_pUsbMondChannel = (UsbMondChannel*)_iter.GetChannel()) )
  {
    if ( _pUsbMondChannel->Write( rMsg ) == FALSE )
    {
      ERROR_INFO( "An error occurs dispatching message to USB MONd Clients", Dispatch() )
    }
  }

  return TRUE;  
} 
