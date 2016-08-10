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

.SILENT
.OPTIMIZE
.ERASE

!ifndef Prefix
Prefix = ..\..
!endif

!ifndef BinDir
BinDir = $(Prefix)\bin
!endif

!ifndef LibDir
LibDir = $(Prefix)\lib
!endif

!ifndef TopSrcDir
TopSrcDir = $(Prefix)\..
!endif

ObjList = &
	Exceptions.obj &
	Helpers.obj &
	Memory.obj &
	Pathname.obj &
	String.obj

Tests = &
	TestArray.exe &
	TestHelpers.exe &
	TestLists.exe &
	TestMakePath.exe &
	TestMemory.exe &
	TestObject.exe &
	TestRandom.exe &
	TestString.exe

Package = b

Version = 1

!ifneq __VERSION__ 11
CompilerSuffix = ow
!else
CompilerSuffix = w
!endif

ConfigFlags = Prefix=$(Prefix) BinDir=$(BinDir) &
	LibDir=$(LibDir) TopSrcDir=$(TopSrcDir)

CxxOptions = -zq -wx -bt=nt -i=$(TopSrcDir)\include &
	-dB_USE_PRECOMPILED_HEADER -fhwe -of -xs -6r

LinkOptions = option quiet option caseexact

!ifdef STATIC
ConfigName = static
ConfigFlags += STATIC=1
ConfigSuffix = a
!else
ConfigName = dll
CxxOptions += -br
ConfigSuffix =
!endif

!ifdef MT
ConfigName = $(ConfigName)_mt
ConfigFlags += MT=1
CxxOptions += -bm
!else
ConfigSuffix = $(ConfigSuffix)s
!endif

!ifdef UNICODE
ConfigName = $(ConfigName)_wchar
ConfigFlags += UNICODE=1
CxxOptions += -dUNICODE -d_UNICODE
!else
ConfigName = $(ConfigName)_char
ConfigSuffix = $(ConfigSuffix)b
!endif

!ifdef DEBUG
ConfigName = $(ConfigName)_dbg
ConfigFlags += DEBUG=1
ConfigSuffix = $(ConfigSuffix)d
CxxOptions += -d3s -dDEBUG -d_DEBUG
LinkOptions += debug all
!else
ConfigName = $(ConfigName)_rel
CxxOptions += -otexan
!endif

BaseName = $(Package)$(Version)$(CompilerSuffix)$(ConfigSuffix)

!ifdef __LOADDLL__
!loaddll wpp386 wppd386
!loaddll wlink wlink
!loaddll wlib wlibd
!endif

!ifdef MAKE_LIBRARY

!ifndef STATIC
CxxOptions += -bd -dB_BUILD
!else
CxxOptions += -dB_BUILD_STATIC
!endif

.cpp: $(TopSrcDir)\src

.cpp.obj: .AUTODEPEND
	wpp386 $(CxxOptions) -fh $<

!ifndef STATIC
$(BinDir)\$(BaseName).dll: $(ObjList)
	-if not exist $(BinDir) mkdir $(BinDir)
	-if not exist $(LibDir) mkdir $(LibDir)
	wlink name $@ system nt_dll $(LinkOptions) &
		option implib=$(LibDir)\$(BaseName).lib file { $< }
!else
$(LibDir)\$(BaseName).lib: $(ObjList)
	-if not exist $(LibDir) mkdir $(LibDir)
	wlib /q /b /n /t $(LibDir)\$(BaseName).lib $<
!endif

!else ifdef MAKE_TESTS

!ifdef STATIC
CxxOptions += -dB_STATIC
!endif

.cpp: $(TopSrcDir)\tests

.cpp.obj: .AUTODEPEND
	wpp386 $(CxxOptions) -bc -fh=$(Package)-tests.pch $<

.obj.exe:
	wlink name $@ system nt $(LinkOptions) &
		option eliminate libpath $(LibDir) file { $< }

all: $(Tests) .SYMBOLIC

!else ifdef TEST

all: .SYMBOLIC
	-$(TEST)
	if not errorlevel 1 echo PASS: $(TEST)
	if errorlevel 1 echo FAIL: $(TEST)

!else

!ifndef MORE_TARGETS

help: .SYMBOLIC
	@echo Please specify one or several targets to make.
	@echo Valid target names are:
	@echo   help    = display this help message
	@echo   all     = build all library configurations
	@echo   build   = "all"
	@echo   tests   = "build" + make test applications
	@echo   check   = "tests" + execute test applications
	@echo Alternatively, only the required library configurations
	@echo can be selected for building:
	%make show_configurations
	@echo It is also possible to build test applications
	@echo for a particular configuration by adding suffix
	@echo _tests to its name. Use check_configuration_name
	@echo for executing these tests.
	@echo Examples:
	@echo wmake.exe -h -f watcom.mak dll_wchar_dbg_tests
	@echo   Build the dynamic linking single-threaded Unicode
	@echo   debug version of the library and tests for it.
	@echo wmake.exe -h -f watcom.mak check_static_mt_wchar_rel
	@echo   Build the static multithreaded Unicode release version
	@echo   of the library and tests for it, then execute the
	@echo   tests.
	@echo wmake.exe -h -f watcom.mak check
	@echo   Build and execute tests for all possible library
	@echo   configurations.

all: build .SYMBOLIC

!endif

$+show_configurations:: .SYMBOLIC
	$+echo   $(ConfigName)

IntDir = obj\$(BaseName)

$+$(ConfigName): .SYMBOLIC
	$+-if not exist $(IntDir) mkdir $(IntDir)
	$+cd $(IntDir)
	$+$(MAKE) -h MAKE_LIBRARY=1 $(ConfigFlags) -f ..\..\watcom.mak
	cd ..\..

$+build:: $(ConfigName)

$(ConfigName)_tests: $(ConfigName) .SYMBOLIC
	$+cd $(IntDir)
	$+$(MAKE) -h MAKE_TESTS=1 $(ConfigFlags) -f ..\..\watcom.mak
	cd ..\..

$+tests:: $(ConfigName)_tests .SYMBOLIC

$+check_$(ConfigName): $(ConfigName)_tests .SYMBOLIC
	$+echo CHECK [$(ConfigName)]
	$+cd $(IntDir)
	$+set PATH=$(BinDir);$(%PATH)
	$+for %t in ($(Tests)) do $(MAKE) -h TEST=%t -f ..\..\watcom.mak
	cd ..\..

$+check:: check_$(ConfigName) .SYMBOLIC

!ifndef MORE_TARGETS

!define MORE_TARGETS

!define DEBUG

!include watcom.mak

!define UNICODE

!include watcom.mak

!undef DEBUG

!include watcom.mak

!define MT

!include watcom.mak

!define DEBUG

!include watcom.mak

!undef UNICODE

!include watcom.mak

!undef DEBUG

!include watcom.mak

!define STATIC

!include watcom.mak

!define DEBUG

!include watcom.mak

!define UNICODE

!include watcom.mak

!undef DEBUG

!include watcom.mak

!undef MT

!include watcom.mak

!define DEBUG

!include watcom.mak

!undef UNICODE

!include watcom.mak

!undef DEBUG

!include watcom.mak

!endif

!endif
