## Source directory


h_sources = csoap.h\
csoapbody.h\
csoapcall.h\
csoapenv.h\
csoaperror.h\
csoapfault.h\
csoaphttp.h\
csoaplog.h\
csoapmethod.h\
csoapparam.h\
csoapres.h\
csoapsocket.h\
csoaptypes.h\
csoapurl.h\
csoapxml.h

cc_sources = csoap.c\
csoapbody.c\
csoapcall.c\
csoapenv.c\
csoapfault.c\
csoaphttp.c\
csoaplog.c\
csoapmethod.c\
csoapparam.c\
csoapres.c\
csoapsocket.c\
csoapurl.c\
csoapxml.c

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)
library_include_HEADERS = $(h_sources)

INCLUDES = -I$(top_srcdir)  $(LIBXML_CFLAGS)

# GENERIC_API_VERSION is 1.0
lib_LTLIBRARIES= libcsoap-1.0.la
libcsoap_1_0_la_SOURCES= $(h_sources) $(cc_sources)
#libexamplelib_1_0_la_LIBADD= sub/libsub.la
libcsoap_1_0_la_LDFLAGS= -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)
#libcsoap_1_0_la_CFLAGS= @LIBXML_CCFLAGS@








