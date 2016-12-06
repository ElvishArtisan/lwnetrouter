// router_breakaway.h
//
// Router for processing Internet breakaways.
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

#ifndef ROUTER_BREAKAWAY_H
#define ROUTER_BREAKAWAY_H

#include <queue>

#include <QDateTime>
#include <QTimer>
#include <QUdpSocket>

#include "router.h"

class RouterBreakawayEvent
{
 public:
  RouterBreakawayEvent(int msec);
  QDateTime timestamp() const;
  int length();

 private:
  QDateTime evt_timestamp;
  int evt_length;
};



class RouterBreakaway : public Router
{
  Q_OBJECT;
 public:
  RouterBreakaway(Config *c,QObject *parent=0);

 public slots:
  void setDelayState(int input,Config::DelayState state,int msec);
  void sendBreakaway(int input,int msec);

 private slots:
  void scanTimerData();

 private:
  void SendBreakaway(int input,int msec);
  std::queue<RouterBreakawayEvent *> break_events[MAX_INPUTS];
  int break_delay_intervals[MAX_INPUTS];
  QUdpSocket *break_socket;
  QTimer *break_scan_timer;
};


#endif  // ROUTER_BREAKAWAY_H
