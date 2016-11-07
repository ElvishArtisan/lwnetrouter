// audioendpoint.cpp
//
// ALSA audio endpoint for lwnetrouterd(8)
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

#include "audioendpoint.h"

AudioEndpoint::AudioEndpoint(unsigned slot,Ringbuffer **rbs,Config *c)
{
  audio_sample_rate=0;
  audio_period_quantity=0;
  audio_buffer_size=0;
  audio_slot_number=slot;
  audio_ringbuffers=rbs;
  audio_config=c;
}


AudioEndpoint::~AudioEndpoint()
{
}


unsigned AudioEndpoint::slotNumber() const
{
  return audio_slot_number;
}


unsigned AudioEndpoint::sampleRate() const
{
  return audio_sample_rate;
}


unsigned AudioEndpoint::periodQuantity() const
{
  return audio_period_quantity;
}


unsigned AudioEndpoint::bufferSize() const
{
  return audio_buffer_size;
}


void AudioEndpoint::setSampleRate(unsigned rate)
{
  audio_sample_rate=rate;
}


void AudioEndpoint::setPeriodQuantity(unsigned periods)
{
  audio_period_quantity=periods;
}


void AudioEndpoint::setBufferSize(unsigned size)
{
  audio_buffer_size=size;
}


Ringbuffer *AudioEndpoint::rb(int input) const
{
  return audio_ringbuffers[input];
}


Config *AudioEndpoint::config() const
{
  return audio_config;
}


bool AudioEndpoint::startDevice(AudioEndpoint::DeviceOp op,QString *err_msg)
{
  snd_pcm_hw_params_t *hwparams;
  snd_pcm_sw_params_t *swparams;
  int dir;
  int aerr;
  //  pthread_attr_t pthread_attr;

  if(snd_pcm_open(&alsa_pcm,config()->audioAlsaDevice(slotNumber()).toUtf8(),
		  (snd_pcm_stream_t)op,0)!=0) {
    *err_msg="unable to open ALSA device \""+
      config()->audioAlsaDevice(slotNumber())+"\"";
    return false;
  }
  snd_pcm_hw_params_alloca(&hwparams);
  snd_pcm_hw_params_any(alsa_pcm,hwparams);

  //
  // Access Type
  //
  if(snd_pcm_hw_params_test_access(alsa_pcm,hwparams,
				   SND_PCM_ACCESS_RW_INTERLEAVED)<0) {
    *err_msg="interleaved access not supported";
    return false;
  }
  snd_pcm_hw_params_set_access(alsa_pcm,hwparams,SND_PCM_ACCESS_RW_INTERLEAVED);

  //
  // Sample Format
  //
  if(snd_pcm_hw_params_set_format(alsa_pcm,hwparams,
				  SND_PCM_FORMAT_S32_LE)!=0) {
    *err_msg="sample format S32_LE not supported";
    return false;
  }

  //
  // Sample Rate
  //
  unsigned samprate=AUDIO_SAMPLE_RATE;
  if(snd_pcm_hw_params_set_rate_near(alsa_pcm,hwparams,&samprate,&dir)!=0) {
    *err_msg="Unable to set sample rate";
    return false;
  }
  setSampleRate(samprate);

  //
  // Channels
  //
  if(snd_pcm_hw_params_set_channels(alsa_pcm,hwparams,AUDIO_CHANNELS)!=0) {
    *err_msg=QString().sprintf("%u channels not supported",AUDIO_CHANNELS);
    return false;
  }

  //
  // Buffer Parameters
  //
  unsigned periods=config()->audioPeriodQuantity();
  snd_pcm_hw_params_set_periods_near(alsa_pcm,hwparams,&periods,&dir);
  if(periods!=(unsigned)config()->audioPeriodQuantity()) {
    syslog(LOG_INFO,"using ALSA period quantity of %u",periods);
  }
  setPeriodQuantity(periods);

  snd_pcm_uframes_t buffer_size=config()->audioBufferSize();
  snd_pcm_hw_params_set_buffer_size_near(alsa_pcm,hwparams,&buffer_size);
  if(buffer_size!=(unsigned)config()->audioBufferSize()) {
    syslog(LOG_INFO,"using ALSA buffer size of %lu frames",buffer_size);
  }
  setBufferSize(buffer_size);

  //
  // Fire It Up
  //
  if((aerr=snd_pcm_hw_params(alsa_pcm,hwparams))<0) {
    *err_msg=QString("ALSA device error 1: ")+snd_strerror(aerr);
    return false;
  }
  alsa_pcm_buffer=new int32_t[buffer_size*AUDIO_CHANNELS];
  st_pcm_buffer=new float[buffer_size*AUDIO_CHANNELS];

  //
  // Set Wake-up Timing
  //
  snd_pcm_sw_params_alloca(&swparams);
  snd_pcm_sw_params_current(alsa_pcm,swparams);
  snd_pcm_sw_params_set_avail_min(alsa_pcm,swparams,buffer_size/AUDIO_CHANNELS);
  if((aerr=snd_pcm_sw_params(alsa_pcm,swparams))<0) {
    *err_msg=QString("ALSA device error 2")+": "+snd_strerror(aerr);
    return false;
  }
  return true;
}
