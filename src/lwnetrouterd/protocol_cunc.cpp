// protocol_cunc.cpp
//
// Protocol driver for the Cuntator Delay Control System
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

#include <QTcpServer>

#include "protocol_cunc.h"

ProtocolCunc::ProtocolCunc(Config *c,QObject *parent)
  : Protocol(c,parent)
{
  //
  // Stream Server
  //
  QTcpServer *server=new QTcpServer(this);
  server->listen(QHostAddress::Any,config()->cunctatorPort());
  std::map<int,QString> cmds;
  std::map<int,int> upper_limits;
  std::map<int,int> lower_limits;

  cmds[ProtocolCunc::DC]="DC";
  upper_limits[ProtocolCunc::DC]=0;
  lower_limits[ProtocolCunc::DC]=0;

  cmds[ProtocolCunc::DQ]="DQ";
  upper_limits[ProtocolCunc::DQ]=0;
  lower_limits[ProtocolCunc::DQ]=0;

  cmds[ProtocolCunc::DM]="DM";
  upper_limits[ProtocolCunc::DM]=1;
  lower_limits[ProtocolCunc::DM]=1;

  cmds[ProtocolCunc::DS]="DS";
  upper_limits[ProtocolCunc::DS]=1;
  lower_limits[ProtocolCunc::DS]=1;

  cmds[ProtocolCunc::SS]="SS";
  upper_limits[ProtocolCunc::SS]=2;
  lower_limits[ProtocolCunc::SS]=2;

  cmds[ProtocolCunc::DP]="DP";
  upper_limits[ProtocolCunc::DP]=1;
  lower_limits[ProtocolCunc::DP]=1;

  cunc_server=
    new StreamCmdServer(cmds,upper_limits,lower_limits,server,this);
  connect(cunc_server,SIGNAL(commandReceived(int,int,const QStringList &)),
	  this,SLOT(commandReceivedData(int,int,const QStringList &)));
}


void ProtocolCunc::sendDelayState(int input,Config::DelayState state,int msec)
{
  QStringList args;

  args.push_back(QString().sprintf("%d",input+1));
  args.push_back(QString().sprintf("%d",state));
  args.push_back(QString().sprintf("%d",msec));
  cunc_server->sendCommand(ProtocolCunc::DS,args);
}


void ProtocolCunc::sendDelayState(int id,int input,Config::DelayState state,
				  int msec)
{
  QStringList args;

  args.push_back(QString().sprintf("%d",input+1));
  args.push_back(QString().sprintf("%d",state));
  args.push_back(QString().sprintf("%d",msec));
  cunc_server->sendCommand(id,ProtocolCunc::DS,args);
}


void ProtocolCunc::sendInputName(int id,int input,const QString &str)
{
  QStringList args;

  args.push_back(QString().sprintf("%d",input+1));
  args.push_back(QString().sprintf("%d",config()->inputDumpDelay(input)));
  args.push_back(str);
  cunc_server->sendCommand(id,(int)ProtocolCunc::DM,args);
}


void ProtocolCunc::sendDelayDumped(int input)
{
  QStringList args;

  args.push_back(QString().sprintf("%d",input+1));
  cunc_server->sendCommand((int)ProtocolCunc::DP,args);
}


void ProtocolCunc::commandReceivedData(int id,int cmd,const QStringList &args)
{
  QStringList reply;
  unsigned input;
  bool ok=false;
  Config::DelayState state;

  switch((ProtocolCunc::Commands)cmd) {
  case ProtocolCunc::DC:
    cunc_server->closeConnection(id);
    break;

  case ProtocolCunc::DQ:
    reply.push_back(QString().sprintf("%d",config()->inputQuantity()));
    cunc_server->sendCommand(id,cmd,reply);
    break;

  case ProtocolCunc::DM:
    input=args.at(0).toUInt(&ok)-1;
    if(ok&&(input<(unsigned)config()->inputQuantity())) {
      emit inputNameRequested(id,input);
    }
    break;

  case ProtocolCunc::DS:
    input=args.at(0).toUInt(&ok)-1;
    if(ok&&(input<(unsigned)config()->inputQuantity())) {
      emit delayStateRequested(id,input);
    }
    break;

  case ProtocolCunc::SS:
    input=args.at(0).toUInt(&ok)-1;
    if(ok&&(input<(unsigned)config()->inputQuantity())) {
      state=(Config::DelayState)args.at(1).toUInt(&ok);
      if(ok) {
	emit delayStateChangeReceived(input,state);
      }
    }
    break;

  case ProtocolCunc::DP:
    input=args.at(0).toUInt(&ok)-1;
    if(ok&&(input<(unsigned)config()->inputQuantity())) {
      emit delayDumpReceived(input);
    }
    break;
    break;
  }
}
