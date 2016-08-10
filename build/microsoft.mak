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

.SILENT:

PACKAGE = b
VERSION = 1.0
APIVER = 1
COMPILERSUFFIX = m

TOPSRCDIR = ..

INCDIR = $(TOPSRCDIR)\include
SRCDIR = $(TOPSRCDIR)\src

CXXFLAGS = /nologo /c /W4 /EHsc /I "$(INCDIR)" \
	/D B_USE_PRECOMPILED_HEADER /D WIN32

LFLAGS = /nologo

!IFDEF DLL
CONFIGNAME = dll
!IFDEF MT
!IFDEF DEBUG
CXXFLAGS = $(CXXFLAGS) /MDd
LFLAGS = $(LFLAGS) /debug
!ELSE
CXXFLAGS = $(CXXFLAGS) /MD
!ENDIF
!ELSE
!ERROR Single-threaded DLL for Microsoft C++ is not available.
!ENDIF
!ELSE
CONFIGNAME = static
!IFDEF MT
CONFIGNAME = $(CONFIGNAME)_mt
!IFDEF DEBUG
CXXFLAGS = $(CXXFLAGS) /MTd
!ELSE
CXXFLAGS = $(CXXFLAGS) /MT
!ENDIF
!ELSE
!IFDEF DEBUG
CXXFLAGS = $(CXXFLAGS) /MLd
!ELSE
CXXFLAGS = $(CXXFLAGS) /ML
!ENDIF
!ENDIF
!ENDIF

!IFDEF DLL
CONFIGFLAGS = DLL=1
CONFIGSUFFIX =
!ELSE
CONFIGSUFFIX = a
CONFIGFLAGS =
!ENDIF

!IFDEF MT
CONFIGFLAGS = $(CONFIGFLAGS) MT=1
!ELSE
CONFIGSUFFIX = $(CONFIGSUFFIX)s
!ENDIF

!IFDEF UNICODE
CONFIGNAME = $(CONFIGNAME)_wchar
CONFIGFLAGS = $(CONFIGFLAGS) UNICODE=1
CXXFLAGS = $(CXXFLAGS) /D UNICODE /D _UNICODE
!ELSE
CONFIGNAME = $(CONFIGNAME)_char
CONFIGSUFFIX = $(CONFIGSUFFIX)b
!ENDIF

!IFDEF DEBUG
CONFIGNAME = $(CONFIGNAME)_dbg
CONFIGFLAGS = $(CONFIGFLAGS) DEBUG=1
CONFIGSUFFIX = $(CONFIGSUFFIX)d
CXXFLAGS = $(CXXFLAGS) /Od /Gm /ZI /RTC1
!ELSE
CONFIGNAME = $(CONFIGNAME)_rel
CXXFLAGS = $(CXXFLAGS) /D NDEBUG /O1
!ENDIF

BASENAME = $(PACKAGE)$(APIVER)$(COMPILERSUFFIX)$(CONFIGSUFFIX)

BUILDDIR = .

BINDIR = $(BUILDDIR)\bin
LIBDIR = $(BUILDDIR)\lib
INTDIR = $(BUILDDIR)\obj\$(BASENAME)

PCHFILE = "$(INTDIR)\$(PACKAGE).pch"

PCHSRC = "$(SRCDIR)\Exceptions.cpp"

PCHOBJ = "$(INTDIR)\Exceptions.obj"

OBJS= \
	"$(INTDIR)\Helpers.obj" \
	"$(INTDIR)\Memory.obj" \
	"$(INTDIR)\Pathname.obj" \
	"$(INTDIR)\String.obj"

TESTS = \
	"$(INTDIR)\TestArray.exe" \
	"$(INTDIR)\TestHelpers.exe" \
	"$(INTDIR)\TestLists.exe" \
	"$(INTDIR)\TestMakePath.exe" \
	"$(INTDIR)\TestMemory.exe" \
	"$(INTDIR)\TestObject.exe" \
	"$(INTDIR)\TestRandom.exe" \
	"$(INTDIR)\TestString.exe"

CXXFLAGS = $(CXXFLAGS) /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF

!IFDEF MAKELIBRARY

CXXFLAGS = $(CXXFLAGS) /Fp$(PCHFILE)

!IFDEF DLL

CXXFLAGS = $(CXXFLAGS) /D _WINDOWS /D B_BUILD

"$(BINDIR)\$(BASENAME).dll": $(OBJS)
	if not exist "$(BINDIR)/$(NULL)" mkdir "$(BINDIR)"
	if not exist "$(LIBDIR)/$(NULL)" mkdir "$(LIBDIR)"
	link @<<
	$(LFLAGS) /dll /incremental:yes /version:$(VERSION) /machine:I386
	/out:"$(BINDIR)\$(BASENAME).dll" /pdb:"$(BINDIR)\$(BASENAME).pdb"
	/implib:"$(LIBDIR)/$(BASENAME).lib" kernel32.lib $(PCHOBJ) $(OBJS)
<<

!ELSE

CXXFLAGS = $(CXXFLAGS) /D _LIB /D B_BUILD_STATIC

