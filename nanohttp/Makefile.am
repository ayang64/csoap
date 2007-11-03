#
# $Revision: 1.17 $
#
lib_LTLIBRARIES=libnanohttp.la

libnanohttp_ladir=$(includedir)/nanohttp-@nanohttp_release@/nanohttp

libnanohttp_la_SOURCES=nanohttp-admin.c    \
                       nanohttp-base64.c   \
                       nanohttp-client.c   \
                       nanohttp-common.c   \
                       nanohttp-error.c    \
                       nanohttp-logging.c  \
                       nanohttp-mime.c     \
                       nanohttp-request.c  \
                       nanohttp-response.c \
                       nanohttp-server.c   \
                       nanohttp-socket.c   \
                       nanohttp-stream.c   \
                       nanohttp-url.c

libnanohttp_la_HEADERS=nanohttp-admin.h    \
                       nanohttp-base64.h   \
                       nanohttp-client.h   \
                       nanohttp-common.h   \
                       nanohttp-error.h    \
                       nanohttp-logging.h  \
                       nanohttp-mime.h     \
                       nanohttp-request.h  \
                       nanohttp-response.h \
                       nanohttp-server.h   \
                       nanohttp-socket.h   \
                       nanohttp-stream.h   \
                       nanohttp-url.h

if BUILD_WITH_SSL
libnanohttp_la_SOURCES+=nanohttp-ssl.c
libnanohttp_la_HEADERS+=nanohttp-ssl.h
endif

libnanohttp_la_LDFLAGS= -version-info @nanohttp_version@ -release @nanohttp_release@
libnanohttp_la_CFLAGS=-I${top_srcdir} -D__NHTTP_INTERNAL=1

