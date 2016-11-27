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
  QHostAddress addr;
  int count=0;
  bool ok=true;

  p->setSource(CONFIG_CONF_FILE);

  //
  // [Global] Section
  //
  conf_input_quantity=
    p->intValue("Global","InputQuantity",CONFIG_DEFAULT_INPUT_QUANTITY);
  conf_output_quantity=
    p->intValue("Global","OutputQuantity",CONFIG_DEFAULT_OUTPUT_QUANTITY);
  conf_rml_port=p->intValue("Global","RmlPort",CONFIG_DEFAULT_RML_PORT);
  conf_cunctator_port=
    p->intValue("Global","CunctatorPort",CONFIG_DEFAULT_CUNCTATOR_PORT);
  conf_cic_port=
    p->intValue("Global","CicPort",CONFIG_DEFAULT_CIC_PORT);
  while(ok) {
    addr=p->addressValue("Global",QString().sprintf("CicIpAddress%d",
						   count+1),"",&ok);
    if(ok) {
      conf_cic_addresses.push_back(addr);
    }
    count++;
  }
  conf_netcue_port=
    p->stringValue("Global","NetcuePort",CONFIG_DEFAULT_NETCUE_PORT);

  //
  // [Audio] Section
  //
  conf_audio_adapter_ip_address=p->addressValue("Audio","AdapterIpAddress","");
  conf_audio_input_bus_xfers=p->
    boolValue("Audio","InputBusXfers",CONFIG_DEFAULT_AUDIO_INPUT_BUS_XFERS);
  conf_audio_output_bus_xfers=p->
    boolValue("Audio","OutputBusXfers",CONFIG_DEFAULT_AUDIO_OUTPUT_BUS_XFERS);

  //
  // [Input<n>] Sections
  //
  for(int i=0;i<MAX_INPUTS;i++) {
    count=0;
    ok=true;
    QString section=QString().sprintf("Input%d",i+1);
    conf_input_delay_change_percent[i]=
      p->intValue(section,"DelayChangePercent",
		  CONFIG_DEFAULT_INPUT_DELAY_CHANGE_PERCENT);
    conf_input_delay_control_sources[i]=
      p->intValue(section,"DelayControlSource");
    while(ok) {
      addr=p->addressValue(section,QString().sprintf("SourceIpAddress%d",
						     count+1),"",&ok);
      if(ok) {
	conf_input_addresses[i].push_back(addr);
      }
      count++;
    }
  }

  //
  // [Output<n>] Sections
  //
  for(int i=0;i<MAX_OUTPUTS;i++) {
    QString section=QString().sprintf("Output%d",i+1);
    for(int j=0;j<SWITCHYARD_GPIO_BUNDLE_SIZE;j++) {
      conf_output_netcues[i][j]=
	p->stringValue(section,QString().sprintf("Netcue%d",j+1));
    }
  }

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


uint16_t Config::cicPort() const
{
  return conf_cic_port;
}


QList<QHostAddress> Config::cicIpAddresses()
{
  return conf_cic_addresses;
}


QString Config::netcuePort() const
{
  return conf_netcue_port;
}


QHostAddress Config::audioAdapterIpAddress() const
{
  return conf_audio_adapter_ip_address;
}


bool Config::audioInputBusXfers() const
{
  return conf_audio_input_bus_xfers;
}


bool Config::audioOutputBusXfers() const
{
  return conf_audio_output_bus_xfers;
}


int Config::inputDelayChangePercent(int input) const
{
  return conf_input_delay_change_percent[input];
}


int Config::inputDelayControlSource(int input) const
{
  return conf_input_delay_control_sources[input];
}


int Config::input(const QHostAddress &addr)
{
  for(int i=0;i<inputQuantity();i++) {
    if(conf_input_addresses[i].contains(addr)) {
      return i;
    }
  }
  return -1;
}


QString Config::outputNetcue(int output,int line) const
{
  return conf_output_netcues[output][line];
}

