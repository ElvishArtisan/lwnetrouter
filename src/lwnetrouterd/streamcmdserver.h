// streamcmdserver.h
//
// Parse commands on connection-oriented protocols.
//
//   (C) Copyright 2012,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef STREAMCMDSERVER_H
#define STREAMCMDSERVER_H

#include <stdint.h>

#include <map>
#include <vector>

#include <QObject>
#include <QSignalMapper>
#include <QStringList>
#include <QTimer>
#include <QTcpSocket>
#include <QTcpServer>

class StreamCmdConnection
{
 public:
  StreamCmdConnection(QTcpSocket *sock);
  StreamCmdConnection(const QString &hostname,uint16_t port);
  ~StreamCmdConnection();
  QTcpSocket *socket();
  QByteArray buffer;
  bool isConnected() const;
  bool isPersistent() const;
  QString hostname() const;
  uint16_t port() const;
  void reconnect(QTcpSocket *sock);
  void deleteLater();

 private:
  QTcpSocket *conn_socket;
  QString conn_hostname;
  uint16_t conn_port;
  bool conn_persistent;
};



class StreamCmdServer : public QObject
{
 Q_OBJECT;
 public:
  StreamCmdServer(const std::map<int,QString> &cmd_table,
		  const std::map<int,int> &upper_table,
		  const std::map<int,int> &lower_table,
		  QTcpServer *server,QObject *parent);
  ~StreamCmdServer();
  char delimiter() const;
  void setDelimiter(char c);
  QHostAddress localAddress(int id) const;
  uint16_t localPort(int id) const;
  QHostAddress peerAddress(int id) const;
  uint16_t peerPort(int id) const;

 public slots:
  void sendCommand(int id,int cmd,const QStringList &args=QStringList());
  void sendCommand(int id,const QString &str);
  void sendCommand(int cmd,const QStringList &args=QStringList());
  void sendCommand(const QString &str);
  void connectToHost(const QString &hostname,uint16_t port);
  void closeConnection(int id);

 signals:
  void commandReceived(int id,int cmd,const QStringList &args);
  void connected(int id);
  void disconnected(int id);

 private slots:
  void newConnectionData();
  void readyReadData(int id);
  void connectionClosedData(int id);
  void collectGarbageData();
  void pendingConnectedData(int pending_id);
  void pendingErrorData(QAbstractSocket::SocketError err);
  void reconnectData();

 private:
  int GetFreeConnectionSlot();
  int GetFreePendingSlot();
  void ProcessCommand(int id);
  QTcpServer *cmd_server;
  std::vector<QTcpSocket *> cmd_pending_sockets;
  std::vector<int> cmd_pending_connection_ids;
  QSignalMapper *cmd_read_mapper;
  QSignalMapper *cmd_closed_mapper;
  QSignalMapper *cmd_pending_connected_mapper;
  QTimer *cmd_garbage_timer;
  QTimer *cmd_reconnect_timer;
  std::vector<StreamCmdConnection *> cmd_connections;
  std::map<int,QString> cmd_cmd_table;
  std::map<int,int> cmd_upper_table;
  std::map<int,int> cmd_lower_table;
  char cmd_delimiter;
};


#endif  // STREAMCMDSERVER_H
