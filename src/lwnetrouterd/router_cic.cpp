// router_cic.cpp
//
// LiveWire CIC router.
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

#include <QStringList>

#include "router_cic.h"

RouterCicEvent::RouterCicEvent(const QString &isci)
{
  evt_timestamp=QDateTime::currentDateTime();
  evt_isci_code=isci;
}


QDateTime RouterCicEvent::timestamp() const
{
  return evt_timestamp;
}


QString RouterCicEvent::isciCode() const
{
  return evt_isci_code;
}




RouterCic::RouterCic(Config *c,QObject *parent)
  : Router(c,parent)
{
  cic_socket=new QUdpSocket(this);
  if(!cic_socket->bind(config()->cicPort())) {
    syslog(LOG_WARNING,"unable to bind port %u for CIC processing",
	   config()->cicPort());
  }
  connect(cic_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));

  for(int i=0;i<MAX_INPUTS;i++) {
    cic_delay_states[i]=Config::DelayBypassed;
    cic_delay_intervals[i]=0;
  }

  cic_scan_timer=new QTimer(this);
  connect(cic_scan_timer,SIGNAL(timeout()),this,SLOT(scanTimerData()));
  cic_scan_timer->start(10);
}


Config::DelayState RouterCic::delayState(int input) const
{
  return cic_delay_states[input];
}


int RouterCic::delayInterval(int input)
{
  return cic_delay_intervals[input];
}


void RouterCic::setDelayState(int input,Config::DelayState state,int msec)
{
  cic_delay_states[input]=state;
  cic_delay_intervals[input]=msec;
}


void RouterCic::readyReadData()
{
  QHostAddress addr;
  uint16_t port;
  char data[1501];
  int n;
  int input;

  while((n=cic_socket->readDatagram(data,1500,&addr,&port))>0) {
    if((input=config()->input(addr))>=0) {
      data[n]=0;
      QStringList f0=QString(data).split(":",Qt::KeepEmptyParts);
      if((f0.size()==4)&&(f0.at(0)=="0")&&(f0.at(3)=="*")) {
	cic_events[input].push(new RouterCicEvent(f0.at(2)));
      }
    }
  }
}


void RouterCic::scanTimerData()
{
  QDateTime now=QDateTime::currentDateTime();
  for(int i=0;i<config()->inputQuantity();i++) {
    while((cic_events[i].size()>0)&&
	  (cic_events[i].front()->
	   timestamp().addMSecs(cic_delay_intervals[i])<=now)) {
      for(int j=0;j<config()->outputQuantity();j++) {
	if((crossPoint(j)==i)&&(!config()->outputCicProgramCode(j).isEmpty())) {
	  SendPacket(i,"0:"+config()->outputCicProgramCode(j)+":"+
		     cic_events[i].front()->isciCode()+":*");
	}
      }
      delete cic_events[i].front();
      cic_events[i].pop();
    }
  }
}


void RouterCic::SendPacket(int input,const QString &cic)
{
  QList<QHostAddress> cic_addrs=config()->cicIpAddresses();

  for(int i=0;i<cic_addrs.size();i++) {
    cic_socket->writeDatagram(cic.toUtf8(),cic_addrs.at(i),config()->cicPort());
    syslog(LOG_DEBUG,"sent CIC \"%s\" from input %d to %s:%u",
	   (const char *)cic.toUtf8(),input+1,
	   (const char *)cic_addrs.at(i).toString().toUtf8(),
	   0xFFFF&config()->cicPort());
  }
}
