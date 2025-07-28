dnl configure.ac
dnl
dnl Autoconf configuration
dnl
dnl Use autoconf to process this into a configure script
dnl
dnl   (C) Copyright 2016-2025 Fred Gleason <fredg@paravelsystems.com>
dnl
dnl   This program is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License version 2 as
dnl   published by the Free Software Foundation.
dnl
dnl   This program is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details.
dnl
dnl   You should have received a copy of the GNU General Public
dnl   License along with this program; if not, write to the Free Software
dnl   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
dnl

AC_INIT(lwnetrouter,m4_esyscmd(cat PACKAGE_VERSION))
AM_INIT_AUTOMAKE
AC_SUBST(RPM_RELEASE,1)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

#
# Basic Compiler Checks
#
AC_PROG_CXX
AC_LANG(C++)

#
# Option Switches
#
AC_ARG_ENABLE(docbook,[  --disable-docbook       disable building of documentation],[DOCBOOK_DISABLED=yes],[])

#
# Check for Qt6 (Mandatory)
#
AQ_FIND_QT6(QT6,Core Gui Widgets Network,[],[AC_MSG_ERROR([*** Qt6 not found ***])])

#
# Check for Switchyard6
#
PKG_CHECK_MODULES(SWITCHYARD6,[switchyard6],[],[AC_MSG_ERROR([*** Switchyard not found ***])])

#
# Check for AudioScience HPI
#
AC_CHECK_HEADER(asihpi/hpi.h,[],[AC_MSG_ERROR([*** AudioScience HPI not found ***])])

#
# Check for ALSA
#
PKG_CHECK_MODULES(ALSA,alsa,[],[AC_MSG_ERROR([*** ALSA not found ***])])

#
# Check for DocBook Toolchain
#
if test -z $DOCBOOK_DISABLED ; then
  USING_DOCBOOK=yes
fi
AM_CONDITIONAL([DOCBOOK_AM], [test "$USING_DOCBOOK" = yes])

#
# Determine the target architecture
#
AR_GCC_TARGET()
AC_SUBST(VENDOR,$ar_gcc_distro)
AC_SUBST(ARCH,$ar_gcc_arch)

#
# Determine Distro
#
AR_GET_DISTRO()
AC_SUBST(DISTRO,$ar_gcc_distro)

#
# Configure RPM Build
#
AC_CHECK_PROG(RPMBUILD_FOUND,rpmbuild,[yes],[])
if test -z $RPMBUILD_FOUND ; then
  AC_SUBST(RPMBUILD,rpm)
else
  AC_SUBST(RPMBUILD,rpmbuild)
fi
if test -d /usr/src/redhat ; then
  AC_SUBST(RPM_ROOT,/usr/src/redhat)
  AC_SUBST(VENDOR,redhat)
else
  AC_SUBST(RPM_ROOT,/usr/src/packages)
  AC_SUBST(VENDOR,suse)
fi 

AC_CONFIG_FILES([ \
    conf/Makefile \
    debian/source/Makefile \
    debian/Makefile \
    docs/Makefile \
    icons/Makefile \
    src/Makefile \
    src/common/Makefile \
    src/lwnetdelay/Makefile \
    src/lwnetrouterd/Makefile \
    src/tests/Makefile \
    systemd/lwnetrouter.service \
    systemd/Makefile \
    build_debs.sh \
    lwnetrouter.spec \
    Makefile ])
AC_OUTPUT()
chmod 755 build_debs.sh

#
# Link Common Elements
#
./link_common.sh lwnetdelay
./link_common.sh lwnetrouterd
./link_common.sh tests

AC_MSG_NOTICE()
AC_MSG_NOTICE(Now enter 'make' to build the software.)
AC_MSG_NOTICE()
