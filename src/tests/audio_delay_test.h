// audio_delay_test.h
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

#ifndef AUDIO_DELAY_TEST_H
#define AUDIO_DELAY_TEST_H

#include <sys/time.h>

#include <alsa/asoundlib.h>

#include <QObject>
#include <QTimer>

#define AUDIO_DELAY_TEST_USAGE "--alsa-device=<dev-name>\n"

class MainObject : public QObject
{
 Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void scanData();

 private:
  bool StartCapture(QString *err_msg);
  bool StartPlayback(QString *err_msg);
  double TimeValue(struct timeval *tv);
  QString test_alsa_device;
  snd_pcm_t *test_capture_pcm;
  snd_pcm_t *test_playback_pcm;
  struct timeval test_send_tv;
  struct timeval test_recv_tv;
  pthread_t test_capture_pthread;
  pthread_t test_playback_pthread;
  QTimer *test_scan_timer;
  friend void *__AlsaCaptureCallback(void *priv);
  friend void *__AlsaPlaybackCallback(void *priv);
};


#endif  // AUDIO_DELAY_TEST_H
