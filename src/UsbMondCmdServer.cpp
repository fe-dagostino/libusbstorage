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


#include "UsbMondCmdServer.h"
#include "UsbMondConfig.h"
#include "UsbMondChannelFactory.h"

#include "FTcpConnectionListener.h"
#include "FTcpConnectionFactory.h"

#include "RCI/FRciCommandCollector.h"

#include "LOGGING/FLogger.h"

#include "COMMANDS/UsbMondList.h"
#include "COMMANDS/UsbMondHelp.h"
#include "COMMANDS/UsbMondGetVersion.h"


USING_NAMESPACE_LOGGING

GENERATE_CLASSINFO( UsbMondCmdServer, FObject )


UsbMondCmdServer::UsbMondCmdServer( const FService& rService )
  : m_rService( rService )
{

}

UsbMondCmdServer::~UsbMondCmdServer()
{

}

BOOL UsbMondCmdServer::Initialize()
{
  // Register RCI commands.
  m_rciServer.GetCommandCollector().Register( new UsbMondList      ( m_rciServer )    );
  m_rciServer.GetCommandCollector().Register( new UsbMondHelp      ( m_rciServer )    );
  m_rciServer.GetCommandCollector().Register( new UsbMondGetVersion( m_rService  )    );
  
  IConnectionFactory*  _pIConnectionFactory	= new FTcpConnectionFactory();
  if ( _pIConnectionFactory == NULL )
  {
    ERROR_INFO( "Tcp Connection Factory is NULL", Initialize() )
    return FALSE;
  }

  IConnectionListener* _pConnectionListener	= new FTcpConnectionListener( 
							      _pIConnectionFactory, 
							      UsbMondConfig::GetInstance().GetCmdServerAddress( NULL ), 
							      UsbMondConfig::GetInstance().GetCmdServerPort   ( NULL ), 
							      TRUE 
						      );
  if ( _pIConnectionFactory == NULL )
  {
    ERROR_INFO( "Connection Factory is NULL", Initialize() )
    return FALSE;
  }

  UsbMondChannelFactory*  _pChannelFactory		= new UsbMondChannelFactory();
  if ( _pChannelFactory == NULL )
  {
    ERROR_INFO( "Channel Factory is NULL", Initialize() )
    return FALSE;
  }

  m_rciServer.AddListener( _pConnectionListener, _pChannelFactory );

  return TRUE;
}

BOOL UsbMondCmdServer::Finalize()
{
  return TRUE;
}
