// streamcmdserver.cpp
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

#include <vector>

#include "streamcmdserver.h"

StreamCmdConnection::StreamCmdConnection(QTcpSocket *sock)
{
  conn_socket=sock;
  conn_hostname=sock->peerName();
  conn_port=sock->peerPort();
  conn_persistent=false;
}


StreamCmdConnection::StreamCmdConnection(const QString &hostname,uint16_t port)
{
  conn_socket=NULL;
  conn_hostname=hostname;
  conn_port=port;
  conn_persistent=true;
}


StreamCmdConnection::~StreamCmdConnection()
{
  delete conn_socket;
}


QTcpSocket *StreamCmdConnection::socket()
{
  return conn_socket;
}


bool StreamCmdConnection::isConnected() const
{
  if(conn_socket==NULL) {
    return false;
  }
  return conn_socket->state()==QAbstractSocket::ConnectedState;
}


bool StreamCmdConnection::isPersistent() const
{
  return conn_persistent;
}


QString StreamCmdConnection::hostname() const
{
  return conn_hostname;
}


uint16_t StreamCmdConnection::port() const
{
  return conn_port;
}


void StreamCmdConnection::reconnect(QTcpSocket *sock)
{
  conn_socket=sock;
  conn_hostname=sock->peerName();
  conn_port=sock->peerPort();
}


void StreamCmdConnection::deleteLater()
{
  if(conn_socket!=NULL) {
    conn_socket->close();
  }
}




StreamCmdServer::StreamCmdServer(const std::map<int,QString> &cmd_table,
				 const std::map<int,int> &upper_table,
				 const std::map<int,int> &lower_table,
				 QTcpServer *server,QObject *parent)
  : QObject(parent)
{
  //
  // Tables
  //
  cmd_cmd_table=cmd_table;
  cmd_upper_table=upper_table;
  cmd_lower_table=lower_table;

  //
  // TCP Server
  //
  cmd_server=server;
  connect(cmd_server,SIGNAL(newConnection()),this,SLOT(newConnectionData()));

  //
  // Mappers
  //
  cmd_read_mapper=new QSignalMapper(this);
  connect(cmd_read_mapper,SIGNAL(mapped(int)),this,SLOT(readyReadData(int)));

  cmd_closed_mapper=new QSignalMapper(this);
  connect(cmd_closed_mapper,SIGNAL(mapped(int)),
	  this,SLOT(connectionClosedData(int)));

  cmd_pending_connected_mapper=new QSignalMapper(this);
  connect(cmd_pending_connected_mapper,SIGNAL(mapped(int)),
	  this,SLOT(pendingConnectedData(int)));

  //
  // Timer
  //
  cmd_garbage_timer=new QTimer(this);
  cmd_garbage_timer->setSingleShot(true);
  connect(cmd_garbage_timer,SIGNAL(timeout()),this,SLOT(collectGarbageData()));

  cmd_reconnect_timer=new QTimer(this);
  cmd_reconnect_timer->setSingleShot(true);
  connect(cmd_reconnect_timer,SIGNAL(timeout()),this,SLOT(reconnectData()));
}


StreamCmdServer::~StreamCmdServer()
{
  delete cmd_garbage_timer;
  delete cmd_read_mapper;
  delete cmd_closed_mapper;
  delete cmd_server;
}


QHostAddress StreamCmdServer::localAddress(int id) const
{
  return cmd_connections.at(id)->socket()->localAddress();
}


uint16_t StreamCmdServer::localPort(int id) const
{
  return cmd_connections.at(id)->socket()->localPort();
}


QHostAddress StreamCmdServer::peerAddress(int id) const
{
  return cmd_connections.at(id)->socket()->peerAddress();
}


uint16_t StreamCmdServer::peerPort(int id) const
{
  return cmd_connections.at(id)->socket()->peerPort();
}


