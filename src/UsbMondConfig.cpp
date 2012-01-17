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


#include "UsbMondConfig.h"
#include "LOGGING/FLogger.h"

USING_NAMESPACE_LOGGING

GENERATE_CLASSINFO( UsbMondConfig, FSingleton )
IMPLEMENT_SINGLETON( UsbMondConfig )

#ifdef _WIN32
# define CFG_FILENAME  "usbmond.cfg"
#else
# define CFG_FILENAME  "/etc/usbmond/usbmond.cfg"
#endif


/////////////////////////////////////
  //
   //
    ////////////////////////////////////////////////////////////////////////////////////////////
   //
  //		Disable warning 4715 : "The specified function can potentially not return a value."
 //
//////////
#pragma warning( disable : 4715 )

/////////////////
////	MOUNT POINTS
//////

FString	UsbMondConfig::GetUsb1MountPoint( BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( "MOUNT POINTS", "USB1", 0, pbStored );
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, GetUsb1MountPoint() );
  }

  //Return default value
  return "/mnt/usbkey1";
}

FString	UsbMondConfig::GetUsb2MountPoint( BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( "MOUNT POINTS", "USB2", 0, pbStored );
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, GetUsb2MountPoint() );
  }

  //Return default value
  return "/mnt/usbkey2";
}

/////////////////////////////
// FILE SYSTEMS
///////////////////////
  
const FParameter*  UsbMondConfig::GetSupportedFileSystems( BOOL * pbStored ) const
{
  FTRY
  {
    return m_cfg.GetParameter( "FILE SYSTEMS", "FS LIST", pbStored );
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, GetSupportedFileSystems() );
  }

  //Return default value
  return NULL;
}

FString            UsbMondConfig::GetFileSystemOptions( const FString& sfs, BOOL * pbStored ) const
{
  FString             _sRetVal;  
  FParametersSection* _pSection = NULL;
  
  FTRY
  {
    _pSection = m_cfg.GetSection( sfs, pbStored );
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, GetFileSystemOptions() );
  }

  if (_pSection != NULL )
  {
    FParametersSection::Iterator  _iter       = FParametersSection::Iterator( _pSection );
    FParameter*                   _pParameter = _iter.GetParameter();
    while ( _pParameter != NULL )
    {
      if ( _pParameter->GetCount() == 0 )
	_sRetVal += FString( 0, "%s,", (const char*)_pParameter->GetName() );
      else
	_sRetVal += FString( 0, "%s=%s,", (const char*)_pParameter->GetName(), (const char*)_pParameter->GetValue(0) );
      
      _pParameter = _iter.GetParameter();
    }
  }
  //Return default value
  return _sRetVal;
}


/////////////////
////	GENERAL
//////

FString	UsbMondConfig::GetMonitorServerAddress( BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( "GENERAL", "MON_SERVER", 0, pbStored );
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, GetMonitorServerAddress() );
  }

  //Return default value
  return "127.0.0.1";
}

WORD	UsbMondConfig::GetMonitorServerPort   ( BOOL* pbStored ) const
{
  FTRY
  {
    return (WORD)m_cfg.GetValue( "GENERAL", "MON_SERVER", 1, pbStored );
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, GetMonitorServerPort() );
  }

  //Return default value
  return 50000;
}

FString	UsbMondConfig::GetLogDiskPath( BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( "GENERAL", "LOG_DISK", 0, pbStored );
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, GetLogDiskPath() );
  }

  //Return default value
  return "/var/log/usbmond/";
}

FString	UsbMondConfig::GetLogDiskPrefix( BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( "GENERAL", "LOG_DISK", 1, pbStored );
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, GetLogDiskPrefix() );
  }

  //Return default value
  return "usbmond_";
}

FString	UsbMondConfig::GetLogDiskExtension( BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( "GENERAL", "LOG_DISK", 2, pbStored );
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, GetLogDiskExtension() );
  }

  //Return default value
  return "log";
}

DWORD	UsbMondConfig::GetLogDiskFiles( BOOL* pbStored ) const
{
  FTRY
  {
    return (DWORD)m_cfg.GetValue( "GENERAL", "LOG_DISK", 3, pbStored );
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, GetLogDiskFiles() );
  }

  //Return default value
  return 10;
}

DWORD	UsbMondConfig::GetLogDiskFileSizeLimit( BOOL* pbStored ) const
{
  FTRY
  {
    return (DWORD)m_cfg.GetValue( "GENERAL", "LOG_DISK", 4, pbStored );
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, GetLogDiskFileSizeLimit() );
  }

  //Return default value
  return 5000;  
}

