// state.cpp
//
// Persistent device state for lwnetrouterd(8)
//
//   (C) Copyright 2016-2025 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>

#include <sy6/syprofile.h>

#include "state.h"

State::State(QObject *parent)
  : QObject(parent)
{
  for(int i=0;i<MAX_INPUTS;i++) {
    state_delay_initializeds[i]=false;
  }
}


int State::crossPoint(int output) const
{
  return state_cross_points[output];
}


bool State::delayActive(int input)
{
  return state_delay_actives[input];
}


void State::setDelayActive(int input,bool state)
{
  state_delay_actives[input]=state;
  save();
}


bool State::delayInitialized(int input) const
{
  return state_delay_initializeds[input];
}


void State::setDelayInitialized(int input)
{
  state_delay_initializeds[input]=true;
}


void State::load()
{
  SyProfile *p=new SyProfile();
  p->setSource(STATE_FILE);

  for(int i=0;i<MAX_INPUTS;i++) {
    state_delay_actives[i]=
      p->intValue(QString::asprintf("Input%d",i+1),"DelayActive");
  }
  for(int i=0;i<MAX_OUTPUTS;i++) {
    state_cross_points[i]=
      p->intValue(QString::asprintf("Output%d",i+1),"CrossPoint",-1);
  }

  delete p;
}


void State::save() const
{
  FILE *f=NULL;

  if((f=fopen((STATE_FILE+"-temp").toUtf8(),"w"))!=NULL) {
    for(int i=0;i<MAX_INPUTS;i++) {
      fprintf(f,"[Input%d]\n",i+1);
      fprintf(f,"DelayActive=%u\n",state_delay_actives[i]);
      fprintf(f,"\n");
    }
    for(int i=0;i<MAX_OUTPUTS;i++) {
      fprintf(f,"[Output%d]\n",i+1);
      fprintf(f,"CrossPoint=%d\n",state_cross_points[i]);
      fprintf(f,"\n");
    }
    fclose(f);
    rename((STATE_FILE+"-temp").toUtf8(),STATE_FILE.toUtf8());
  }
}


void State::setCrossPoint(int output,int input)
{
  state_cross_points[output]=input;
  save();
}
