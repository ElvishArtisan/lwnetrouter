// lwnetrouterd.h
//
// lwnetrouterd(8) routing daemon
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

#ifndef LWNETROUTERD_H
#define LWNETROUTERD_H

#include <QObject>

#include "config.h"
#include "protocol_cunc.h"
#include "protocol_rml.h"
#include "router_hpiaudio.h"

#define LWNETROUTERD_USAGE "[options]\n"

class MainObject : public QObject
{
 Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void cuncDelayStateRequestedData(int id,int input);

 private:
  RouterHpiAudio *main_audio_router;
  ProtocolCunc *main_cunc_protocol;
  ProtocolRml *main_rml_protocol;
  Config *main_config;
};


#endif  // LWNETROUTERD_H
