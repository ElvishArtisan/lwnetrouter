// lwnetdelay.cpp
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

#include <stdlib.h>
#include <ctype.h>

#include <QApplication>
#include <QMessageBox>
#include <QPixmap>

#include <sy/sycmdswitch.h>

#include "config.h"
#include "lwnetdelay.h"

//
// Icons
//
#include "../../icons/lwnetdelay-16x16.xpm"

MainWidget::MainWidget(QWidget *parent)
  : QMainWindow(parent)
{
QString hostname="localhost";
  unsigned port=3749;
  bool ok;
  cunc_delay_id=0;
  cunc_safe_delay=1000*CONFIG_DEFAULT_INPUT_FULL_DELAY;

  //
  // Read Command Options
  //
  SyCmdSwitch *cmd=
    new SyCmdSwitch(qApp->argc(),qApp->argv(),"cunc",VERSION,LWNETDELAY_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--hostname") {
      hostname=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--port") {
      port=cmd->value(i).toUInt(&ok);
      if((!ok)||(port>0xFFFF)) {
	fprintf(stderr,"cunc: invalid port value\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--delay") {
      cunc_delay_id=cmd->value(i).toUInt(&ok);
      if(!ok) {
	fprintf(stderr,"cunc: invalid delay number\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"cuncd: unrecognized option \"%s\"\n",
		(const char *)cmd->key(i).toUtf8());
      exit(256);
    }
  }
  delete cmd;

  //
  // Set Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Generate Fonts
  //
  QFont button_font("helvetica",14,QFont::Bold);
  button_font.setPixelSize(14);
  QFont label_font("courier",40,QFont::Bold);
  label_font.setPixelSize(40);

  setWindowTitle(tr("Delay Control"));

  //
  // Create Icons
  //
  setWindowIcon(QPixmap(lwnetdelay_16x16_xpm));

  //
  // Enter Button
  //
  cunc_enter_button=new PushButton(tr("Start"),this);
  cunc_enter_button->setFont(button_font);
  connect(cunc_enter_button,SIGNAL(clicked()),this,SLOT(enterPushed()));

  //
  // Exit Button
  //
  cunc_exit_button=new PushButton(tr("Exit"),this);
  cunc_exit_button->setFont(button_font);
  connect(cunc_exit_button,SIGNAL(clicked()),this,SLOT(exitPushed()));

  //
  // Delay Time Display
  //
  cunc_delay_label=new QLabel(this);
  cunc_delay_label->setFont(label_font);
  cunc_delay_label->setAlignment(Qt::AlignCenter);
  cunc_delay_label->setFrameStyle(QFrame::Box|QFrame::Raised);

  //
  // Dump Button
  //
  cunc_dump_button=new PushButton(tr("Dump"),this);
  cunc_dump_button->setFont(button_font);
  cunc_dump_button->setFlashColor(Qt::darkGreen);
  connect(cunc_dump_button,SIGNAL(clicked()),this,SLOT(dumpPushed()));

  cunc_dump_timer=new QTimer(this);
  cunc_dump_timer->setSingleShot(true);
  connect(cunc_dump_timer,SIGNAL(timeout()),this,SLOT(dumpFlashResetData()));

  //
  // Data Socket
  //
  cunc_socket=new QTcpSocket(this);
  connect(cunc_socket,SIGNAL(connected()),this,SLOT(socketConnectedData()));
  connect(cunc_socket,SIGNAL(disconnected()),
	  this,SLOT(socketDisconnectedData()));
  connect(cunc_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(cunc_socket,SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(errorData(QAbstractSocket::SocketError)));
  cunc_socket->connectToHost(hostname,port);
}


QSize MainWidget::sizeHint() const
{
  return QSize(430,70);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::enterPushed()
{
  SendCommand(QString().sprintf("SS %u %u",cunc_delay_id,
				Config::DelayEntering));
}


void MainWidget::exitPushed()
{
  SendCommand(QString().sprintf("SS %u %u",cunc_delay_id,
				Config::DelayExiting));
}


void MainWidget::dumpPushed()
{
  SendCommand(QString().sprintf("DP %u",cunc_delay_id));
}


void MainWidget::dumpFlashResetData()
{
  cunc_dump_button->setButtonMode(PushButton::ButtonOff);
}


void MainWidget::socketConnectedData()
{
  SendCommand(QString().sprintf("DM %u",cunc_delay_id));
  SendCommand(QString().sprintf("DS %u",cunc_delay_id));
}


void MainWidget::socketDisconnectedData()
{
  QMessageBox::warning(this,tr("Broadcast Delay Control"),
		       tr("Remote system closed connection!"));
  exit(256);
}


void MainWidget::readyReadData()
{
  int n;
  char data[1500];

  while((n=cunc_socket->read(data,1500))>0) {
    for(int i=0;i<n;i++) {
      if(isprint(data[i])) {
	cunc_buffer+=data[i];
      }
      else {
	if(data[i]==13) {
	  ProcessCommand(cunc_buffer);
	  cunc_buffer="";
	}
      }
    }
  }
}


void MainWidget::errorData(QAbstractSocket::SocketError err)
{
  QString msg=tr("Received network socket error")+
    QString().sprintf(" %d.",err);

  switch(err) {
  case QAbstractSocket::ConnectionRefusedError:
    msg=tr("Remote connection refused.");
    break;

  case QAbstractSocket::RemoteHostClosedError:
    socketDisconnectedData();
    break;

  case QAbstractSocket::HostNotFoundError:
    msg=tr("Remote host not found.");
    break;

  default:
    break;
  }
  QMessageBox::critical(this,"LwNetDelay",msg);

  exit(256);
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  cunc_dump_button->setGeometry(10,10,80,50);
  cunc_exit_button->setGeometry(100,10,80,50);
  cunc_enter_button->setGeometry(190,10,80,50);
  cunc_delay_label->setGeometry(280,10,120,50);
}


void MainWidget::ProcessCommand(const QString &msg)
{
  //printf("msg: %s\n",(const char *)msg);

  QStringList cmds=msg.split(" ");
  bool ok;
  unsigned id;
  int delay_len;
  QString desc;

  if(cmds.size()<1) {
    return;
  }
  if(cmds[0]=="DS") {   // Delay State
    id=cmds[1].toUInt(&ok);
    if((!ok)||(id!=cunc_delay_id)) {
      return;
    }

    //
    // Update Buttons
    //
    
    switch((Config::DelayState)cmds[2].toInt()) {
    case Config::DelayEntering:
      cunc_enter_button->setButtonMode(PushButton::ButtonSlowFlash);
      cunc_exit_button->setButtonMode(PushButton::ButtonOff);
      break;

    case Config::DelayEntered:
      cunc_enter_button->setButtonMode(PushButton::ButtonOn);
      cunc_exit_button->setButtonMode(PushButton::ButtonOff);
      break;

    case Config::DelayExiting:
      cunc_enter_button->setButtonMode(PushButton::ButtonOff);
      cunc_exit_button->setButtonMode(PushButton::ButtonSlowFlash);
      break;

    case Config::DelayBypassed:
    case Config::DelayExited:
      cunc_enter_button->setButtonMode(PushButton::ButtonOff);
      cunc_exit_button->setButtonMode(PushButton::ButtonOn);
      break;

    case Config::DelayUnknown:
      break;
    }

    //
    // Update Delay Time Counter
    //
    delay_len=cmds[3].toInt(&ok);
    if(!ok) {
      return;
    }
    cunc_delay_label->
      setText(QString().sprintf("%4.1f",(float)delay_len/1000.0));
  
    if(delay_len>=cunc_safe_delay) {
      cunc_dump_button->setButtonMode(PushButton::ButtonOn);
    }
    else {
      cunc_dump_button->setButtonMode(PushButton::ButtonOff);
    }
  }


  if(cmds[0]=="DP") {   // Delay Dump
    id=cmds[1].toUInt(&ok);
    if((!ok)||(id!=cunc_delay_id)) {
      return;
    }
    cunc_dump_button->setButtonMode(PushButton::ButtonFastFlash);
    cunc_dump_timer->start(5000);
  }

  if(cmds[0]=="DM") {   // Input Name
    id=cmds[1].toUInt(&ok);
    if((!ok)||(id!=cunc_delay_id)) {
      return;
    }
    delay_len=cmds[2].toInt(&ok);
    if(!ok) {
      return;
    }
    cunc_safe_delay=1000*delay_len;
    for(int i=3;i<cmds.size();i++) {
      desc+=(cmds[i]+" ");
    }
    if(desc.isEmpty()) {
      setWindowTitle(tr("Delay Control"));
    }
    else {
      setWindowTitle(tr("Delay Control")+": "+desc);
    }
  }
}


void MainWidget::SendCommand(const QString &msg)
{
  cunc_socket->write((msg+"\r\n").toAscii());
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  MainWidget *w=new MainWidget();
  w->show();
  return a.exec();
}
