## automake.am
##
## icons/automake.am for lwnetrouter
##
## (C) Copyright 2016 Fred Gleason <fredg@salemradiolabs.com>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License version 2 as
##   published by the Free Software Foundation.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
##
## Use automake to process this into a Makefile.in

install-exec-am:	
	mkdir -p $(DESTDIR)@prefix@/share/icons/48x48/apps
	cp lwnetdelay-48x48.png $(DESTDIR)@prefix@/share/icons/48x48/apps/lwnetdelay

uninstall-local:	
	rm -f $(DESTDIR)@prefix@/share/icons/48x48/apps/lwnetdelay

EXTRA_DIST = lwnetdelay-16x16.xpm\
             lwnetdelay-48x48.png\
             lwnetdelay-48x48.xpm

CLEANFILES = *~
MAINTAINERCLEANFILES = *~ Makefile.in configure aclocal.m4
