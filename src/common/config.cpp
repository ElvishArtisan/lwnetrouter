// config.cpp
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

#include <stdio.h>

#include <sy/syprofile.h>

#include "config.h"

Config::Config()
{
  SyProfile *p=new SyProfile();

  p->setSource(CONFIG_CONF_FILE);
  conf_input_quantity=
    p->intValue("Global","InputQuantity",CONFIG_DEFAULT_INPUT_QUANTITY);
  conf_output_quantity=
    p->intValue("Global","OutputQuantity",CONFIG_DEFAULT_OUTPUT_QUANTITY);
  conf_rml_port=p->intValue("Global","RmlPort",CONFIG_DEFAULT_RML_PORT);
  conf_cunctator_port=
    p->intValue("Global","CunctatorPort",CONFIG_DEFAULT_CUNCTATOR_PORT);
  conf_audio_delay_change_percent=
    p->intValue("Audio","DelayChangePercent",
		CONFIG_DEFAULT_AUDIO_DELAY_CHANGE_PERCENT);
  conf_audio_input_bus_xfers=p->
    boolValue("Audio","InputBusXfers",CONFIG_DEFAULT_AUDIO_INPUT_BUS_XFERS);
  conf_audio_output_bus_xfers=p->
    boolValue("Audio","OutputBusXfers",CONFIG_DEFAULT_AUDIO_OUTPUT_BUS_XFERS);

  delete p;
}


int Config::inputQuantity() const
{
  return conf_input_quantity;
}


int Config::outputQuantity() const
{
  return conf_output_quantity;
}


uint16_t Config::rmlPort() const
{
  return conf_rml_port;
}


uint16_t Config::cunctatorPort() const
{
  return conf_cunctator_port;
}


int Config::audioDelayChangePercent() const
{
  return conf_audio_delay_change_percent;
}


bool Config::audioInputBusXfers() const
{
  return conf_audio_input_bus_xfers;
}


bool Config::audioOutputBusXfers() const
{
  return conf_audio_output_bus_xfers;
}
