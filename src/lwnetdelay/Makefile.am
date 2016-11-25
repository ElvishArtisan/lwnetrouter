## automake.am
##
## Automake.am for src/lwnetdelay
##
##   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing @QT4GUI_CFLAGS@ @SWITCHYARD_CFLAGS@
MOC = @QT4_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

bin_PROGRAMS = lwnetdelay

dist_lwnetdelay_SOURCES = lwnetdelay.cpp lwnetdelay.h\
                          pushbutton.cpp pushbutton.h

nodist_lwnetdelay_SOURCES = config.cpp config.h\
                            moc_lwnetdelay.cpp\
                            moc_pushbutton.cpp

lwnetdelay_LDADD = @QT4GUI_LIBS@ @SWITCHYARD_LIBS@

CLEANFILES = *~ moc_* *.qm *.obj *.idb *.pdb *ilk

DISTCLEANFILES = config.cpp config.h

MAINTAINERCLEANFILES = *~ Makefile.in configure aclocal.m4 *.tar.gz moc_*
