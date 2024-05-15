bin_PROGRAMS = ruler
ruler_SOURCES = main.c

# libraries
PKGLIBS=sdl2
ruler_CFLAGS  = $(shell pkg-config --cflags $(PKGLIBS))
ruler_LDFLAGS = $(shell pkg-config --libs $(PKGLIBS))

