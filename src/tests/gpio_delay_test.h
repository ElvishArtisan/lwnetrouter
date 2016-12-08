// gpio_delay_test.h
//
// Test delay intervals of LiveWire GPIO events
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

#ifndef GPIO_DELAY_TEST_H
#define GPIO_DELAY_TEST_H

#include <sys/time.h>

#include <QObject>

#include <sy/sygpio_server.h>

#define GPIO_DELAY_TEST_USAGE "--send-source=<src-num> --recv-source=<src-num>\n"

class MainObject : public QObject
{
 Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void gpoReceivedData(int gpo,int line,bool state,bool pulse);

 private:
  double TimeValue(struct timeval *tv);
  int test_send_sourcenum;
  struct timeval test_send_tv;
  int test_recv_sourcenum;
  struct timeval test_recv_tv;
  SyRouting *test_routing_server;
  SyGpioServer *test_gpio_server;
};


#endif  // GPIO_DELAY_TEST_H
