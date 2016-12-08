## automake.am
##
## Automake.am for src/tests
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing @QT4CLI_CFLAGS@ @SWITCHYARD_CFLAGS@ @ALSA_CFLAGS@
MOC = @QT4_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

noinst_PROGRAMS = audio_delay_test\
                  gpio_delay_test

dist_audio_delay_test_SOURCES = audio_delay_test.cpp audio_delay_test.h
nodist_audio_delay_test_SOURCES = moc_audio_delay_test.cpp
audio_delay_test_LDADD = @QT4CLI_LIBS@ @SWITCHYARD_LIBS@ @ALSA_LIBS@ -lpthread

dist_gpio_delay_test_SOURCES = gpio_delay_test.cpp gpio_delay_test.h
nodist_gpio_delay_test_SOURCES = moc_gpio_delay_test.cpp
gpio_delay_test_LDADD = @QT4CLI_LIBS@ @SWITCHYARD_LIBS@

CLEANFILES = *~ moc_* *.qm *.obj *.idb *.pdb *ilk

DISTCLEANFILES = config.cpp config.h

MAINTAINERCLEANFILES = *~ Makefile.in configure aclocal.m4 *.tar.gz moc_*