FString		UsbMondConfig::GetLogServerAddress( BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( "GENERAL", "LOG_SERVER", 0, pbStored );
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, GetLogServerAddress() );
  }

  //Return default value
  return "127.0.0.1";
}

WORD		UsbMondConfig::GetLogServerPort( BOOL* pbStored ) const
{
  FTRY
  {
    return (WORD)m_cfg.GetValue( "GENERAL", "LOG_SERVER", 1, pbStored );
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, GetLogServerPort() );
  }

  //Return default value
  return 55000;
}

FString		UsbMondConfig::GetCmdServerAddress( BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( "GENERAL", "CMD_SERVER", 0, pbStored );
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, GetCmdServerAddress() );
  }

  //Return default value
  return "127.0.0.1";
}

WORD	UsbMondConfig::GetCmdServerPort( BOOL* pbStored ) const
{
  FTRY
  {
    return (WORD)m_cfg.GetValue( "GENERAL", "CMD_SERVER", 1, pbStored );
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, GetCmdServerPort() );
  }

  //Return default value
  return 31000;
}

////////////////////////////////////////

VOID     UsbMondConfig::OnInitialize()
{
  m_cfg.SetFileName( CFG_FILENAME );

  FTRY
  {
    m_cfg.LoadFile();
  }
  FCATCH( FConfigFileException, ex )
  {
    switch( ex.GetErrorCode() )
    {
	case FConfigFileException::CFG_FILE_NOT_FOUND :
	{
	  THROW_MSG_EXCEPTION( UsbMondConfigException, FString( 0,"  Filed to open [%s]", (const char*)m_cfg.GetFileName() ), UsbMondConfigException::CFG_FILE_NOT_FOUND_OR_INACCESSIBLE
  , OnInitialize()  )
	}; break;
    }
  }
}

VOID	UsbMondConfig::OnFinalize()
{

}

BOOL	UsbMondConfig::IsTraceExceptionEnabled( const FString& sSection, BOOL* pbStored ) const
{
	FTRY
	{
		return m_cfg.GetValue( sSection,"MT_TRACE_EXCEPTION", 0, pbStored ) == "ENABLED";
	}
	FCATCH( FConfigFileException, fexception )
	{
		TRACE_EXCEPTION_CATCH( fexception, IsTraceExceptionEnabled() );
	}
	return TRUE;
}

VOID	UsbMondConfig::SetTraceExceptionStatus( const FString& sSection, BOOL bStatus )
ENTER( SetTraceExceptionStatus() )
  FTRY
  {
    m_cfg.SetValue( (bStatus)?"ENABLED":"DISABLED", sSection,"MT_TRACE_EXCEPTION", 0 );
    if ( GET_LOG_MAILBOX() )
    {
      if ( bStatus )
	(GET_LOG_MAILBOX())->AddLogMessageFlag( FLogMessage::MT_TRACE_EXCEPTION );
      else
	(GET_LOG_MAILBOX())->SubLogMessageFlag( FLogMessage::MT_TRACE_EXCEPTION );
    }
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, SetTraceExceptionStatus() );
  }
EXIT

BOOL	UsbMondConfig::IsCatchExceptionEnabled( const FString& sSection, BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( sSection, "MT_CATCH_EXCEPTION", 0, pbStored  ) == "ENABLED";
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, IsCatchExceptionEnabled() );
  }
  return TRUE;
}

VOID	UsbMondConfig::SetCatchExceptionStatus( const FString& sSection, BOOL bStatus )
ENTER( SetCatchExceptionStatus() )
    FTRY
    {
      m_cfg.SetValue( (bStatus)?"ENABLED":"DISABLED", sSection, "MT_CATCH_EXCEPTION", 0 );
      if ( GET_LOG_MAILBOX() )
      {
	if ( bStatus )
	  (GET_LOG_MAILBOX())->AddLogMessageFlag( FLogMessage::MT_CATCH_EXCEPTION );
	else
	  (GET_LOG_MAILBOX())->SubLogMessageFlag( FLogMessage::MT_CATCH_EXCEPTION );
      }
    }
    FCATCH( FConfigFileException, fexception  )
    {
      TRACE_EXCEPTION_CATCH( fexception, SetCatchExceptionStatus() );
    }
EXIT

BOOL	UsbMondConfig::IsAssertionFailureEnabled( const FString& sSection, BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( sSection, "MT_ASSERTION_FAILURE", 0, pbStored  ) == "ENABLED";
  }
  FCATCH( FConfigFileException, fexception )
  {
    TRACE_EXCEPTION_CATCH( fexception, IsAssertionFailureEnabled() );
  }
  return TRUE;
}

