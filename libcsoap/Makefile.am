#
# $Revision: 1.15 $
#
lib_LTLIBRARIES=libcsoap.la

libcsoap_ladir=$(includedir)/libcsoap-@csoap_release@/libcsoap

libcsoap_la_SOURCES=soap-xml.c soap-fault.c soap-env.c soap-service.c        \
                    soap-router.c soap-client.c soap-server.c soap-ctx.c     \
                    soap-admin.c soap-addressing.c soap-transport.c          \
                    soap-nudp.c soap-nhttp.c soap-wsil.c

libcsoap_la_HEADERS=soap-fault.h soap-env.h soap-service.h soap-router.h     \
                    soap-client.h soap-server.h soap-ctx.h soap-addressing.h \
                    soap-transport.h soap-admin.h soap-wsil.h soap-nudp.h    \
                    soap-nhttp.h soap-xml.h

if BUILD_WITH_XMLSEC1
libcsoap_la_SOURCES+=soap-xmlsec.c
libcsoap_la_HEADERS+=soap-xmlsec.h
endif

libcsoap_la_LDFLAGS= -version-info @csoap_version@ -release @csoap_release@
libcsoap_la_CFLAGS=-I${top_srcdir} -D__CSOAP_INTERNAL=1

