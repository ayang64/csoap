## Source directory


h_sources = soap-xml.h\
soap-fault.h\
soap-call.h

cc_sources = soap-xml.c\
soap-fault.c\
soap-call.c

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)
library_include_HEADERS = $(h_sources)

INCLUDES = -I$(top_srcdir)  $(LIBXML_CFLAGS)

# GENERIC_API_VERSION is 1.0
lib_LTLIBRARIES= libcsoap-1.0.la
libcsoap_1_0_la_SOURCES= $(h_sources) $(cc_sources)
#libexamplelib_1_0_la_LIBADD= sub/libsub.la
libcsoap_1_0_la_LDFLAGS= -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)
#libcsoap_1_0_la_CFLAGS= @LIBXML_CCFLAGS@








