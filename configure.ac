# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([librds], [0.9], [tobias.lorenz@gmx.net])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_LIBOBJ_DIR([lib])
AM_INIT_AUTOMAKE([-Wall -Werror dist-bzip2])
# active this to get nicer tests
#AM_INIT_AUTOMAKE([-Wall -Werror dist-bzip2 color-tests parallel-tests])

# Conditionals.
AC_ARG_ENABLE([oda_irds],
[  --enable-oda-irds    Turn on ODA I-RDS],
[case "${enablevar}" in
  yes) oda_irds=true ;;
  no)  oda_irds=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-oda-irds]) ;;
esac],[oda_irds=false])
AM_CONDITIONAL([ODA_IRDS], [test x$oda_irds = xtrue])
AC_ARG_ENABLE([oda_tmc],
[  --disable-oda-tmc    Turn on ODA TMC],
[case "${enablevar}" in
  yes) oda_tmc=true ;;
  no)  oda_tmc=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --disable-oda-tmc]) ;;
esac],[oda_tmc=true])
AM_CONDITIONAL([ODA_TMC], [test x$oda_tmc = xtrue])
AC_ARG_ENABLE([oda_rasant],
[  --enable-oda-rasant    Turn on ODA RASANT],
[case "${enablevar}" in
  yes) oda_rasant=true ;;
  no)  oda_rasant=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-oda-rasant]) ;;
esac],[oda_rasant=false])
AM_CONDITIONAL([ODA_RASANT], [test x$oda_rasant = xtrue])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_SED
AC_CHECK_PROG(ASTYLE, astyle, astyle)
AC_CHECK_PROG(DOXYGEN, doxygen, doxygen)
AC_CHECK_PROG(FIND, find, find)
AC_CHECK_PROG(GENHTML, genhtml, genhtml)
AC_CHECK_PROG(LCOV, lcov, lcov)
AC_CHECK_PROG(MV, mv, mv)
AC_CHECK_PROG(RM, rm, rm)
AC_CHECK_PROG(VALGRIND, valgrind, valgrind)
AM_PROG_CC_C_O
LT_INIT

# Checks for libraries.
PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MKTIME
AC_CHECK_FUNCS([memset setenv tzset])

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 db/Makefile
                 db/rds_lang/Makefile
                 db/rds_program/Makefile
                 db/tmc_el/Makefile
                 db/tmc_lcl/Makefile
                 src/Makefile
                 tests/Makefile])

AC_OUTPUT
