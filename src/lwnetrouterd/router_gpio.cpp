// router_gpio.cpp
//
// LiveWire GPIO router.
//
//   (C) Copyright 2016-2025 Fred Gleason <fredg@paravelsystems.com>
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

#include <syslog.h>

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




RouterGpio::RouterGpio(SyGpioServer *gpioserv,SyLwrpClient *lwrp,Config *c,
		       QObject *parent)
  : Router(c,parent)
{
  //
  // GPIO Server
  //
  gpio_server=gpioserv;
  connect(gpio_server,SIGNAL(gpioReceived(SyGpioEvent *)),
	  this,SLOT(gpioReceivedData(SyGpioEvent *)));

  for(int i=0;i<MAX_INPUTS;i++) {
    gpio_delay_states[i]=Config::DelayBypassed;
    gpio_delay_intervals[i]=0;
  }

  //
  // LWRP Connection
  //
  gpio_lwrp=lwrp;

  //
  // Netcue Port
  //
  gpio_netcue_device=new TTYDevice(this);
  if(!config()->netcuePort().isEmpty()) {
    gpio_netcue_device->setName(config()->netcuePort());
    gpio_netcue_device->setSpeed(NETCUE_TTY_SPEED);
    gpio_netcue_device->setParity(NETCUE_TTY_DATA_PARITY);
    gpio_netcue_device->setWordLength(NETCUE_TTY_DATA_BITS);
    gpio_netcue_device->setFlowControl(NETCUE_TTY_FLOW_CONTROL);
    if(!gpio_netcue_device->open(QIODevice::WriteOnly)) {
      syslog(LOG_WARNING,"unable to open netcue serial device \"%s\"",
	     config()->netcuePort().toUtf8().constData());
    }
  }

  //
  // Timers
  //
  gpio_scan_timer=new QTimer(this);
  connect(gpio_scan_timer,SIGNAL(timeout()),this,SLOT(scanTimerData()));
  gpio_scan_timer->start(10);
  for(int i=0;i<config()->inputQuantity();i++) {
    for(int j=0;j<SWITCHYARD_GPIO_BUNDLE_SIZE;j++) {
      gpio_debounce_timers[i][j]=new QTimer(this);
      gpio_debounce_timers[i][j]->setSingleShot(true);
    }
  }
}


Config::DelayState RouterGpio::delayState(int input) const
{
  return gpio_delay_states[input];
}


int RouterGpio::delayInterval(int input)
{
  return gpio_delay_intervals[input];
}


void RouterGpio::gpioReceivedData(SyGpioEvent *e)
{
  int input;

  if((input=config()->input(e->originAddress()))>=0) {
    if(e->state()&&(e->sourceNumber()==
	      (int)SyRouting::livewireNumber(gpio_lwrp->dstAddress(input)))) {
      sendRelay(input,e->line());
    }
  }
}


void RouterGpio::sendRelay(int input,int line)
{
  if(!gpio_debounce_timers[input][line]->isActive()) {
    if(config()->relayDebounceInterval()>0) {
      gpio_debounce_timers[input][line]->
	start(config()->relayDebounceInterval());
    }
    gpio_events[input].push(new RouterGpioEvent(line));
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
  //  printf("SendGpo(%d,%d)\n",input,line);
  for(int i=0;i<config()->outputQuantity();i++) {
    if(crossPoint(i)==input) {
      if(config()->forwardNetcuesViaLivewire()) {
	gpio_server->sendGpo(SyRouting::livewireNumber(gpio_lwrp->srcAddress(i)),
			     line,true,true);
	syslog(LOG_DEBUG,"sent LiveWire GPO from input %d to %d:%d",input+1,
	       SyRouting::livewireNumber(gpio_lwrp->srcAddress(i)),line+1);
      }
      if(gpio_netcue_device->isOpen()) {
	QString netcue=config()->outputNetcue(i,line)+"\n";
	gpio_netcue_device->write(netcue.toUtf8(),netcue.toUtf8().length());
	syslog(LOG_DEBUG,"sent NetCue \"%s\" from input %d to %s",
	       config()->outputNetcue(i,line).toUtf8().constData(),input+1,
	       config()->netcuePort().toUtf8().constData());
      }
    }
  }
}

