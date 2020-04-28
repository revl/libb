dnl Check for the availability of platform-specific atomic operations
dnl
AC_DEFUN([AX_ATOMIC],
[AC_LANG_SAVE
AC_LANG_CPLUSPLUS

ac_save_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -std=c++11"

AC_CACHE_CHECK([for std::atomic<> in <atomic>],
	[ax_cv_std_atomic],
	[AC_TRY_COMPILE([#include <atomic>],
	[
		std::atomic<int> val = ATOMIC_VAR_INIT(1);
		++val;
		if (--val)
			val = 1;

		struct
		{
			std::atomic<int> val;
		} wrapper = {ATOMIC_VAR_INIT(1)};
	],
	[ax_cv_std_atomic='yes'],
	[ax_cv_std_atomic='no'])])

CXXFLAGS="$ac_save_CXXFLAGS"

AS_IF([test x"$ax_cv_std_atomic" = x'yes'],
	[AC_DEFINE([HAVE_STD_ATOMIC], [1],
	[Define to 1 if you have the <atomic> header file.])])

AC_CACHE_CHECK([for __sync built-ins for atomic access],
	[ax_cv_sync_fetch],
	[AC_TRY_COMPILE([],
	[
		int val = 0;

		__sync_add_and_fetch(&val, 1);
		__sync_add_and_fetch(&val, -1);
	],
	[ax_cv_sync_fetch='yes'],
	[ax_cv_sync_fetch='no'])])

AS_IF([test x"$ax_cv_sync_fetch" = x'yes'],
	[AC_DEFINE([HAVE_ATOMIC_SYNC], [1],
	[Define to 1 if you have __sync built-ins for atomic access.])])

AC_CACHE_CHECK([for the __gnu_cxx:: atomic ops in ext/atomicity.h],
	[ax_cv_ext_atomicity_namespace],
	[AC_TRY_COMPILE([#include <ext/atomicity.h>],
	[
		_Atomic_word val = 0;
		__gnu_cxx::__atomic_add(&val, 1);
		__gnu_cxx::__exchange_and_add(&val, -1);
	],
	[ax_cv_ext_atomicity_namespace='yes'],
	[ax_cv_ext_atomicity_namespace='no'])])

AS_IF([test x"$ax_cv_ext_atomicity_namespace" = x'yes'],
	[AC_DEFINE([HAVE_EXT_ATOMICITY_H], [1],
	[Define to 1 if you have the <ext/atomicity.h> header file.])])

AC_CACHE_CHECK([for the __gnu_cxx:: atomic ops in bits/atomicity.h],
	[ax_cv_bits_atomicity_namespace],
	[AC_TRY_COMPILE([#include <bits/atomicity.h>],
	[
		_Atomic_word val = 0;
		__gnu_cxx::__atomic_add(&val, 1);
		__gnu_cxx::__exchange_and_add(&val, -1);
	],
	[ax_cv_bits_atomicity_namespace='yes'],
	[ax_cv_bits_atomicity_namespace='no'])])

AS_IF([test x"$ax_cv_bits_atomicity_namespace" = x'yes'],
	[AC_DEFINE([HAVE_BITS_ATOMICITY_H], [1],
	[Define to 1 if you have the <bits/atomicity.h> header file.])])

AC_CACHE_CHECK([if asm/atomic.h defines atomic functions],
	[ax_cv_asm_atomic],
	[AC_TRY_COMPILE([#include <asm/atomic.h>],
	[
		atomic_t val = ATOMIC_INIT(0);
		atomic_set(&val, atomic_read(&val));
		atomic_inc(&val);
		atomic_dec_and_test(&val);
	],
	[ax_cv_asm_atomic='yes'],
	[ax_cv_asm_atomic='no'])])

AS_IF([test x"$ax_cv_asm_atomic" = x'yes'],
	[AC_DEFINE([HAVE_ASM_ATOMIC_H], [1],
	[Define to 1 if you have the <asm/atomic.h> header.])])

AC_LANG_RESTORE
])
