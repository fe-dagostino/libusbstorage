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


#include <libudev.h>
#include <sys/mount.h>

#include "UsbMondEvents.h"
#include "UsbMond.h"
#include "UsbMondEventsDispatcher.h"

#include "FFileSystem.h"
#include "LOGGING/FLogger.h"
#include "LOGGING/FLogThread.h"
#include "LOGGING/FLogDeviceCollector.h"
#include "LOGGING/FLogMessageColorization.h"
#include "LOGGING/FTcpLogDevice.h"
#include "LOGGING/FDiskLogDevice.h"
#include "LOGGING/FConsoleLogDevice.h"

GENERATE_CLASSINFO( UsbMondEvents, FServiceEventInterface )

USING_NAMESPACE_FED
USING_NAMESPACE_LOGGING


UsbMondEvents::UsbMondEvents( const FService& rService )
{
  
}

UsbMondEvents::~UsbMondEvents()
{
  
}


BOOL UsbMondEvents::OnStart( const FService* pService )
ENTER( OnStart() )
  const UsbMond* _pUsbMond = (const UsbMond*)pService;

  //////////////
  //  Initialize Recd configuration
  FTRY
  {
    UsbMondConfig::Initialize();
  }
  FCATCH( FException, ex )
  {
    TRACE_EXCEPTION_CATCH( ex, OnStart() )
  }

  ///////////////////
  // Initialize the log device collector
  FLogDeviceCollector::Initialize();

  ///////////////////
  // Create message colorozation class instance used to colorize
  // streamed messages on the consolle and on the tcp.
  const ILogMessageColorization* pLogMessageColorization  = new FLogMessageColorization();

  ///////////////////
  // Create an instace for a Log device over TCP.
  // The device will accept row tcp connection on port 10000 on each active interface.
  ILogDevice* _pLogDevice0 = new FTcpLogDevice( 
			      "TCP", 
			      UsbMondConfig::GetInstance().GetLogServerAddress( NULL ), // Default "127.0.0.1" 
			      UsbMondConfig::GetInstance().GetLogServerPort( NULL ),    // Default 55000 
			      pLogMessageColorization 
			);

  ILogDevice* _pLogDevice1 = new FDiskLogDevice( 
			      "DSK", 
			      0xFFFFFFFF,
			      UsbMondConfig::GetInstance().GetLogDiskPath( NULL ),
			      UsbMondConfig::GetInstance().GetLogDiskPrefix( NULL ),
			      UsbMondConfig::GetInstance().GetLogDiskExtension( NULL ),
			      UsbMondConfig::GetInstance().GetLogDiskFiles( NULL ),
			      UsbMondConfig::GetInstance().GetLogDiskFileSizeLimit( NULL )
			);
#ifdef _DEBUG 
  ILogDevice* _pLogDevice2 = new FConsoleLogDevice( "CON", pLogMessageColorization );
#endif
  ///////////////////
  // Add the new devices to the list of available devices.
  FLogDeviceCollector::GetInstance().AddDevice( _pLogDevice0 );
  FLogDeviceCollector::GetInstance().AddDevice( _pLogDevice1 );
#ifdef _DEBUG 
  FLogDeviceCollector::GetInstance().AddDevice( _pLogDevice2 );
#endif
  ///////////////////
  // Initialize the Logger
  FLogger::Initialize();

  ///////////////////
  // Register the devices for message dispatching.
  FLogger::GetInstance().RegisterDevice( "TCP" );
  FLogger::GetInstance().RegisterDevice( "DSK" );
#ifdef _DEBUG
  FLogger::GetInstance().RegisterDevice( "CON" );
#endif
  (GET_LOG_MAILBOX())->SetLogMessageFlags( _pUsbMond->GetLogMessageFlags()     );
  (GET_LOG_MAILBOX())->SetVerbosityFlags ( _pUsbMond->GetVerbosityLevelFlags() );

  //Initialize member variable that control the program exit
  m_bExit = FALSE;

  //
  UsbMondEventsDispatcher::Initialize();
  
  //
  LoadMountPoints();
  
  /////////////////////////
  // Work around in order to avoid calling udev functions from thread
  // and not from the process.
  OnRun( pService );
  
  return TRUE;
EXIT

