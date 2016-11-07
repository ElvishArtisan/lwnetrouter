// audiosource.cpp
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

#include <syslog.h>

#include <samplerate.h>

#include "audiosource.h"

void *__AudioSourceCallback(void *ptr)
{
  static AudioSource *src=(AudioSource *)ptr;
  static int n;

  while(1==1) {
    while(src->rb(0)->readSpace()<1024);
    n=src->rb(0)->
      read(src->st_pcm_buffer,src->bufferSize()/(src->periodQuantity()*2));
    src_float_to_int_array(src->st_pcm_buffer,src->alsa_pcm_buffer,
			   n*AUDIO_CHANNELS);
    snd_pcm_writei(src->alsa_pcm,src->alsa_pcm_buffer,n);
    if((snd_pcm_state(src->alsa_pcm)!=SND_PCM_STATE_RUNNING)&&(n<0)) {
      snd_pcm_drop(src->alsa_pcm);
      snd_pcm_prepare(src->alsa_pcm);
      if(n==-EPIPE) {
	syslog(LOG_NOTICE,"****** ALSA Playback Xrun ******");
      }
      else {
	syslog(LOG_WARNING,"ALSA Error [%s]",snd_strerror(n));
      }
    }
    else {
      src_int_to_float_array(src->alsa_pcm_buffer,src->st_pcm_buffer,
			     n*AUDIO_CHANNELS);
      src->rb(0)->write(src->st_pcm_buffer,n);
      printf("read %d frames\n",n);
    }
  }

  /*
  while(1==1) {
    n=snd_pcm_readi(dest->alsa_pcm,dest->alsa_pcm_buffer,
		    dest->bufferSize()/(dest->periodQuantity()*AUDIO_CHANNELS));
    if((snd_pcm_state(dest->alsa_pcm)!=SND_PCM_STATE_RUNNING)&&(n<0)) {
      snd_pcm_drop(dest->alsa_pcm);
      snd_pcm_prepare(dest->alsa_pcm);
      if(n==-EPIPE) {
	syslog(LOG_NOTICE,"****** ALSA Capture Xrun ******");
      }
      else {
	syslog(LOG_WARNING,"ALSA Error [%s]",snd_strerror(n));
      }
    }
    else {
      src_int_to_float_array(dest->alsa_pcm_buffer,dest->st_pcm_buffer,
			     n*AUDIO_CHANNELS);
      dest->rb(0)->write(dest->st_pcm_buffer,n);
      printf("wrote %d frames\n",n);
    }
  }
  */
  return NULL;
}


AudioSource::AudioSource(unsigned slot,Ringbuffer **rb,Config *c)
  : AudioEndpoint(slot,rb,c)
{
}


bool AudioSource::start(QString *err_msg)
{
  pthread_attr_t pthread_attr;

  if(!startDevice(AudioEndpoint::Playback,err_msg)) {
    return false;
  }
  //
  // Start the Callback
  //
  pthread_attr_init(&pthread_attr);
  pthread_attr_setschedpolicy(&pthread_attr,SCHED_FIFO);
  pthread_create(&alsa_pthread,&pthread_attr,__AudioSourceCallback,this);

  return true;
}
