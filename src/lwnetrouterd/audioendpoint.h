// audioendpoint.h
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

#ifndef AUDIOENDPOINT_H
#define AUDIOENDPOINT_H

#include <stdint.h>

#include <alsa/asoundlib.h>

#include "config.h"
#include "ringbuffer.h"

class AudioEndpoint
{
 public:
  enum DeviceOp {Capture=SND_PCM_STREAM_CAPTURE,
		 Playback=SND_PCM_STREAM_PLAYBACK};
  AudioEndpoint(unsigned slot,Ringbuffer **rbs,Config *c);
  virtual ~AudioEndpoint();
  unsigned slotNumber() const;
  unsigned sampleRate() const;
  unsigned periodQuantity() const;
  unsigned bufferSize() const;
  virtual bool start(QString *err_msg)=0;

 protected:
  void setSampleRate(unsigned rate);
  void setPeriodQuantity(unsigned periods);
  void setBufferSize(unsigned size);
  Ringbuffer *rb(int input) const;
  Config *config() const;

 protected:
  bool startDevice(DeviceOp op,QString *err_msg);
  pthread_t alsa_pthread;
  snd_pcm_t *alsa_pcm;
  int32_t *alsa_pcm_buffer;
  float *st_pcm_buffer;

 private:
  unsigned audio_sample_rate;
  unsigned audio_period_quantity;
  unsigned audio_buffer_size;
  unsigned audio_slot_number;
  Ringbuffer **audio_ringbuffers;
  Config *audio_config;
};


#endif  // AUDIOENDPOINT_H
