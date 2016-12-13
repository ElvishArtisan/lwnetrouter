// protocol_sap.cpp
//
// Software Authority protocol driver
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

#include "protocol_sap.h"

ProtocolSap::ProtocolSap(SyLwrpClient *lwrp,Config *c,QObject *parent)
  : Protocol(c,parent)
{
  sap_lwrp=lwrp;

  //
  // Stream Server
  //
  QTcpServer *server=new QTcpServer(this);
  server->listen(QHostAddress::Any,config()->softwareAuthorityPort());
  std::map<int,QString> cmds;
  std::map<int,int> upper_limits;
  std::map<int,int> lower_limits;

  cmds[ProtocolSap::Login]="Login";
  upper_limits[ProtocolSap::Login]=2;
  lower_limits[ProtocolSap::Login]=1;

  cmds[ProtocolSap::Exit]="Exit";
  upper_limits[ProtocolSap::Exit]=0;
  lower_limits[ProtocolSap::Exit]=0;

  cmds[ProtocolSap::RouterNames]="RouterNames";
  upper_limits[ProtocolSap::RouterNames]=0;
  lower_limits[ProtocolSap::RouterNames]=0;

  cmds[ProtocolSap::SourceNames]="SourceNames";
  upper_limits[ProtocolSap::SourceNames]=1;
  lower_limits[ProtocolSap::SourceNames]=1;

  cmds[ProtocolSap::DestNames]="DestNames";
  upper_limits[ProtocolSap::DestNames]=1;
  lower_limits[ProtocolSap::DestNames]=1;

  cmds[ProtocolSap::ActivateRoute]="ActivateRoute";
  upper_limits[ProtocolSap::ActivateRoute]=3;
  lower_limits[ProtocolSap::ActivateRoute]=3;

  cmds[ProtocolSap::RouteStat]="RouteStat";
  upper_limits[ProtocolSap::RouteStat]=2;
  lower_limits[ProtocolSap::RouteStat]=1;

  sap_server=
    new StreamCmdServer(cmds,upper_limits,lower_limits,server,this);
  connect(sap_server,SIGNAL(commandReceived(int,int,const QStringList &)),
	  this,SLOT(commandReceivedData(int,int,const QStringList &)));
}


void ProtocolSap::sendCrossPoint(int output,int input)
{
  sap_server->sendCommand(QString().sprintf("RouteStat 1 %d %d False\r\n",
					    output+1,input+1));
}


void ProtocolSap::sendCrossPoint(int id,int output,int input)
{
  sap_server->sendCommand(id,QString().sprintf("RouteStat 1 %d %d False\r\n",
					       output+1,input+1));
}


void ProtocolSap::commandReceivedData(int id,int cmd,const QStringList &args)
{
  QString reply;
  unsigned cardnum;
  int input;
  int output;
  bool ok=false;

  switch((ProtocolSap::Commands)cmd) {
  case ProtocolSap::Login:
    sap_server->sendCommand(id,"Login Successful\r\n");
    break;

  case ProtocolSap::Exit:
    sap_server->closeConnection(id);
    break;

  case ProtocolSap::RouterNames:
    sap_server->sendCommand(id,"Begin RouterNames\r\n");
    sap_server->sendCommand(id,"    1 LiveWire Audio\r\n");
    sap_server->sendCommand(id,"End RouterNames\r\n");
    break;

  case ProtocolSap::SourceNames:
    if(args.at(0).toInt()==1) {
      sap_server->sendCommand(id,"Begin SourceNames - 1\r\n");
      for(int i=0;i<config()->inputQuantity();i++) {
	reply=QString().sprintf("    %d",i+1)+
	  "\t"+sap_lwrp->dstName(i)+
	  "\t"+sap_lwrp->dstName(i)+
	  "\t"+sap_lwrp->hostAddress().toString()+
	  "\t"+sap_lwrp->hostName()+
	  "\t"+QString().sprintf("%d",i+1)+
	  "\t"+QString().sprintf("%u",SyRouting::livewireNumber(sap_lwrp->dstAddress(i)))+
	  "\t"+sap_lwrp->dstAddress(i).toString()+
	  "\r\n";
	sap_server->sendCommand(id,reply);
      }
      sap_server->sendCommand(id,"End SourceNames - 1\r\n");
    }
    else {
      sap_server->sendCommand(id,"Error - Router Does Not exist.\r\n");
    }
    break;

  case ProtocolSap::DestNames:
    if(args.at(0).toInt()==1) {
      sap_server->sendCommand(id,"Begin DestNames - 1\r\n");
      for(int i=0;i<config()->outputQuantity();i++) {
	reply=QString().sprintf("    %d",i+1)+
	  "\t"+sap_lwrp->srcName(i)+
	  "\t"+sap_lwrp->srcName(i)+
	  "\t"+sap_lwrp->hostAddress().toString()+
	  "\t"+sap_lwrp->hostName()+
	  "\t"+QString().sprintf("%d",i+1)+
	  "\r\n";
	sap_server->sendCommand(id,reply);
      }
      sap_server->sendCommand(id,"End DestNames - 1\r\n");
    }
    else {
      sap_server->sendCommand(id,"Error - Router Does Not exist.\r\n");
    }
    break;

  case ProtocolSap::ActivateRoute:
    cardnum=args[0].toUInt(&ok);
    if(ok&&(cardnum==1)) {
      output=args[1].toInt(&ok)-1;
      if(ok) {
	input=args[2].toInt(&ok)-1;
	if(ok) {
	  emit crosspointChangeReceived(output,input);
	}
	else {
	  sap_server->sendCommand(id,"Error\r\n");
	  sap_server->sendCommand(id,">>");
	}
      }
      else {
	sap_server->sendCommand(id,"Error\r\n");
	sap_server->sendCommand(id,">>");
      }
    }
    else {
      sap_server->sendCommand(id,"Error\r\n");
      sap_server->sendCommand(id,">>");
    }
    break;

  case ProtocolSap::RouteStat:
    if(args.size()>=1) {
      cardnum=args[0].toUInt(&ok);
      if(ok) {
	if(args.size()>=2) {
	  output=args[1].toInt(&ok)-1;
	  if(!ok) {
	    sap_server->sendCommand(id,"Error\r\n");
	    sap_server->sendCommand(id,">>");
	    return;
	  }
	}
	else {
	  output=-1;
	}
      }
      else {
	sap_server->sendCommand(id,"Error\r\n");
	sap_server->sendCommand(id,">>");
	return;
      }
      emit crosspointRequested(id,output);
    }
    else {
      sap_server->sendCommand(id,"Error\r\n");
      sap_server->sendCommand(id,">>");
    }
    break;
  }
}
