// lwnetrouterd.cpp
//
// lwnetrouterd(8) routing daemon
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

#include <signal.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>

#include <QCoreApplication>

#include <sy/sycmdswitch.h>

#include "lwnetrouterd.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  SyCmdSwitch *cmd=
    new SyCmdSwitch(qApp->argc(),qApp->argv(),"lwnetrouterd",VERSION,LWNETROUTERD_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(!cmd->processed(i)) {
      fprintf(stderr,"lwnetrouterd: unknown option\n");
      exit(256);
    }
  }

  main_config=new Config();

  //
  // Open the Syslog
  //
  openlog("lwnetrouterd",LOG_PERROR,LOG_DAEMON);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
