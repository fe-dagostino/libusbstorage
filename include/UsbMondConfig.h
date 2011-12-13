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


#ifndef __USBMOND_CONFIG_H__
#define __USBMOND_CONFIG_H__

#include "FSingleton.h"
#include "FConfigFileEx.h"
#include "UsbMondConfigException.h"

USING_NAMESPACE_FED

/**
 * This is a singleton object used to facilitates access 
 * to configuration parameters.
 */
class UsbMondConfig : public FSingleton
{
  ENABLE_FRTTI( UsbMondConfig )
  DECLARE_SINGLETON( UsbMondConfig )
protected:

  /***/
  VOID         OnInitialize();
  /***/
  VOID         OnFinalize();

public:
  /////////////////////////////
  // MOUNT POINTS
  ///////////////////////
  
  /**
   * Return mount point for first  USBKEY.
   * Default value is /mnt/usbkey1
   */
  FString	GetUsb1MountPoint( BOOL* pbStored ) const;
  /**
   * Return mount point for second USBKEY.
   * Default value is /mnt/usbkey2
   */
  FString	GetUsb2MountPoint( BOOL* pbStored ) const;

  
  /////////////////////////////
  // FILE SYSTEMS
  ///////////////////////
  
  /**
   * Return the list of supported file systems.
   * When an usb device will be attached the deamons automatically 
   * will try all available FS before to raise a failure.
   *
   * Default value is NULL, so be sure to provide a valid parameter 
   * in configuration.
   */
  const FParameter*  GetSupportedFileSystems( BOOL * pbStored ) const;
  
  FString            GetFileSystemOptions( const FString& sfs, BOOL * pbStored ) const;
  
  /////////////////////////////
  // GENERAL SECTION 
  ///////////////////////

  /**
   */
  FString	GetMonitorServerAddress( BOOL* pbStored ) const;
  /**
   */
  WORD		GetMonitorServerPort   ( BOOL* pbStored ) const;

  /**
   * Return destination log path on file system. Format must be supported from host OS, so 
   * do not use something like c:\var\log under linux.
   */
  FString	GetLogDiskPath( BOOL* pbStored ) const;
  /**
   * Return the prefix used to form all log filenames.
   */
  FString	GetLogDiskPrefix( BOOL* pbStored ) const;
  /**
   * Return the extension to be appended to each log file.
   */
  FString	GetLogDiskExtension( BOOL* pbStored ) const;
  /**
   */
  DWORD		GetLogDiskFiles( BOOL* pbStored ) const;
  /**
   */
  DWORD		GetLogDiskFileSizeLimit( BOOL* pbStored ) const;
  
  /**
   */
  FString	GetLogServerAddress( BOOL* pbStored ) const;
  /**
   */
  WORD		GetLogServerPort   ( BOOL* pbStored ) const;
   
   
  /**
   */
  FString	GetCmdServerAddress( BOOL* pbStored ) const;
  /**
   */
  WORD		GetCmdServerPort   ( BOOL* pbStored ) const;
   

  /////////////////////////////
  // LOG INFO
  ///////////////////////
  /***/
  BOOL		IsTraceExceptionEnabled( const FString& sSection, BOOL* pbStored ) const;
  VOID		SetTraceExceptionStatus( const FString& sSection, BOOL bStatus );
  /***/
  BOOL		IsCatchExceptionEnabled( const FString& sSection, BOOL* pbStored ) const;
  VOID		SetCatchExceptionStatus( const FString& sSection, BOOL bStatus );
  /***/
  BOOL		IsAssertionFailureEnabled( const FString& sSection, BOOL* pbStored ) const;
  VOID		SetAssertionFailureStatus( const FString& sSection, BOOL bStatus );
  /***/
  BOOL		IsErrorInfoEnabled( const FString& sSection, BOOL* pbStored ) const;
  VOID		SetErrorInfoStatus( const FString& sSection, BOOL bStatus );
  /***/
  BOOL		IsLoggingInfoEnabled( const FString& sSection, BOOL* pbStored ) const;
  VOID		SetLoggingInfoStatus( const FString& sSection, BOOL bStatus );
  /***/
  BOOL		IsVerboseInfoEnabled( const FString& sSection, BOOL* pbStored ) const;
  VOID		SetVerboseInfoStatus( const FString& sSection, BOOL bStatus );
  /***/
  BOOL		IsRawInfoEnabled( const FString& sSection, BOOL* pbStored ) const;
  VOID		SetRawInfoStatus( const FString& sSection, BOOL bStatus );
  /***/
  BOOL		IsEnterMethodEnabled( const FString& sSection, BOOL* pbStored ) const;
  VOID		SetEnterMethodStatus( const FString& sSection, BOOL bStatus );
  /***/
  BOOL		IsExitMethodEnabled( const FString& sSection, BOOL* pbStored ) const;
  VOID		SetExitMethodStatus( const FString& sSection, BOOL bStatus );


  /////////////////////////////
  // LOG VERBOSITY
  ///////////////////////

  /***/
  BOOL		IsStartUpMessageEnabled( const FString& sSection, BOOL* pbStored ) const;
  VOID		SetStartUpMessageStatus( const FString& sSection, BOOL bStatus );
  /***/
  BOOL		IsShutDownMessageEnabled( const FString& sSection, BOOL* pbStored ) const;
  VOID		SetShutDownMessageStatus( const FString& sSection, BOOL bStatus );
  /***/
  BOOL		IsLowPeriodicMessageEnabled( const FString& sSection, BOOL* pbStored ) const;
  VOID		SetLowPeriodicMessageStatus( const FString& sSection, BOOL bStatus );
  /***/
  BOOL		IsMediumPeriodicMessageEnabled( const FString& sSection, BOOL* pbStored ) const;
  VOID		SetMediumPeriodicMessageStatus( const FString& sSection, BOOL bStatus );
  /***/
  BOOL		IsHighPeriodicMessageEnabled( const FString& sSection, BOOL* pbStored ) const;
  VOID		SetHighPeriodicMessageStatus( const FString& sSection, BOOL bStatus );


private:
  FConfigFileEx        m_cfg;

};

#endif // __USBMOND_CONFIG_H__

