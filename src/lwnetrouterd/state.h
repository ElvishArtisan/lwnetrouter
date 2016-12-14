// state.h
//
// Persistent device state for lwnetrouterd(8)
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

#ifndef STATE_H
#define STATE_H

#include <QObject>

#include "config.h"

#define STATE_FILE QString("/var/cache/aoip/lwnetrouterd.state")

class State : public QObject
{
  Q_OBJECT;
 public:
  State(QObject *parent=0);
  int crossPoint(int output) const;
  bool delayActive(int input);
  void setDelayActive(int input,bool state);
  bool delayInitialized(int input) const;
  void setDelayInitialized(int input);
  void load();
  void save() const;

 public slots:
  void setCrossPoint(int output,int input);

 private:
  int state_cross_points[MAX_OUTPUTS];
  bool state_delay_actives[MAX_INPUTS];
  bool state_delay_initializeds[MAX_INPUTS];
};


#endif  // STATE_H
