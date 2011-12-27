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

#ifndef USBMOND_COMMAND_H
#define USBMOND_COMMAND_H

#include "FChannel.h"
#include "RCI/IRciCommand.h"

USING_NAMESPACE_FED
USING_NAMESPACE_RCI

/**
 * Extension for IRciCommand.
 */
class IUsbMondCommand : public IRciCommand
{
public:
  /**
   * Constructor.
   * @param sCommandName          command to be registered on command collector.
   *                              CommandName should be unique.
   * @param dwRequiredArguments   number of arguments required by command. This
   *                              value will be used by command parser for checking
   *                              input parametes.
   */
  IUsbMondCommand( const FString& sCommandName, DWORD dwRequiredArguments )
    : IRciCommand( sCommandName, dwRequiredArguments )
  {}

  /***/
  virtual ~IUsbMondCommand()
  {}

  /**
   * To be implemented in order to return command description.
   */
  virtual FString	Description() const = 0;

  /**
   * To be implemented in order to return command usage information.
   */
  virtual FString	Help() const = 0;

  /**
   * To be implemented in order to perform command operation.
   */
  virtual RciResponse	Execute( FChannel& rChannel, const FArguments& rArgs, FArguments& rResults ) const = 0;

};

#endif //USBMOND_COMMAND_H


