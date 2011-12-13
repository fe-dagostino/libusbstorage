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


#ifndef USBMOND_CHANNEL_FACTORY_H
#define USBMOND_CHANNEL_FACTORY_H

#include "FChannelFactory.h"
#include "UsbMondChannel.h"

USING_NAMESPACE_FED
USING_NAMESPACE_RCI

/**
 */
class UsbMondChannelFactory : public FChannelFactory
{
  ENABLE_FRTTI( UsbMondChannelFactory )
public:
  /***/
  virtual FChannel*	CreateChannel( IConnection* pIConnection ) const;

protected:
private:

};

#endif //USBMOND_CHANNEL_FACTORY_H

