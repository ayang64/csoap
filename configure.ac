dnl
dnl package version
dnl
AC_INIT(csoap, 1.2.0)

AC_CONFIG_SRCDIR([nanohttp/nanohttp-server.c])
AC_CONFIG_SRCDIR([libcsoap/soap-server.c])
AM_INIT_AUTOMAKE

AM_CONFIG_HEADER(config.h)

dnl
dnl checks for programs
dnl
AC_LANG_C
AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LIBTOOL

dnl
dnl release and version information
dnl
csoap_major=`echo $VERSION | $AWK -F. '{print $1}'`
csoap_minor=`echo $VERSION | $AWK -F. '{print $2}'`
csoap_patch=`echo $VERSION | $AWK -F. '{print $3}'`
AC_SUBST(csoap_major)
AC_SUBST(csoap_minor)
AC_SUBST(csoap_patch)

csoap_release="$csoap_major.$csoap_minor"
csoap_version=`expr $csoap_major + $csoap_minor`":$csoap_patch:$csoap_minor"
AC_SUBST(csoap_release)
AC_SUBST(csoap_version)

nanohttp_release=$csoap_release
nanohttp_version=$csoap_version
AC_SUBST(nanohttp_release)
AC_SUBST(nanohttp_version)

dnl
dnl checks for headers
dnl
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h malloc.h netdb.h netinet/in.h stdint.h stdlib.h signal.h pthread.h string.h sys/socket.h time.h sys/time.h unistd.h io.h stdio.h stdarg.h errno.h ctype.h openssl/ssl.h openssl/rand.h openssl/err.h])

dnl
dnl Checks for typedefs, structures, and compiler characteristics.
dnl
AC_C_CONST
AC_STRUCT_TM

AC_TYPE_SIGNAL

AC_TYPE_SIZE_T
AC_CHECK_TYPE(ssize_t, int)

dnl
dnl check for functions
dnl
AC_FUNC_MALLOC
AC_FUNC_VPRINTF
AC_FUNC_STRFTIME
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([gethostbyname inet_ntoa memmove memset mkdir select socket strchr strdup strcasecmp strncasecmp strpbrk strspn strstr strtol])

dnl
dnl check for uuid
dnl
AC_CHECK_HEADERS([uuid.h])
AC_CHECK_LIB(uuid, uuid_create, [LIBUUID="-luuid"]) 
AC_CHECK_FUNCS([uuid_to_string, uuid_create])
AC_SUBST(LIBUUID)

