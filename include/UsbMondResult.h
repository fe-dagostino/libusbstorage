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


#ifndef USBMOND_RESULT_H
#define USBMOND_RESULT_H

#include "FChannel.h"
#include "RCI/IRciCommand.h"

USING_NAMESPACE_FED
USING_NAMESPACE_RCI

/**
 */
class UsbMondResult : public FArguments
{
public:
  /***/
  UsbMondResult( const FString& sSeparator = "," )
  : FArguments( )
  { 
    SetSeparator( sSeparator );
  }

  /***/
  virtual ~UsbMondResult()
  {}

  /***/
  VOID SetSeparator( const FString& sSeparator = "," )
  { m_sSeparator = sSeparator; }

  /***/
  const FString&  GetSeparator() const
  { return m_sSeparator; }
  
private:
  FString m_sSeparator;
};

#endif //USBMOND_RESULT_H
