#
# $Revision: 1.16 $
#
lib_LTLIBRARIES=libnanohttp.la

libnanohttp_ladir=$(includedir)/nanohttp-@nanohttp_release@/nanohttp

libnanohttp_la_SOURCES=nanohttp-common.c nanohttp-socket.c nanohttp-client.c     \
                       nanohttp-server.c nanohttp-stream.c nanohttp-mime.c       \
                       nanohttp-request.c nanohttp-response.c nanohttp-base64.c  \
                       nanohttp-logging.c nanohttp-admin.c nanohttp-error.c      \
                       nanohttp-url.c

libnanohttp_la_HEADERS=nanohttp-common.h nanohttp-socket.h nanohttp-client.h     \
                       nanohttp-server.h nanohttp-stream.h nanohttp-mime.h       \
                       nanohttp-request.h nanohttp-response.h nanohttp-logging.h \
                       nanohttp-error.h nanohttp-base64.h nanohttp-admin.h       \
                       nanohttp-url.h

if BUILD_WITH_SSL
libnanohttp_la_SOURCES+=nanohttp-ssl.c
libnanohttp_la_HEADERS+=nanohttp-ssl.h
endif

libnanohttp_la_LDFLAGS= -version-info @nanohttp_version@ -release @nanohttp_release@
libnanohttp_la_CFLAGS=-I${top_srcdir} -D__NHTTP_INTERNAL=1