dnl @synopsis ACX_PTHREAD([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
dnl
dnl @summary figure out how to build C programs using POSIX threads
dnl
dnl This macro figures out how to build C programs using POSIX threads.
dnl It sets the PTHREAD_LIBS output variable to the threads library and
dnl linker flags, and the PTHREAD_CFLAGS output variable to any special
dnl C compiler flags that are needed. (The user can also force certain
dnl compiler flags/libs to be tested by setting these environment
dnl variables.)
dnl
dnl Also sets PTHREAD_CC to any special C compiler that is needed for
dnl multi-threaded programs (defaults to the value of CC otherwise).
dnl (This is necessary on AIX to use the special cc_r compiler alias.)
dnl
dnl NOTE: You are assumed to not only compile your program with these
dnl flags, but also link it with them as well. e.g. you should link
dnl with $PTHREAD_CC $CFLAGS $PTHREAD_CFLAGS $LDFLAGS ... $PTHREAD_LIBS
dnl $LIBS
dnl
dnl If you are only building threads programs, you may wish to use
dnl these variables in your default LIBS, CFLAGS, and CC:
dnl
dnl        LIBS="$PTHREAD_LIBS $LIBS"
dnl        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
dnl        CC="$PTHREAD_CC"
dnl
dnl In addition, if the PTHREAD_CREATE_JOINABLE thread-attribute
dnl constant has a nonstandard name, defines PTHREAD_CREATE_JOINABLE to
dnl that name (e.g. PTHREAD_CREATE_UNDETACHED on AIX).
dnl
dnl ACTION-IF-FOUND is a list of shell commands to run if a threads
dnl library is found, and ACTION-IF-NOT-FOUND is a list of commands to
dnl run it if it is not found. If ACTION-IF-FOUND is not specified, the
dnl default action will define HAVE_PTHREAD.
dnl
dnl Please let the authors know if this macro fails on any platform, or
dnl if you have any other suggestions or comments. This macro was based
dnl on work by SGJ on autoconf scripts for FFTW (www.fftw.org) (with
dnl help from M. Frigo), as well as ac_pthread and hb_pthread macros
dnl posted by Alejandro Forero Cuervo to the autoconf macro repository.
dnl We are also grateful for the helpful feedback of numerous users.
dnl
dnl @category InstalledPackages
dnl @author Steven G. Johnson <stevenj@alum.mit.edu>
dnl @version 2006-05-29
dnl @license GPLWithACException

AC_DEFUN([ACX_PTHREAD], [
AC_REQUIRE([AC_CANONICAL_HOST])
AC_LANG_SAVE
AC_LANG_C
acx_pthread_ok=no

# We used to check for pthread.h first, but this fails if pthread.h
# requires special compiler flags (e.g. on True64 or Sequent).
# It gets checked for in the link test anyway.

# First of all, check if the user has set any of the PTHREAD_LIBS,
# etcetera environment variables, and if threads linking works using
# them:
if test x"$PTHREAD_LIBS$PTHREAD_CFLAGS" != x; then
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        AC_MSG_CHECKING([for pthread_join in LIBS=$PTHREAD_LIBS with CFLAGS=$PTHREAD_CFLAGS])
        AC_TRY_LINK_FUNC(pthread_join, acx_pthread_ok=yes)
        AC_MSG_RESULT($acx_pthread_ok)
        if test x"$acx_pthread_ok" = xno; then
                PTHREAD_LIBS=""
                PTHREAD_CFLAGS=""
        fi
        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"
fi

# We must check for the threads library under a number of different
# names; the ordering is very important because some systems
# (e.g. DEC) have both -lpthread and -lpthreads, where one of the
# libraries is broken (non-POSIX).

# Create a list of thread flags to try.  Items starting with a "-" are
# C compiler flags, and other items are library names, except for "none"
# which indicates that we try without any flags at all, and "pthread-config"
# which is a program returning the flags for the Pth emulation library.

acx_pthread_flags="pthreads none -Kthread -kthread lthread -pthread -pthreads -mthreads pthread --thread-safe -mt pthread-config"

# The ordering *is* (sometimes) important.  Some notes on the
# individual items follow:

# pthreads: AIX (must check this before -lpthread)
# none: in case threads are in libc; should be tried before -Kthread and
#       other compiler flags to prevent continual compiler warnings
# -Kthread: Sequent (threads in libc, but -Kthread needed for pthread.h)
# -kthread: FreeBSD kernel threads (preferred to -pthread since SMP-able)
# lthread: LinuxThreads port on FreeBSD (also preferred to -pthread)
# -pthread: Linux/gcc (kernel threads), BSD/gcc (userland threads)
# -pthreads: Solaris/gcc
# -mthreads: Mingw32/gcc, Lynx/gcc
# -mt: Sun Workshop C (may only link SunOS threads [-lthread], but it
#      doesn't hurt to check since this sometimes defines pthreads too;
#      also defines -D_REENTRANT)
#      ... -mt is also the pthreads flag for HP/aCC
# pthread: Linux, etcetera
# --thread-safe: KAI C++
# pthread-config: use pthread-config program (for GNU Pth library)

case "${host_cpu}-${host_os}" in
        *solaris*)

        # On Solaris (at least, for some versions), libc contains stubbed
        # (non-functional) versions of the pthreads routines, so link-based
        # tests will erroneously succeed.  (We need to link with -pthreads/-mt/
        # -lpthread.)  (The stubs are missing pthread_cleanup_push, or rather
        # a function called by this macro, so we could check for that, but
        # who knows whether they'll stub that too in a future libc.)  So,
        # we'll just look for -pthreads and -lpthread first:

        acx_pthread_flags="-pthreads pthread -mt -pthread $acx_pthread_flags"
        ;;
esac

if test x"$acx_pthread_ok" = xno; then
for flag in $acx_pthread_flags; do

        case $flag in
                none)
                AC_MSG_CHECKING([whether pthreads work without any flags])
                ;;

                -*)
                AC_MSG_CHECKING([whether pthreads work with $flag])
                PTHREAD_CFLAGS="$flag"
                ;;

		pthread-config)
		AC_CHECK_PROG(acx_pthread_config, pthread-config, yes, no)
		if test x"$acx_pthread_config" = xno; then continue; fi
		PTHREAD_CFLAGS="`pthread-config --cflags`"
		PTHREAD_LIBS="`pthread-config --ldflags` `pthread-config --libs`"
		;;

                *)
                AC_MSG_CHECKING([for the pthreads library -l$flag])
                PTHREAD_LIBS="-l$flag"
                ;;
        esac

        save_LIBS="$LIBS"
        save_CFLAGS="$CFLAGS"
        LIBS="$PTHREAD_LIBS $LIBS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Check for various functions.  We must include pthread.h,
        # since some functions may be macros.  (On the Sequent, we
        # need a special flag -Kthread to make this header compile.)
        # We check for pthread_join because it is in -lpthread on IRIX
        # while pthread_create is in libc.  We check for pthread_attr_init
        # due to DEC craziness with -lpthreads.  We check for
        # pthread_cleanup_push because it is one of the few pthread
        # functions on Solaris that doesn't have a non-functional libc stub.
        # We try pthread_create on general principles.
        AC_TRY_LINK([#include <pthread.h>],
                    [pthread_t th; pthread_join(th, 0);
                     pthread_attr_init(0); pthread_cleanup_push(0, 0);
                     pthread_create(0,0,0,0); pthread_cleanup_pop(0); ],
                    [acx_pthread_ok=yes])

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        AC_MSG_RESULT($acx_pthread_ok)
        if test "x$acx_pthread_ok" = xyes; then
                break;
        fi

        PTHREAD_LIBS=""
        PTHREAD_CFLAGS=""
done
fi

# Various other checks:
if test "x$acx_pthread_ok" = xyes; then
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Detect AIX lossage: JOINABLE attribute is called UNDETACHED.
	AC_MSG_CHECKING([for joinable pthread attribute])
	attr_name=unknown
	for attr in PTHREAD_CREATE_JOINABLE PTHREAD_CREATE_UNDETACHED; do
	    AC_TRY_LINK([#include <pthread.h>], [int attr=$attr; return attr;],
                        [attr_name=$attr; break])
	done
        AC_MSG_RESULT($attr_name)
        if test "$attr_name" != PTHREAD_CREATE_JOINABLE; then
            AC_DEFINE_UNQUOTED(PTHREAD_CREATE_JOINABLE, $attr_name,
                               [Define to necessary symbol if this constant
                                uses a non-standard name on your system.])
        fi

        AC_MSG_CHECKING([if more special flags are required for pthreads])
        flag=no
        case "${host_cpu}-${host_os}" in
            *-aix* | *-freebsd* | *-darwin*) flag="-D_THREAD_SAFE";;
            *solaris* | *-osf* | *-hpux*) flag="-D_REENTRANT";;
        esac
        AC_MSG_RESULT(${flag})
        if test "x$flag" != xno; then
            PTHREAD_CFLAGS="$flag $PTHREAD_CFLAGS"
        fi

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        # More AIX lossage: must compile with xlc_r or cc_r
	if test x"$GCC" != xyes; then
          AC_CHECK_PROGS(PTHREAD_CC, xlc_r cc_r, ${CC})
        else
          PTHREAD_CC=$CC
	fi
