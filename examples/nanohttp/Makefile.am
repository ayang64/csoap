bin_PROGRAMS=httpget httpgetcb httppost httpcpost postserver

INCLUDES=-I$(top_srcdir)/ 
nanohttp_LDFLAG=-L$(top_builddir)/nanohttp -lnanohttp-$(GENERIC_API_VERSION) \
	$(LIBSOCKET) $(LIBNSL) -lpthread

httpget_SOURCES=httpget.c
httpget_LDFLAGS=$(nanohttp_LDFLAG)

httpgetcb_SOURCES=httpgetcb.c
httpgetcb_LDFLAGS=$(nanohttp_LDFLAG)

httppost_SOURCES=httppost.c
httppost_LDFLAGS=$(nanohttp_LDFLAG)

httpcpost_SOURCES=httpcpost.c
httpcpost_LDFLAGS=$(nanohttp_LDFLAG)

postserver_SOURCES=postserver.c
postserver_LDFLAGS=$(nanohttp_LDFLAG)