VOID UsbMondEvents::OnRun(const FService* pService)
ENTER( OnRun() )

  UsbMond* _pService =  (UsbMond*)pService;

  struct udev         *udev = NULL;
  struct udev_monitor *mon  = NULL;
  struct udev_device  *dev  = NULL;
  int fd;

  while ( !m_bExit )
  {
    if ( udev == NULL )
    {
      /**
       * Create the udev object
       */
      udev = udev_new();
      if (!udev) 
      {
	udev_unref(udev);
	udev = NULL;
	
	ERROR_INFO( "Failed to initilize udev object.", OnRun() )  
	
	continue;
      }
    
      /**
       * Set up a monitor to monitor hidraw devices
       */
      mon = udev_monitor_new_from_netlink(udev, "udev");
      if ( mon == NULL )
      {
	udev_unref(udev);
	udev = NULL;
	
	ERROR_INFO( "Failed to connect netlink.", OnRun() )  
	
	continue;
      }
      
      /**
       * Add filter for specific subsystem and device type
       */

      if ( 
	  ( udev_monitor_filter_add_match_subsystem_devtype(mon, "usb"  , "usb_device") != 0 ) ||
	  ( udev_monitor_filter_add_match_subsystem_devtype(mon, "block", "disk"      ) != 0 ) ||
	  ( udev_monitor_filter_add_match_subsystem_devtype(mon, "block", "partition" ) != 0 )
         )
      {
	udev_unref(udev);
	udev = NULL;
	
	ERROR_INFO( "Failed to add filters.", OnRun() )  
	
	continue;
      }

      /**
       * Enable receaving of events.
       */
      if ( udev_monitor_enable_receiving(mon) != 0 )
      {
	udev_unref(udev);
	udev = NULL;
	
	ERROR_INFO( "Failed to enable receaving.", OnRun() )  
	
	continue;
      }
      
      /**
       * Get the file descriptor (fd) for the monitor.
       * This fd will get passed to select() 
       */
      fd = udev_monitor_get_fd(mon);
    } // if ( udev == NULL )
    
    /** 
     * Set up the call to select(). In this case, select() will
     * only operate on a single file descriptor, the one
     * associated with our udev_monitor. Note that the timeval
     * object is set to 0, which will cause select() to not
     * block. 
     */
    fd_set         fds;
    struct timeval tv;
    int            ret;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    tv.tv_sec  = 0;
    tv.tv_usec = 0;

    ret = select(fd+1, &fds, NULL, NULL, &tv);
    
    /**
     * Check if our file descriptor has received data.
     */
    if (ret > 0 && FD_ISSET(fd, &fds))
    {
      /** 
       * Make the call to receive the device.
       * select() ensured that this will not block.
       */
      dev = udev_monitor_receive_device(mon);
      if (dev)
      {
	  eUsbAction eAction  = strtoaction( udev_device_get_action(dev) );
	  FString    sDevnode   ( udev_device_get_devnode(dev)   );
	  FString    sSubsystem ( udev_device_get_subsystem(dev) );
	  FString    sDevtype   ( udev_device_get_devtype(dev)   );

	  if ( sDevtype == "usb_device" )
	  {
	    UsbMondNotifyUsbDevice _devInfo(
				      eAction,
				      (const char*)sSubsystem,
				      (const char*)sDevtype,
				      (const char*)sDevnode
				    );
						    
	    UsbMondEventsDispatcher::GetInstance().Dispatch( _devInfo );
	  }
	  
	  if ( sDevtype == "disk" )
	  {
	    UsbMondNotifyDisk _devInfo(
				      eAction,
				      (const char*)sSubsystem,
				      (const char*)sDevtype,
				      (const char*)sDevnode
				    );
						    
	    UsbMondEventsDispatcher::GetInstance().Dispatch( _devInfo );
	  }
	  
	  if ( sDevtype == "partition" )
	  {
	    FString    sMountPoint("NULL");
	    double     dTotalSize     = -1.0;
	    double     dAvailableSize = -1.0;

	    switch( eAction )
	    {
	      case eAdd:
	      {
		FTList<FString >::Iterator _iter     = m_listMountPoints.Begin();
		bool                       _bMounted = false;
		
		while( _iter && (_bMounted==false) )
		{
		  const FParameter* pFileSystems = UsbMondConfig::GetInstance().GetSupportedFileSystems( NULL );
		  if ( pFileSystems != NULL )
		  {
		    for ( DWORD dwNdx = 0;  dwNdx < pFileSystems->GetCount(); dwNdx++ )
		    {
		      // FileSystem
		      FString  _sFs     = pFileSystems->GetValue(dwNdx);
		      FString  _sFsOpts = UsbMondConfig::GetInstance().GetFileSystemOptions( _sFs, NULL );
		      
		      
		      LOG_INFO( FString( 0, "Try to mount [%s] --> [%s] fs[%s] opts[%s]", 
							  (const char*)sDevnode, 
							  (const char*)(*_iter),
							  (const char*)_sFs,
							  (const char*)_sFsOpts ),
							  OnRun() )

		      int iRetVal = mount( 
				    (const char*)sDevnode,
				    (const char*)(*_iter),
				    (const char*)_sFs,
				    S_WRITE,
				    (const char*)_sFsOpts
				  );
				  
		      if ( iRetVal == 0 )
		      {
			MNT_MAP::iterator _mapIter = m_mapMntInfos.find( (const char*)sDevnode );
			
			// Check if specified item already exist
			if ( _mapIter == m_mapMntInfos.end() )
			{
			  MntInfo* pMntInfo = new MntInfo( sDevnode, (*_iter) );
			  
			  m_mapMntInfos[ (const char*)pMntInfo->sDevNode ] = pMntInfo;
			}
			else //Update existing device with new mount point
			{
			  MntInfo* pMntInfo  = _mapIter->second;
			  
			  pMntInfo->sMountPoint = (*_iter);
			}
			
			// Store current mount point in a local variable
			sMountPoint = (*_iter);

			FTRY
			{
			  FDiskInfo  _nfoDisk;
			  FFileSystem::GetDiskInfo( sMountPoint, _nfoDisk );
			  
			  ////////////////////////////////////////////
			  // Following values are repoted here in order to clarify operations 
			  //
			  // 1 Byte     = 8 Bits
			  // 1 Kilobyte = 1024 Bytes
			  // 1 Megabyte = 1048576 Bytes
			  // 1 Gigabyte = 1073741824 Bytes
			  
			  dTotalSize     = double(_nfoDisk.GetTotalBytes()    ) / 1073741824; 
			  dAvailableSize = double(_nfoDisk.GetAvailableBytes()) / 1073741824;
			}
			FCATCH( FFileSystemException, ex )
			{
			}
			
			_bMounted   = true;
			break;
		      }
		      else
		      {
			ERROR_INFO( FString( 0, "Failed to mount [%s] --> [%s] fs[%s] code[%d]", 
								      (const char*)sDevnode, 
								      (const char*)(*_iter),
								      (const char*)_sFs,
								      iRetVal ), OnRun() );
								      
			// Failed to mount current mount point, so we will try on next
			// mount point.
			_iter++;
		      }
		    }// for
		  }//if ( pFileSystems != NULL )
		  else
		  {
		    ERROR_INFO( "No File Systems has been specified in configuration", OnRun() );
		  }
		}
	      }; break;

	      case eRemove:
	      {
		LOG_INFO( FString( 0, "Removed device [%s] ", (const char*)sDevnode ), OnRun() )

		MNT_MAP::iterator _mapIter = m_mapMntInfos.find( (const char*)sDevnode );
		// Check if specified item already exist
		if ( _mapIter != m_mapMntInfos.end() )
		{
		  MntInfo* pMntInfo  = _mapIter->second;
		  
		  int iRetVal = umount( (const char*)pMntInfo->sMountPoint );
		  if ( iRetVal == 0 )
		  {
		  }
		  
		  m_mapMntInfos.erase( _mapIter );
		  
		  delete pMntInfo;
		}

	      }; break;
	    }
	    
	    UsbMondNotifyPartition _devInfo(
					      eAction,
					      (const char*)sSubsystem,
					      (const char*)sDevtype,
					      (const char*)sDevnode,
					      (const char*)sMountPoint,
					      dTotalSize,
					      dAvailableSize
					    );
						    
	    UsbMondEventsDispatcher::GetInstance().Dispatch( _devInfo );
	  } //if ( sDevtype == "partition" )

	  
	  udev_device_unref(dev);
      }
      else 
      {
	ERROR_INFO( "Failed to receive device.", OnRun() )  
      }					
    }
    
    // Suspend execution for 100 ms
    FThread::Sleep( 100 );
    
  }//while ( !m_bExit )
  
  udev_unref(udev);
