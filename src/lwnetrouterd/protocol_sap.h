// protocol_sap.h
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

#ifndef PROTOCOL_SAP_H
#define PROTOCOL_SAP_H

#include <sy/sylwrp_client.h>

#include "protocol.h"
#include "streamcmdserver.h"

class ProtocolSap : public Protocol
{
  Q_OBJECT;
 public:
  ProtocolSap(SyLwrpClient *lwrp,Config *c,QObject *parent=0);

 signals:
  void crosspointRequested(int id,int output);

 public slots:
   void sendCrossPoint(int output,int input);
   void sendCrossPoint(int id,int output,int input);

 private slots:
  void commandReceivedData(int id,int cmd,const QStringList &args);

 private:
  enum Commands {Login=0,Exit=1,RouterNames=2,SourceNames=3,DestNames=4,
		 ActivateRoute=5,RouteStat=6};
  StreamCmdServer *sap_server;
  SyLwrpClient *sap_lwrp;
};


#endif  // PROTOCOL_SAP_H
