noinst_PROGRAMS=simpleclient simpleserver echoattachments-client echoattachments-server

bin_PROGRAMS=soapclient

INCLUDES=-I$(top_srcdir)/ $(LIBXML_CFLAGS)
csoap_LDFLAG=-L$(top_builddir)/nanohttp -lnanohttp-$(GENERIC_API_VERSION) \
	$(LIBSOCKET) $(LIBNSL) -lpthread\
	-L$(top_builddir)/libcsoap/ -lcsoap-$(GENERIC_API_VERSION) \
	$(LIBXML_LIBS)

simpleclient_SOURCES=simpleclient.c
simpleclient_LDFLAGS=$(csoap_LDFLAG)

simpleserver_SOURCES=simpleserver.c
simpleserver_LDFLAGS=$(csoap_LDFLAG)

echoattachments_server_SOURCES=echoattachments-server.c
echoattachments_server_LDFLAGS=$(csoap_LDFLAG)

echoattachments_client_SOURCES=echoattachments-client.c
echoattachments_client_LDFLAGS=$(csoap_LDFLAG)

soapclient_SOURCES=soapclient.c
soapclient_LDFLAGS=$(csoap_LDFLAG)





