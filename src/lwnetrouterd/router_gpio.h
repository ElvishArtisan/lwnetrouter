// router_gpio.h
//
// LiveWire GPIO router.
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

#ifndef ROUTER_GPIO_H
#define ROUTER_GPIO_H

#include <queue>

#include <QDateTime>
#include <QTimer>

#include <sy/sygpio_server.h>
#include <sy/sylwrp_client.h>

#include "router.h"
#include "ttydevice.h"

class RouterGpioEvent
{
 public:
  RouterGpioEvent(int line);
  QDateTime timestamp() const;
  int line() const;

 private:
  QDateTime evt_timestamp;
  int evt_line;
};



class RouterGpio : public Router
{
  Q_OBJECT;
 public:
  RouterGpio(SyGpioServer *gpioserv,Config *c,QObject *parent=0);
  Config::DelayState delayState(int input) const;
  int delayInterval(int input);

 public slots:
  void setDelayState(int input,Config::DelayState state,int msec);

 private slots:
  void gpoReceivedData(int gpo,int line,bool state,bool pulse);
  void scanTimerData();

 private:
  void SendGpo(int input,int line);
  SyGpioServer *gpio_server;
  SyLwrpClient *gpio_lwrp;
  TTYDevice *gpio_netcue_device;
  QTimer *gpio_scan_timer;
  Config::DelayState gpio_delay_states[MAX_INPUTS];
  int gpio_delay_intervals[MAX_INPUTS];
  std::queue<RouterGpioEvent *> gpio_events[MAX_INPUTS];
};


#endif  // ROUTER_GPIO_H
