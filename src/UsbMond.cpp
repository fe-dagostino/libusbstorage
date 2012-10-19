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


#include "UsbMond.h"
#include "UsbMondEvents.h"

#include "LOGGING/FLogger.h"

USING_NAMESPACE_LOGGING

GENERATE_CLASSINFO( UsbMond, FService )

UsbMond::UsbMond( int argc, char* argv[] )
 : FService(
                 FSP_HIGH,
                 TRUE,
                 new UsbMondEvents( *this ),
                 "Usb Monitor Daemon",
                 MAKEVERSION( 1,0,4,0 ),
                 argc, argv
              )
ENTER(UsbMond())

EXIT

UsbMond::~UsbMond()
ENTER(~UsbMond())
EXIT

DWORD   UsbMond::GetLogMessageFlags() const
{
  DWORD _dwRetVal = 0;

  _dwRetVal |= UsbMondConfig::GetInstance().IsTraceExceptionEnabled  ( "Global Log Message", NULL)?FLogMessage::MT_TRACE_EXCEPTION:0;
  _dwRetVal |= UsbMondConfig::GetInstance().IsCatchExceptionEnabled  ( "Global Log Message", NULL)?FLogMessage::MT_CATCH_EXCEPTION:0;
  _dwRetVal |= UsbMondConfig::GetInstance().IsAssertionFailureEnabled( "Global Log Message", NULL)?FLogMessage::MT_ASSERTION_FAILURE:0;
  _dwRetVal |= UsbMondConfig::GetInstance().IsErrorInfoEnabled       ( "Global Log Message", NULL)?FLogMessage::MT_ERROR_INFO:0;
  _dwRetVal |= UsbMondConfig::GetInstance().IsLoggingInfoEnabled     ( "Global Log Message", NULL)?FLogMessage::MT_LOGGING_INFO:0;
  _dwRetVal |= UsbMondConfig::GetInstance().IsVerboseInfoEnabled     ( "Global Log Message", NULL)?FLogMessage::MT_VERBOSE_INFO:0;
  _dwRetVal |= UsbMondConfig::GetInstance().IsRawInfoEnabled         ( "Global Log Message", NULL)?FLogMessage::MT_RAW_INFO:0;
  _dwRetVal |= UsbMondConfig::GetInstance().IsEnterMethodEnabled     ( "Global Log Message", NULL)?FLogMessage::MT_ENTER_METHOD:0;
  _dwRetVal |= UsbMondConfig::GetInstance().IsExitMethodEnabled      ( "Global Log Message", NULL)?FLogMessage::MT_EXIT_METHOD:0;

  return _dwRetVal;
}


DWORD   UsbMond::GetVerbosityLevelFlags() const
{
  DWORD _dwRetVal = 0;

  _dwRetVal |= UsbMondConfig::GetInstance().IsStartUpMessageEnabled       ( "Global Verbosity Level", NULL)?FLogMessage::VL_START_UP_MESSAGE:0;
  _dwRetVal |= UsbMondConfig::GetInstance().IsShutDownMessageEnabled      ( "Global Verbosity Level", NULL)?FLogMessage::VL_SHUT_DOWN_MESSAGE:0;
  _dwRetVal |= UsbMondConfig::GetInstance().IsLowPeriodicMessageEnabled   ( "Global Verbosity Level", NULL)?FLogMessage::VL_LOW_PERIODIC_MESSAGE:0;
  _dwRetVal |= UsbMondConfig::GetInstance().IsMediumPeriodicMessageEnabled( "Global Verbosity Level", NULL)?FLogMessage::VL_MEDIUM_PERIODIC_MESSAGE:0;
  _dwRetVal |= UsbMondConfig::GetInstance().IsHighPeriodicMessageEnabled  ( "Global Verbosity Level", NULL)?FLogMessage::VL_HIGH_PERIODIC_MESSAGE:0;

  return _dwRetVal;
}

