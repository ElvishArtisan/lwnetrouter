// lwnetrouterd.cpp
//
// lwnetrouterd(8) routing daemon
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

#include <signal.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>

#include <QCoreApplication>

#include <sy6/sycmdswitch.h>

#include "lwnetrouterd.h"

bool global_exiting=false;

void SignalHandler(int signo)
{
  switch(signo) {
  case SIGINT:
  case SIGTERM:
    global_exiting=true;
    break;
  }
}


MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  SyCmdSwitch *cmd=
    new SyCmdSwitch("lwnetrouterd",VERSION,LWNETROUTERD_USAGE);
  for(int i=0;i<cmd->keys();i++) {
    if(!cmd->processed(i)) {
      fprintf(stderr,"lwnetrouterd: unknown option\n");
      exit(256);
    }
  }

  main_config=new Config();

  main_state=new State();
  main_state->load();

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
  connect(main_lwrp,SIGNAL(connected(unsigned,bool)),
	  this,SLOT(lwrpConnectedData(unsigned,bool)));
  connect(main_lwrp,SIGNAL(connectionError(unsigned,
					   QAbstractSocket::SocketError)),
	  this,SLOT(lwrpConnectionErrorData(unsigned,
					    QAbstractSocket::SocketError)));
  main_lwrp->connectToHost(main_config->adapterIpAddress(),
			   SWITCHYARD_LWRP_PORT,
			   main_config->lwrpPassword(),true);

  //
  // Routers
  //
  main_audio_router=new RouterHpiAudio(main_config,this);
  connect(main_audio_router,
	  SIGNAL(delayStateChanged(int,Config::DelayState,int)),
	  this,
	  SLOT(delayStateChangedData(int,Config::DelayState,int)));

  main_gpio_router=new RouterGpio(main_gpio,main_lwrp,main_config,this);
  connect(main_audio_router,
	  SIGNAL(delayStateChanged(int,Config::DelayState,int)),
	  main_gpio_router,SLOT(setDelayState(int,Config::DelayState,int)));

  main_breakaway_router=new RouterBreakaway(main_config,this);
  connect(main_audio_router,
	  SIGNAL(delayStateChanged(int,Config::DelayState,int)),
	  main_breakaway_router,
	  SLOT(setDelayState(int,Config::DelayState,int)));

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
  connect(main_rml_protocol,SIGNAL(crosspointChangeReceived(int,int)),
	  main_breakaway_router,SLOT(setCrossPoint(int,int)));
  connect(main_rml_protocol,SIGNAL(crosspointChangeReceived(int,int)),
	  main_state,SLOT(setCrossPoint(int,int)));
  connect(main_rml_protocol,SIGNAL(relayReceived(int,int)),
	  main_gpio_router,SLOT(sendRelay(int,int)));
  connect(main_rml_protocol,SIGNAL(breakawayReceived(int,int)),
	  main_breakaway_router,SLOT(sendBreakaway(int,int)));

  main_sap_protocol=new ProtocolSap(main_lwrp,main_config,this);
  connect(main_sap_protocol,SIGNAL(crosspointChangeReceived(int,int)),
	  main_audio_router,SLOT(setCrossPoint(int,int)));
  connect(main_sap_protocol,SIGNAL(crosspointChangeReceived(int,int)),
	  main_gpio_router,SLOT(setCrossPoint(int,int)));
  connect(main_sap_protocol,SIGNAL(crosspointChangeReceived(int,int)),
	  main_breakaway_router,SLOT(setCrossPoint(int,int)));
  connect(main_sap_protocol,SIGNAL(crosspointChangeReceived(int,int)),
	  main_cic_router,SLOT(setCrossPoint(int,int)));
  connect(main_sap_protocol,SIGNAL(crosspointChangeReceived(int,int)),
	  main_state,SLOT(setCrossPoint(int,int)));
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
  connect(main_cunc_protocol,SIGNAL(inputNameRequested(int,int)),
	  this,SLOT(cuncInputNameRequestedData(int,int)));
  connect(main_audio_router,
	  SIGNAL(delayStateChanged(int,Config::DelayState,int)),
	  main_cunc_protocol,SLOT(sendDelayState(int,Config::DelayState,int)));
  connect(main_audio_router,SIGNAL(delayDumped(int)),
	  main_cunc_protocol,SLOT(sendDelayDumped(int)));

  main_gpio_protocol=new ProtocolGpio(main_gpio,main_config,this);
  connect(main_gpio_protocol,
	  SIGNAL(delayStateChangeReceived(int,Config::DelayState)),
	  main_audio_router,SLOT(setDelayState(int,Config::DelayState)));
  connect(main_gpio_protocol,SIGNAL(delayDumpReceived(int)),
	  main_audio_router,SLOT(dumpDelay(int)));
  connect(main_audio_router,
	  SIGNAL(delayStateChanged(int,Config::DelayState,int)),
	  main_gpio_protocol,SLOT(sendDelayState(int,Config::DelayState,int)));
  connect(main_audio_router,SIGNAL(delayDumped(int)),
	  main_gpio_protocol,SLOT(sendDelayDumped(int)));

  //
  // Signals
  //
  ::signal(SIGINT,SignalHandler);
  ::signal(SIGTERM,SignalHandler);
  main_exit_timer=new QTimer(this);
  connect(main_exit_timer,SIGNAL(timeout()),this,SLOT(exitData()));
  main_exit_timer->start(500);

  //
  // Initialize State
  //
  for(int i=0;i<main_config->outputQuantity();i++) {
    main_audio_router->setCrossPoint(i,main_state->crossPoint(i));
    main_cic_router->setCrossPoint(i,main_state->crossPoint(i));
    main_gpio_router->setCrossPoint(i,main_state->crossPoint(i));
    main_breakaway_router->setCrossPoint(i,main_state->crossPoint(i));
  }
}


void MainObject::lwrpConnectedData(unsigned id,bool state)
{
  if(state) {
    syslog(LOG_INFO,"LWRP connection established");
  }
  else {
    syslog(LOG_WARNING,"LWRP connection failed");
  }
}


void MainObject::lwrpConnectionErrorData(unsigned id,
					 QAbstractSocket::SocketError err)
{
  syslog(LOG_WARNING,"LWRP connection error: %s",
	 SyMcastSocket::socketErrorText(err).toUtf8().constData());
}


void MainObject::cuncDelayStateRequestedData(int id,int input)
{
  main_cunc_protocol->
    sendDelayState(id,input,main_audio_router->delayState(input),
		   main_audio_router->delayInterval(input));
}


void MainObject::cuncInputNameRequestedData(int id,int input)
{
  main_cunc_protocol->sendInputName(id,input,main_lwrp->dstName(input));
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


void MainObject::delayStateChangedData(int input,Config::DelayState state,
				       int msec)
{
  if(main_state->delayInitialized(input)) {
    switch(state) {
    case Config::DelayUnknown:
    case Config::DelayBypassed:
    case Config::DelayExiting:
    case Config::DelayExited:
      main_state->setDelayActive(input,false);
      break;
      
    case Config::DelayEntering:
    case Config::DelayEntered:
      main_state->setDelayActive(input,true);
      break;
    }
  }
  else {
    if(main_state->delayActive(input)) {
      main_audio_router->setDelayState(input,Config::DelayEntering);
    }
    main_state->setDelayInitialized(input);
  }
}


void MainObject::exitData()
{
  if(global_exiting) {
    delete main_gpio_protocol;
    exit(0);
  }
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
