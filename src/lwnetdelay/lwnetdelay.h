// lwnetdelay.h
//
// Utility for controlling broadcast delays.
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
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

#ifndef LWNETDELAY_H
#define LWNETDELAY_H

#include <QLabel>
#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>

#include "pushbutton.h"

#define LWNETDELAY_USAGE "[--hostname=<hostname>] [--port=<port>] [--delay=<delay-num>]\n\nWhere <hostname> is the name or IP address of the host to connect to\n(default = localhost), <port> is the TCP port to connect to\n(default = 3749)\n"
#define CUNC_DEFAULT_ADDR "localhost"

class MainWidget : public QMainWindow
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void enterPushed();
  void exitPushed();
  void dumpPushed();
  void dumpFlashResetData();
  void socketConnectedData();
  void socketDisconnectedData();
  void readyReadData();
  void errorData(QAbstractSocket::SocketError err);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void ProcessCommand(const QString &msg);
  void SendCommand(const QString &msg);
  PushButton *cunc_enter_button;
  PushButton *cunc_exit_button;
  PushButton *cunc_dump_button;
  QLabel *cunc_delay_label;
  QTcpSocket *cunc_socket;
  QString cunc_buffer;
  unsigned cunc_delay_id;
  QTimer *cunc_dump_timer;
};


#endif  // LWNETDELAY_H
