
bin_PROGRAMS = ruler
ruler_SOURCES = main.c \
                default.h \
                runtime.h settings.c settings.h imgmode.h color.h \
                str_utils.c str_utils.h \
                arguments.c arguments.h \
                log_helper.c log_helper.h \
                display.c display.h \
                window.c window.h \
                render.c render.h \
                events.c events.h \
                background.c background.h \
                nonportable_windows.c nonportable_windows.h

# PKGCONFIG variables set by ./configure 
ruler_CFLAGS  = ${CARGS_CFLAGS}  ${SDL2_CFLAGS}  ${SDL2_IMAGE_CFLAGS}
ruler_LDFLAGS = ${CARGS_LDFLAGS} ${SDL2_LDFLAGS} ${SDL2_IMAGE_LDFLAGS}
ruler_LDADD   = ${CARGS_LIBS}    ${SDL2_LIBS}    ${SDL2_IMAGE_LIBS} 

# other libraries 
ruler_LDADD   += -ltinyfd -lm

# windows only
if WINDOWS
ruler_LDFLAGS += -mconsole
endif


# vim: ts=4 sts=4 sw=4 et
