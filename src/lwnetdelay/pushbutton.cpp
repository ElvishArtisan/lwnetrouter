//   pushbutton.cpp
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

#include <QMouseEvent>

#include "pushbutton.h"

PushButton::PushButton(QWidget *parent=0)
  : QPushButton(parent)
{
  Init();
}


PushButton::PushButton(const QString &text,QWidget *parent)
  : QPushButton(text,parent)
{
  Init();
}


QColor PushButton::flashColor() const
{
  return flash_color;
}


void PushButton::setFlashColor(QColor color)
{
  int h=0;
  int s=0;
  int v=0;

  flash_background_color=color;

  color.getHsv(&h,&s,&v);
  if((h>180)&&(h<300)) {
    v=255;
  }
  else {
    if(v<168) {
      v=255;
    }
    else {
      v=0;
    }
  }
  s=0;
  flash_color.setHsv(h,s,v);
}


bool PushButton::flashingEnabled() const
{
  return flashing_enabled;
}


void PushButton::setFlashingEnabled(bool state)
{
  flashing_enabled=state;
  if(flashing_enabled) {
    flashOn();
  }
  else {
    flashOff();
  }
}


int PushButton::flashPeriod() const
{
  return flash_period;
}


void PushButton::setFlashPeriod(int period)
{
  flash_period=period;
  if(flash_timer->isActive()) {
    flash_timer->stop();
    flash_timer->start(flash_period);
  }
}


PushButton::ClockSource PushButton::clockSource() const
{
  return flash_clock_source;
}


void PushButton::setClockSource(ClockSource src)
{
  if(src==flash_clock_source) {
    return;
  }
  flash_clock_source=src;
  if((src==PushButton::ExternalClock)&&(flash_timer->isActive())) {
    flash_timer->stop();
  }
  if((src==PushButton::InternalClock)&&flashing_enabled) {
    flashOn();
  }
}


void PushButton::tickClock()
{
  tickClock(flash_state);
}


void PushButton::tickClock(bool state)
{
  if(!flashing_enabled) {
    return;
  }
  //  QKeySequence a=accel();
  if(state) {
    flash_state=false;
    setStyleSheet("");
  }
  else {
    flash_state=true;
    setStyleSheet("color: "+flash_color.name()+
		  ";background-color: "+flash_background_color.name()+";");
  }
  //  setAccel(a);
}


void PushButton::flashOn()
{
  if((!flash_timer->isActive())&&
     (flash_clock_source==PushButton::InternalClock)) {
    flash_timer->start(flash_period);
  }
}


void PushButton::flashOff()
{
  if(flash_timer->isActive()&&
     (flash_clock_source==PushButton::InternalClock)) {
    flash_timer->stop();
  }
  setStyleSheet("");
}


void PushButton::Init()
{
  flash_timer=new QTimer(this);
  connect(flash_timer,SIGNAL(timeout()),this,SLOT(tickClock()));
  flash_state=true;
  flashing_enabled=false;
  flash_clock_source=PushButton::InternalClock;
  flash_period=PUSHBUTTON_DEFAULT_FLASH_PERIOD;
  setFlashColor(PUSHBUTTON_DEFAULT_FLASH_COLOR);
}


