bin_PROGRAMS=httpget httpgetcb httppost httpcpost helloserver postserver

INCLUDES=-I$(top_srcdir)/ 
LDFLAGS=-L$(top_builddir)/nanohttp -lnanohttp-$(GENERIC_API_VERSION) \
	$(LIBSOCKET) $(LIBNSL) -lpthread

httpget_SOURCES=httpget.c
httpget_LDFLAGS=$(LDFLAGS)

httpgetcb_SOURCES=httpgetcb.c
httpgetcb_LDFLAGS=$(LDFLAGS)

httppost_SOURCES=httppost.c
httppost_LDFLAGS=$(LDFLAGS)

httpcpost_SOURCES=httpcpost.c
httpcpost_LDFLAGS=$(LDFLAGS)

helloserver_SOURCES=helloserver.c
helloserver_LDFLAGS=$(LDFLAGS)

postserver_SOURCES=postserver.c
postserver_LDFLAGS=$(LDFLAGS)





