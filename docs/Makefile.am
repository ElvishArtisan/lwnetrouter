## automake.am
##
## docs/Makefile.am for lwnetrouter
##
## (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

##
## Build Dependencies
##
%.html:	%.xml
	xsltproc -o $@ $(DOCBOOK_STYLESHEETS)/xhtml/docbook.xsl $<

%.pdf:	%.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/fo/docbook.xsl $< | fop - -pdf $@

%.1:	%.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/manpages/docbook.xsl $<

%.8:	%.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/manpages/docbook.xsl $<

#all-local:	lwckmap.8\
#                lwckmap.xml\
#                lwmap.8\
#                lwmap.xml\
#                lwpanel.1\
#                lwpanel.xml\
#                lwxypanel.1\
#                lwxypanel.xml\
#		lwpathd.8\
#                lwpathd.xml\
#                protocol.pdf\
#                protocol.xml

#man_MANS = lwckmap.8\
#           lwmap.8\
#           lwpanel.1\
#           lwxypanel.1\
#           lwpathd.8

#EXTRA_DIST = lwckmap.8\
#             lwckmap.xml\
#             lwmap.8\
#             lwmap.xml\
#             lwpanel.1\
#             lwpanel.xml\
#             lwxypanel.1\
#             lwxypanel.xml\
#             lwpathd.8\
#             lwpathd.xml\
#             protocol.pdf\
#             protocol.xml

CLEANFILES = *~
MAINTAINERCLEANFILES = *~\
                       *.1\
                       *.8\
                       *.html\
                       *.pdf\
                       aclocal.m4\
                       configure\
                       Makefile.in
