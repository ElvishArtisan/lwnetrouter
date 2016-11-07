// protocol_rml.h
//
// RML protocol for lwnetrouterd(8)
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

#ifndef PROTOCOL_RML_H
#define PROTOCOL_RML_H

#include <QUdpSocket>

#include "protocol.h"

class ProtocolRml : public Protocol
{
  Q_OBJECT;
 public:
  ProtocolRml(Config *c,QObject *parent=0);

 private slots:
  void readyReadData();

 private:
  void ProcessCommand(const QString &cmd);
  QString rml_accum;
  QUdpSocket *rml_socket;
};


#endif  // PROTOCOL_H
