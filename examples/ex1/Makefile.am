bin_PROGRAMS=simple
simple_SOURCES=simple.c
INCLUDES=-I$(top_srcdir)/ $(LIBXML_CFLAGS)
LDFLAGS=-L$(top_builddir)/nanohttp -lnanohttp-$(GENERIC_API_VERSION) \
	$(LIBSOCKET) $(LIBNSL) -lpthread

simple_LDFLAGS=-L$(top_builddir)/libcsoap/ -lcsoap-$(GENERIC_API_VERSION) \
$(LIBXML_LIBS)




