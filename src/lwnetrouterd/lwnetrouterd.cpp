// lwnetrouterd.cpp
//
// lwnetrouterd(8) routing daemon
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

#include <signal.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>

#include <QCoreApplication>

#include <sy/sycmdswitch.h>

#include "lwnetrouterd.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  SyCmdSwitch *cmd=
    new SyCmdSwitch(qApp->argc(),qApp->argv(),"lwnetrouterd",VERSION,LWNETROUTERD_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(!cmd->processed(i)) {
      fprintf(stderr,"lwnetrouterd: unknown option\n");
      exit(256);
    }
  }

  main_config=new Config();

  //
  // Open the Syslog
  //
  openlog("lwnetrouterd",LOG_PERROR,LOG_DAEMON);

  //
  // GPIO Server
  //
  SyRouting *r=new SyRouting(0,0);
  r->setNicAddress(main_config->livewireIpAddress());
  r->save();
  main_gpio=new SyGpioServer(r,this);

  //
  // Adapter Control
  //
  main_lwrp=new SyLwrpClient(0,this);
  main_lwrp->
    connectToHost(main_config->adapterIpAddress(),SWITCHYARD_LWRP_PORT,"",true);

  //
  // Routers
  //
  main_audio_router=new RouterHpiAudio(main_config,this);
  main_gpio_router=new RouterGpio(main_gpio,main_lwrp,main_config,this);
  connect(main_audio_router,
	  SIGNAL(delayStateChanged(int,Config::DelayState,int)),
	  main_gpio_router,SLOT(setDelayState(int,Config::DelayState,int)));
  main_cic_router=new RouterCic(main_config,this);
  connect(main_audio_router,
	  SIGNAL(delayStateChanged(int,Config::DelayState,int)),
	  main_cic_router,SLOT(setDelayState(int,Config::DelayState,int)));

  //
  // Protocols
  //
  main_rml_protocol=new ProtocolRml(main_config,this);
  connect(main_rml_protocol,SIGNAL(crosspointChangeReceived(int,int)),
	  main_audio_router,SLOT(setCrossPoint(int,int)));
  connect(main_rml_protocol,SIGNAL(crosspointChangeReceived(int,int)),
	  main_gpio_router,SLOT(setCrossPoint(int,int)));

  main_sap_protocol=new ProtocolSap(main_lwrp,main_config,this);
  connect(main_sap_protocol,SIGNAL(crosspointChangeReceived(int,int)),
	  main_audio_router,SLOT(setCrossPoint(int,int)));
  connect(main_sap_protocol,SIGNAL(crosspointChangeReceived(int,int)),
	  main_gpio_router,SLOT(setCrossPoint(int,int)));
  connect(main_sap_protocol,SIGNAL(crosspointRequested(int,int)),
	  this,SLOT(sapCrosspointRequestedData(int,int)));
  connect(main_audio_router,SIGNAL(crossPointChanged(int,int)),
	  main_sap_protocol,SLOT(sendCrossPoint(int,int)));

  main_cunc_protocol=new ProtocolCunc(main_config,this);
  connect(main_cunc_protocol,
	  SIGNAL(delayStateChangeReceived(int,Config::DelayState)),
	  main_audio_router,SLOT(setDelayState(int,Config::DelayState)));
  connect(main_cunc_protocol,SIGNAL(delayDumpReceived(int)),
	  main_audio_router,SLOT(dumpDelay(int)));
  connect(main_cunc_protocol,SIGNAL(delayStateRequested(int,int)),
	  this,SLOT(cuncDelayStateRequestedData(int,int)));
  connect(main_audio_router,
	  SIGNAL(delayStateChanged(int,Config::DelayState,int)),
	  main_cunc_protocol,SLOT(sendDelayState(int,Config::DelayState,int)));

  main_gpio_protocol=new ProtocolGpio(main_gpio,main_config,this);
  connect(main_gpio_protocol,
	  SIGNAL(delayStateChangeReceived(int,Config::DelayState)),
	  main_audio_router,SLOT(setDelayState(int,Config::DelayState)));
  connect(main_gpio_protocol,SIGNAL(delayDumpReceived(int)),
	  main_audio_router,SLOT(dumpDelay(int)));
  connect(main_audio_router,
	  SIGNAL(delayStateChanged(int,Config::DelayState,int)),
	  main_gpio_protocol,SLOT(sendDelayState(int,Config::DelayState,int)));
}


void MainObject::cuncDelayStateRequestedData(int id,int input)
{
  main_cunc_protocol->
    sendDelayState(id,input,main_audio_router->delayState(input),
		   main_audio_router->delayInterval(input));
}


void MainObject::sapCrosspointRequestedData(int id,int output)
{
  if(output<0) {
    for(int i=0;i<main_config->outputQuantity();i++) {
      main_sap_protocol->sendCrossPoint(id,i,main_audio_router->crossPoint(i));
    }
  }
  else {
    main_sap_protocol->
      sendCrossPoint(id,output,main_audio_router->crossPoint(output));
  }
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
