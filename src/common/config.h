// config.h
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

#ifndef CONFIG_H
#define CONFIG_H

#define MAX_INPUTS 8
#define MAX_OUTPUTS 8
#define MAX_DELAY 60
#define AUDIO_CHANNELS 2
#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_HPI_POLLING_INTERVAL 10000
#define CONFIG_CONF_FILE "/etc/lwnetrouter.conf"
#define NETCUE_TTY_SPEED 9600
#define NETCUE_TTY_DATA_BITS 8
#define NETCUE_TTY_DATA_PARITY TTYDevice::None
#define NETCUE_TTY_FLOW_CONTROL TTYDevice::FlowNone

//
// Default Values
//
#define CONFIG_DEFAULT_INPUT_QUANTITY 8
#define CONFIG_DEFAULT_OUTPUT_QUANTITY 8
#define CONFIG_DEFAULT_RML_PORT 5858
#define CONFIG_DEFAULT_CUNCTATOR_PORT 3749
#define CONFIG_DEFAULT_SOFTWARE_AUTHORITY_PORT 9500
#define CONFIG_DEFAULT_CIC_PORT 5001
#define CONFIG_DEFAULT_NETCUE_UDP_PORT 7001
#define CONFIG_DEFAULT_NETCUE_UDP_REPEAT 1
#define CONFIG_DEFAULT_NETCUE_PORT QString("/dev/ttyS0")
#define CONFIG_DEFAULT_FORWARD_NETCUES_VIA_LIVEWIRE true
#define CONFIG_DEFAULT_RELAY_DEBOUNCE_INTERVAL 500
#define CONFIG_DEFAULT_INPUT_BUS_XFERS false
#define CONFIG_DEFAULT_OUTPUT_BUS_XFERS true
#define CONFIG_DEFAULT_INPUT_FULL_DELAY 10
#define CONFIG_DEFAULT_INPUT_DELAY_CHANGE_PERCENT 5

#include <stdint.h>

#include <QHostAddress>
#include <QList>
#include <QString>

#include <sy6/syconfig.h>

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
  uint16_t softwareAuthorityPort() const;
  uint16_t cicPort() const;
  QList<QHostAddress> cicIpAddresses();
  QList<QHostAddress> netcueUdpAddresses() const;
  uint16_t netcueUdpPort() const;
  int netcueUdpRepeat() const;
  QString netcuePort() const;
  bool forwardNetcuesViaLivewire() const;
  QHostAddress livewireIpAddress() const;
  QHostAddress adapterIpAddress() const;
  int relayDebounceInterval() const;
  QString lwrpPassword() const;
  bool inputBusXfers() const;
  bool outputBusXfers() const;
  int inputFullDelay(int input) const;
  int inputDumpDelay(int input) const;
  int inputDelayChangePercent(int input) const;
  int inputDelayControlSource(int input) const;
  int input(const QHostAddress &addr);
  QString outputCicProgramCode(int output) const;
  QHostAddress outputBreakawayIpAddress(int output) const;
  int outputBreakawaySlotNumber(int output) const;
  QString outputNetcue(int output,int line) const;
  static QString delayStateText(DelayState state);

 private:
  int conf_input_quantity;
  int conf_output_quantity;
  uint16_t conf_rml_port;
  uint16_t conf_cunctator_port;
  uint16_t conf_software_authority_port;
  uint16_t conf_cic_port;
  QList<QHostAddress> conf_cic_addresses;
  QString conf_netcue_port;
  QList<QHostAddress> conf_netcue_udp_addresses;
  uint16_t conf_netcue_udp_port;
  int conf_netcue_udp_repeat;
  bool conf_forward_netcues_via_livewire;
  QHostAddress conf_livewire_ip_address;
  QHostAddress conf_adapter_ip_address;
  int conf_relay_debounce_interval;
  QString conf_lwrp_password;
  bool conf_input_bus_xfers;
  bool conf_output_bus_xfers;
  int conf_input_full_delays[MAX_INPUTS];
  int conf_input_dump_delays[MAX_INPUTS];
  int conf_input_delay_change_percent[MAX_INPUTS];
  int conf_input_delay_control_sources[MAX_INPUTS];
  QList<QHostAddress> conf_input_addresses[MAX_INPUTS];
  QString conf_output_cic_program_codes[MAX_INPUTS];
  QHostAddress conf_output_breakaway_ip_addresses[MAX_OUTPUTS];
  int conf_output_breakaway_slot_numbers[MAX_OUTPUTS];
  QString conf_output_netcues[MAX_OUTPUTS][SWITCHYARD_GPIO_BUNDLE_SIZE];
};


#endif  // CONFIG_H
