// audio_delay_test.cpp
//
// Test delay intervals of LiveWire Audio
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

#include <stdint.h>
#include <pthread.h>

#include <QCoreApplication>

#include <sy/sycmdswitch.h>

#include "audio_delay_test.h"

bool test_started=false;
bool test_finished=false;

void *__AlsaCaptureCallback(void *priv)
{
  static MainObject *mo=(MainObject *)priv;
  static int32_t pcm[8192];
  static snd_pcm_sframes_t n;
  static int i=0;

  while(!test_finished) {
    n=snd_pcm_readi(mo->test_capture_pcm,pcm,1024);
    if(n>=0) {
      for(i=0;i<(2*n);i++) {
	if(pcm[i]>=0x0000FFFF) {
	  gettimeofday(&mo->test_recv_tv,NULL);
	  test_finished=true;
	  break;
	}
      }
    }
    else {
      fprintf(stderr,"*** Capture XRUN ***\n");
    }
  }

  return NULL;
}


void *__AlsaPlaybackCallback(void *priv)
{
  static MainObject *mo=(MainObject *)priv;
  static int32_t silence[8192];
  static int32_t impulse[8192];
  static unsigned count=0;

  memset(silence,0,8192*sizeof(int32_t));
  memset(impulse,0,8192*sizeof(int32_t));
  impulse[0]=0x00FFFFFF;
  impulse[1]=0x00FFFFFF;
  while(!test_finished) {
    if(count++==10) {
      if(snd_pcm_writei(mo->test_playback_pcm,impulse,4096)>=0) {
	gettimeofday(&mo->test_send_tv,NULL);
      }
      else {
	fprintf(stderr,"*** Playback XRUN ***\n");
      }
      test_started=true;
    }
    else {
      snd_pcm_writei(mo->test_playback_pcm,silence,4096);
    }
  }

  return NULL;
}




MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  test_alsa_device="hw:0";

  SyCmdSwitch *cmd=new SyCmdSwitch(qApp->argc(),qApp->argv(),"audio_delay_test",
				   VERSION,AUDIO_DELAY_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--alsa-device") {
      test_alsa_device=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"gpio_delay_test: unknown option: %s\n",
	      (const char *)cmd->key(i).toUtf8());
      exit(256);
    }
  }

  test_capture_pcm=NULL;
  test_playback_pcm=NULL;
  memset(&test_send_tv,0,sizeof(test_send_tv));
  memset(&test_recv_tv,0,sizeof(test_recv_tv));
  test_finished=false;

  test_scan_timer=new QTimer(this);
  connect(test_scan_timer,SIGNAL(timeout()),this,SLOT(scanData()));
  test_scan_timer->start(200);

  QString err_msg;
  if(!StartCapture(&err_msg)) {
    fprintf(stderr,"audio_delay_test: unable to start audio capture [%s]\n",
	    (const char *)err_msg.toUtf8());
    exit(256);
  }
  if(!StartPlayback(&err_msg)) {
    fprintf(stderr,"audio_delay_test: unable to start audio playback [%s]\n",
	    (const char *)err_msg.toUtf8());
    exit(256);
  }
}


void MainObject::scanData()
{
  if(test_started) {
    printf("Sent audio impulse to ALSA device %s\n",
	   (const char *)test_alsa_device.toUtf8());
    test_started=false;
  }

  if(test_finished) {
    printf("Received audio impulse from ALSA device %s: Propagation delay was %9.6lf seconds\n",
	   (const char *)test_alsa_device.toUtf8(),
	   TimeValue(&test_recv_tv)-TimeValue(&test_send_tv));

    pthread_join(test_playback_pthread,NULL);
    snd_pcm_close(test_playback_pcm);

    pthread_join(test_capture_pthread,NULL);
    snd_pcm_close(test_capture_pcm);

    exit(0);
  }
}


