dnl --------------------------------
dnl Initialization macros.
dnl --------------------------------

AC_INIT(libcsoap/csoap.h)
AM_CONFIG_HEADER(config.h)

dnl -----------------------------------------------
dnl Package name and version number (user defined)
dnl -----------------------------------------------

GENERIC_LIBRARY_NAME=libcsoap

#release versioning
GENERIC_MAJOR_VERSION=1
GENERIC_MINOR_VERSION=0
GENERIC_MICRO_VERSION=0

#API version (often = GENERIC_MAJOR_VERSION.GENERIC_MINOR_VERSION)
GENERIC_API_VERSION=1.0
AC_SUBST(GENERIC_API_VERSION)

#shared library versioning
GENERIC_LIBRARY_VERSION=1:0:0
#                       | | |
#                +------+ | +---+
#                |        |     |
#             current:revision:age
#                |        |     |
#                |        |     +- increment if interfaces have been added
#                |        |        set to zero if interfaces have been removed
#                                  or changed
#                |        +- increment if source code has changed
#                |           set to zero if current is incremented
#                +- increment if interfaces have been added, removed or changed


NANOHTTP_LIBRARY_NAME=nanohttp
#release versioning
NANOHTTP_MAJOR_VERSION=1
NANOHTTP_MINOR_VERSION=0
NANOHTTP_MICRO_VERSION=0

#API version (often = GENERIC_MAJOR_VERSION.GENERIC_MINOR_VERSION)
NANOHTTP_API_VERSION=1.0
AC_SUBST(NANOHTTP_API_VERSION)

#shared library versioning
NANOHTTP_LIBRARY_VERSION=1:0:0

dnl --------------------------------
dnl Package name and version number
dnl --------------------------------

AC_SUBST(GENERIC_LIBRARY_VERSION)

PACKAGE=$GENERIC_LIBRARY_NAME
AC_SUBST(GENERIC_LIBRARY_NAME)

GENERIC_VERSION=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION.$GENERIC_MICRO_VERSION
GENERIC_RELEASE=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION
AC_SUBST(GENERIC_RELEASE)
AC_SUBST(GENERIC_VERSION)

VERSION=$GENERIC_VERSION

AM_INIT_AUTOMAKE($PACKAGE, $VERSION, no-define)

dnl --------------------------------
dnl Package name and version number (nanohttp)
dnl --------------------------------

AC_SUBST(NANOHTTP_LIBRARY_VERSION)

PACKAGE=$NANOHTTP_LIBRARY_NAME
AC_SUBST(NANOHTTP_LIBRARY_NAME)

NANOHTTP_VERSION=$NANOHTTP_MAJOR_VERSION.$NANOHTTP_MINOR_VERSION.$NANOHTTP_MICRO_VERSION
NANOHTTP_RELEASE=$NANOHTTP_MAJOR_VERSION.$NANOHTTP_MINOR_VERSION
AC_SUBST(NANOHTTP_RELEASE)
AC_SUBST(NANOHTTP_VERSION)



# Process this file with autoconf to produce a configure script.
#AC_INIT(FULL-PACKAGE-NAME, VERSION, BUG-REPORT-ADDRESS)
#AC_INIT(src/csoap.c)
# AC_CONFIG_SRCDIR([csoap.c])

# Checks for programs.
AC_PROG_CC
AM_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h netdb.h string.h sys/socket.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME
AC_STRUCT_TM

 Checks for library functions.
#AC_FUNC_MALLOC
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([gethostbyname socket])

AC_CHECK_TYPE(ssize_t, int)

dnl
dnl The following new parameters were added to offer
dnl the ability to specify the location of the libxml
dnl library during linking and compilation.
dnl
dnl original work - Mathieu Lacage 30/03/2000
dnl

LIBXML_CONFIG_PREFIX=""
LIBXML_SRC=""

AC_ARG_WITH(libxml-prefix,
        [  --with-libxml-prefix=[PFX]		Specify location of libxml config],
	LIBXML_CONFIG_PREFIX=$withval
)
        
