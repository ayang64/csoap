## Source directory


h_sources = soap-xml.h\
soap-fault.h\
soap-env.h\
soap-service.h\
soap-router.h\
soap-client.h\
soap-server.h

cc_sources = soap-xml.c\
soap-fault.c\
soap-env.c\
soap-service.c\
soap-router.c\
soap-client.c\
soap-server.c

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)
library_include_HEADERS = $(h_sources)

INCLUDES = -I$(top_srcdir)  $(LIBXML_CFLAGS)

# GENERIC_API_VERSION is 1.0
lib_LTLIBRARIES= libcsoap-1.0.la
libcsoap_1_0_la_SOURCES= $(h_sources) $(cc_sources)
#libexamplelib_1_0_la_LIBADD= sub/libsub.la
libcsoap_1_0_la_LDFLAGS= -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE) -L$(top_builddir)/nanohttp -lnanohttp-$(GENERIC_API_VERSION)
#libcsoap_1_0_la_CFLAGS= @LIBXML_CCFLAGS@








