bin_PROGRAMS = ruler
ruler_SOURCES = main.c \
                str_utils.c str_utils.h \
                common.h default.h \
                settings.c settings.h \
                arguments.c arguments.h \
                runtime.h \
                log_helper.c log_helper.h \
                display.c display.h \
                window.c window.h \
                render.c render.h \
                events.c events.h \
                background.c background.h \
                nonportable_windows.c nonportable_windows.h

# libraries
PKGLIBS=sdl2 SDL2_image cargs
ruler_CFLAGS  = $(shell pkg-config --cflags $(PKGLIBS))
ruler_LDFLAGS = $(shell pkg-config --libs $(PKGLIBS))

if WINDOWS
ruler_LDFLAGS += -mconsole
endif


# vim: ts=4 sts=4 sw=4 et
