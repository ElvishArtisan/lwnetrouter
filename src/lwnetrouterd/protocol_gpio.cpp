// protocol_gpio.cpp
//
// Protocol driver for LiveWire GPIO
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

#include "protocol_gpio.h"

ProtocolGpio::ProtocolGpio(SyGpioServer *gpioserv,Config *c,QObject *parent)
  : Protocol(c,parent)
{
  gpio_server=gpioserv;
  connect(gpio_server,SIGNAL(gpoReceived(int,int,bool,bool)),
	  this,SLOT(gpoReceivedData(int,int,bool,bool)));

  gpio_dump_reset_mapper=new QSignalMapper(this);
  connect(gpio_dump_reset_mapper,SIGNAL(mapped(int)),
	  this,SLOT(dumpResetData(int)));
  for(int i=0;i<config()->inputQuantity();i++) {
    gpio_controls[i]=
      new DelayControl(config()->inputDelayControlSource(i),gpio_server,this);
    gpio_dump_reset_timers[i]=new QTimer(this);
    gpio_dump_reset_timers[i]->setSingleShot(true);
    gpio_dump_reset_mapper->setMapping(gpio_dump_reset_timers[i],i);
    connect(gpio_dump_reset_timers[i],SIGNAL(timeout()),
	    gpio_dump_reset_mapper,SLOT(map()));
  }
}


ProtocolGpio::~ProtocolGpio()
{
  for(int i=0;i<config()->inputQuantity();i++) {
    delete gpio_dump_reset_timers[i];
    delete gpio_controls[i];
  }
  delete gpio_dump_reset_mapper;
}


void ProtocolGpio::sendDelayState(int input,Config::DelayState state,int msec)
{
  int srcnum=0;

  if((srcnum=config()->inputDelayControlSource(input))>0) {
    switch(state) {
    case Config::DelayUnknown:
    case Config::DelayBypassed:
      for(int i=1;i<SWITCHYARD_GPIO_BUNDLE_SIZE;i++) {
	gpio_controls[input]->setButtonState(i,DelayControl::ButtonOff);
      }
      break;

    case Config::DelayEntering:
      for(int i=1;i<SWITCHYARD_GPIO_BUNDLE_SIZE;i++) {
	gpio_controls[input]->setButtonState(i,DelayControl::ButtonOff);
      }
      gpio_controls[input]->setButtonState(3,DelayControl::ButtonSlowFlash);
      break;

    case Config::DelayEntered:
      for(int i=1;i<SWITCHYARD_GPIO_BUNDLE_SIZE;i++) {
	gpio_controls[input]->setButtonState(i,DelayControl::ButtonOff);
      }
      gpio_controls[input]->setButtonState(3,DelayControl::ButtonOn);
      break;

    case Config::DelayExiting:
      for(int i=1;i<SWITCHYARD_GPIO_BUNDLE_SIZE;i++) {
	gpio_controls[input]->setButtonState(i,DelayControl::ButtonOff);
      }
      gpio_controls[input]->setButtonState(1,DelayControl::ButtonSlowFlash);
      break;

    case Config::DelayExited:
      for(int i=1;i<SWITCHYARD_GPIO_BUNDLE_SIZE;i++) {
	gpio_controls[input]->setButtonState(i,DelayControl::ButtonOff);
      }
      gpio_controls[input]->setButtonState(1,DelayControl::ButtonOn);
      break;
    }
  }
  if(!gpio_dump_reset_timers[input]->isActive()) {
    if(msec>=(1000*config()->inputDumpDelay(input))) {
      gpio_controls[input]->setButtonState(0,DelayControl::ButtonOn);
    }
    else {
      gpio_controls[input]->setButtonState(0,DelayControl::ButtonOff);
    }
  }
}


void ProtocolGpio::sendDelayDumped(int input)
{
  gpio_controls[input]->setButtonState(0,DelayControl::ButtonFastFlash);
  gpio_dump_reset_timers[input]->start(2000);
}


void ProtocolGpio::gpoReceivedData(int gpo,int line,bool state,bool pulse)
{
  for(int i=0;i<config()->inputQuantity();i++) {
    if((gpo==config()->inputDelayControlSource(i))&&state) {
      switch(line) {
      case 0:   // Dump
	emit delayDumpReceived(i);
	break;

      case 1:   // Exit
	emit delayStateChangeReceived(i,Config::DelayExiting);
	break;

      case 2:   // Pause
	break;

      case 3:   // Engage
	emit delayStateChangeReceived(i,Config::DelayEntering);
	break;
      }
    }
  }
}


void ProtocolGpio::dumpResetData(int input)
{
  gpio_controls[input]->setButtonState(0,DelayControl::ButtonOff);
}
