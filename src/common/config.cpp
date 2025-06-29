// config.cpp
//
// Configuration File Accessor for lwnetrouter
//
//   (C) Copyright 2016-2025 Fred Gleason <fredg@paravelsystems.com>
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

#include <sy6/syprofile.h>

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
  conf_software_authority_port=
    p->intValue("Global","SoftwareAutorityPort",
		CONFIG_DEFAULT_SOFTWARE_AUTHORITY_PORT);
  conf_cic_port=
    p->intValue("Global","CicPort",CONFIG_DEFAULT_CIC_PORT);
  ok=true;
  count=0;
  while(ok) {
    addr=p->addressValue("Global",QString::asprintf("CicIpAddress%d",
						   count+1),"",&ok);
    if(ok) {
      conf_cic_addresses.push_back(addr);
    }
    count++;
  }

  ok=true;
  count=0;
  while(ok) {
    addr=p->addressValue("Global",QString::asprintf("NetcueUdpAddress%d",
						   count+1),"",&ok);
    if(ok) {
      conf_netcue_udp_addresses.push_back(addr);
    }
    count++;
  }
  conf_netcue_udp_port=
    p->intValue("Global","NetcueUdpPort",CONFIG_DEFAULT_NETCUE_UDP_PORT);
  conf_netcue_udp_repeat=
    p->intValue("Global","NetcueUdpRepeat",CONFIG_DEFAULT_NETCUE_UDP_REPEAT);



  conf_netcue_port=
    p->stringValue("Global","NetcuePort",CONFIG_DEFAULT_NETCUE_PORT);
  conf_forward_netcues_via_livewire=
    p->boolValue("Global","ForwardNetcuesViaLivewire",
		 CONFIG_DEFAULT_FORWARD_NETCUES_VIA_LIVEWIRE);
  conf_livewire_ip_address=p->addressValue("Global","LivewireIpAddress","");
  conf_adapter_ip_address=p->addressValue("Global","AdapterIpAddress","");
  conf_relay_debounce_interval=
    p->intValue("Global","RelayDebounceInterval",
		CONFIG_DEFAULT_RELAY_DEBOUNCE_INTERVAL);
  conf_lwrp_password=p->stringValue("Global","LwrpPassword","");
  conf_input_bus_xfers=p->
    boolValue("Global","InputBusXfers",CONFIG_DEFAULT_INPUT_BUS_XFERS);
  conf_output_bus_xfers=p->
    boolValue("Global","OutputBusXfers",CONFIG_DEFAULT_OUTPUT_BUS_XFERS);

  //
  // [Input<n>] Sections
  //
  for(int i=0;i<MAX_INPUTS;i++) {
    count=0;
    ok=true;
    QString section=QString::asprintf("Input%d",i+1);
    conf_input_full_delays[i]=
      p->intValue(section,"FullDelay",CONFIG_DEFAULT_INPUT_FULL_DELAY);
    conf_input_dump_delays[i]=
      p->intValue(section,"DumpDelay",conf_input_full_delays[i]);
    conf_input_delay_change_percent[i]=
      p->intValue(section,"DelayChangePercent",
		  CONFIG_DEFAULT_INPUT_DELAY_CHANGE_PERCENT);
    conf_input_delay_control_sources[i]=
      p->intValue(section,"DelayControlSource");
    while(ok) {
      addr=p->addressValue(section,QString::asprintf("SourceIpAddress%d",
						     count+1),"",&ok);
      for(int j=0;j<=i;j++) {   // Check for duplicates
	if(conf_input_addresses[j].contains(addr)) {
	  fprintf(stderr,"lwnetrouterd: duplicate SourceIpAddress entry \"%s\" in [Input%d] and [Input%d]\n",
		  (const char *)addr.toString().toUtf8(),j+1,i+1);
	  exit(256);
	}
      }
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
    QString section=QString::asprintf("Output%d",i+1);
    conf_output_cic_program_codes[i]=p->stringValue(section,"CicProgramCode");
    for(int j=0;j<SWITCHYARD_GPIO_BUNDLE_SIZE;j++) {
      conf_output_breakaway_ip_addresses[i]=
	p->addressValue(section,"BreakawayIpAddress","");
      conf_output_breakaway_slot_numbers[i]=
	p->intValue(section,"BreakawaySlotNumber",i+1);
      conf_output_netcues[i][j]=
	p->stringValue(section,QString::asprintf("Netcue%d",j+1));
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


uint16_t Config::softwareAuthorityPort() const
{
  return conf_software_authority_port;
}


uint16_t Config::cicPort() const
{
  return conf_cic_port;
}


QList<QHostAddress> Config::cicIpAddresses()
{
  return conf_cic_addresses;
}


QList<QHostAddress> Config::netcueUdpAddresses() const
{
  return conf_netcue_udp_addresses;
}


uint16_t Config::netcueUdpPort() const
{
  return conf_netcue_udp_port;
}


int Config::netcueUdpRepeat() const
{
  return conf_netcue_udp_repeat;
}


QString Config::netcuePort() const
{
  return conf_netcue_port;
}


bool Config::forwardNetcuesViaLivewire() const
{
  return conf_forward_netcues_via_livewire;
}


QHostAddress Config::livewireIpAddress() const
{
  return conf_livewire_ip_address;
}


QHostAddress Config::adapterIpAddress() const
{
  return conf_adapter_ip_address;
}


int Config::relayDebounceInterval() const
{
  return conf_relay_debounce_interval;
}


QString Config::lwrpPassword() const
{
  return conf_lwrp_password;
}


bool Config::inputBusXfers() const
{
  return conf_input_bus_xfers;
}


bool Config::outputBusXfers() const
{
  return conf_output_bus_xfers;
}


int Config::inputFullDelay(int input) const
{
  return conf_input_full_delays[input];
}


int Config::inputDumpDelay(int input) const
{
  return conf_input_dump_delays[input];
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


QString Config::outputCicProgramCode(int output) const
{
  return conf_output_cic_program_codes[output];
}


QHostAddress Config::outputBreakawayIpAddress(int output) const
{
  return conf_output_breakaway_ip_addresses[output];
}


int Config::outputBreakawaySlotNumber(int output) const
{
  return conf_output_breakaway_slot_numbers[output];
}


QString Config::outputNetcue(int output,int line) const
{
  return conf_output_netcues[output][line];
}


QString Config::delayStateText(DelayState state)
{
  QString ret=
    QObject::tr("Unknown delay state")+QString::asprintf(" [%d]",state);

  switch(state) {
  case Config::DelayUnknown:
    ret=QObject::tr("unknown");
    break;

  case Config::DelayBypassed:
    ret=QObject::tr("bypassed");
    break;

  case Config::DelayEntered:
    ret=QObject::tr("entered");
    break;

  case Config::DelayEntering:
    ret=QObject::tr("entering");
    break;

  case Config::DelayExited:
    ret=QObject::tr("exited");
    break;

  case Config::DelayExiting:
    ret=QObject::tr("exiting");
    break;
  }

  return ret;
}
