#
# $Revision: 1.16 $
#
lib_LTLIBRARIES=libcsoap.la

libcsoap_ladir=$(includedir)/libcsoap-@csoap_release@/libcsoap

libcsoap_la_SOURCES=soap-addressing.c \
                    soap-admin.c      \
                    soap-client.c     \
                    soap-ctx.c        \
                    soap-env.c        \
                    soap-fault.c      \
                    soap-logging.c    \
                    soap-nhttp.c      \
                    soap-nudp.c       \
                    soap-router.c     \
                    soap-service.c    \
                    soap-server.c     \
                    soap-transport.c  \
                    soap-wsil.c       \
                    soap-xml.c

libcsoap_la_HEADERS=soap-addressing.h \
                    soap-admin.h      \
                    soap-client.h     \
                    soap-ctx.h        \
                    soap-env.h        \
                    soap-fault.h      \
                    soap-logging.h    \
                    soap-nhttp.h      \
                    soap-nudp.h       \
                    soap-router.h     \
                    soap-service.h    \
                    soap-server.h     \
                    soap-transport.h  \
                    soap-wsil.h       \
                    soap-xml.h

if BUILD_WITH_XMLSEC1
libcsoap_la_SOURCES+=soap-xmlsec.c
libcsoap_la_HEADERS+=soap-xmlsec.h
endif

libcsoap_la_LDFLAGS= -version-info @csoap_version@ -release @csoap_release@
libcsoap_la_CFLAGS=-I${top_srcdir} -D__CSOAP_INTERNAL=1

