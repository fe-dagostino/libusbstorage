cmake_minimum_required (VERSION 2.6)
PROJECT(libusbstorage)

include_directories( ./include/ )
include_directories( ../FEDLibrary/source/include )
include_directories( ../FEDLibrary/source/template_include )

IF(NOT CMAKE_BUILD_TYPE )
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
ENDIF()

IF( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  set(  USBMOND_VERSION
	-1.0.1-debug
    )
  set(  USBSTORAGE_VERSION
	-1.0.0-debug
    )
  set(  FEDLIB_VER
        -3.0.0-debug
     )
ELSE()
  set(  USBMOND_VERSION
    )
  set(  USBSTORAGE_VERSION
	-1.0.0
    )
  set(  FEDLIB_VER
        -3.0.0
     )
ENDIF()

set(  USBMOND_INCLUDE
        ./include/COMMANDS/UsbMondList.h
        ./include/COMMANDS/UsbMondHelp.h  
        ./include/COMMANDS/UsbMondGetVersion.h
        ./include/IUsbMondCommand.h
        ./include/UsbMond.h
        ./include/UsbMondChannel.h
        ./include/UsbMondChannelFactory.h
        ./include/UsbMondCmdServer.h
        ./include/UsbMondProto.h
        ./include/UsbMondEvents.h
        ./include/UsbMondResult.h
        ./include/UsbMondConfig.h
        ./include/UsbMondConfigException.h
        ./include/UsbMondEventsDispatcher.h
)

set(  USBMOND_SRC 
        ./src/main.cpp
        ./src/UsbMond.cpp
        ./src/UsbMondChannel.cpp
        ./src/UsbMondChannelFactory.cpp
        ./src/UsbMondCmdServer.cpp
        ./src/UsbMondProto.cpp
        ./src/UsbMondEvents.cpp
        ./src/UsbMondConfig.cpp
        ./src/UsbMondConfigException.cpp
        ./src/UsbMondEventsDispatcher.cpp
)


set(  LIBUSBSTORAGE_INCLUDE 
        ./include/UsbMondProto.h
        ./include/UsbStorageMonitor.h
        ./include/UsbStorageMonitorEvents.h
)


set(  LIBUSBSTORAGE_SRC 
        ./src/UsbMondProto.cpp
        ./src/UsbStorageMonitor.cpp
        ./src/UsbStorageMonitorEvents.cpp
)

set(  LIBUSBMOND_LIBRARIES
      fedlibrary${FEDLIB_VER}
      pthread
      udev
)

set(  LIBUSBSTORAGE_LIBRARIES
      fedlibrary${FEDLIB_VER}
      pthread
      udev
)


IF( CMAKE_BUILD_TYPE STREQUAL "Debug" )
add_definitions(
                -ggdb3
)
ENDIF()

add_definitions(
                -D__STDC_CONSTANT_MACROS
                -D_NO_FED_NAMESPACE
                -D_TRACING_EXCEPTION_RAISE
                -D_TRACING_EXCEPTION_CATCH
                -D_ENABLE_ERROR_INFO
                -D_ENABLE_LOG_INFO
                -D_ENABLE_RAW_INFO
                -D_ENABLE_METHOD_TRACING
                -D_ENABLE_VERBOSE_INFO 
                -D_ENABLE_ASSERTION_TRACING
               )

add_library   ( usbstorage${USBSTORAGE_VERSION}  SHARED ${LIBUSBSTORAGE_SRC} )

add_executable( udevtest                                  udevtest.cpp)
add_executable( usbmond${USBMOND_VERSION}                 ${USBMOND_SRC} )

add_executable( usbdiskmon                                ${LIBUSBSTORAGE_SRC} main.cpp )

target_link_libraries( usbstorage${USBSTORAGE_VERSION}    ${LIBUSBSTORAGE_LIBRARIES} )

target_link_libraries( udevtest                    ${LIBUSBSTORAGE_LIBRARIES} )
target_link_libraries( usbmond${USBMOND_VERSION}   ${LIBUSBMOND_LIBRARIES}    )
target_link_libraries( usbdiskmon                  ${LIBUSBSTORAGE_LIBRARIES} )

install( TARGETS usbstorage${USBSTORAGE_VERSION}  LIBRARY DESTINATION /usr/local/lib )
install( FILES   ${LIBUSBSTORAGE_INCLUDE} DESTINATION /usr/include/libusbstorage )
