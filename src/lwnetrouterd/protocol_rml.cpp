// protocol_rml.cpp
//
// RML protocol for lwnetrouterd(8)
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

#include <syslog.h>

#include <QByteArray>
#include <QStringList>

#include "protocol_rml.h"

ProtocolRml::ProtocolRml(Config *c,QObject *parent)
  : Protocol(c,parent)
{
  rml_socket=new QUdpSocket(this);
  if(!rml_socket->bind(config()->rmlPort())) {
    syslog(LOG_WARNING,"unable to bind to RML port");
  }
  connect(rml_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
}


void ProtocolRml::readyReadData()
{
  char data[1501];
  int n;

  while((n=rml_socket->readDatagram(data,1500))>0) {
    for(int i=0;i<n;i++) {
      switch(0xFF&data[i]) {
      case '!':
	ProcessCommand(rml_accum);
	rml_accum="";
	break;

      default:
	rml_accum+=data[i];
	break;
      }
    }
  }
}


void ProtocolRml::ProcessCommand(const QString &cmd)
{
  QStringList cmds=cmd.split(" ",QString::SkipEmptyParts);
  bool ok=false;

  if((cmds.at(0)=="ST")&&(cmds.size()==4)) {
    unsigned matrix=cmds.at(1).toUInt(&ok);
    if(ok&&(matrix==0)) {
      unsigned input=cmds.at(2).toUInt(&ok);
      if(ok&&(input<=(unsigned)config()->inputQuantity())) {
	unsigned output=cmds.at(3).toUInt(&ok);
	if(ok&&(output<=(unsigned)config()->outputQuantity())) {
	  emit crosspointChangeReceived(output-1,input-1);
	}
      }
    }
  }
}
