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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing @QT4CLI_CFLAGS@ @SWITCHYARD_CFLAGS@ @ALSA_CFLAGS@ @SRC_CFLAGS@ @PTHREADS_CFLAGS@
MOC = @QT4_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	@MOC@ $< -o $@


sbin_PROGRAMS = lwnetrouterd

dist_lwnetrouterd_SOURCES = audiodestination.cpp audiodestination.h\
                            audioendpoint.cpp audioendpoint.h\
                            audiosource.cpp audiosource.h\
                            config.cpp config.h\
                            lwnetrouterd.cpp lwnetrouterd.h\
                            protocol.cpp protocol.h\
                            protocol_rml.cpp protocol_rml.h\
                            ringbuffer.cpp ringbuffer.h\
                            router.cpp router.h\
                            router_audio.cpp router_audio.h\
                            router_hpiaudio.cpp router_hpiaudio.h

nodist_lwnetrouterd_SOURCES = moc_lwnetrouterd.cpp\
                              moc_protocol.cpp\
                              moc_protocol_rml.cpp\
                              moc_router.cpp\
                              moc_router_audio.cpp\
                              moc_router_hpiaudio.cpp

lwnetrouterd_LDADD = @QT4CLI_LIBS@ @SWITCHYARD_LIBS@ @ALSA_LIBS@ @PTHREADS_LIBS@ @SRC_LIBS@ -lhpi -lpthread

CLEANFILES = *~\
             moc_*\
             *.obj\
             *.idb\
             *.pdb\
             *ilk
MAINTAINERCLEANFILES = *~\
                       Makefile.in
