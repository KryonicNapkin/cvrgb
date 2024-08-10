#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "converts.h"                           /* Include converts.h library */

/* Check for return value of errno and convert str to decimal */
col_val 
any_to_dec(char *str, c_rgb_t type) {
    col_val col_num = 0;
    char *endptr = NULL;
    errno = 0;
    switch (type) {
        case DECIMAL:
            col_num = strtol(str, &endptr, 10);
            break;
        case HEXADECIMAL:
            col_num = strtol(str, &endptr, 16);
            break;
        case OCTAL:
            col_num = strtol(str, &endptr, 8);
            break;
        case BINARY:
            col_num = strtol(str, &endptr, 2);
            break;
        case ERROR:
            break;
        default:
            return 0;
            break;
    }
    if (errno == EINVAL || str == endptr) {
        fprintf(stderr, "Error occured when converting %s to the type %d\n", str, type);
        return 0;
    }
    return col_num;
}