bool MainObject::StartCapture(QString *err_msg)
{
  snd_pcm_hw_params_t *hwparams;
  snd_pcm_sw_params_t *swparams;
  int dir;
  int aerr;
  pthread_attr_t pthread_attr;
  unsigned samprate=48000;
  unsigned periods=4;
  snd_pcm_uframes_t buffer_size;

  if(snd_pcm_open(&test_capture_pcm,test_alsa_device.toUtf8(),
		  SND_PCM_STREAM_CAPTURE,0)!=0) {
    *err_msg=tr("unable to open ALSA device")+" \""+test_alsa_device+"\"";
    return false;
  }
  snd_pcm_hw_params_alloca(&hwparams);
  snd_pcm_hw_params_any(test_capture_pcm,hwparams);

  //
  // Access Type
  //
  if(snd_pcm_hw_params_test_access(test_capture_pcm,hwparams,
				   SND_PCM_ACCESS_RW_INTERLEAVED)<0) {
    *err_msg=tr("interleaved access not supported");
    return false;
  }
  snd_pcm_hw_params_set_access(test_capture_pcm,hwparams,
			       SND_PCM_ACCESS_RW_INTERLEAVED);

  //
  // Sample Format
  //
  if(snd_pcm_hw_params_set_format(test_capture_pcm,hwparams,
				  SND_PCM_FORMAT_S32_LE)!=0) {
    *err_msg=tr("unsupported sample format (S32_LE)");
    return false;
  }

  //
  // Sample Rate
  //
  snd_pcm_hw_params_set_rate_near(test_capture_pcm,hwparams,&samprate,&dir);
  if(samprate!=48000) {
    fprintf(stderr,"WARNING: using sample rate of %u\n",samprate);
  }

  //
  // Channels
  //
  if(snd_pcm_hw_params_set_channels(test_capture_pcm,hwparams,2)!=0) {
    *err_msg=QObject::tr("unsupported channel count");
    return false;
  }

  //
  // Buffer Parameters
  //
  snd_pcm_hw_params_set_periods_near(test_capture_pcm,hwparams,&periods,&dir);
  if(periods!=4) {
    fprintf(stderr,"WARNING: using period quantity of %u\n",periods);
  }
  buffer_size=samprate/2;
  snd_pcm_hw_params_set_buffer_size_near(test_capture_pcm,hwparams,
					 &buffer_size);

  //
  // Fire It Up
  //
  if((aerr=snd_pcm_hw_params(test_capture_pcm,hwparams))<0) {
    *err_msg=tr("ALSA device error 1")+": "+snd_strerror(aerr);
    return false;
  }

  //
  // Set Wake-up Timing
  //
  snd_pcm_sw_params_alloca(&swparams);
  snd_pcm_sw_params_current(test_capture_pcm,swparams);
  snd_pcm_sw_params_set_avail_min(test_capture_pcm,swparams,buffer_size/2);
  if((aerr=snd_pcm_sw_params(test_capture_pcm,swparams))<0) {
    *err_msg=tr("ALSA device error 2")+": "+snd_strerror(aerr);
    return false;
  }

  //
  // Start the Callback
  //
  pthread_attr_init(&pthread_attr);
  pthread_attr_setschedpolicy(&pthread_attr,SCHED_FIFO);
  pthread_create(&test_capture_pthread,&pthread_attr,__AlsaCaptureCallback,this);

  return true;
}


bool MainObject::StartPlayback(QString *err_msg)
{
  snd_pcm_hw_params_t *hwparams;
  snd_pcm_sw_params_t *swparams;
  int dir;
  int aerr;
  pthread_attr_t pthread_attr;
  unsigned samprate=48000;
  unsigned periods=4;
  snd_pcm_uframes_t buffer_size;

  if(snd_pcm_open(&test_playback_pcm,test_alsa_device.toUtf8(),
		  SND_PCM_STREAM_PLAYBACK,0)!=0) {
    *err_msg=tr("unable to open ALSA device")+" \""+test_alsa_device+"\"";
    return false;
  }
  snd_pcm_hw_params_alloca(&hwparams);
  snd_pcm_hw_params_any(test_playback_pcm,hwparams);

  //
  // Access Type
  //
  if(snd_pcm_hw_params_test_access(test_playback_pcm,hwparams,
				   SND_PCM_ACCESS_RW_INTERLEAVED)<0) {
    *err_msg=tr("interleaved access not supported");
    return false;
  }
  snd_pcm_hw_params_set_access(test_playback_pcm,hwparams,
			       SND_PCM_ACCESS_RW_INTERLEAVED);

  //
  // Sample Format
  //
  if(snd_pcm_hw_params_set_format(test_playback_pcm,hwparams,
				  SND_PCM_FORMAT_S32_LE)!=0) {
    *err_msg=tr("unsupported sample format (S32_LE)");
    return false;
  }

  //
  // Sample Rate
  //
  snd_pcm_hw_params_set_rate_near(test_playback_pcm,hwparams,&samprate,&dir);
  if(samprate!=48000) {
    fprintf(stderr,"WARNING: using sample rate of %u\n",samprate);
  }

  //
  // Channels
  //
  if(snd_pcm_hw_params_set_channels(test_playback_pcm,hwparams,2)!=0) {
    *err_msg=QObject::tr("unsupported channel count");
    return false;
  }

  //
  // Buffer Parameters
  //
  snd_pcm_hw_params_set_periods_near(test_playback_pcm,hwparams,&periods,&dir);
  if(periods!=4) {
    fprintf(stderr,"WARNING: using period quantity of %u\n",periods);
  }
  buffer_size=samprate/2;
  snd_pcm_hw_params_set_buffer_size_near(test_playback_pcm,hwparams,
					 &buffer_size);

  //
  // Fire It Up
  //
  if((aerr=snd_pcm_hw_params(test_playback_pcm,hwparams))<0) {
    *err_msg=tr("ALSA device error 1")+": "+snd_strerror(aerr);
    return false;
  }

  //
  // Set Wake-up Timing
  //
  snd_pcm_sw_params_alloca(&swparams);
  snd_pcm_sw_params_current(test_playback_pcm,swparams);
  snd_pcm_sw_params_set_avail_min(test_playback_pcm,swparams,buffer_size/2);
  if((aerr=snd_pcm_sw_params(test_playback_pcm,swparams))<0) {
    *err_msg=tr("ALSA device error 2")+": "+snd_strerror(aerr);
    return false;
  }

  //
  // Start the Callback
  //
  pthread_attr_init(&pthread_attr);
  pthread_attr_setschedpolicy(&pthread_attr,SCHED_FIFO);
  pthread_create(&test_playback_pthread,&pthread_attr,__AlsaPlaybackCallback,this);

  return true;
}


double MainObject::TimeValue(struct timeval *tv)
{
  return (double)tv->tv_sec+(double)tv->tv_usec/1000000.0;
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();

  return a.exec();
}
