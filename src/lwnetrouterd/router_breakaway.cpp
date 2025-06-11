// router_breakaway.cpp
//
// Router for processing Internet breakaways.
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

#include "router_breakaway.h"

RouterBreakawayEvent::RouterBreakawayEvent(int msec)
{
  evt_timestamp=QDateTime::currentDateTime();
  evt_length=msec;
}


QDateTime RouterBreakawayEvent::timestamp() const
{
  return evt_timestamp;
}


int RouterBreakawayEvent::length()
{
  return evt_length;
}




RouterBreakaway::RouterBreakaway(Config *c,QObject *parent)
  : Router(c,parent)
{
  for(int i=0;i<config()->inputQuantity();i++) {
    break_delay_intervals[i]=0;
  }

  break_socket=new QUdpSocket(this);

  break_scan_timer=new QTimer(this);
  connect(break_scan_timer,SIGNAL(timeout()),this,SLOT(scanTimerData()));
  break_scan_timer->start(10);
}


void RouterBreakaway::setDelayState(int input,Config::DelayState state,int msec)
{
  break_delay_intervals[input]=msec;
}


void RouterBreakaway::sendBreakaway(int input,int msec)
{
  break_events[input].push(new RouterBreakawayEvent(msec));
}


void RouterBreakaway::scanTimerData()
{
  QDateTime now=QDateTime::currentDateTime();
  for(int i=0;i<config()->inputQuantity();i++) {
    while((break_events[i].size()>0)&&
	  (break_events[i].front()->
	   timestamp().addMSecs(break_delay_intervals[i])<=now)) {
      SendBreakaway(i,break_events[i].front()->length());
      delete break_events[i].front();
      break_events[i].pop();
    }
  }
}


void RouterBreakaway::SendBreakaway(int input,int msec)
{
  for(int i=0;i<config()->outputQuantity();i++) {
    if(crossPoint(i)==input) {
      if(!config()->outputBreakawayIpAddress(i).isNull()) {
	QString rml=QString::asprintf("DX %d %d!",
			      config()->outputBreakawaySlotNumber(i),msec);
	break_socket->writeDatagram(rml.toUtf8(),
				    config()->outputBreakawayIpAddress(i),5859);
	syslog(LOG_DEBUG,"sent breakaway \"%s\" from input %d to %s",
	       (const char *)rml.toUtf8(),input+1,
	       (const char *)config()->outputBreakawayIpAddress(i).toString().
	       toUtf8());
      }
    }
  }
}