else
        PTHREAD_CC="$CC"
fi

AC_SUBST(PTHREAD_LIBS)
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_CC)

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$acx_pthread_ok" = xyes; then
        ifelse([$1],,AC_DEFINE(HAVE_PTHREAD,1,[Define if you have POSIX threads libraries and header files.]),[$1])
        :
else
        acx_pthread_ok=no
        $2
fi
AC_LANG_RESTORE
])dnl ACX_PTHREAD

ACX_PTHREAD(LIBS="$PTHREAD_LIBS $LIBS"; CFLAGS="$PTHREAD_CFLAGS $CFLAGS",)

AC_CHECK_FUNCS([localtime_r strtok_r])

# ------------------------------------------
# Check socket library (for Solaris)
# ------------------------------------------
AC_CHECK_LIB(socket, socket, [LIBSOCKET="-lsocket"])
AC_SUBST(LIBSOCKET)

AC_CHECK_LIB(nsl, inet_ntoa, [LIBNSL="-lnsl"])
AC_CHECK_LIB(nsl, gethostbyname, [LIBNSL="-lnsl"])
AC_SUBST(LIBNSL)

AM_PATH_XML2(2.6.0,
	CFLAGS="$CFLAGS $XML_CPPFLAGS";
	LDFLAGS="$LDFLAGS $XML_LIBS"
,
	exit 1
)