"$(LIBDIR)\$(BASENAME).lib": $(OBJS)
	if not exist "$(LIBDIR)/$(NULL)" mkdir "$(LIBDIR)"
	link -lib @<<
	$(LFLAGS) /out:"$(LIBDIR)\$(BASENAME).lib" $(PCHOBJ) $(OBJS)
<<

!ENDIF

{$(SRCDIR)}.cpp{$(INTDIR)}.obj::
	cl @<<
	$(CXXFLAGS) /Yu"B/Host.h" $<
<<

$(OBJS): $(PCHFILE)

$(PCHFILE) $(PCHOBJ): $(PCHSRC)
	if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"
	cl @<<
	$(CXXFLAGS) /Yc"B/Host.h" $?
<<

!ELSE IFDEF MAKETEST

TESTSRCDIR = $(TOPSRCDIR)\tests

!IFNDEF DLL
CXXFLAGS = $(CXXFLAGS) /D B_STATIC
!ENDIF

.SUFFIXES: .exe .cpp

{$(TESTSRCDIR)}.cpp{$(INTDIR)}.exe:
	cl @<<
	$(CXXFLAGS) /D _CONSOLE $<
<<
	link @<<
	$(LFLAGS) /incremental:yes /subsystem:console /machine:I386 /out:$@
	/pdb:$*.pdb /libpath:"$(LIBDIR)" kernel32.lib $*.obj
<<

all: $(MAKETEST)

!ELSE IFDEF TEST

all:
	-$(TEST) && echo PASS: $(TEST) || echo FAIL: $(TEST)

!ELSE

!IFNDEF MORETARGETS
help: help_begin show_configurations help_end

help_begin:
	more < <<
Please specify one or several targets to make.
Valid target names are:
	help    = display this help message
	all     = build all library configurations
	build   = "all"
	tests   = "build" + make test applications
	check   = "tests" + execute test applications
Alternatively, only the required library configurations
can be selected for building:
<<

help_end:
	more < <<
It is also possible to build test applications
for a particular configuration by adding suffix
_tests to its name. Use check_configuration_name
for executing these tests.
Examples:
NMAKE /NOLOGO /F microsoft.mak dll_wchar_dbg_tests
	Build the dynamic linking multithreaded Unicode
	debug version of the library and tests for it.
NMAKE /NOLOGO /F microsoft.mak check_static_wchar_rel
	Build the static single-threaded Unicode release version
	of the library and tests for it, then execute the
	tests.
NMAKE /NOLOGO /F microsoft.mak check
	Build and execute tests for all possible library
	configurations.
<<

!ENDIF

show_configurations::
	more < <<
	$(CONFIGNAME)
<<

$(CONFIGNAME):
	if not exist $(INTDIR) mkdir $(INTDIR)
	$(MAKE) /$(MAKEFLAGS) /NOLOGO /F microsoft.mak \
		BUILDDIR="$(BUILDDIR)" $(CONFIGFLAGS) MAKELIBRARY=1

build:: $(CONFIGNAME)

$(CONFIGNAME)_tests: $(CONFIGNAME)
	for %%t in ($(TESTS)) do \
		@$(MAKE) /$(MAKEFLAGS) /NOLOGO /F microsoft.mak \
		BUILDDIR="$(BUILDDIR)" $(CONFIGFLAGS) MAKETEST=%%t

tests:: $(CONFIGNAME)_tests

check_$(CONFIGNAME): $(CONFIGNAME)_tests
	echo CHECK [$(CONFIGNAME)]
	set PATH=$(BINDIR);%%PATH%%
	for %%t in ($(TESTS)) do \
		@$(MAKE) /$(MAKEFLAGS) /NOLOGO /F microsoft.mak \
		BUILDDIR="$(BUILDDIR)" $(CONFIGFLAGS) TEST=%%t

check:: check_$(CONFIGNAME)

clean_$(CONFIGNAME):
	-rmdir /S /Q "$(BUILDDIR)\obj\$(BASENAME)" 2> nul
	-rmdir "$(BUILDDIR)\obj" 2> nul
	-erase /Q "$(BINDIR)\$(BASENAME)*.*" 2> nul
	-rmdir "$(BINDIR)" 2> nul
	-erase /Q "$(LIBDIR)\$(BASENAME)*.*" 2> nul
	-rmdir "$(LIBDIR)" 2> nul

clean:: clean_$(CONFIGNAME)

!IFNDEF MORETARGETS

MORETARGETS = 1

DEBUG = 1

!INCLUDE microsoft.mak

UNICODE = 1

!INCLUDE microsoft.mak

!UNDEF DEBUG

!INCLUDE microsoft.mak

MT = 1

!INCLUDE microsoft.mak

DEBUG = 1

!INCLUDE microsoft.mak

!UNDEF UNICODE

!INCLUDE microsoft.mak

!UNDEF DEBUG

!INCLUDE microsoft.mak

DLL = 1

!INCLUDE microsoft.mak

DEBUG = 1

!INCLUDE microsoft.mak

UNICODE = 1

!INCLUDE microsoft.mak

!UNDEF DEBUG

!INCLUDE microsoft.mak

!ENDIF

!ENDIF
