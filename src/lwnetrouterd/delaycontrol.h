// delaycontrol.h
//
// Controller for a delay device via LiveWire GPIO
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

#ifndef DELAYCONTROL_H
#define DELAYCONTROL_H

#include <QObject>
#include <QTimer>

#include <sy/sygpio_server.h>

class DelayControl : public QObject
{
  Q_OBJECT;
 public:
  enum ButtonState {ButtonOff=0,ButtonOn=1,ButtonSlowFlash=2,ButtonFastFlash=3};
  DelayControl(int source,SyGpioServer *gpio,QObject *parent=0);
  ~DelayControl();

 public slots:
  void setButtonState(int line,ButtonState state);

 private slots:
  void flashData();

 private:
  void SendGpi(int line,bool state);
  int delay_source;
  SyGpioServer *delay_gpio;
  ButtonState delay_states[SWITCHYARD_GPIO_BUNDLE_SIZE];
  bool delay_gpio_states[SWITCHYARD_GPIO_BUNDLE_SIZE];
  QTimer *delay_flash_timer;
  unsigned delay_flash_phase;
};


#endif  // DELAYCONTROL_H
