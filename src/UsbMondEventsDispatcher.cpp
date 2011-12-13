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

#include "FChannel.h"
#include "FChannelFactory.h"
#include "UsbMondConfig.h"


NAMESPACE_FED_BEGIN
NAMESPACE_LOGGING_BEGIN


/**
 *
 */
class UsbMondEventsDispatcher::UsbMondChannel : public FChannel
{
public:
	/***/
	UsbMondChannel( IConnection* pIConnection )
		: FChannel( pIConnection )
	{
	}

	/***/
	virtual ~UsbMondChannel()
	{
	}

/* FChannelimplementation */
protected:
	/***/
	IChannelReaderEvents*   GetReaderEvents() { return NULL; };
	/***/
	IChannelWriterEvents*   GetWriterEvents() { return NULL; };

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
		return new UsbMondChannel( pIConnection );
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
  FChannelCollector::Iterator _iter            = m_server.GetChannelCollector().Begin();
  UsbMondChannel*             _pUsbMondChannel = NULL;

  while ( (_pUsbMondChannel = (UsbMondChannel*)_iter.GetChannel()) )
  {
    FTRY
    {
      _pUsbMondChannel->GetConnection()->Write( FData(	
                                                        &rMsg, 
                                                        (DWORD)rMsg.size
                                                     ) 
                                               );
    }
    FCATCH( FChannelException, ex ) 
    {

    }
  }

  return TRUE;  
} 
