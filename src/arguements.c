#include "arguements.h"


static int valid_hexstr (const char *str);

static int 
valid_hexstr (const char *str)
{
    /*{{{*/
    char *iter = (char *)str;

    if (iter[0] != '0' || iter[1] != 'x')
        return 0;

    iter += 2;

    while (*iter != '\0')
    {
        if (!isxdigit (*iter))
            return 0;

        iter++;
    }

    return 1;
    /*}}}*/
}


void
parse_arguments (int argc, char **argv)
{
    /*{{{*/
    int ret;
    int c;
    struct option long_options[] =
    {
    /*   NAME       ARGUEMENT          FLAG        SHORTNAME */
        {"color",   required_argument, NULL,       'c'},
        {"colour",  required_argument, NULL,       'c'},
        {"opacity", required_argument, NULL,       'o'},
        {"width",   required_argument, NULL,       'W'},
        {"height",  required_argument, NULL,       'H'},
        {"help",    no_argument,       NULL,       'h'},
        {"version", no_argument,       NULL,       'V'},
        {NULL,      0,                 NULL,       0},
    };
    int option_index = 0;

    while ((c = getopt_long (argc, argv, "c:o:W:H:hV",
                             long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'c':
            if (!valid_hexstr (optarg))
                print_help (stderr, EXIT_FAILURE);
            sscanf (optarg, "%x", &g_hex);
            break;
        case 'o':
            sscanf (optarg, "%f", &g_opacity);
            break;
        case 'W':
            sscanf (optarg, "%u", &g_width);
            break;
        case 'H':
            sscanf (optarg, "%u", &g_height);
            break;
        case 'V':
            print_version (stdout, EXIT_SUCCESS);
        case 'h':
            print_help (stdout, EXIT_SUCCESS);
        default:
            print_help (stderr, EXIT_FAILURE);
        }
    }

    return;
    /*}}}*/
}


/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
