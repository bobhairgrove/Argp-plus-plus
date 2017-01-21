AC_PREREQ([2.69])
AC_INIT([Argp++], [0.9.0], [code@roberthairgrove.com], [libargp++], [https://github.com/bobhairgrove/Argp-plus-plus])

dnl #### Set tar-ustar so that adding the HTML documentation succeeds 
dnl #### when "make dist" is called; otherwise, tar complains about names
dnl #### longer than 99 characters:
AM_INIT_AUTOMAKE([foreign subdir-objects tar-ustar])

LT_INIT

AC_PROG_CXX
AC_LANG([C++])

AC_CONFIG_SRCDIR([src/argpp.cpp])
AC_CONFIG_FILES([Makefile src/Makefile examples/build/Makefile])
AC_CONFIG_MACRO_DIR([m4])

dnl #### For testing C++11 support: 
m4_include([m4/ax_require_defined.m4])
m4_include([m4/ax_cxx_compile_stdcxx.m4])
m4_include([m4/ax_cxx_compile_stdcxx_11.m4])
AX_CXX_COMPILE_STDCXX_11([noext],[optional])

dnl #### Checking for system-level argp.h: 
AC_CHECK_HEADERS(
  [argp.h], [AC_DEFINE([HAVE_SYSTEM_ARGP_H], [1], [argpp.hpp will try to include <argp.h> instead of "argp.h"])], 
            [AC_DEFINE([WITH_LOCAL_ARGP], [1], [using local ARGP])])

dnl #### Set up "--enable-pkgconfig" option for configure: 
AC_ARG_ENABLE([pkgconfig],
  [AS_HELP_STRING([--enable-pkgconfig],
                  [installs an Argp++.pc file in {prefix}/pkgconfig for use with the pkg-config program (default: disabled)])])
AS_IF([test "x$enable_pkgconfig" = "xyes"],
  [AC_MSG_NOTICE([Proceeding to add target 'install-data-hook' to the Makefile which will install the file 'argp++.pc' to {prefix}/lib/pkgconfig along with the library...])
   AC_SUBST([ARGPP_DO_PKGCONFIG],["yes"])
   AC_SUBST([ARGPP_PKGCONFIG_TMPFILE],["tmp_pkconf.pc"])
   AC_SUBST([ARGPP_PKGCONFIG_NAME],["Argp++"])
   AC_SUBST([ARGPP_PKGCONFIG_LIB],["-largp++"])
   AC_SUBST([ARGPP_PKGCONFIG_DESCR],["a C++ wrapper library around the GNU argp option parser utility."])],
  [AC_SUBST([ARGPP_DO_PKGCONFIG],["no"])])

AM_CONDITIONAL([ARGPP_PKGCONFIG_COND],[test "x$ARGPP_DO_PKGCONFIG" = "xyes"])

AC_OUTPUT
