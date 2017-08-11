dnl * Copyright 2012-2017 Luke Dashjr
dnl *
dnl * This program is free software; you can redistribute it and/or modify it
dnl * under the terms of the standard MIT license.  See COPYING for more details.

AC_INIT(
	[jsmn],
	[0.1.0],
	[zaitsev.serge@gmail.com],
	[jsmn])
AC_CONFIG_AUX_DIR([.])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.11 -Wall dist-xz foreign subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC_C99
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
LT_INIT([])

# http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
AC_SUBST([LIBJSMN_SO_VERSION], [0:0:0])

AC_CONFIG_FILES([Makefile
	jsmn.pc:jsmn.pc.in
])

AC_ARG_ENABLE([tool],
	[AC_HELP_STRING([--disable-tool],[Compile command line jsmndump tool (default enabled)])],
	[use_tool=$enableval],
	[use_tool=auto])
if test x$use_tool != xno; then
	use_tool=yes
fi
AM_CONDITIONAL([USE_TOOL], [test x$use_tool = xyes])

AM_CONDITIONAL([NEVER_ENABLED], [false])

AC_OUTPUT
