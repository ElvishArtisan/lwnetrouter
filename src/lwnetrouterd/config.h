// config.h
//
// Configuration File Accessor for lwnetrouter
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

#ifndef CONFIG_H
#define CONFIG_H

#define MAX_INPUTS 8
#define MAX_OUTPUTS 8
#define MAX_DELAY 10
#define AUDIO_CHANNELS 2
#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_HPI_POLLING_INTERVAL 50
#define CONFIG_CONF_FILE "/etc/lwnetrouter.conf"

//
// Default Values
//
#define CONFIG_DEFAULT_INPUT_QUANTITY 8
#define CONFIG_DEFAULT_OUTPUT_QUANTITY 8
#define CONFIG_DEFAULT_AUDIO_ALSA_DEVICE "plughw:Axia"
#define CONFIG_DEFAULT_AUDIO_PERIOD_QUANTITY 4
#define CONFIG_DEFAULT_AUDIO_BUFFER_SIZE 22000

#include <QString>

class Config
{
 public:
  Config();
  int inputQuantity() const;
  int outputQuantity() const;
  QString audioAlsaDevice(int subdev=-1) const;
  int audioPeriodQuantity() const;
  int audioBufferSize() const;

 private:
  int conf_input_quantity;
  int conf_output_quantity;
  QString conf_audio_alsa_device;
  int conf_audio_period_quantity;
  int conf_audio_buffer_size;
};


#endif  // CONFIG_H
