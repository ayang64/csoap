bin_PROGRAMS=wsdl2c

INCLUDES=$(LIBXML_CFLAGS) -I$(top_srcdir)
LDFLAGS=$(LIBXML_LIBS)

xsd2c_root= $(top_srcdir)/xsd2c
xsd2c_objs=../xsd2c/util.o \
../xsd2c/formatter.o \
../xsd2c/obj.o \
../xsd2c/xsd2c.o \
../xsd2c/tr.o

wsdl2c_SOURCES=wsdl2c.c
wsdl2c_LDFLAGS=$(LDFLAGS) $(xsd2c_objs)

