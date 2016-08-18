dnl Check for the availability of platform-specific atomic operations
dnl
AC_DEFUN([AX_ATOMIC],
[AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_CACHE_CHECK([for the __gnu_cxx:: atomic ops in ext/atomicity.h],
ax_cv_ext_atomicity_namespace,
[AC_TRY_COMPILE([#include <ext/atomicity.h>],
[
	_Atomic_word val = 0;
	__gnu_cxx::__atomic_add(&val, 1);
	__gnu_cxx::__exchange_and_add(&val, -1);
],
[ax_cv_ext_atomicity_namespace=yes],
[ax_cv_ext_atomicity_namespace=no])])

if test "$ax_cv_ext_atomicity_namespace" = yes; then
	AC_DEFINE([HAVE_EXT_ATOMICITY_H], 1,
	[Define to 1 if you have the <ext/atomicity.h> header file.])
else
	AC_CACHE_CHECK([for the __gnu_cxx:: atomic ops in bits/atomicity.h],
	ax_cv_bits_atomicity_namespace,
	[AC_TRY_COMPILE([#include <bits/atomicity.h>],
	[
		_Atomic_word val = 0;
		__gnu_cxx::__atomic_add(&val, 1);
		__gnu_cxx::__exchange_and_add(&val, -1);
	],
	[ax_cv_bits_atomicity_namespace=yes],
	[ax_cv_bits_atomicity_namespace=no])])

	if test "$ax_cv_bits_atomicity_namespace" = yes; then
		AC_DEFINE([HAVE_BITS_ATOMICITY_H], 1,
		[Define to 1 if you have the <bits/atomicity.h> header file.])
	else
		AC_CACHE_CHECK([if asm/atomic.h defines atomic functions],
		ax_cv_asm_atomic,
		[AC_TRY_COMPILE([#include <asm/atomic.h>],
		[
			atomic_t val = ATOMIC_INIT(0);
			atomic_set(&val, atomic_read(&val));
			atomic_inc(&val);
			atomic_dec_and_test(&val);
		],
		[ax_cv_asm_atomic=yes],
		[ax_cv_asm_atomic=no])])

		if test "$ax_cv_asm_atomic" = yes; then
			AC_DEFINE([HAVE_ASM_ATOMIC_H], 1,
			[Define to 1 if you have the <asm/atomic.h> header.])
		fi
	fi
fi
AC_LANG_RESTORE
])
