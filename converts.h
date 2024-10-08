/*
 * convert.h library includes types, struct and function needed for getting and
 * converting rgb to decimal
 */

#if !defined(CONVERTS_H)
/* if CONVERTS_H is not defined*/
#define CONVERTS_H                              /* define CONVERTS_H library */
/* Specific int types for better compatibility and preventing overflowing */
#include <stdint.h>

typedef uint8_t col_val;                        /* define col_val as unsigned int */
typedef col_val* rgb_val;                       /* define rgb_val array of unsigned ints */

typedef enum {
    ERROR = -1,
    DECIMAL,
    HEXADECIMAL,
    OCTAL,
    BINARY,
} c_rgb_t;                                      /* define enumuration of type c_rgb_t */

/*
 * This function does two things:
 * a) checks if string you entered is valid with the type you've provided
 * b) convert rgb color variable from any type of numbering system to decimal
 */
int64_t any_to_dec(char* str, c_rgb_t type);
/* return array of unsigned ints each one representing R, G and B values */
rgb_val get_rgb_val(char* str);

#endif                                          /* if CONVERTS_H is not defined */
