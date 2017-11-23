# B: Common C++ library
# Copyright (C) 2002-2007, 2016, 2017 Damon Revoe
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

AC_INIT([b], [1.0.1])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([src/string.cc])
AM_INIT_AUTOMAKE([foreign])

library_version_info=3:0:0
AC_SUBST(library_version_info)

AC_CONFIG_HEADERS(config.h)

AC_ARG_WITH([stl], AS_HELP_STRING([--without-stl], [disable STL support]))

AS_IF([test "x$with_stl" != "xno"],
	[AC_DEFINE([USE_STL], 1, [Define to 1 to enable STL support.])])

docdir=$datadir/doc

AC_ARG_WITH(docdir, AS_HELP_STRING([--with-docdir=DIR],
	[install documentation in DIR (default=DATADIR/doc)]),
	[test "$withval" != yes -a "$withval" != no && docdir=$withval])

AC_SUBST(docdir)

pkgdocdir=$docdir/$PACKAGE-$VERSION
AC_SUBST(pkgdocdir)

test -z "$CXXFLAGS" && CXXFLAGS=""

dnl Checks for programs.
AC_PROG_CXX
LT_INIT([disable-shared])
PKG_PROG_PKG_CONFIG
AC_DEFUN([PKG_INSTALLDIR],
	 [m4_pushdef([pkg_default], [m4_default([$1], ['${libdir}/pkgconfig'])])
	 m4_pushdef([pkg_description],
		        [pkg-config installation directory @<:@]pkg_default[@:>@])
	 AC_ARG_WITH([pkgconfigdir],
		         [AS_HELP_STRING([--with-pkgconfigdir], pkg_description)],,
			     [with_pkgconfigdir=]pkg_default)
	 AC_SUBST([pkgconfigdir], [$with_pkgconfigdir])
	 m4_popdef([pkg_default])
	 m4_popdef([pkg_description])
	 ]) dnl PKG_INSTALLDIR
PKG_INSTALLDIR

CPPFLAGS="$CPPFLAGS -I\$(top_srcdir)/include -I\$(top_builddir)/include"

dnl When compiling with GNU C++, display more warnings.
AS_IF([test "$GXX" = yes],
	[CXXFLAGS="$CXXFLAGS -ansi -pedantic -Wall \
-Woverloaded-virtual -Wsign-promo -W -Wshadow -Wpointer-arith -Wcast-qual \
-Wwrite-strings -Wconversion -Wsign-compare -Wredundant-decls -Winline"],
dnl Display all levels of the Digital (Compaq) C++ warnings.
[test "$CXX" = cxx &&
	cxx -V < /dev/null 2>&1 | grep -Eiq 'digital|compaq'],
	[DIGITALCXX="yes"
	CXXFLAGS="$CXXFLAGS -w0 -msg_display_tag -std strict_ansi"],
dnl Enable all warnings and remarks of the Intel C++ compiler.
[test "$CXX" = icpc && icpc -V < /dev/null 2>&1 | grep -iq intel],
	[CXXFLAGS="$CXXFLAGS -w2"])

AC_ARG_ENABLE(debug, AS_HELP_STRING([--enable-debug],
	[enable debug info and runtime checks (default=no)]))

AM_CONDITIONAL(DEBUG, [test "$enable_debug" = yes])

AS_IF([test "$enable_debug" != yes],
	[CONFIG_FLAGS="-O3"],
[test "$DIGITALCXX" = yes],
	[CONFIG_FLAGS="-DB_DEBUG -gall"],
[test "$ac_cv_prog_cxx_g" = yes],
	[CONFIG_FLAGS="-DB_DEBUG -g"])

AC_ARG_ENABLE(threads, AS_HELP_STRING([--enable-threads],
	[build multithreaded library (default=no)]))

AM_CONDITIONAL(THREADS, [test "$enable_threads" = yes])

AS_IF([test "$enable_threads" = yes],
	[AX_PTHREAD(,[AC_MSG_ERROR(
		[no pthread support; consider --disable-threads])])

	CONFIG_FLAGS="$CONFIG_FLAGS -DB_MT $PTHREAD_CFLAGS"
	PRIVATE_CONFIG_LIBS=$PTHREAD_LIBS])

UNINST_FLAGS="$CONFIG_FLAGS -DB_UNINSTALLED"
CXXFLAGS="$CXXFLAGS $UNINST_FLAGS"
CONFIG_FLAGS="$CONFIG_FLAGS -I$includedir"

UNINST_PREFIX="`pwd`"
UNINST_FLAGS="$UNINST_FLAGS -I$UNINST_PREFIX/include"
dummy="`cd $srcdir && pwd`"
test $UNINST_PREFIX = $dummy || UNINST_FLAGS="$UNINST_FLAGS -I$dummy/include"

CONFIG_LIBS="-l$PACKAGE"
UNINST_LIBS="-L$UNINST_PREFIX/src $CONFIG_LIBS"
CONFIG_LIBS="-L$libdir $CONFIG_LIBS"

AC_SUBST(CONFIG_FLAGS)
AC_SUBST(CONFIG_LIBS)
AC_SUBST(PRIVATE_CONFIG_LIBS)
AC_SUBST(UNINST_PREFIX)
AC_SUBST(UNINST_FLAGS)
AC_SUBST(UNINST_LIBS)

dnl Do compilation tests using CXX
AC_LANG([C++])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(errno.h fcntl.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AX_COMPILE_CHECK_SIZEOF([size_t], [#include <stddef.h>])

AS_IF([test "$enable_threads" = yes], [AX_ATOMIC()])

dnl Checks for library functions.
AC_FUNC_MEMCMP
AC_CHECK_FUNCS(strerror)

AC_CONFIG_FILES([Makefile
include/Makefile
include/b/Makefile
src/Makefile
tests/Makefile
b.pc
b-uninstalled.pc
doc/Makefile
m4/Makefile])
AC_OUTPUT
