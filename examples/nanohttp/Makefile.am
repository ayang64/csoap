bin_PROGRAMS=httpget httpgetcb httppost

INCLUDES=-I$(top_srcdir)/ 
LDFLAGS=-L$(top_builddir)/nanohttp -lnanohttp-$(GENERIC_API_VERSION) \
	$(LIBSOCKET) $(LIBNSL)

httpget_SOURCES=httpget.c
httpget_LDFLAGS=$(LDFLAGS)

httpgetcb_SOURCES=httpgetcb.c
httpgetcb_LDFLAGS=$(LDFLAGS)

httppost_SOURCES=httppost.c
httppost_LDFLAGS=$(LDFLAGS)





