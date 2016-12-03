// delaycontrol.cpp
//
// Controller for a delay device via LiveWire GPIO
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

#include <stdio.h>

#include "delaycontrol.h"

DelayControl::DelayControl(int source,SyGpioServer *gpio,QObject *parent)
  : QObject(parent)
{
  delay_source=source;
  delay_gpio=gpio;
  delay_flash_phase=0;
  for(int i=0;i<SWITCHYARD_GPIO_BUNDLE_SIZE;i++) {
    delay_states[i]=DelayControl::ButtonOff;
    delay_gpio->sendGpi(delay_source,i,false,false);
    delay_gpio_states[i]=false;
  }

  delay_flash_timer=new QTimer(this);
  connect(delay_flash_timer,SIGNAL(timeout()),this,SLOT(flashData()));
  delay_flash_timer->start(200);
}


DelayControl::~DelayControl()
{
  for(int i=0;i<SWITCHYARD_GPIO_BUNDLE_SIZE;i++) {
    delay_gpio->sendGpi(delay_source,i,false,false);
  }
  delete delay_flash_timer;
}


void DelayControl::setButtonState(int line,DelayControl::ButtonState state)
{
  unsigned phase=delay_flash_phase%4;

  switch(state) {
  case DelayControl::ButtonOff:
    SendGpi(line,false);
    break;

  case DelayControl::ButtonOn:
    SendGpi(line,true);
    break;

  case DelayControl::ButtonSlowFlash:
    SendGpi(line,(phase==0)||(phase==1));
    break;

  case DelayControl::ButtonFastFlash:
    SendGpi(line,(phase==0)||(phase==2));
    break;
  }
  delay_states[line]=state;
}


void DelayControl::flashData()
{
  ++delay_flash_phase;
  for(int i=0;i<4;i++) {
    setButtonState(i,delay_states[i]);
  }
}


void DelayControl::SendGpi(int line,bool state)
{
  if(state!=delay_gpio_states[line]) {
    delay_gpio->sendGpi(delay_source,line,state,false);
    delay_gpio_states[line]=state;
  }
}
