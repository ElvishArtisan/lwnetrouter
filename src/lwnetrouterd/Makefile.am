## automake.am
##
## Makefile for the lwnetrouterd(8) daemon
##
## (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License as
##   published by the Free Software Foundation; either version 2 of
##   the License, or (at your option) any later version.
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing @QT4CLI_CFLAGS@ @SWITCHYARD_CFLAGS@
MOC = @QT4_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@


sbin_PROGRAMS = lwnetrouterd

dist_lwnetrouterd_SOURCES = delaycontrol.cpp delaycontrol.h\
                            lwnetrouterd.cpp lwnetrouterd.h\
                            protocol.cpp protocol.h\
                            protocol_cunc.cpp protocol_cunc.h\
                            protocol_gpio.cpp protocol_gpio.h\
                            protocol_rml.cpp protocol_rml.h\
                            protocol_sap.cpp protocol_sap.h\
                            ringbuffer.cpp ringbuffer.h\
                            router_breakaway.cpp router_breakaway.h\
                            router.cpp router.h\
                            router_cic.cpp router_cic.h\
                            router_gpio.cpp router_gpio.h\
                            router_hpiaudio.cpp router_hpiaudio.h\
                            state.cpp state.h\
                            streamcmdserver.cpp streamcmdserver.h\
                            ttydevice.cpp ttydevice.h

nodist_lwnetrouterd_SOURCES = config.cpp config.h\
                              moc_delaycontrol.cpp\
                              moc_lwnetrouterd.cpp\
                              moc_protocol.cpp\
                              moc_protocol_cunc.cpp\
                              moc_protocol_gpio.cpp\
                              moc_protocol_rml.cpp\
                              moc_protocol_sap.cpp\
                              moc_router.cpp\
                              moc_router_breakaway.cpp\
                              moc_router_cic.cpp\
                              moc_router_gpio.cpp\
                              moc_router_hpiaudio.cpp\
                              moc_state.cpp\
                              moc_streamcmdserver.cpp\
                              moc_ttydevice.cpp

lwnetrouterd_LDADD = @QT4CLI_LIBS@ @SWITCHYARD_LIBS@ -lhpi -lpthread

CLEANFILES = *~\
             moc_*\
             *.obj\
             *.idb\
             *.pdb\
             *ilk

DISTCLEANFILES = config.cpp config.h

MAINTAINERCLEANFILES = *~\
                       Makefile.in
