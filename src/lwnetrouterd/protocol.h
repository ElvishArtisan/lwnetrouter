// protocol.h
//
// Abstract base class for protocol objects.
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

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QObject>

#include "config.h"

class Protocol : public QObject
{
  Q_OBJECT;
 public:
  Protocol(Config *c,QObject *parent=0);

 signals:
  void crosspointChangeReceived(int output,int input);
  void delayStateChangeReceived(int input,Config::DelayState state);
  void delayDumpReceived(int input);

 public slots:
  virtual void sendCrossPoint(int output,int input);
  virtual void sendDelayState(int input,Config::DelayState state,int msec);

 protected:
  Config *config() const;

 private:
  Config *protocol_config;
};


#endif  // PROTOCOL_H
