bin_PROGRAMS=simpleclient simpleserver

INCLUDES=-I$(top_srcdir)/ $(LIBXML_CFLAGS)
LDFLAGS=-L$(top_builddir)/nanohttp -lnanohttp-$(GENERIC_API_VERSION) \
	$(LIBSOCKET) $(LIBNSL) -lpthread\
	-L$(top_builddir)/libcsoap/ -lcsoap-$(GENERIC_API_VERSION) \
	$(LIBXML_LIBS)

simpleclient_SOURCES=simpleclient.c
simpleclient_LDFLAGS=$(LDFLAGS)

simpleserver_SOURCES=simpleserver.c
simpleserver_LDFLAGS=$(LDFLAGS)




