// gpio_delay_test.cpp
//
// Test delay intervals of LiveWire GPIO events
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

#include <stdio.h>

#include <QCoreApplication>

#include <sy6/sycmdswitch.h>

#include "gpio_delay_test.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  SyCmdSwitch *cmd=
    new SyCmdSwitch("gpio_delay_test",VERSION,GPIO_DELAY_TEST_USAGE);
  test_send_sourcenum=0;
  test_recv_sourcenum=0;
  bool ok;

  for(int i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--send-source") {
      test_send_sourcenum=cmd->value(i).toInt(&ok);
      if((!ok)||(test_send_sourcenum<=0)||(test_send_sourcenum>32768)) {
	fprintf(stderr,"gpio_delay_test: invalid send source number\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--recv-source") {
      test_recv_sourcenum=cmd->value(i).toInt(&ok);
      if((!ok)||(test_recv_sourcenum<=0)||(test_recv_sourcenum>32768)) {
	fprintf(stderr,"gpio_delay_test: invalid recv source number\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"gpio_delay_test: unknown option: %s\n",
	      (const char *)cmd->key(i).toUtf8());
      exit(256);
    }
  }
  if(test_send_sourcenum==0) {
    fprintf(stderr,"gpio_delay_test: you must specify a --send-source\n");
    exit(256);
  }
  if(test_recv_sourcenum==0) {
    fprintf(stderr,"gpio_delay_test: you must specify a --recv-source\n");
    exit(256);
  }

  //
  // LiveWire Servers
  //
  test_routing_server=new SyRouting(0,0);

  test_gpio_server=new SyGpioServer(test_routing_server,this);
  connect(test_gpio_server,SIGNAL(gpoReceived(int,int,bool,bool)),
	  this,SLOT(gpoReceivedData(int,int,bool,bool)));

  printf("Sending GPO pulse to LiveWire source %d\n",test_send_sourcenum);
  gettimeofday(&test_send_tv,NULL);
  test_gpio_server->sendGpo(test_send_sourcenum,1,true,true);
}


void MainObject::gpoReceivedData(int gpo,int line,bool state,bool pulse)
{
  gettimeofday(&test_recv_tv,NULL);

  // printf("gpoReceivedData(%d,%d,%d,%d)\n",gpo,line,state,pulse);

  if(gpo==test_recv_sourcenum) {
    printf("Received GPO pulse from LiveWire source %d: Propagation delay was %9.6lf seconds\n",
	   gpo,TimeValue(&test_recv_tv)-TimeValue(&test_send_tv));
    exit(0);
  }
}


double MainObject::TimeValue(struct timeval *tv)
{
  return (double)tv->tv_sec+(double)tv->tv_usec/1000000.0;
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);

  new MainObject();
  return a.exec();
}
