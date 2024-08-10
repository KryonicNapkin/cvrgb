#if !defined(CONVERT_H)
#define CONVERT_H
#include <stdint.h>

typedef uint8_t col_val;
typedef col_val* rgb_val;

typedef enum {
    TO_DEC,
    TO_HEX,
    TO_OCT,
    TO_BIN,
} convert_to_t;

typedef enum {
    ERROR = -1,
    DECIMAL,
    HEXADECIMAL,
    OCTAL,
    BINARY,
} c_rgb_t;

col_val any_to_dec(char* str, c_rgb_t type);
rgb_val dconv_str_with_type(char** vals, c_rgb_t type);
rgb_val get_rgb_val(char* str);
char* rgb_val_convert_to(col_val rgb_val, convert_to_t type);

#endif
