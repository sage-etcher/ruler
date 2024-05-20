AC_INIT([ruler], [1.0], [sage.message@email.com], [], [https://github.com/sage-etcher/ruler.git])
AC_CONFIG_AUX_DIR([autoconf_aux])
AM_CONDITIONAL([WINDOWS], [test x$(echo ${OS}) = x"Windows_NT"])
AM_INIT_AUTOMAKE([-Wall foreign])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])


# Checks for programs
AC_PROG_CC
AC_PROG_INSTALL

# check for pkg-config (or pkgconf w/ symlink) program
AC_CHECK_PROG([PKG_CONFIG_CHECK], [pkg-config], [yes])
AS_IF([test x"$PKG_CONFIG_CHECK" != x"yes"], [AC_MSG_ERROR([pkg-config count not be found.])])

# Check for libraries
# check for the sdl2 library.
AC_CHECK_LIB([SDL2], [SDL_Init], [], [AC_MSG_ERROR([The SDL2 library could not be found.])])

#check for the cargs library
AC_CHECK_LIB([cargs], [cag_option_print], [], [AC_MSG_ERROR([The cargs library could not be found.])])

# Checks for header files.
AC_CHECK_HEADERS([malloc.h 
                  SDL2/SDL.h
                  cargs.h])

# Checks for typedefs, structures, and compile characteristics.

# Check for library functions.
AC_CHECK_FUNCS([])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

# vim: ts=4 sts=4 sw=4 et
