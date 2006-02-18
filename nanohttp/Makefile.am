## Source directory

	
h_sources = nanohttp-common.h\
nanohttp-socket.h\
nanohttp-client.h\
nanohttp-server.h\
nanohttp-stream.h\
nanohttp-mime.h\
nanohttp-request.h\
nanohttp-response.h\
nanohttp-base64.h\
nanohttp-ssl.h


cc_sources = nanohttp-common.c\
nanohttp-socket.c\
nanohttp-client.c\
nanohttp-server.c\
nanohttp-stream.c\
nanohttp-mime.c\
nanohttp-request.c\
nanohttp-response.c\
nanohttp-base64.c\
nanohttp-ssl.c



library_includedir=$(includedir)/$(NANOHTTP_LIBRARY_NAME)-$(NANOHTTP_API_VERSION)/$(NANOHTTP_LIBRARY_NAME)
library_include_HEADERS = $(h_sources)

INCLUDES = -I$(top_srcdir) 

# GENERIC_API_VERSION is 1.0
lib_LTLIBRARIES= libnanohttp-1.0.la
libnanohttp_1_0_la_SOURCES= $(h_sources) $(cc_sources)
libnanohttp_1_0_la_LDFLAGS= -version-info $(NANOHTTP_LIBRARY_VERSION) -release $(NANOHTTP_RELEASE)









