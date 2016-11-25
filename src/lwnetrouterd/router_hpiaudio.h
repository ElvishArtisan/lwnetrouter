// router_hpiaudio.h
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

#ifndef ROUTER_HPIAUDIO_H
#define ROUTER_HPIAUDIO_H

#include <pthread.h>

#include <asihpi/hpi.h>

#include <QTimer>

#include "ringbuffer.h"
#include "router.h"

class RouterHpiAudio : public Router
{
  Q_OBJECT;
 public:
  RouterHpiAudio(Config *c,QObject *parent=0);
  Config::DelayState delayState(int input) const;
  int delayInterval(int input);

 public slots:
  void setDelayState(int input,Config::DelayState state);
  void dumpDelay(int input);

 private slots:
  void scanTimerData();

 private:
  struct hpi_format *hpi_format;
  hpi_handle_t hpi_input_streams[MAX_INPUTS];
  hpi_handle_t hpi_output_streams[MAX_OUTPUTS];
  hpi_handle_t hpi_mixer;
  hpi_handle_t hpi_output_volumes[HPI_MAX_STREAMS][MAX_OUTPUTS];
  pthread_t hpi_pthread;
  Config::DelayState hpi_delay_state[MAX_INPUTS];
  uint32_t hpi_delay_interval[MAX_INPUTS];
  QTimer *hpi_scan_timer;
  friend void *__AudioCallback(void *ptr);

  //
  // Class -> Callback
  //
  Config::DelayState delay_state_set[MAX_INPUTS];
  bool delay_dump[MAX_INPUTS];
  float delay_change_down;
  float delay_change_up;

  //
  // Callback -> Class
  //
  Config::DelayState delay_state_taken[MAX_INPUTS];
  uint32_t delay_interval[MAX_INPUTS];
};


#endif  // ROUTER_HPIAUDIO_H
