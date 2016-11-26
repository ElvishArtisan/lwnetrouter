// router_cic.h
//
// LiveWire CIC router.
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

#ifndef ROUTER_CIC_H
#define ROUTER_CIC_H

#include <queue>

#include <QByteArray>
#include <QDateTime>
#include <QTimer>
#include <QUdpSocket>

#include "router.h"

class RouterCicEvent
{
 public:
  RouterCicEvent(const QByteArray &data);
  QDateTime timestamp() const;
  QByteArray data() const;

 private:
  QDateTime evt_timestamp;
  QByteArray evt_data;
};



class RouterCic : public Router
{
  Q_OBJECT;
 public:
  RouterCic(Config *c,QObject *parent=0);
  Config::DelayState delayState(int input) const;
  int delayInterval(int input);

 public slots:
  void setDelayState(int input,Config::DelayState state,int msec);

 private slots:
  void readyReadData();
  void scanTimerData();

 private:
  void SendPacket(int input,const QByteArray &data);
  QUdpSocket *cic_socket;
  QTimer *cic_scan_timer;
  Config::DelayState cic_delay_states[MAX_INPUTS];
  int cic_delay_intervals[MAX_INPUTS];
  std::queue<RouterCicEvent *> cic_events[MAX_INPUTS];
};


#endif  // ROUTER_CIC_H
