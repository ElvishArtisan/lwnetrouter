// router.cpp
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

#include "router.h"

Router::Router(Config *config,QObject *parent)
  : QObject(parent)
{
  router_config=config;

  for(int i=0;i<config->outputQuantity();i++) {
    router_crosspoints.push_back(0);
  }
}


int Router::crossPoint(int output) const
{
  return router_crosspoints.at(output);
}


Config::DelayState Router::delayState(int input) const
{
  return router_delay_states[input];
}


int Router::delayInterval(int input)
{
  return 0;
}


void Router::setCrossPoint(int output,int input)
{
  crossPointSet(output,input);
  router_crosspoints[output]=input;
  emit crossPointChanged(output,input);
}


void Router::setDelayState(int input,Config::DelayState state)
{
  router_delay_states[input]=state;
}


void Router::dumpDelay(int input)
{
}


void Router::sendBreakaway(int input,int msec)
{
}


void Router::crossPointSet(int output,int input)
{
}


Config *Router::config() const
{
  return router_config;
}