# ------------------------------------------
# Check ssl library 
# Original work at: http://autoconf-archive.cryp.to/check_ssl.html
# ------------------------------------------
AC_DEFUN([AM_PATH_SSL],
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
        ifelse([$2], , :, [$2])
    fi
],
[
    AC_MSG_RESULT(no)
    ifelse([$3], , :, [$3])
])
])dnl

AM_PATH_SSL(,
        AC_DEFINE(HAVE_SSL,1,Define to 1 if you have requested --with-ssl)
	AM_CONDITIONAL(BUILD_WITH_SSL, true)
,
        AM_CONDITIONAL(BUILD_WITH_SSL, false)
)


# ---------------------------------------
# Check xmlsec library
# ---------------------------------------
AC_DEFUN([AM_PATH_XMLSEC1],[

  AC_ARG_WITH(xmlsec1-prefix,
    AC_HELP_STRING(
      [--with-xmlsec1-prefix=PFX],
      [Prefix where libxmlsec1 is installed (optional)]
    ),
    xmlsec1_config_prefix="$withval", xmlsec1_config_prefix=""
  )

  AC_ARG_WITH(xmlsec1-exec-prefix,
    AC_HELP_STRING(
      [--with-xmlsec1-exec-prefix=PFX],
      [Exec prefix where libxmlsec1 is installed (optional)]
    ),
    xmlsec1_config_exec_prefix="$withval", xmlsec1_config_exec_prefix=""
  )

  AC_ARG_ENABLE(xmlsec1test,
    AC_HELP_STRING(
      [--disable-xmlsec1test],
      [Do not try to compile and run a test cSOAP program]
    ),,
    enable_xmlsec1test=yes
  )

  if test x$xmlsec1_config_exec_prefix != x ; then
     xmlsec1_config_args="$xmlsec1_config_args --exec-prefix=$xmlsec1_config_exec_prefix"
     if test x${XMLSEC1_CONFIG+set} != xset ; then
        XMLSEC1_CONFIG=$xmlsec1_config_exec_prefix/bin/xmlsec1-config
     fi
  fi
  if test x$xmlsec1_config_prefix != x ; then
     xmlsec1_config_args="$xmlsec1_config_args --prefix=$xmlsec1_config_prefix"
     if test x${XMLSEC1_CONFIG+set} != xset ; then
        XMLSEC1_CONFIG=$xmlsec1_config_prefix/bin/xmlsec1-config
     fi
  fi


  AC_PATH_PROG(XMLSEC1_CONFIG, xmlsec1-config, no)
  min_xmlsec1_version=ifelse([$1], ,1.0.0,[$1])
  AC_MSG_CHECKING(for libxmlsec1 - version >= $min_xmlsec1_version)
  no_xmlsec1=""
  if test "$XMLSEC1_CONFIG" = "no" ; then
    no_xmlsec1=yes
  else
    XMLSEC1_CFLAGS=`$XMLSEC1_CONFIG $xmlsec1_config_args --cflags`
    XMLSEC1_LIBS=`$XMLSEC1_CONFIG $xmlsec1_config_args --libs`
    xmlsec1_config_major_version=`$XMLSEC1_CONFIG $xmlsec1_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    xmlsec1_config_minor_version=`$XMLSEC1_CONFIG $xmlsec1_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    xmlsec1_config_micro_version=`$XMLSEC1_CONFIG $xmlsec1_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_xmlsec1test" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $XMLSEC1_CFLAGS"
      LIBS="$XMLSEC1_LIBS $LIBS"
dnl
dnl Now check if the installed libxmlsec1 is sufficiently new.
dnl (Also sanity checks the results of xmlsec1-config to some extent)
dnl
      rm -f conf.xmlsec1test
      AC_TRY_RUN([
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libxml/tree.h>
#include <xmlsec/xmlsec.h>

int main(int argc, char **argv) {

	int major, minor, micro;
	char *tmp_version;

	system("touch conf.xmlsec1test");

	/* Capture xmlsec1-config output via autoconf/configure variables */
	/* HP/UX 9 (%@#!) writes to sscanf strings */
	tmp_version = (char *)strdup("$min_xmlsec1_version");
	if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {

		printf("%s, bad version string from xmlsec1-config\n", "$min_xmlsec1_version");
		exit(1);
	}
	free(tmp_version);

	/* Test that the library is greater than our minimum version */
	if (($xmlsec1_config_major_version < major) ||
		(($xmlsec1_config_major_version == major) && ($xmlsec1_config_minor_version < minor)) ||
		(($xmlsec1_config_major_version == major) && ($xmlsec1_config_minor_version == minor) && ($xmlsec1_config_micro_version < micro))) {

		printf("\n*** An old version of libxmlsec1 (%d.%d.%d) was found.\n", $xmlsec1_config_major_version, $xmlsec1_config_minor_version, $xmlsec1_config_micro_version);
		printf("*** You need a version of libxmlsec1 newer than %d.%d.%d. The latest version of\n", major, minor, micro);
		printf("*** libxmlsec1 is always available from http://xmlsec1.sf.net.\n\n");
		printf("*** If you have already installed a sufficiently new version, this error\n");
		printf("*** probably means that the wrong copy of the xmlsec1-config shell script is\n");
		printf("*** being found. The easiest way to fix this is to remove the old version\n");
		printf("*** of libxmlsec1, but you can also set the XMLSEC1_CONFIG environment to point to the\n");
		printf("*** correct copy of xmlsec1-config. (In this case, you will have to\n");
		printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
		printf("*** so that the correct libraries are found at run-time))\n");
		return 1;
	}
	else {

		return 0;
	}
}
],, no_xmlsec1=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi

  if test "x$no_xmlsec1" = x ; then
     AC_MSG_RESULT(yes (version $xmlsec1_config_major_version.$xmlsec1_config_minor_version.$xmlsec1_config_micro_version))
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$XMLSEC1_CONFIG" = "no" ; then
       echo "*** The xmlsec1-config script installed by LIBXMLSEC1 could not be found"
       echo "*** If libxmlsec1 was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the XMLSEC1_CONFIG environment variable to the"
       echo "*** full path to xmlsec1-config."
     else
       if test -f conf.xmlsec1test ; then
        :
       else
          echo "*** Could not run libxmlsec1 test program, checking why..."
          CFLAGS="$CFLAGS $XMLSEC1_CFLAGS"
          LIBS="$LIBS $XMLSEC1_LIBS"
          AC_TRY_LINK([
#include <libxmlsec1/soap-client.h>
#include <stdio.h>
],      [ soap_client_destroy(); return 0;],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding LIBXMLSEC1 or finding the wrong"
          echo "*** version of LIBXMLSEC1. If it is not finding LIBXMLSEC1, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
          echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means LIBXMLSEC1 was incorrectly installed"
          echo "*** or that you have moved LIBXMLSEC1 since it was installed. In the latter case, you"
          echo "*** may want to edit the xmlsec1-config script: $XMLSEC1_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi

     XMLSEC1_CFLAGS=""
     XMLSEC1_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(XMLSEC1_CFLAGS)
  AC_SUBST(XMLSEC1_LIBS)
  rm -f conf.xmlsec1test
])

AM_PATH_XMLSEC1(1.2.10,
	AC_DEFINE(HAVE_XMLSEC1,1,Define to 1 if you have requested --with-xmlsec1)
	AM_CONDITIONAL(BUILD_WITH_XMLSEC1, true)
	CFLAGS="$CFLAGS $XMLSEC1_CFLAGS";
	LDFLAGS="$LDFLAGS $XMLSEC1_LIBS"
,
	AM_CONDITIONAL(BUILD_WITH_XMLSEC1, false)
)

AC_OUTPUT(Makefile 
libcsoap/Makefile 
nanohttp/Makefile 
examples/Makefile
examples/nanohttp/Makefile
examples/csoap/Makefile
libcsoap.pc
csoap-config)

echo "Configured cSOAP $csoap_major.$csoap_minor.$csoap_patch for installation in $prefix";


