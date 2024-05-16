bin_PROGRAMS = ruler
ruler_SOURCES = main.c \
                common.h \
                arguments.c arguments.h \
                display.c display.h

# libraries
PKGLIBS=sdl2 cargs
ruler_CFLAGS  = $(shell pkg-config --cflags $(PKGLIBS))
ruler_LDFLAGS = $(shell pkg-config --libs $(PKGLIBS))

# vim: ts=4 sts=4 sw=4 et
