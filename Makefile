#########################################################
#
# CSOAP Makefile
#
#########################################################

SHELL	= bash
CC	= gcc


VPATH	= . src

LIBNAME	= libsoap.a
EXLIB	= -L. -lsoap

SOURCE  = ${wildcard src/csoap*.c}
OBJECTS = $(addsuffix .o, $(basename $(SOURCE)))

CFLAGS	= `xml2-config --cflags` # -fwritable-strings
CLIBS	= `xml2-config --libs`

lib: $(OBJECTS)
	ar -rc $(LIBNAME) $(OBJECTS)

shared: $(OBJECTS)
	gcc --shared -o libsoap.dll $(OBJECTS) `xml2-config --libs`

example1: lib
	$(CC) -Isrc $(CFLAGS) ex1.c -o $@ $(EXLIB) $(CLIBS)

clean:
	-@rm -f src/*.o
	-@rm -f $(LIBNAME)
	-@rm -f *~
	-@rm -f src/*~