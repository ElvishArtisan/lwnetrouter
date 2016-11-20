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

#include "ringbuffer.h"
#include "router.h"

class RouterHpiAudio : public Router
{
  Q_OBJECT;
 public:
  RouterHpiAudio(Config *c,QObject *parent=0);

 protected:
  void crossPointSet(int output,int input);

 private:
  struct hpi_format *hpi_format;
  hpi_handle_t hpi_input_streams[MAX_INPUTS];
  hpi_handle_t hpi_output_streams[MAX_OUTPUTS];
  hpi_handle_t hpi_mixer;
  hpi_handle_t hpi_output_volumes[HPI_MAX_STREAMS][MAX_OUTPUTS];
  pthread_t hpi_pthread;
  friend void *__AudioCallback(void *ptr);
};


#endif  // ROUTER_HPIAUDIO_H
