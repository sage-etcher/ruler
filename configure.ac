AC_INIT([ruler], [1.0], [sage.message@email.com], [], [https://github.com/sage-etcher/ruler.git])
AM_INIT_AUTOMAKE([-Wall foreign])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs
AC_PROG_CC
AC_PROG_INSTALL

# check for pkg-config (or pkgconf w/ symlink) program
AC_CHECK_PROG([PKG_CONFIG_CHECK], [pkg-config], [yes])
AS_IF([test x"$PKG_CONFIG_CHECK" != x"yes"], [AC_MSG_ERROR([Please install pkg-config before configuring.])])

# Check for libraries
# check for the sdl2 library.
AC_CHECK_LIB([SDL2], [SDL_Init], [], [AC_MSG_ERROR([Please install SDL2 before configuring.])])


# Checks for header files.
AC_CHECK_HEADERS([malloc.h 
                  getopt.h
                  SDL2/SDL.h])

# Checks for typedefs, structures, and compile characteristics.

# Check for library functions.
AC_CHECK_FUNCS([])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