VOID	UsbMondConfig::SetAssertionFailureStatus( const FString& sSection, BOOL bStatus ) 
ENTER( SetAssertionFailureStatus() )
  FTRY
  {
    m_cfg.SetValue( (bStatus)?"ENABLED":"DISABLED", sSection,"MT_ASSERTION_FAILURE", 0 );
    if ( GET_LOG_MAILBOX() )
    {
      if ( bStatus )
	(GET_LOG_MAILBOX())->AddLogMessageFlag( FLogMessage::MT_ASSERTION_FAILURE );
      else
	(GET_LOG_MAILBOX())->SubLogMessageFlag( FLogMessage::MT_ASSERTION_FAILURE );
    }
  }
  FCATCH( FConfigFileException, fexception )
  {
    TRACE_EXCEPTION_CATCH( fexception, SetAssertionFailureStatus() );
  }
EXIT

BOOL	UsbMondConfig::IsErrorInfoEnabled( const FString& sSection, BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( sSection, "MT_ERROR_INFO", 0, pbStored ) == "ENABLED";
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, IsErrorInfoEnabled() );
  }
  return TRUE;
}

VOID	UsbMondConfig::SetErrorInfoStatus( const FString& sSection, BOOL bStatus )
ENTER( SetErrorInfoStatus() )
  FTRY
  {
    m_cfg.SetValue( (bStatus)?"ENABLED":"DISABLED", sSection, "MT_ERROR_INFO", 0 );
    if ( GET_LOG_MAILBOX() )
    {
      if ( bStatus )
	(GET_LOG_MAILBOX())->AddLogMessageFlag( FLogMessage::MT_ERROR_INFO );
      else
	(GET_LOG_MAILBOX())->SubLogMessageFlag( FLogMessage::MT_ERROR_INFO );
    }
  }
  FCATCH( FConfigFileException, fexception )
  {
  TRACE_EXCEPTION_CATCH( fexception, SetErrorInfoStatus() );
  }
EXIT

BOOL	UsbMondConfig::IsLoggingInfoEnabled( const FString& sSection, BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( sSection, "MT_LOGGING_INFO", 0, pbStored ) == "ENABLED";
  }
  FCATCH( FConfigFileException, fexception )
  {
    TRACE_EXCEPTION_CATCH( fexception, IsLoggingInfoEnabled() );
  }
  return TRUE;
}

VOID	UsbMondConfig::SetLoggingInfoStatus( const FString& sSection, BOOL bStatus )
ENTER( SetLoggingInfoStatus() )
  FTRY
  {
    m_cfg.SetValue( (bStatus)?"ENABLED":"DISABLED", sSection, "MT_LOGGING_INFO", 0 );
    if ( GET_LOG_MAILBOX() )
    {
      if ( bStatus )
	(GET_LOG_MAILBOX())->AddLogMessageFlag( FLogMessage::MT_LOGGING_INFO );
      else
	(GET_LOG_MAILBOX())->SubLogMessageFlag( FLogMessage::MT_LOGGING_INFO );
    }
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, SetLoggingInfoStatus() );
  }
EXIT

BOOL	UsbMondConfig::IsVerboseInfoEnabled( const FString& sSection, BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( sSection, "MT_VERBOSE_INFO", 0, pbStored ) == "ENABLED";
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, IsVerboseInfoEnabled() );
  }
  return TRUE;
}

VOID	UsbMondConfig::SetVerboseInfoStatus( const FString& sSection, BOOL bStatus )
ENTER( SetVerboseInfoStatus() )
  FTRY
  {
    m_cfg.SetValue( (bStatus)?"ENABLED":"DISABLED", sSection, "MT_VERBOSE_INFO", 0 );
    if ( GET_LOG_MAILBOX() )
    {
      if ( bStatus )
	(GET_LOG_MAILBOX())->AddLogMessageFlag( FLogMessage::MT_VERBOSE_INFO );
      else
	(GET_LOG_MAILBOX())->SubLogMessageFlag( FLogMessage::MT_VERBOSE_INFO );
    }
  }
  FCATCH( FConfigFileException, fexception )
  {
  TRACE_EXCEPTION_CATCH( fexception, SetVerboseInfoStatus() );
  }
EXIT

BOOL	UsbMondConfig::IsRawInfoEnabled( const FString& sSection, BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( sSection, "MT_RAW_INFO", 0, pbStored ) == "ENABLED";
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, IsRawInfoEnabled() );
  }
  return TRUE;
}

