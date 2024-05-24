AC_INIT([ruler], [1.0], [sage.message@email.com], [], [https://github.com/sage-etcher/ruler.git])
AC_CONFIG_AUX_DIR([autoconf_aux])
AM_INIT_AUTOMAKE([-Wall foreign])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_INSTALL
AM_PROG_AR

# check for the -mconsole & -mwindows flags, determine if we should use a windows build from them
AX_CHECK_COMPILE_FLAG([-mconsole], [WINDOWS_FLAG=yes], [WINDOWS_FLAG=no], [-Werror -mwindows])
AM_CONDITIONAL([WINDOWS], [test "$WINDOWS_FLAG" = "yes"])

# Check for libraries
PKG_CHECK_MODULES([SDL2],       [sdl2],,       [AC_MSG_ERROR([The SDL2 library could not be found.])])
PKG_CHECK_MODULES([SDL2_IMAGE], [SDL2_image],, [AC_MSG_ERROR([The SDL2_Image library could not be found.])])
PKG_CHECK_MODULES([CARGS],      [cargs],,      [AC_MSG_ERROR([The library could not be found.])])

# Checks for header files.
AC_CHECK_HEADERS([malloc.h])
AC_CHECK_HEADERS([SDL2/SDL.h])
AC_CHECK_HEADERS([SDL2/SDL_image.h])
AC_CHECK_HEADERS([cargs.h])

# Checks for typedefs, structures, and compile characteristics.

# Check for library functions.
AC_CHECK_FUNCS([])

AC_CONFIG_FILES([
    Makefile
    src/tinyfd/Makefile
    src/Makefile
])

AC_OUTPUT

# vim: ts=4 sts=4 sw=4 et
