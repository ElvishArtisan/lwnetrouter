// router_audio.h
//
// Audio router for lwnetrouterd(8)
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

#ifndef ROUTER_AUDIO_H
#define ROUTER_AUDIO_H

#include "audiodestination.h"
#include "audiosource.h"
#include "router.h"

class RouterAudio : public Router
{
  Q_OBJECT;
 public:
  RouterAudio(Config *config,QObject *parent=0);
  ~RouterAudio();

 protected:
  void crossPointSet(int output,int input);

 private:
  AudioDestination *audio_destinations[MAX_INPUTS];
  AudioSource *audio_sources[MAX_OUTPUTS];
  Ringbuffer *audio_ringbuffers[MAX_INPUTS];
};


#endif  // ROUTER_AUDIO_H
