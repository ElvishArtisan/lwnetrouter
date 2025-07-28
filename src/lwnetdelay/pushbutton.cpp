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


PushButton::ButtonMode PushButton::buttonMode() const
{
  return flash_button_mode;
}


void PushButton::setButtonMode(PushButton::ButtonMode mode)
{
  if(mode!=flash_button_mode) {
    flashData();
    flash_button_mode=mode;
  }
}


void PushButton::flashData()
{
  flash_phase++;

  unsigned phase=flash_phase%4;

  switch(flash_button_mode) {
  case PushButton::ButtonOff:
    UpdateButton(false);
    break;

  case PushButton::ButtonOn:
    UpdateButton(true);
    break;

  case PushButton::ButtonSlowFlash:
    UpdateButton((phase==0)||(phase==1));
    break;

  case PushButton::ButtonFastFlash:
    UpdateButton((phase==0)||(phase==2));
    break;
  }
}


void PushButton::UpdateButton(bool state)
{
  if(state) {
    setStyleSheet("color: "+flash_color.name()+
		  ";background-color: "+flash_background_color.name()+";");
  }
  else {
    setStyleSheet("");
  }
}


void PushButton::Init()
{
  flash_phase=0;
  flash_button_mode=PushButton::ButtonOff;
  flash_timer=new QTimer(this);
  connect(flash_timer,SIGNAL(timeout()),this,SLOT(flashData()));
  setFlashColor(PUSHBUTTON_DEFAULT_FLASH_COLOR);
  flash_timer->start(200);
}


