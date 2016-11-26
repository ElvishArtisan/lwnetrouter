// router_gpio.cpp
//
// LiveWire GPIO router.
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

#include "router_gpio.h"

RouterGpioEvent::RouterGpioEvent(int line)
{
  evt_timestamp=QDateTime::currentDateTime();
  evt_line=line;
}


QDateTime RouterGpioEvent::timestamp() const
{
  return evt_timestamp;
}


int RouterGpioEvent::line() const
{
  return evt_line;
}




RouterGpio::RouterGpio(SyGpioServer *gpioserv,Config *c,QObject *parent)
  : Router(c,parent)
{
  gpio_server=gpioserv;
  connect(gpio_server,SIGNAL(gpoReceived(int,int,bool,bool)),
	  this,SLOT(gpoReceivedData(int,int,bool,bool)));

  for(int i=0;i<MAX_INPUTS;i++) {
    gpio_delay_states[i]=Config::DelayBypassed;
    gpio_delay_intervals[i]=0;
  }

  gpio_lwrp=new SyLwrpClient(0,this);
  gpio_lwrp->
    connectToHost(c->audioAdapterIpAddress(),SWITCHYARD_LWRP_PORT,"",true);

  gpio_scan_timer=new QTimer(this);
  connect(gpio_scan_timer,SIGNAL(timeout()),this,SLOT(scanTimerData()));
  gpio_scan_timer->start(50);
}


Config::DelayState RouterGpio::delayState(int input) const
{
  return gpio_delay_states[input];
}


int RouterGpio::delayInterval(int input)
{
  return gpio_delay_intervals[input];
}


void RouterGpio::gpoReceivedData(int gpo,int line,bool state,bool pulse)
{
  //  printf("gpoReceivedData(%d,%d,%d,%d)\n",gpo,line,state,pulse);

  for(int i=0;i<config()->inputQuantity();i++) {
    if(state&&(gpo==(int)SyRouting::livewireNumber(gpio_lwrp->dstAddress(i)))) {
      gpio_events[i].push(new RouterGpioEvent(line));
      //      SendGpo(i,line);
    }
  }
}


void RouterGpio::setDelayState(int input,Config::DelayState state,int msec)
{
  gpio_delay_states[input]=state;
  gpio_delay_intervals[input]=msec;
}


void RouterGpio::scanTimerData()
{
  QDateTime now=QDateTime::currentDateTime();
  for(int i=0;i<config()->inputQuantity();i++) {
    while((gpio_events[i].size()>0)&&
	  (gpio_events[i].front()->
	   timestamp().addMSecs(gpio_delay_intervals[i])<=now)) {
      SendGpo(i,gpio_events[i].front()->line());
      delete gpio_events[i].front();
      gpio_events[i].pop();
    }
  }
}


void RouterGpio::SendGpo(int input,int line)
{
  for(int i=0;i<config()->outputQuantity();i++) {
    if(crossPoint(i)==input) {
      gpio_server->sendGpo(SyRouting::livewireNumber(gpio_lwrp->srcAddress(i)),
			   line,true,true);
    }
  }
}

