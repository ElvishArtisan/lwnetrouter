## Makefile.am
##
## Top level Makefile.am
##
## Use automake to process this into a Makefile.in
##
##   (C) Copyright 2016-2025 Fred Gleason <fredg@paravelsystems.com>
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

if DOCBOOK_AM
  DOCBOOK_AM_OPT = docs
endif

SUBDIRS = $(DOCBOOK_AM_OPT) conf\
          debian\
          src\
          systemd

deb:	dist
	./build_debs.sh

rpm:	dist
	cp lwnetrouter-$(VERSION).tar.gz $(HOME)/rpmbuild/SOURCES/
	$(RPMBUILD) -ba --target $(ARCH)-$(VENDOR)-linux lwnetrouter.spec
	mv $(HOME)/rpmbuild/RPMS/$(ARCH)/lwnetrouter-*.rpm $(top_srcdir)/
	mv $(HOME)/rpmbuild/SRPMS/lwnetrouter-*.src.rpm $(top_srcdir)/
	rm $(HOME)/rpmbuild/SOURCES/lwnetrouter-$(VERSION).tar.gz
	rm -rf $(HOME)/rpmbuild/BUILD/lwnetrouter-$(VERSION)

EXTRA_DIST = autogen.sh\
             get_distro.pl\
             get_target.sh\
             link_common.sh\
             lwnetrouter.spec.in\
             PACKAGE_VERSION

CLEANFILES = *~\
             *.buildinfo\
             *.changes\
             *.ddeb\
             *.deb\
             *.dsc\
             *.mo\
             *.rpm\
             *.tar.gz\
             *.tar.xz\
             *.tgz\
             *.zip

DISTCLEANFILES = build_debs.sh\
                 config.guess\
                 config.sub\
                 ltmain.sh\
                 lwnetrouter.service\
                 lwnetrouter.spec

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       compile\
                       config.guess\
                       config.status\
                       config.sub\
                       configure\
                       depcomp\
                       install-sh\
                       libtool\
                       ltmain.sh\
                       Makefile.in\
                       missing\
                       mkinstalldirs
