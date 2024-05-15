bin_PROGRAMS = ruler
ruler_SOURCES = main.c \
                common.h \
                arguements.c arguements.h \
                display.c display.h \
                help.c help.h \
                version.c version.h

# libraries
PKGLIBS=sdl2
ruler_CFLAGS  = $(shell pkg-config --cflags $(PKGLIBS))
ruler_LDFLAGS = $(shell pkg-config --libs $(PKGLIBS))

# vim: ts=4 sts=4 sw=4 et
