# B: Common C++ library
# Copyright (C) 2002-2007, 2016 Damon Revoe
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
AC_CONFIG_SRCDIR([src/string.cpp])
AM_INIT_AUTOMAKE

library_version_info=3:0:0
AC_SUBST(library_version_info)

AC_CONFIG_HEADERS(config.h)

AC_ARG_WITH([stl], [  --with-stl              ]dnl
[include support for STL (default=no)],
[AC_DEFINE([USE_STL], 1, [Define to 1 for enabling STL compatibility.])])

AC_ARG_WITH(graphviz, [  --with-graphviz         ]dnl
[use graph visualization toolkit (default=no)],
HAVE_DOT=$withval, HAVE_DOT=no)

AC_SUBST(HAVE_DOT)

AC_ARG_WITH(doxyconfig, [  --with-doxyconfig=PROG  ]dnl
[override the default Doxygen configuration variables],
[test "$withval" != yes -a "$withval" != no && DOXYCONFIG=$withval])

AC_SUBST(DOXYCONFIG)

AM_CONDITIONAL(DOXYCONFIG, [test -x "$DOXYCONFIG"])

docdir=$datadir/doc

AC_ARG_WITH(docdir, [  --with-docdir=DIR       ]dnl
[install documentation in DIR (default=DATADIR/doc)],
[test "$withval" != yes -a "$withval" != no && docdir=$withval])

AC_SUBST(docdir)

pkgdocdir=$docdir/$PACKAGE-$VERSION
AC_SUBST(pkgdocdir)

test -z "$CXXFLAGS" && CXXFLAGS=""

dnl Checks for programs.
AC_PROG_CXX
AC_DISABLE_STATIC
AC_PROG_LIBTOOL
AC_PATH_PROG(DOXYGEN, doxygen)

AM_CONDITIONAL(DOXYGEN, [test -x "$DOXYGEN"])

CPPFLAGS="$CPPFLAGS -I\$(top_srcdir)/include -I\$(top_builddir)/include"

dnl When compiling with GNU C++, display more warnings.
if test "$GXX" = yes; then
	CXXFLAGS="$CXXFLAGS -ansi -pedantic -Wall \
-Woverloaded-virtual -Wsign-promo -W -Wshadow -Wpointer-arith -Wcast-qual \
-Wwrite-strings -Wconversion -Wsign-compare -Wredundant-decls -Winline"
dnl Display all levels of the Digital (Compaq) C++ warnings.
elif test "$CXX" = cxx &&
	cxx -V < /dev/null 2>&1 | grep -Eiq 'digital|compaq'; then
	DIGITALCXX="yes"
	CXXFLAGS="$CXXFLAGS -w0 -msg_display_tag -std strict_ansi"
dnl Enable all warnings and remarks of the Intel C++ compiler.
elif test "$CXX" = icpc && icpc -V < /dev/null 2>&1 | grep -iq intel; then
	CXXFLAGS="$CXXFLAGS -w2"
fi

AC_ARG_ENABLE(debug, [  --enable-debug          ]dnl
[enable debug info and runtime checks (default=no)])

AM_CONDITIONAL(DEBUG, [test "$enable_debug" = yes])

if test "$enable_debug" = yes; then
	configsuffix="d"
	CONFIG_PARAMS=" --debug"
	CONFIG_FLAGS="-DB_DEBUG"

	if test "$DIGITALCXX" = yes; then
		CONFIG_FLAGS="$CONFIG_FLAGS -gall"
	elif test "$ac_cv_prog_cxx_g" = yes; then
		CONFIG_FLAGS="$CONFIG_FLAGS -g"
	fi
else
	CONFIG_FLAGS="-O3"
fi

AC_ARG_ENABLE(threads, [  --enable-threads        ]dnl
[build multithreaded library (default=no)])

AM_CONDITIONAL(THREADS, [test "$enable_threads" = yes])

if test "$enable_threads" = yes; then
	configsuffix="${configsuffix}_r"
	CONFIG_PARAMS=" --multithreaded$CONFIG_PARAMS"

	ACX_PTHREAD(,[AC_MSG_ERROR(
		[no pthreads support; consider --disable-threads])])

	CONFIG_FLAGS="$CONFIG_FLAGS -DB_MT $PTHREAD_CFLAGS"
	CONFIG_LIBS=$PTHREAD_LIBS
fi

AC_SUBST(configsuffix)
AC_SUBST(CONFIG_PARAMS)

UNINST_FLAGS="$CONFIG_FLAGS -DB_UNINSTALLED"
CXXFLAGS="$CXXFLAGS $UNINST_FLAGS"

