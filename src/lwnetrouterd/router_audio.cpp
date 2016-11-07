// router_audio.cpp
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

#include "router_audio.h"

RouterAudio::RouterAudio(Config *c,QObject *parent)
  : Router(c,parent)
{
  QString err_msg;

  //
  // Ringbuffers
  //
  for(int i=0;i<config()->inputQuantity();i++) {
    audio_ringbuffers[i]=new Ringbuffer(48000*MAX_DELAY,2);
  }

  //
  // Sources
  //
  for(int i=0;i<config()->inputQuantity();i++) {
    audio_sources[i]=new AudioSource(i,audio_ringbuffers,config());
    if(!audio_sources[i]->start(&err_msg)) {
      fprintf(stderr,
	      "lwnetrouterd: failed to start playback device \"%s\" [%s]",
	      (const char *)config()->audioAlsaDevice(i).toUtf8(),
	      (const char *)err_msg.toUtf8());
      exit(256);
    }
  }

  //
  // Destinations
  //
  for(int i=0;i<config()->outputQuantity();i++) {
    audio_destinations[i]=new AudioDestination(i,audio_ringbuffers,config());
    if(!audio_destinations[i]->start(&err_msg)) {
      fprintf(stderr,
	      "lwnetrouterd: failed to start capture device \"%s\" [%s]",
	      (const char *)config()->audioAlsaDevice(i).toUtf8(),
	      (const char *)err_msg.toUtf8());
      exit(256);
    }
  }
}


RouterAudio::~RouterAudio()
{
  for(int i=0;i<config()->outputQuantity();i++) {
    delete audio_destinations[i];
  }
  for(int i=0;i<config()->inputQuantity();i++) {
    delete audio_sources[i];
  }
  for(int i=0;i<config()->inputQuantity();i++) {
    delete audio_ringbuffers[i];
  }
}


void RouterAudio::crossPointSet(int output,int input)
{
}
