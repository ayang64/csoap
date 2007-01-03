#
# $Revision: 1.16 $
#

SUBDIRS=nanohttp \
        libcsoap \
        xsd2c    \
        wsdl2c   \
        examples 

bin_SCRIPTS=csoap-config

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA=libcsoap.pc

m4datadir=$(datadir)/aclocal
m4data_DATA=libcsoap.m4

documentation=doc/Doxyfile.api \
              doc/compile.xml  \
              doc/doxygen.css  \
              doc/footer.html  \
              doc/header.html  \
              doc/tutorial.xml \
              doc/images/logo02.gif

broken_examples=examples/nanohttp/httpcpost.c    \
                examples/nanohttp/httppost.c     \
                examples/nanohttp/postserver.c

win32_env=win32/MinGW/Makefile    \
          win32/MinGW/compile.bat \
          win32/MinGW/config.h    \
          win32/MinGW/readme.txt  \
          win32/include/wsockcompat.h

EXTRA_DIST=$(documentation) \
           $(win32_env)     \
           libcsoap.pc.in   \
           libcsoap.m4      \
           indent.pro       \
           $(broken_examples)