void StreamCmdServer::sendCommand(int id,int cmd,const QStringList &args)
{
  QString str=cmd_cmd_table[cmd];
  for(int i=0;i<args.size();i++) {
    str+=QString(" ")+args[i];
  }
  str+="\r\n";
  cmd_connections.at(id)->socket()->write(str.toAscii(),str.length());
}


void StreamCmdServer::sendCommand(int cmd,const QStringList &args)
{
  StreamCmdConnection *conn;

  for(unsigned i=0;i<cmd_connections.size();i++) {
    if((conn=cmd_connections.at(i))!=NULL) {
      if(conn->isConnected()) {
	sendCommand(i,cmd,args);
      }
    }
    else {
      closeConnection(i);
    }
  }
}


void StreamCmdServer::connectToHost(const QString &hostname,uint16_t port)
{
  int new_id=-1;

  new_id=GetFreePendingSlot();
  cmd_pending_sockets[new_id]=new QTcpSocket(this);
  cmd_pending_connection_ids[new_id]=GetFreeConnectionSlot();
  cmd_connections[cmd_pending_connection_ids[new_id]]=
    new StreamCmdConnection(hostname,port);
  connect(cmd_pending_sockets[new_id],SIGNAL(connected()),
	  cmd_pending_connected_mapper,SLOT(map()));
  connect(cmd_pending_sockets[new_id],
	  SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(pendingErrorData(QAbstractSocket::SocketError)));
  cmd_pending_connected_mapper->setMapping(cmd_pending_sockets[new_id],new_id);
  cmd_pending_sockets[new_id]->connectToHost(hostname,port);
}


void StreamCmdServer::closeConnection(int id)
{
  cmd_connections.at(id)->deleteLater();
  cmd_garbage_timer->start(1);
}


void StreamCmdServer::newConnectionData()
{
  QTcpSocket *sock=cmd_server->nextPendingConnection();
  int id=GetFreeConnectionSlot();

  //
  // Create Connection
  //
  cmd_connections[id]=new StreamCmdConnection(sock);
  cmd_read_mapper->setMapping(sock,id);
  connect(sock,SIGNAL(readyRead()),cmd_read_mapper,SLOT(map()));
  cmd_closed_mapper->setMapping(sock,id);
  connect(sock,SIGNAL(disconnected()),cmd_closed_mapper,SLOT(map()));
  emit connected(id);
}


void StreamCmdServer::readyReadData(int id)
{
  int n=-1;
  char data[1500];

  n=cmd_connections.at(id)->socket()->read(data,1500);
  for(int i=0;i<n;i++) {
    if(data[i]==13) {
      ProcessCommand(id);
      cmd_connections.at(id)->buffer="";
    }
    else {
      if(data[i]!=10) {
	cmd_connections.at(id)->buffer+=data[i];
      }
    }
  }
}


void StreamCmdServer::connectionClosedData(int id)
{
  cmd_garbage_timer->start(5000);
  emit disconnected(id);
}


void StreamCmdServer::collectGarbageData()
{
  for(unsigned i=0;i<cmd_connections.size();i++) {
    if(cmd_connections.at(i)!=NULL) {
      if(!cmd_connections.at(i)->isConnected()) {
	if(cmd_connections.at(i)->isPersistent()) {
	  int pending_id=GetFreePendingSlot();
	  cmd_pending_sockets[pending_id]=new QTcpSocket(this);
	  cmd_pending_connection_ids[pending_id]=i;
	  connect(cmd_pending_sockets[pending_id],SIGNAL(connected()),
		  cmd_pending_connected_mapper,SLOT(map()));
	  connect(cmd_pending_sockets[pending_id],
		  SIGNAL(error(QAbstractSocket::SocketError)),
		  this,SLOT(pendingErrorData(QAbstractSocket::SocketError)));
	  cmd_pending_connected_mapper->
	    setMapping(cmd_pending_sockets[pending_id],pending_id);
	  cmd_pending_sockets[pending_id]->
	    connectToHost(cmd_connections[i]->hostname(),
			  cmd_connections[i]->port());
	}
	else {
	  delete cmd_connections[i];
	  cmd_connections[i]=NULL;
	}
      }
    }
  }
}