EXIT

VOID UsbMondEvents::OnStop(const FService* pService)
ENTER( OnStop() )

  //
  m_bExit = TRUE;
EXIT

VOID UsbMondEvents::OnInterrogate(const FService* pService)
ENTER( OnInterrogate() )

EXIT

VOID UsbMondEvents::OnPause(const FService* pService)
ENTER( OnPause() )

EXIT

VOID UsbMondEvents::OnContinue(const FService* pService)
ENTER( OnContinue() )

EXIT

VOID UsbMondEvents::OnShutdown(const FService* pService)
ENTER( OnShutdown() )

EXIT

VOID UsbMondEvents::OnNetBindAdd(const FService* pService)
ENTER( OnNetBindAdd() )

EXIT

VOID UsbMondEvents::OnNetBindDisable(const FService* pService)
ENTER( OnNetBindDisable() )

EXIT

VOID UsbMondEvents::OnNetBindEnable(const FService* pService)
ENTER( OnNetBindEnable() )

EXIT

VOID UsbMondEvents::OnNetBindRemove(const FService* pService)
ENTER( OnNetBindRemove() )

EXIT

VOID UsbMondEvents::OnParamChange(const FService* pService)
ENTER( OnParamChange() )

EXIT

BOOL UsbMondEvents::OnUserControl(const FService* pService, DWORD dwOpcode)
ENTER( OnUserControl() )
  return TRUE;
EXIT

BOOL UsbMondEvents::LoadMountPoints()
ENTER( LoadMountPoints() )
  m_listMountPoints.PushTail( UsbMondConfig::GetInstance().GetUsb1MountPoint( NULL ) );
  m_listMountPoints.PushTail( UsbMondConfig::GetInstance().GetUsb2MountPoint( NULL ) );
  
  return TRUE;
EXIT
