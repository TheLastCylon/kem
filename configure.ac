AC_INIT([kem], [0.1], [bothadjt@gmail.com], [kem], [http://djb.co.za/])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.10 -Wall no-define])
AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX
AC_CONFIG_MACRO_DIR([m4])

BOOST_REQUIRE([1.54.0])
#BOOST_SYSTEM
#BOOST_ASIO
#BOOST_DATE_TIME
#BOOST_FILESYSTEM
#BOOST_REGEX
#BOOST_THREAD
#BOOST_DATE_TIME
#BOOST_PROGRAM_OPTIONS

MYSQL_CLIENT([5.5.40], [client])
MYSQLPP_DEVEL()

BOOST_CFLAGS="-I/usr/include"
AC_ARG_WITH([boost-include-path],
            [AS_HELP_STRING([--with-boost-include-path], [location of the boost headers, defaults to /usr/include])],
            [BOOST_CFLAGS="-I$withval"],
            [BOOST_CFLAGS="-I/usr/include"])
AC_SUBST([BOOST_CFLAGS]) 

AC_ARG_WITH([boost-lib-path],
            [AS_HELP_STRING([--with-boost-lib-path], [location of the boost libraries])],
            [BOOST_LIBS="-L$withval -lboost_system -lboost_thread -lboost_filesystem -lboost_regex -lboost_date_time"],
            [BOOST_LIBS="-lboost_system -lboost_thread -lboost_filesystem -lboost_regex -lboost_date_time -lboost_program_options"])
AC_SUBST([BOOST_LIBS])

AC_ARG_WITH([pthread],
            [AS_HELP_STRING([--without-pthread], [ignore pthread support @<:@default: automatic@:>@])],
            [PTHREAD_LIB=""],
            [PTHREAD_LIB="-lpthread"])
AC_SUBST([PTHREAD_LIB])

KISSCPP_CFLAGS="-I/usr/include/kisscpp-0.5"
AC_ARG_WITH([kisscpp-include-path],
            [AS_HELP_STRING([--with-kisscpp-include-path], [location of the kisscpp headers, defaults to /usr/include/kisscpp-0.5])],
            [KISSCPP_CFLAGS="-I$withval"],
            [KISSCPP_CFLAGS="-I/usr/include/kisscpp-0.5"])
AC_SUBST([KISSCPP_CFLAGS]) 

AC_ARG_WITH([kisscpp-lib-path],
            [AS_HELP_STRING([--with-kisscpp-lib-path], [location of the kisscpp library])],
            [KISSCPP_LIB="-L$withval -lkisscpp"],
            [KISSCPP_LIB="-lkisscpp"])
AC_SUBST([KISSCPP_LIB])

PKG_CHECK_MODULES([DEPS], [kisscpp-0.5 >= 0.5])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

