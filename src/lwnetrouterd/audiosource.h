// audiosource.h
//
// ALSA audio source for lwnetrouterd(8)
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

#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include <alsa/asoundlib.h>
#include <pthread.h>

#include "config.h"
#include "ringbuffer.h"

class AudioSource
{
 public:
  AudioSource(Ringbuffer *rb,Config *config);
  ~AudioSource();
  bool start(int input);

 private:
  snd_pcm_t *alsa_pcm;
  unsigned alsa_samplerate;
  unsigned alsa_channels;
  unsigned alsa_period_quantity;
  snd_pcm_uframes_t alsa_buffer_size; 
  float *alsa_pcm_buffer;
  pthread_t alsa_pthread;
  Config *audio_config;
  friend void *__AudioSourceCallback(void *ptr);
};


#endif  // AUDIOSOURCE_H
