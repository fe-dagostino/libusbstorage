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

#ifndef USBMONDEVENTSDISPATCHER_H
#define USBMONDEVENTSDISPATCHER_H

#include "FSingleton.h"
#include "FServer.h"
#include "FTcpConnectionListener.h"
#include "FTcpConnectionFactory.h"
#include "UsbMondProto.h"

/**
 * Event dispatcher singleton class.
 */
class UsbMondEventsDispatcher : public FSingleton
{
  ENABLE_FRTTI( UsbMondEventsDispatcher )
  DECLARE_SINGLETON( UsbMondEventsDispatcher ) 
  
protected:

  /***/
  VOID         OnInitialize();
  /***/
  VOID         OnFinalize();

public:  
  
  /**
   * Dispatch message to all connected channels.
   */
  BOOL         Dispatch( const UsbMondHeader& rMsg );
  
private:
  class UsbMondChannel;
  class UsbMondThreadFactory;
  class UsbMondChannelFactory;
  
public:  

  
private:
  FMutex      m_mtxDispatch;  
  FServer     m_server;
    
};

#endif // USBMONDEVENTSDISPATCHER_H
