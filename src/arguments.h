#ifndef ARGUMENTS_H
#define ARGUMENTS_H
#include "extern-c-helper.h"
EXTERN_C_START()

#include "settings.h"


void parse_arguments (settings_obj *s, int argc, char **argv);
const char *get_shortcuts (void);

EXTERN_C_END()
#endif /* ARGUMENTS_H */

/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
