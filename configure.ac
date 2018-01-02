AC_PREREQ([2.67])
AC_INIT([permute],m4_esyscmd_s([cat .tarball-version 2>/dev/null || git describe --always --dirty]))
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_FILES([Makefile])

AC_GNU_SOURCE
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CC_C99
if test "x$ac_cv_prog_cc_c99" = "xno"; then
	AC_MSG_ERROR([A c99 compiler is required])
fi
AM_PROG_CC_C_O
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])    # Fixes automake 1.12 (where -Wall implies portability warnings), but automake <= 1.11 does not recognize it
AC_LANG_C
AC_PROG_INSTALL

AX_VALGRIND_DFLT([drd],[off])
AX_VALGRIND_DFLT([helgrind],[off])
AX_VALGRIND_DFLT([sgcheck],[off])
AX_VALGRIND_CHECK

AC_CHECK_HEADERS([fcntl.h limits.h malloc.h stdlib.h],
                 [],
                 [AC_MSG_ERROR([A standard header file was not found])])

AC_FUNC_ALLOCA
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([dup2 memset munmap strdup strerror strtol],
               [],
               [AC_MSG_ERROR([A standard library function was not found])])

AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

AC_OUTPUT
