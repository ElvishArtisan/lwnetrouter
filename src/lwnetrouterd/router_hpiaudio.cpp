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
  static int xpoints[MAX_OUTPUTS];
  static unsigned frames_avail;
  static int i;

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
    // Get Crosspoint Table
    //
    for(i=0;i<outputs;i++) {
      xpoints[i]=rha->crossPoint(i);
    }

    //
    // Process Inputs
    //
    HpiError(HPI_InStreamGetInfoEx(NULL,rha->hpi_input_streams[0],&in_state,
				   &in_buffer_size,&in_data_len,&in_frame_len,
				   &in_aux_len));
    for(i=0;i<inputs;i++) {
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
    for(i=0;i<outputs;i++) {
      if(rb[i]->readSpace()<frames_avail) {
	frames_avail=rb[i]->readSpace();
      }
    }
    //    printf("frames_avail: %u\n",frames_avail);
    if(frames_avail>0) {
      for(i=0;i<outputs;i++) {
	if(xpoints[i]<0) {
	  memset(pcm,0,262144);
	}
	else {
	  rb[xpoints[i]]->peek((float *)pcm,frames_avail);
	}
	HpiError(HPI_OutStreamWriteBuf(NULL,rha->hpi_output_streams[i],pcm,
				       frames_avail,rha->hpi_format));
	if(out_state==HPI_STATE_STOPPED) {
	  HpiError(HPI_OutStreamStart(NULL,rha->hpi_output_streams[i]));
	}
      }
      for(i=0;i<inputs;i++) {
	rb[i]->read((float *)pcm,frames_avail);
      }
    }
    usleep(AUDIO_HPI_POLLING_INTERVAL);
  }

  return NULL;
}




RouterHpiAudio::RouterHpiAudio(Config *c,QObject *parent)
  : Router(c,parent)
{
  hpi_format=new struct hpi_format;
  short on_gain[HPI_MAX_CHANNELS]={0,0};
  short off_gain[HPI_MAX_CHANNELS]={-10000,-10000};
  uint16_t ostreams;
  uint16_t istreams;
  uint16_t version;
  uint32_t serial;
  uint16_t type;
  uint32_t bufsize;

  //
  // Get Adapter Info
  //
  HpiError(HPI_AdapterGetInfo(NULL,0,&ostreams,&istreams,&version,&serial,
			      &type));
  HpiError(HPI_FormatCreate(hpi_format,2,HPI_FORMAT_PCM32_FLOAT,
			    AUDIO_SAMPLE_RATE,0,0));
  HpiError(HPI_StreamEstimateBufferSize(hpi_format,AUDIO_HPI_POLLING_INTERVAL,
					&bufsize));
  printf("SIZE: %u\n",bufsize);

  //
  // Initialize Mixer
  //
  HpiError(HPI_MixerOpen(NULL,0,&hpi_mixer));
  for(int i=0;i<ostreams;i++) {
    for(int j=0;j<config()->outputQuantity();j++) {
      HpiError(HPI_MixerGetControl(NULL,hpi_mixer,
				   HPI_SOURCENODE_OSTREAM,i,
				   HPI_DESTNODE_LINEOUT,j,
				   HPI_CONTROL_VOLUME,
				   &hpi_output_volumes[i][j]));
      HpiError(HPI_VolumeSetGain(NULL,hpi_output_volumes[i][j],off_gain));
    }
  }
  for(int i=0;i<config()->outputQuantity();i++) {
    HpiError(HPI_VolumeSetGain(NULL,hpi_output_volumes[i][i],on_gain));
  }

  //
  // Open Inputs
  //
  for(int i=0;i<config()->inputQuantity();i++) {
    HpiError(HPI_InStreamOpen(NULL,0,i,&hpi_input_streams[i]));
    HpiError(HPI_InStreamSetFormat(NULL,hpi_input_streams[i],hpi_format));
    if(HpiError(HPI_InStreamHostBufferAllocate(NULL,hpi_input_streams[i],
				 bufsize))==HPI_ERROR_INVALID_DATASIZE) {
      syslog(LOG_DEBUG,"unable to enable bus mastering for input stream %d",i);
    }
    HpiError(HPI_InStreamStart(NULL,hpi_input_streams[i]));
  }

  //
  // Open Outputs
  //
  for(int i=0;i<config()->outputQuantity();i++) {
    HpiError(HPI_OutStreamOpen(NULL,0,i,&hpi_output_streams[i]));
    HpiError(HPI_OutStreamSetFormat(NULL,hpi_output_streams[i],hpi_format));
    if(HpiError(HPI_OutStreamHostBufferAllocate(NULL,hpi_output_streams[i],
				 bufsize))==HPI_ERROR_INVALID_DATASIZE) {
      syslog(LOG_DEBUG,"unable to enable bus mastering for output stream %d",i);
    }
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


bool RouterHpiAudio::EnableInputBusMastering(int input,uint32_t bufsize)
{
  hpi_err_t err;

  if((err=HPI_InStreamHostBufferAllocate(NULL,hpi_input_streams[input],
					 bufsize))==HPI_ERROR_INVALID_DATASIZE) {
    return false;
  }
  printf("INPUT ERR: %d\n",err);
  HpiError(err);
  return true;
}


bool RouterHpiAudio::EnableOutputBusMastering(int output,uint32_t bufsize)
{
  hpi_err_t err;

  if((err=HPI_OutStreamHostBufferAllocate(NULL,hpi_output_streams[output],
					  bufsize))==HPI_ERROR_INVALID_DATASIZE) {
    return false;
  }
  printf("OUTPUT ERR: %d\n",err);
  HpiError(err);
  return true;
}
