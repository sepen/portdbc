AC_INIT([portdbc],[1.1])

AC_PROG_CC

AC_CHECK_HEADERS(stdio.h string.h, [break])

# get libcurl library and include locations
AC_ARG_WITH([curl-inc-path],
  [AS_HELP_STRING([--with-curl-inc-path],
     [location of the curl headers, defaults to /usr/include])],
  [curl_CFLAGS="-I$withval"],
  [curl_CFLAGS='-I/usr/include'])
AC_SUBST([curl_CFLAGS])

AC_ARG_WITH([curl-lib-path],
  [AS_HELP_STRING([--with-curl-lib-path],
     [location of the curl libraries, defaults to /usr/lib])],
  [curl_LIBS="-L$withval -lcurl"],
  [curl_LIBS="-L/usr/lib -lcurl"])
AC_SUBST([curl_LIBS])

# get libxml2 library and include locations
AC_ARG_WITH([xml2-inc-path],
  [AS_HELP_STRING([--with-xml2-inc-path],
     [location of the xml2 headers, defaults to /usr/include/libxml2])],
  [xml2_CFLAGS="-I$withval"],
  [xml2_CFLAGS='-I/usr/include/libxml2'])
AC_SUBST([xml2_CFLAGS])

AC_ARG_WITH([xml2-lib-path],
  [AS_HELP_STRING([--with-xml2-lib-path],
     [location of the xml2 libraries, defaults to /usr/lib])],
  [xml2_LIBS="-L$withval -lxml2"],
  [xml2_LIBS="-L/usr/lib -lxml2"])
AC_SUBST([xml2_LIBS])


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
