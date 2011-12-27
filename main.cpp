
#include "FStopWatch.h"
#include "./include/UsbStorageMonitor.h"


class UsbEvents : public UsbStorageMonitorEvents
{
  void OnErrorOccurs()
  {
    printf( "Error@\n" );
  }

  void OnDeviceUsbDeviceEvent( const UsbMondNotifyUsbDevice& event )
  {
    printf( "Node[%s], Subsystem[%s], Devtype[%s], Action[%d]\n", 
	    event.devnode, event.subsystem, event.devtype, event.action ); 
  }
    
  void OnDeviceDiskEvent( const UsbMondNotifyDisk& event )
  {
    printf( "Node[%s], Subsystem[%s], Devtype[%s], Action[%d]\n", 
	    event.devnode, event.subsystem, event.devtype, event.action ); 
  }

  void OnDevicePartitionEvent( const UsbMondNotifyPartition& event )
  {
    printf( "Node[%s], Subsystem[%s], Devtype[%s], MountPoint[%s], Action[%d], Tot[%f], Aval[%f]\n", 
	    event.devnode, event.subsystem, event.devtype, event.mountpoint, event.action, event.disksize, event.availablesize ); 
  }
  
  void OnDevicePartitionReleasedEvent( const UsbMondNotifyPartitionReleased& event )
  {
    printf( "MountPoint[%s], Released[%d]\n", 
	    event.mountpoint, event.released ); 
  }
};

int main (void)
{
  UsbStorageMonitor _monUsbStorage;
  FStopWatch        _swReqRelease;

  _monUsbStorage.Start( 
			"127.0.0.1",
			50000,
			new UsbEvents() );
  
  _swReqRelease.Reset();
  
  while ( true )
  {
     printf( "Running .. \n" );
     FThread::Sleep( 10000 );
     
     if ( _swReqRelease.Peek() > 60 )
     {
       printf( "Request Partition Release for [/mnt/usbkey1] .. \n" );
       _monUsbStorage.RequestPartitionRelease( "/mnt/usbkey1" );
       
       _swReqRelease.Reset();
     }
  }

  return 0;       
}