VOID	UsbMondConfig::SetRawInfoStatus( const FString& sSection, BOOL bStatus )
ENTER( SetRawInfoStatus() )
  FTRY
  {
    m_cfg.SetValue( (bStatus)?"ENABLED":"DISABLED", sSection,"MT_RAW_INFO", 0 );
    if ( GET_LOG_MAILBOX() )
    {
      if ( bStatus )
	(GET_LOG_MAILBOX())->AddLogMessageFlag( FLogMessage::MT_RAW_INFO );
      else
	(GET_LOG_MAILBOX())->SubLogMessageFlag( FLogMessage::MT_RAW_INFO );
    }
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, SetRawInfoStatus() );
  }
EXIT

BOOL	UsbMondConfig::IsEnterMethodEnabled( const FString& sSection, BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( sSection, "MT_ENTER_METHOD", 0, pbStored ) == "ENABLED";
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, IsEnterMethodEnabled() );
  }
  return TRUE;
}

VOID	UsbMondConfig::SetEnterMethodStatus( const FString& sSection, BOOL bStatus )
ENTER( SetEnterMethodStatus() )
  FTRY
  {
    m_cfg.SetValue( (bStatus)?"ENABLED":"DISABLED", sSection, "MT_ENTER_METHOD", 0 );
    if ( GET_LOG_MAILBOX() )
    {
      if ( bStatus )
	(GET_LOG_MAILBOX())->AddLogMessageFlag( FLogMessage::MT_ENTER_METHOD );
      else
	(GET_LOG_MAILBOX())->SubLogMessageFlag( FLogMessage::MT_ENTER_METHOD );
    }
  }
  FCATCH( FConfigFileException, fexception )
  {
    TRACE_EXCEPTION_CATCH( fexception, SetEnterMethodStatus() );
  }
EXIT

BOOL	UsbMondConfig::IsExitMethodEnabled( const FString& sSection, BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( sSection,"MT_EXIT_METHOD", 0, pbStored ) == "ENABLED";
  }
  FCATCH( FConfigFileException, fexception )
  {
    TRACE_EXCEPTION_CATCH( fexception, IsExitMethodEnabled() );
  }
  return TRUE;
}

VOID	UsbMondConfig::SetExitMethodStatus( const FString& sSection, BOOL bStatus )
ENTER( SetExitMethodStatus() )
  FTRY
  {
    m_cfg.SetValue( (bStatus)?"ENABLED":"DISABLED", sSection,"MT_EXIT_METHOD", 0 );
    if ( GET_LOG_MAILBOX() )
    {
      if ( bStatus )
	(GET_LOG_MAILBOX())->AddLogMessageFlag( FLogMessage::MT_EXIT_METHOD );
      else
	(GET_LOG_MAILBOX())->SubLogMessageFlag( FLogMessage::MT_EXIT_METHOD );
    }
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, SetExitMethodStatus() );
  }
EXIT

BOOL	UsbMondConfig::IsStartUpMessageEnabled( const FString& sSection, BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( sSection, "VL_START_UP_MESSAGE", 0, pbStored ) == "ENABLED";
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, IsStartUpMessageEnabled() );
  }
  return TRUE;
}

VOID	UsbMondConfig::SetStartUpMessageStatus( const FString& sSection, BOOL bStatus )
ENTER( SetStartUpMessageStatus() )
  FTRY
  {
    m_cfg.SetValue( (bStatus)?"ENABLED":"DISABLED", sSection,"VL_START_UP_MESSAGE", 0 );
    if ( GET_LOG_MAILBOX() )
    {
      if ( bStatus )
	(GET_LOG_MAILBOX())->AddVerbosityFlag( FLogMessage::VL_START_UP_MESSAGE );
      else
	(GET_LOG_MAILBOX())->SubVerbosityFlag( FLogMessage::VL_START_UP_MESSAGE );
    }
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, SetStartUpMessageStatus() );
  }
EXIT

BOOL	UsbMondConfig::IsShutDownMessageEnabled( const FString& sSection, BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( sSection,"VL_SHUT_DOWN_MESSAGE", 0, pbStored ) == "ENABLED";
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, IsShutDownMessageEnabled() );
  }
  return TRUE;
}

