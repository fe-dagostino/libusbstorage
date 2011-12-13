#include <iostream>

/*******************************************
 libudev example.

 This example prints out properties of
 each of the hidraw devices. It then
 creates a monitor which will report when
 hidraw devices are connected or removed
 from the system.

 This code is meant to be a teaching
 resource. It can be used for anyone for
 any reason, including embedding into
 a commercial product.
 
 The document describing this file, and
 updated versions can be found at:
    http://www.signal11.us/oss/udev/

 Alan Ott
 Signal 11 Software
 2010-05-22 - Initial Revision
 2010-05-27 - Monitoring initializaion
              moved to before enumeration.
*******************************************/

#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>

void printProps(struct udev_device *dev)
{
     struct udev_list_entry *e = udev_device_get_properties_list_entry(dev);
     while (e) {
         printf( "prop{%s} = %s\n", udev_list_entry_get_name(e), udev_list_entry_get_value(e));
         e = udev_list_entry_get_next(e);
     }
}

int main (void)
{
	struct udev *udev;
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;
	struct udev_device *dev;

   	struct udev_monitor *mon;
	int fd;
	
	/* Create the udev object */
	udev = udev_new();
	if (!udev) {
		printf("Can't create udev\n");
		exit(1);
	}

	/* This section sets up a monitor which will report events when
	   devices attached to the system change.  Events include "add",
	   "remove", "change", "online", and "offline".
	   
	   This section sets up and starts the monitoring. Events are
	   polled for (and delivered) later in the file.
	   
	   It is important that the monitor be set up before the call to
	   udev_enumerate_scan_devices() so that events (and devices) are
	   not missed.  For example, if enumeration happened first, there
	   would be no event generated for a device which was attached after
	   enumeration but before monitoring began.
	   
	   Note that a filter is added so that we only get events for
	   "hidraw" devices. */
	
	/* Set up a monitor to monitor hidraw devices */
	mon = udev_monitor_new_from_netlink(udev, "udev");
        /*
	udev_monitor_filter_add_match_subsystem_devtype(mon, "dvb", NULL);
	udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", NULL);
        */
	udev_monitor_enable_receiving(mon);
	/* Get the file descriptor (fd) for the monitor.
	   This fd will get passed to select() */
	fd = udev_monitor_get_fd(mon);


	/* Create a list of the devices in the 'hidraw' subsystem. */
        const char *systems[] = { "dvb", "video4linux", "usb", NULL };
        int nr = 0;
        for (nr= 0; systems[nr]; nr++) {
        enumerate = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(enumerate, systems[nr]);
	udev_enumerate_scan_devices(enumerate);
	devices = udev_enumerate_get_list_entry(enumerate);
	/* For each item enumerated, print out its information.
	   udev_list_entry_foreach is a macro which expands to
	   a loop. The loop will be executed for each member in
	   devices, setting dev_list_entry to a list entry
	   which contains the device's path in /sys. */
	udev_list_entry_foreach(dev_list_entry, devices) {
		const char *path;
		
		/* Get the filename of the /sys entry for the device
		   and create a udev_device object (dev) representing it */
		path = udev_list_entry_get_name(dev_list_entry);
		dev = udev_device_new_from_syspath(udev, path);

		/* usb_device_get_devnode() returns the path to the device node
		   itself in /dev. */
		printf("\n------------------------\nDevice Node Path: %s\n", udev_device_get_devnode(dev));

		/* The device pointed to by dev contains information about
		   the hidraw device. In order to get information about the
		   USB device, get the parent device with the
		   subsystem/devtype pair of "usb"/"usb_device". This will
		   be several levels up the tree, but the function will find
		   it.*/
		struct udev_device *parent = udev_device_get_parent(dev);
		if (parent) {
			printf("parent device:\n");
			printProps(parent);
		/* From here, we can call get_sysattr_value() for each file
		   in the device's /sys entry. The strings passed into these
		   functions (idProduct, idVendor, serial, etc.) correspond
		   directly to the files in the /sys directory which
		   represents the USB device. Note that USB strings are
		   Unicode, UCS2 encoded, but the strings returned from
		   udev_device_get_sysattr_value() are UTF-8 encoded. */
		printf("  VID/PID: %s %s\n",
		        udev_device_get_sysattr_value(parent,"idVendor"),
		        udev_device_get_sysattr_value(parent, "idProduct"));
		printf("  %s\n  %s\n",
		        udev_device_get_sysattr_value(parent,"manufacturer"),
		        udev_device_get_sysattr_value(parent,"product"));
		printf("  serial: %s\n",
		         udev_device_get_sysattr_value(parent, "serial"));
		printf("  devname: %s\n",
		         udev_device_get_devnode(parent));
                }
                printProps(dev);
		udev_device_unref(dev);
	}
	/* Free the enumerator object */
	udev_enumerate_unref(enumerate);
        }
	
	/* Begin polling for udev events. Events occur when devices
	   attached to the system are added, removed, or change state. 
	   udev_monitor_receive_device() will return a device
	   object representing the device which changed and what type of
	   change occured.

	   The select() system call is used to ensure that the call to
	   udev_monitor_receive_device() will not block.
	   
	   The monitor was set up earler in this file, and monitoring is
	   already underway.
	   
	   This section will run continuously, calling usleep() at the end
	   of each pass. This is to demonstrate how to use a udev_monitor
	   in a non-blocking way. */
        char *identifier = 0;
	while (1) {
		/* Set up the call to select(). In this case, select() will
		   only operate on a single file descriptor, the one
		   associated with our udev_monitor. Note that the timeval
		   object is set to 0, which will cause select() to not
		   block. */
		fd_set fds;
		struct timeval tv;
		int ret;
		
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		
		ret = select(fd+1, &fds, NULL, NULL, &tv);
		
		/* Check if our file descriptor has received data. */
		if (ret > 0 && FD_ISSET(fd, &fds)) {
			/* Make the call to receive the device.
			   select() ensured that this will not block. */
			dev = udev_monitor_receive_device(mon);
			if (dev) {
printf("------------------------\n");
struct udev_list_entry *e = udev_device_get_properties_list_entry(dev);
const char *action = udev_device_get_action(dev);
if (action)
{
 if (strcmp(action, "add") == 0)
 {
  const char *subsystem = udev_device_get_subsystem(dev);
  if (subsystem && (strcmp(subsystem, "dvb") == 0))
  {
   struct udev_list_entry *ddt = udev_list_entry_get_by_name(e, "DVB_DEVICE_TYPE");
   if (ddt)
   {
    const char *dvb_device_type = udev_list_entry_get_value(ddt);
    if (dvb_device_type && (strcmp(dvb_device_type, "frontend") == 0))
    {
     const char *id = udev_device_get_devpath(dev);
     if (id)
     {
      identifier = strdup(id);
      const char *tmp = strstr(identifier, "/dvb/");
      if (tmp)
      {
       int pos = tmp - identifier;
       identifier[pos] = 0;
       printf("found identifier %s\n", identifier);
       printProps(dev);
      }
     }
    }
   }
  }
 }
 else if (strcmp(action, "remove") == 0)
 {
  if (identifier)
  {
   const char *devpath = udev_device_get_devpath(dev);
   printf("remove devpath %s\n", devpath);
   struct udev_list_entry *devlink = udev_device_get_devlinks_list_entry(dev);
   while (devlink) {
      printf("devlink: %s\n", udev_list_entry_get_name(devlink));
      devlink = udev_list_entry_get_next(devlink);
      }
   if (devpath && (strncmp(devpath, identifier, strlen(identifier)) == 0))
    printf("remove identified device!\n");
  }
 }
}

 				printf("Got Device\n");
				printf("   Node: %s\n", udev_device_get_devnode(dev));
				printf("   Subsystem: %s\n", udev_device_get_subsystem(dev));
				printf("   Devtype: %s\n", udev_device_get_devtype(dev));
				printf("   Action: %s\n", udev_device_get_action(dev));

                                while (e) {
                                      printf( "prop{%s} = %s\n", udev_list_entry_get_name(e), udev_list_entry_get_value(e));
                                      e = udev_list_entry_get_next(e);
                                }
                                struct udev_device *parentDev = udev_device_get_parent(dev);
                                if (parentDev) {
                                  printf("parent device:\n");
                                  printProps(parentDev);
                                }

				udev_device_unref(dev);
			}
			else {
				printf("No Device from receive_device(). An error occured.\n");
			}					
		        fflush(stdout);
		}
		usleep(250*1000);
                /*
		printf(".");
		fflush(stdout);
                */
	}
if (identifier)
 free(identifier);

	udev_unref(udev);

	return 0;       
}
