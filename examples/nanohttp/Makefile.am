bin_PROGRAMS=httpget httpgetcb

httpget_SOURCES=httpget.c
INCLUDES=-I$(top_srcdir)/ $(LIBXML_CFLAGS)
httpget_LDFLAGS=-L$(top_builddir)/nanohttp -lnanohttp-$(GENERIC_API_VERSION) \
$(LIBXML_LIBS)

httpgetcb_SOURCES=httpgetcb.c
httpgetcb_LDFLAGS=-L$(top_builddir)/nanohttp -lnanohttp-$(GENERIC_API_VERSION) \
$(LIBXML_LIBS)




