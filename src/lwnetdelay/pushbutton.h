// pushbutton.h
//
//   Flashing button widget.
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QTimer>
#include <QPushButton>
#include <QWidget>

/*
 * Widget Defaults
 */
#define PUSHBUTTON_DEFAULT_FLASH_PERIOD 300
#define PUSHBUTTON_DEFAULT_FLASH_COLOR Qt::blue


class PushButton : public QPushButton
{
  Q_OBJECT
 public:
  enum ClockSource {InternalClock=0,ExternalClock=1};
  PushButton(QWidget *parent);
  PushButton(const QString &text,QWidget *parent);
  QColor flashColor() const;
  void setFlashColor(QColor color);
  int flashPeriod() const;
  void setFlashPeriod(int period);
  ClockSource clockSource() const;
  void setClockSource(ClockSource src);
  bool flashingEnabled() const;

 public slots:
  void setFlashingEnabled(bool state);
  void tickClock();
  void tickClock(bool state);

 private:
  void flashOn();
  void flashOff();
  void Init();
  bool flash_state;
  int flash_period;
  bool flashing_enabled;
  QColor flash_color;
  QColor flash_background_color;
  QTimer *flash_timer;
  ClockSource flash_clock_source;
};



#endif  // PUSHBUTTON_H