void StreamCmdServer::pendingConnectedData(int pending_id)
{
  int conn_id=cmd_pending_connection_ids[pending_id];
  QTcpSocket *sock=cmd_pending_sockets[pending_id];

  
  cmd_pending_connected_mapper->removeMappings(sock);
  sock->disconnect();
  cmd_read_mapper->setMapping(sock,conn_id);
  connect(sock,SIGNAL(readyRead()),cmd_read_mapper,SLOT(map()));
  cmd_closed_mapper->setMapping(sock,conn_id);
  connect(sock,SIGNAL(disconnected()),cmd_closed_mapper,SLOT(map()));
  cmd_connections[cmd_pending_connection_ids[pending_id]]->reconnect(sock);
  cmd_pending_sockets[pending_id]=NULL;
  cmd_pending_connection_ids[pending_id]=-1;
  emit connected(conn_id);
}


void StreamCmdServer::pendingErrorData(QAbstractSocket::SocketError err)
{
  cmd_reconnect_timer->start(5000);
}


void StreamCmdServer::reconnectData()
{
  for(unsigned i=0;i<cmd_pending_sockets.size();i++) {
    if(cmd_pending_sockets[i]->state()==QAbstractSocket::UnconnectedState) {
      cmd_pending_connected_mapper->removeMappings(cmd_pending_sockets[i]);
      cmd_pending_sockets[i]->disconnect();
      cmd_pending_sockets[i]->deleteLater();
      cmd_pending_sockets[i]=new QTcpSocket(this);
      int conn_id=cmd_pending_connection_ids[i];
      connect(cmd_pending_sockets[i],SIGNAL(connected()),
	      cmd_pending_connected_mapper,SLOT(map()));
      connect(cmd_pending_sockets[i],
	      SIGNAL(error(QAbstractSocket::SocketError)),
	      this,SLOT(pendingErrorData(QAbstractSocket::SocketError)));
      cmd_pending_connected_mapper->
	setMapping(cmd_pending_sockets[i],i);

      cmd_pending_sockets[i]->
	connectToHost(cmd_connections[conn_id]->hostname(),
		      cmd_connections[conn_id]->port());
    }
  }
}


int StreamCmdServer::GetFreeConnectionSlot()
{
  int id=-1;
  for(unsigned i=0;i<cmd_connections.size();i++) {
    if(cmd_connections.at(i)==NULL) {
      id=i;
      break;
    }
  }
  if(id<0) {
    id=cmd_connections.size();
    cmd_connections.push_back(NULL);
  }

  return id;
}


int StreamCmdServer::GetFreePendingSlot()
{
  int id=-1;
  for(unsigned i=0;i<cmd_pending_sockets.size();i++) {
    if(cmd_pending_sockets.at(i)==NULL) {
      id=i;
      break;
    }
  }
  if(id<0) {
    id=cmd_pending_sockets.size();
    cmd_pending_sockets.push_back(NULL);
    cmd_pending_connection_ids.push_back(-1);
  }

  return id;
}


void StreamCmdServer::ProcessCommand(int id)
{
  QStringList cmds=cmd_connections.at(id)->buffer.split(" ");
  for(unsigned i=0;i<cmd_cmd_table.size();i++) {
    if(cmd_cmd_table[i]==cmds[0]) {
      if(((cmd_upper_table[i]<0)||((cmds.size()-1)<=cmd_upper_table[i]))&&
	 ((cmd_lower_table[i]<0)||((cmds.size()-1)>=cmd_lower_table[i]))) {
	QStringList args;
	for(int j=1;j<cmds.size();j++) {
	  args.push_back(cmds[j]);
	}
	emit commandReceived(id,i,args);
      }
    }
  }
}
