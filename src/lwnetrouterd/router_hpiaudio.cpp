// router_hpiaudio.cpp
//
// Audio router using an AudioScience HPI device.
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

#include <stdio.h>
#include <syslog.h>
#include <unistd.h>

#include <samplerate.h>

#include "router_hpiaudio.h"

hpi_err_t HpiError(hpi_err_t err)
{
  char str[200];
  if(err!=0) {
    HPI_GetErrorText(err,str);
    syslog(LOG_INFO,"HPI error %s",str);
  }
  return err;
}


void *__AudioCallback(void *ptr)
{
  static RouterHpiAudio *rha=(RouterHpiAudio *)ptr;
  static uint16_t in_state;
  static uint32_t in_buffer_size;
  static uint32_t in_data_len;
  static uint32_t in_frame_len;
  static uint32_t in_aux_len;
  static uint16_t out_state;
  static uint32_t out_buffer_size;
  static uint32_t out_data_to_play;
  static uint32_t out_frames_played;
  static uint32_t out_aux_to_play;
  static uint8_t pcm[262144];
  static int inputs=rha->config()->inputQuantity();
  static int outputs=rha->config()->outputQuantity();
  static Ringbuffer *rb[MAX_INPUTS];
  static unsigned frames_avail;

  //
  // Initialize Ringbuffers
  //
  for(int i=0;i<inputs;i++) {
    rb[i]=new Ringbuffer(4800000,2);
  }

  //
  // Main Loop
  //
  while(1==1) {
    //
    // Process Inputs
    //
    HpiError(HPI_InStreamGetInfoEx(NULL,rha->hpi_input_streams[0],&in_state,
				   &in_buffer_size,&in_data_len,&in_frame_len,
				   &in_aux_len));
    for(int i=0;i<inputs;i++) {
      HpiError(HPI_InStreamReadBuf(NULL,rha->hpi_input_streams[i],pcm,
				   in_data_len));
      rb[i]->write((float *)pcm,in_data_len);
    }

    //
    // Process Outputs
    //
    HpiError(HPI_OutStreamGetInfoEx(NULL,rha->hpi_output_streams[0],&out_state,
				    &out_buffer_size,&out_data_to_play,
				    &out_frames_played,&out_aux_to_play));
    frames_avail=(out_buffer_size-out_data_to_play)/8;
    for(int i=0;i<outputs;i++) {
      if(rb[i]->readSpace()<frames_avail) {
	frames_avail=rb[i]->readSpace();
      }
    }
    //    printf("frames_avail: %u\n",frames_avail);
    if(frames_avail>0) {
      for(int i=0;i<outputs;i++) {
	rb[i]->peek((float *)pcm,frames_avail);
	HpiError(HPI_OutStreamWriteBuf(NULL,rha->hpi_output_streams[i],pcm,
				       frames_avail,rha->hpi_format));
	if(out_state==HPI_STATE_STOPPED) {
	  HpiError(HPI_OutStreamStart(NULL,rha->hpi_output_streams[i]));
	}
      }
      for(int i=0;i<inputs;i++) {
	rb[i]->read((float *)pcm,frames_avail);
      }
    }
    usleep(5000);
  }

  /*
  //
  // Main Loop
  //
  while(1==1) {
    HpiError(HPI_InStreamGetInfoEx(NULL,rha->hpi_input_streams[0],&in_state,
				   &in_buffer_size,&in_data_len,&in_frame_len,
				   &in_aux_len));
    for(int i=0;i<inputs;i++) {
      HpiError(HPI_InStreamReadBuf(NULL,rha->hpi_input_streams[i],pcm,in_data_len));
      if(in_data_len>0) {
	HpiError(HPI_OutStreamGetInfoEx(NULL,rha->hpi_output_streams[i],&out_state,
					&out_buffer_size,&out_data_to_play,
					&out_frames_played,&out_aux_to_play));
	HpiError(HPI_OutStreamWriteBuf(NULL,rha->hpi_output_streams[i],pcm,
				       in_data_len,rha->hpi_format));
	if(out_state==HPI_STATE_STOPPED) {
	  HpiError(HPI_OutStreamStart(NULL,rha->hpi_output_streams[i]));
	}
      }
    }
    usleep(5000);
  }
  */
  return NULL;
}




RouterHpiAudio::RouterHpiAudio(Config *c,QObject *parent)
  : Router(c,parent)
{
  hpi_format=new struct hpi_format;
  short on_gain[HPI_MAX_CHANNELS]={0,0};
  short off_gain[HPI_MAX_CHANNELS]={-10000,-10000};

  //
  // Initialize Mixer
  //
  HpiError(HPI_MixerOpen(NULL,0,&hpi_mixer));
  for(int i=0;i<config()->inputQuantity();i++) {
    for(int j=0;j<config()->outputQuantity();j++) {
      HpiError(HPI_MixerGetControl(NULL,hpi_mixer,
				   HPI_SOURCENODE_OSTREAM,i,
				   HPI_DESTNODE_LINEOUT,j,
				   HPI_CONTROL_VOLUME,
				   &hpi_output_volumes[i][j]));
      HpiError(HPI_VolumeSetGain(NULL,hpi_output_volumes[i][j],off_gain));
    }
    HpiError(HPI_VolumeSetGain(NULL,hpi_output_volumes[i][i],on_gain));
  }

  //
  // Open Inputs
  //
  HpiError(HPI_FormatCreate(hpi_format,2,HPI_FORMAT_PCM32_FLOAT,
			    AUDIO_SAMPLE_RATE,0,0));
  for(int i=0;i<config()->inputQuantity();i++) {
    HpiError(HPI_InStreamOpen(NULL,0,i,&hpi_input_streams[i]));
    HpiError(HPI_InStreamSetFormat(NULL,hpi_input_streams[i],hpi_format));
    HpiError(HPI_InStreamStart(NULL,hpi_input_streams[i]));
  }

  //
  // Open Outputs
  //
  for(int i=0;i<config()->outputQuantity();i++) {
    HpiError(HPI_OutStreamOpen(NULL,0,i,&hpi_output_streams[i]));
    HpiError(HPI_OutStreamSetFormat(NULL,hpi_output_streams[i],hpi_format));
  }

  //
  // Start the Callback
  //
  pthread_attr_t pthread_attr;
  pthread_attr_init(&pthread_attr);
  pthread_attr_setschedpolicy(&pthread_attr,SCHED_FIFO);
  pthread_create(&hpi_pthread,&pthread_attr,__AudioCallback,this);
}


void RouterHpiAudio::crossPointSet(int output,int input)
{
  printf("RouterHpiAudio::crossPointSet(%d,%d)\n",output,input);
}