UNINST_PREFIX="`pwd`"
UNINST_FLAGS="$UNINST_FLAGS -I$UNINST_PREFIX/include"
dummy="`cd $srcdir && pwd`"
test $UNINST_PREFIX = $dummy || UNINST_FLAGS="$UNINST_FLAGS -I$dummy/include"

CONFIG_LIBS="-l$PACKAGE$configsuffix $CONFIG_LIBS"
UNINST_LIBS="-L$UNINST_PREFIX/src $CONFIG_LIBS"

AC_SUBST(UNINST_PREFIX)
AC_SUBST(UNINST_FLAGS)
AC_SUBST(UNINST_LIBS)

dnl Do compilation tests using CXX
AC_LANG_CPLUSPLUS

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(errno.h fcntl.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

if test "$enable_threads" = yes; then
	AC_CACHE_CHECK([for the __gnu_cxx:: atomic ops in bits/atomicity.h],
	b_cv_header_bits_atomicity_namespace,
	[AC_TRY_COMPILE([#include <bits/atomicity.h>],
	[
	using namespace __gnu_cxx;
	_Atomic_word Atomic = 0;
	__atomic_add(&Atomic, 1);
	__exchange_and_add(&Atomic, -1);
	],
	[b_cv_header_bits_atomicity_namespace=yes],
	[b_cv_header_bits_atomicity_namespace=no])])

	if test "$b_cv_header_bits_atomicity_namespace" != yes; then
		AC_CACHE_CHECK([for the global atomic ops in bits/atomicity.h],
		b_cv_header_bits_atomicity_global,
		[AC_TRY_COMPILE([#include <bits/atomicity.h>],
		[
		_Atomic_word Atomic = 0;
		__atomic_add(&Atomic, 1);
		__exchange_and_add(&Atomic, -1);
		],
		[b_cv_header_bits_atomicity_global=yes],
		[b_cv_header_bits_atomicity_global=no])])
	fi

	if test "$b_cv_header_bits_atomicity_namespace" = yes -o \
		"$b_cv_header_bits_atomicity_global" = yes; then
		AC_DEFINE([HAVE_BITS_ATOMICITY_H], 1,
		[Define to 1 if you have the <bits/atomicity.h> header file.])

		if test "$b_cv_header_bits_atomicity_namespace" = yes; then
			AC_DEFINE([ATOMICITY_GNU_CXX], 1,
			[Define to 1 if atomic ops are
			declared in namespace __gnu_cxx.])
		fi
	fi

	AC_CACHE_CHECK([if asm/atomic.h defines all the required functions],
	b_cv_header_asm_atomic,
	[AC_TRY_COMPILE([#include <asm/atomic.h>],
	[
	atomic_t Atomic = ATOMIC_INIT(0);
	atomic_set(&Atomic, atomic_read(&Atomic));
	atomic_inc(&Atomic);
	atomic_dec_and_test(&Atomic);
	],
	[b_cv_header_asm_atomic=yes],
	[b_cv_header_asm_atomic=no])])

	if test "$b_cv_header_asm_atomic" = yes; then
		AC_DEFINE([HAVE_ASM_ATOMIC_H], 1,
		[Define to 1 if you have the <asm/atomic.h> header file.])
	fi
fi

dnl Checks for library functions.
AC_FUNC_MEMCMP
AC_CHECK_FUNCS(strerror)

AC_OUTPUT([Makefile
include/Makefile
include/b/Makefile
src/Makefile
tests/Makefile
b-config
uninstalled-config
doc/Makefile
config/Makefile
m4/Makefile],
[chmod +x uninstalled-config

if test "$includedir" != /usr/include -a \
	"$includedir" != /usr/local/include; then
	CONFIG_FLAGS="$CONFIG_FLAGS -I$includedir"
fi

if test "$libdir" != /usr/lib -a \
	"$libdir" != /usr/local/lib; then
	CONFIG_LIBS="-L$libdir $CONFIG_LIBS"
fi

configfile="$PACKAGE$configsuffix.conf"
echo creating $configfile
echo "version=\"$VERSION\"" > $configfile
echo "prefix=\"$prefix\"" >> $configfile
echo "exec_prefix=\"$exec_prefix\"" >> $configfile
echo "flags=\"$CONFIG_FLAGS\"" >> $configfile
echo "libs=\"$CONFIG_LIBS\"" >> $configfile],
[configsuffix="$configsuffix"
PACKAGE="$PACKAGE"
VERSION="$VERSION"
prefix="$prefix"
exec_prefix="$exec_prefix"
includedir="$includedir"
libdir="$libdir"
CONFIG_FLAGS="$CONFIG_FLAGS"
CONFIG_LIBS="$CONFIG_LIBS"])
