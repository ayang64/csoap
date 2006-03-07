dnl
dnl release information
dnl
AC_INIT(csoap, 1.1.0)

dnl
dnl version information
dnl
csoap_version=1:1:0
nanohttp_version=1:1:0

AC_CONFIG_SRCDIR([nanohttp/nanohttp-server.c])
AC_CONFIG_SRCDIR([libcsoap/soap-server.c])
AM_INIT_AUTOMAKE

csoap_release=`echo $VERSION | awk -F. '{print $1"."$2}'`
AC_SUBST(csoap_release)
AC_SUBST(csoap_version)
csoap_version_dotted=`echo $csoap_version | sed 's/:/./g'`
AC_SUBST(csoap_version_dotted)

nanohttp_release=$csoap_release
AC_SUBST(nanohttp_release)
AC_SUBST(nanohttp_version)

AM_CONFIG_HEADER(config.h)

AC_LANG_C
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h malloc.h netdb.h netinet/in.h stdint.h stdlib.h signal.h pthread.h string.h sys/socket.h sys/time.h unistd.h io.h stdio.h stdarg.h errno.h ctype.h openssl/rand.h openssl/err.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_STRUCT_TM

AC_TYPE_SIGNAL

AC_TYPE_SIZE_T
AC_CHECK_TYPE(ssize_t, int)

AC_FUNC_MALLOC
AC_FUNC_VPRINTF
AC_FUNC_STRFTIME
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([gethostbyname inet_ntoa memmove memset mkdir select socket strchr strdup strncasecmp strpbrk strspn strstr strtol])

if test x_$build_os = x_solaris2.8; then
  CFLAGS="$CFLAGS -D_REENTRANT"
fi

AC_CHECK_FUNCS([localtime_r strtok_r])

# ------------------------------------------
# Check socket library (for Solaris)
# ------------------------------------------
AC_CHECK_LIB(socket, socket, [LIBSOCKET="-lsocket"])
AC_SUBST(LIBSOCKET)

AC_CHECK_LIB(nsl, inet_ntoa, [LIBNSL="-lnsl"])
AC_CHECK_LIB(nsl, gethostbyname, [LIBNSL="-lnsl"])
AC_SUBST(LIBNSL)

AM_PATH_XML2(2.6.0,CFLAGS="$CFLAGS $XML_CPPFLAGS"; LDFLAGS="$LDFLAGS $XML_LIBS",exit 1)

# ------------------------------------------
# Check ssl library 
# Original work at: http://autoconf-archive.cryp.to/check_ssl.html
# ------------------------------------------
AC_DEFUN([CHECK_SSL],
[AC_MSG_CHECKING(if ssl is wanted)
AC_ARG_WITH(ssl,
[  --with-ssl=PFX	will check PFX for ssl library
  --with-ssl		will check /usr/local/ssl /usr/lib/ssl /usr/ssl /usr/pkg /usr/local /usr for ssl library
],
[   AC_MSG_RESULT(yes)
    for dir in $withval /usr/local/ssl /usr/lib/ssl /usr/ssl /usr/pkg /usr/local /usr; do
        ssldir="$dir"
        if test -f "$dir/include/openssl/ssl.h"; then
            found_ssl="yes";
            CFLAGS="$CFLAGS -I$ssldir/include/openssl";
            break;
        fi
        if test -f "$dir/include/ssl.h"; then
            found_ssl="yes";
            CFLAGS="$CFLAGS -I$ssldir/include";
            break
        fi
    done
    if test x_$found_ssl != x_yes; then
        AC_MSG_ERROR(Cannot find ssl libraries
		Please goto http://www.openssl.org and install OpenSSL or
		install it from your os distribution)
    else
        printf "OpenSSL found in $ssldir\n";
        LIBS="$LIBS -lssl -lcrypto";
        LDFLAGS="$LDFLAGS -L$ssldir/lib";
        AC_DEFINE(HAVE_SSL,1,Define to 1 if you have requested --with-ssl)
    fi
],
[
    AC_MSG_RESULT(no)
])
])dnl

CHECK_SSL

AC_OUTPUT(Makefile 
libcsoap/Makefile 
nanohttp/Makefile 
examples/Makefile
examples/nanohttp/Makefile
examples/csoap/Makefile
libcsoap.pc
csoap-config)

