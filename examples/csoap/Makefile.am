bin_PROGRAMS=simpleclient simpleserver echoattachments-client echoattachments-server

INCLUDES=-I$(top_srcdir)/ $(LIBXML_CFLAGS)
LDFLAGS=-L$(top_builddir)/nanohttp -lnanohttp-$(GENERIC_API_VERSION) \
	$(LIBSOCKET) $(LIBNSL) -lpthread\
	-L$(top_builddir)/libcsoap/ -lcsoap-$(GENERIC_API_VERSION) \
	$(LIBXML_LIBS)

simpleclient_SOURCES=simpleclient.c
simpleclient_LDFLAGS=$(LDFLAGS)

simpleserver_SOURCES=simpleserver.c
simpleserver_LDFLAGS=$(LDFLAGS)

echoattachments_server_SOURCES=echoattachments-server.c
echoattachments_server_LDFLAGS=$(LDFLAGS)

echoattachments_client_SOURCES=echoattachments-client.c
echoattachments_client_LDFLAGS=$(LDFLAGS)