VOID	UsbMondConfig::SetShutDownMessageStatus( const FString& sSection, BOOL bStatus )
ENTER( SetShutDownMessageStatus() )
  FTRY
  {
    m_cfg.SetValue( (bStatus)?"ENABLED":"DISABLED", sSection,"VL_SHUT_DOWN_MESSAGE", 0 );
    if ( GET_LOG_MAILBOX() )
    {
      if ( bStatus )
	(GET_LOG_MAILBOX())->AddVerbosityFlag( FLogMessage::VL_SHUT_DOWN_MESSAGE );
      else
	(GET_LOG_MAILBOX())->SubVerbosityFlag( FLogMessage::VL_SHUT_DOWN_MESSAGE );
    }
  }
  FCATCH( FConfigFileException, fexception )
  {
    TRACE_EXCEPTION_CATCH( fexception, SetShutDownMessageStatus() );
  }
EXIT

BOOL	UsbMondConfig::IsLowPeriodicMessageEnabled( const FString& sSection, BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( sSection, "VL_LOW_PERIODIC_MESSAGE", 0, pbStored ) == "ENABLED";
  }
  FCATCH( FConfigFileException, fexception )
  {
    TRACE_EXCEPTION_CATCH( fexception, IsLowPeriodicMessageEnabled() );
  }
  return TRUE;
}

VOID	UsbMondConfig::SetLowPeriodicMessageStatus( const FString& sSection, BOOL bStatus )
ENTER( SetLowPeriodicMessageStatus() )
  FTRY
  {
    m_cfg.SetValue( (bStatus)?"ENABLED":"DISABLED", sSection,"VL_LOW_PERIODIC_MESSAGE", 0 );
    if ( GET_LOG_MAILBOX() )
    {
      if ( bStatus )
	(GET_LOG_MAILBOX())->AddVerbosityFlag( FLogMessage::VL_LOW_PERIODIC_MESSAGE );
      else
	(GET_LOG_MAILBOX())->SubVerbosityFlag( FLogMessage::VL_LOW_PERIODIC_MESSAGE );
    }
  }
  FCATCH( FConfigFileException, fexception )
  {
    TRACE_EXCEPTION_CATCH( fexception, SetLowPeriodicMessageStatus() );
  }
EXIT

BOOL	UsbMondConfig::IsMediumPeriodicMessageEnabled( const FString& sSection, BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( sSection,"VL_MEDIUM_PERIODIC_MESSAGE", 0, pbStored ) == "ENABLED";
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, IsMediumPeriodicMessageEnabled() );
  }
  return TRUE;
}

VOID	UsbMondConfig::SetMediumPeriodicMessageStatus( const FString& sSection, BOOL bStatus )
ENTER( SetMediumPeriodicMessageStatus() )
    FTRY
    {
      m_cfg.SetValue( (bStatus)?"ENABLED":"DISABLED", sSection,"VL_MEDIUM_PERIODIC_MESSAGE", 0 );
      if ( GET_LOG_MAILBOX() )
      {
	if ( bStatus )
	  (GET_LOG_MAILBOX())->AddVerbosityFlag( FLogMessage::VL_MEDIUM_PERIODIC_MESSAGE );
	else
	  (GET_LOG_MAILBOX())->SubVerbosityFlag( FLogMessage::VL_MEDIUM_PERIODIC_MESSAGE );
      }
    }
    FCATCH( FConfigFileException, fexception )
    {
      TRACE_EXCEPTION_CATCH( fexception, SetMediumPeriodicMessageStatus() );
    }
EXIT

BOOL	UsbMondConfig::IsHighPeriodicMessageEnabled( const FString& sSection, BOOL* pbStored ) const
{
  FTRY
  {
    return m_cfg.GetValue( sSection,"VL_HIGH_PERIODIC_MESSAGE", 0, pbStored ) == "ENABLED";
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, IsHighPeriodicMessageEnabled() );
  }
  return TRUE;
}

VOID	UsbMondConfig::SetHighPeriodicMessageStatus( const FString& sSection, BOOL bStatus )
ENTER( SetHighPeriodicMessageStatus() )
  FTRY
  {
    m_cfg.SetValue( (bStatus)?"ENABLED":"DISABLED", sSection,"VL_HIGH_PERIODIC_MESSAGE", 0 );
    if ( GET_LOG_MAILBOX() )
    {
      if ( bStatus )
	(GET_LOG_MAILBOX())->AddVerbosityFlag( FLogMessage::VL_HIGH_PERIODIC_MESSAGE );
      else
	(GET_LOG_MAILBOX())->SubVerbosityFlag( FLogMessage::VL_HIGH_PERIODIC_MESSAGE );
    }
  }
  FCATCH( FConfigFileException, fexception  )
  {
    TRACE_EXCEPTION_CATCH( fexception, SetHighPeriodicMessageStatus() );
  }
EXIT