AC_ARG_WITH(libxml-include-prefix,
        [  --with-libxml-include-prefix=[PFX]	Specify location of libxml headers],
        LIBXML_CFLAGS="-I$withval"
)

AC_ARG_WITH(libxml-libs-prefix,
        [  --with-libxml-libs-prefix=[PFX]	Specify location of libxml libs],
        LIBXML_LIBS="-L$withval"
)

AC_ARG_WITH(libxml-src,
	[  --with-libxml-src=[DIR]              For libxml thats not installed yet (sets all three above)],
	LIBXML_SRC="$withval"
)

dnl
dnl where is xml2-config
dnl

AC_SUBST(LIBXML_REQUIRED_VERSION)
AC_MSG_CHECKING(for libxml libraries >= $LIBXML_REQUIRED_VERSION) 
if test "x$LIBXML_CONFIG_PREFIX" != "x"
then
	XML_CONFIG=${LIBXML_CONFIG_PREFIX}/bin/xml2-config
else
	XML_CONFIG=xml2-config
fi

dnl
dnl imported from libxml2, c.f. #77827 
dnl
if test "${GCC}" != "yes" ; then
    case "${host}" in
          *-*-hpux* )
	       CFLAGS="${CFLAGS} -Wp,-H30000"
	       ;;
          *-dec-osf* )
               CFLAGS="${CFLAGS} -ieee"
               ;;
    esac
else
    CFLAGS="${CFLAGS} -Wall"
    case "${host}" in
          alpha*-*-linux* )
	       CFLAGS="${CFLAGS} -mieee"
	       ;;
	  alpha*-*-osf* )
	       CFLAGS="${CFLAGS} -mieee"
	       ;;
    esac
fi

dnl
dnl Override other variables if LIBXML_SRC is set
dnl

if test "x$LIBXML_SRC" != "x"
then
	CWD=`pwd`
	if cd $LIBXML_SRC
	then
		SRC_DIR=`pwd`
		XML_CONFIG=${SRC_DIR}/xml2-config
		LIBXML_CFLAGS="-I${SRC_DIR}/include"
		LIBXML_LIBS="-L${SRC_DIR}"
		cd $CWD
	else
		AC_MSG_ERROR([libxml source dir not found (${LIBXML_SRC}), typo?])
	fi
fi

dnl
dnl make sure xml2-config is executable,
dnl test version and init our variables
dnl

AC_DEFUN(VERSION_TO_NUMBER,
[`$1 | sed -e 's/libxml //' | awk 'BEGIN { FS = "."; } { printf "%d", ([$]1 * 1000 + [$]2) * 1000 + [$]3;}'`])
if ${XML_CONFIG} --libs print > /dev/null 2>&1
then
	XMLVERS=`$XML_CONFIG --version`
	if test VERSION_TO_NUMBER(echo $XMLVERS) -ge VERSION_TO_NUMBER(echo $LIBXML_REQUIRED_VERSION)
	then
		LIBXML_LIBS="$LIBXML_LIBS `$XML_CONFIG --libs`"
		LIBXML_CFLAGS="$LIBXML_CFLAGS `$XML_CONFIG --cflags`"
		AC_MSG_RESULT($XMLVERS found)
	else
		AC_MSG_ERROR(Version $XMLVERS found. You need at least libxml2 $LIBXML_REQUIRED_VERSION for this version of libxslt)
	fi
else
	AC_MSG_ERROR([Could not find libxml2 anywhere, check ftp://xmlsoft.org/.])
fi

AC_SUBST(XML_CONFIG)
AC_SUBST(LIBXML_LIBS)
AC_SUBST(LIBXML_CFLAGS)

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)


#AC_CONFIG_FILES([])
AC_OUTPUT(Makefile 
libcsoap/Makefile 
nanohttp/Makefile 
examples/Makefile
examples/nanohttp/Makefile
examples/ex1/Makefile
libcsoap-1.0.pc)
