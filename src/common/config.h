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
#define MAX_DELAY 1
#define AUDIO_CHANNELS 2
#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_HPI_POLLING_INTERVAL 10000
#define CONFIG_CONF_FILE "/etc/lwnetrouter.conf"

//
// Default Values
//
#define CONFIG_DEFAULT_INPUT_QUANTITY 8
#define CONFIG_DEFAULT_OUTPUT_QUANTITY 8
#define CONFIG_DEFAULT_RML_PORT 5858
#define CONFIG_DEFAULT_CUNCTATOR_PORT 3749
#define CONFIG_DEFAULT_AUDIO_DELAY_CHANGE_PERCENT 5
#define CONFIG_DEFAULT_AUDIO_INPUT_BUS_XFERS false
#define CONFIG_DEFAULT_AUDIO_OUTPUT_BUS_XFERS true

#include <stdint.h>

#include <QString>

class Config
{
 public:
  enum DelayState {DelayUnknown=0,DelayBypassed=1,DelayEntered=2,
		   DelayEntering=3,DelayExited=4,DelayExiting=5};
  Config();
  int inputQuantity() const;
  int outputQuantity() const;
  uint16_t rmlPort() const;
  uint16_t cunctatorPort() const;
  int audioDelayChangePercent() const;
  bool audioInputBusXfers() const;
  bool audioOutputBusXfers() const;
  int inputDelayControlSource(int input) const;

 private:
  int conf_input_quantity;
  int conf_output_quantity;
  uint16_t conf_rml_port;
  uint16_t conf_cunctator_port;
  int conf_audio_delay_change_percent;
  bool conf_audio_input_bus_xfers;
  bool conf_audio_output_bus_xfers;
  int conf_input_delay_control_sources[MAX_INPUTS];
};


#endif  // CONFIG_H
