// protocol_cunc.h
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

#ifndef PROTOCOL_CUNC_H
#define PROTOCOL_CUNC_H

#include "protocol.h"
#include "streamcmdserver.h"

class ProtocolCunc : public Protocol
{
  Q_OBJECT;
 public:
  ProtocolCunc(Config *c,QObject *parent=0);

 public slots:
  void sendDelayState(int input,Config::DelayState state,int msec);
  void sendDelayState(int id,int input,Config::DelayState state,int msec);
  void sendInputName(int id,int input,const QString &str);
  void sendDelayDumped(int input);

 signals:
  void delayStateRequested(int id,int input);
  void inputNameRequested(int id,int input);

 private slots:
  void commandReceivedData(int id,int cmd,const QStringList &args);

 private:
  enum Commands {DC=0,DQ=1,DM=2,DS=3,SS=4,DP=5};
  StreamCmdServer *cunc_server;
};


#endif  // PROTOCOL_CUNC_H
