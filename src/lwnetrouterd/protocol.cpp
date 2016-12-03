// protocol.cpp
//
// Abstract base class for protocol objects.
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; either version 2 of
//   the License, or (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include "protocol.h"

Protocol::Protocol(Config *c,QObject *parent)
  : QObject(parent)
{
  protocol_config=c;
}


void Protocol::sendCrossPoint(int output,int input)
{
}


void Protocol::sendDelayState(int input,Config::DelayState state,int msec)
{
}


void Protocol::sendDelayDumped(int input)
{
}


Config *Protocol::config() const
{
  return protocol_config;
}
