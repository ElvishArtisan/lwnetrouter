// router.h
//
// Abstract base class for router objects.
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

#ifndef ROUTER_H
#define ROUTER_H

#include <vector>

#include <QObject>

#include "config.h"

class Router : public QObject
{
  Q_OBJECT;
 public:
  Router(Config *config,QObject *parent=0);
  int crossPoint(int output) const;
  virtual Config::DelayState delayState(int input) const=0;
  virtual int delayInterval(int input)=0;

 signals:
  void delayStateChanged(int input,Config::DelayState state,int msec);

 public slots:
  void setCrossPoint(int output,int input);
  virtual void setDelayState(int input,Config::DelayState state)=0;
  virtual void dumpDelay(int input)=0;

 protected:
  virtual void crossPointSet(int output,int input);
  Config *config() const;

 private:
  std::vector<int> router_crosspoints;
  Config *router_config;
};


#endif  // ROUTER_H
