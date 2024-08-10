#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "converts.h"

/* Constants definition */
#define PRG_NAME        "cvrgb"

#define VALUE_FIELDS    3
#define DELIM           ","

#define DEC_STR_LEN_MIN 7 
#define DEC_STR_LEN_MAX 13
#define HEX_STR_LEN     8
#define OCT_STR_LEN_MIN 7
#define OCT_STR_LEN_MAX 13
#define BIN_STR_LEN     28

#define ANSI_START_CODE "\x1b["
#define ANSI_END_CODE   "\x1b[0m"

/* Function declarations */
/* Check type */
c_rgb_t check_type(char* str);
/* Separate str to Red, Green and Blue fields */
char** sep_str_fields(char* str, char* delim, size_t nmemb, size_t memb_size);
/* Insert delim into hexadecimal str to separate red, green and blue values */
char* strinsrt(char* str, char c, size_t index);
/* Stolen implementation of itoa() function from https://www.strudel.org.uk/itoa/ ver 0.4 */
char* itoa(int value, char* result, int base);
/* Display the actual color in your terminal */
void dpy_color(uint8_t* rgb_cols, char* str);
/* Free the rgb color char arrays */
void free_cols_vals(char** cols_vals);
/* Check type */
void usage(char* prg_name);

int
main(int argc, char* argv[]) {
    rgb_val rgb_col_stuff;
    char *buffer = malloc(64 * sizeof(char));
    if (argc >= 3 && argv[1][0] == '-' && argv[2]) {
        char* rgb_str = strdup(argv[2]);
        rgb_col_stuff = get_rgb_val(rgb_str);
        if (rgb_col_stuff == NULL) {
            free(rgb_str);
            fprintf(stderr, "Invalid format passed as rgb value: %s\n", argv[2]);
            exit(EXIT_FAILURE);
        }
        free(rgb_str);
        switch (argv[1][1]) {
            case 'd':
                printf("0d%d,%d,%d", rgb_col_stuff[0], rgb_col_stuff[1],
                       rgb_col_stuff[2]);
                break;
            case 'b':
                printf("0b");
                for (int i = 0; i < VALUE_FIELDS; ++i) {
                    itoa(rgb_col_stuff[i], buffer, 2);
                    size_t len = strlen(buffer);
                    if (len < 8) {
                        for (size_t x = 0; x < (8 - len); ++x) {
                            printf("0");
                        }
                    }
                    if (i == 2) {
                        printf("%s", buffer);
                    } else {
                        printf("%s,", buffer);
                    }
                }
                break;
            case 'x':
                printf("0x");
                sprintf(buffer, "%02x%02x%02x", rgb_col_stuff[0], rgb_col_stuff[1],
                       rgb_col_stuff[2]);
                for (size_t i = 0; i < strlen(buffer); ++i) {
                    buffer[i] = toupper(buffer[i]);
                }
                printf("%s", buffer);
                break;
            case 'o':
                sprintf(buffer, "0o%o,%o,%o", rgb_col_stuff[0], rgb_col_stuff[1],
                        rgb_col_stuff[2]);
                printf("%s", buffer);
                break;
            case 'h':
                usage(PRG_NAME);
                exit(EXIT_SUCCESS);
                break;
            default:
                fprintf(stderr, "Unknown option %s", argv[1]);
                exit(EXIT_FAILURE);
                break;
        }
        free(buffer);
        if (argc >= 5 && !strncmp(argv[3], "-p", 2)) {
            printf("\n");
            dpy_color(rgb_col_stuff, argv[4]);
        }
    } else {
        usage(PRG_NAME);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

rgb_val 
get_rgb_val(char* str) {
    char** s_strs = malloc(VALUE_FIELDS * sizeof(char*));
    rgb_val d_rgb = malloc(VALUE_FIELDS * sizeof(uint8_t));
    for (int i = 0; i < VALUE_FIELDS; ++i) {
        s_strs[i] = malloc(8 * sizeof(char));
    }
    c_rgb_t type = check_type(str);
    if (type == ERROR) { 
        free_cols_vals(s_strs);
        free(d_rgb);
        return NULL; 
    }
    str += 2;
    if (type == HEXADECIMAL) {
        char* hex_str = strinsrt(str, *DELIM, 2);
        char* hex_str2 = strinsrt(hex_str, *DELIM, 5);
        s_strs = sep_str_fields(hex_str2, DELIM, VALUE_FIELDS, 8);
        for (int x = 0; x < VALUE_FIELDS; ++x) {
            d_rgb[x] = any_to_dec(s_strs[x], type);
        }
        free(hex_str);
        free(hex_str2);
    } else {
        s_strs = sep_str_fields(str, DELIM, VALUE_FIELDS, 8);
        for (int x = 0; x < VALUE_FIELDS; ++x) {
            d_rgb[x] = any_to_dec(s_strs[x], type);
        }
    }
    free_cols_vals(s_strs);
    return d_rgb;
}

char*
strinsrt(char* str, char c, size_t index) {
    size_t len = strlen(str);
    size_t nlen = len + 1 + 1;
    char* nstr = malloc(nlen * sizeof(char));
    if (nstr == NULL) {
        return NULL;
    }
    if (index < len) {
        memcpy(nstr, str, len);
        for (size_t i = len; i > index-1; --i) {
            memmove(&nstr[i], &nstr[i-1], 1);
        }
        memset(&nstr[nlen - 1], '\0', 1);
        memset(&nstr[index], c, 1);
        return nstr;
    }
    free(nstr);
    return NULL;
}

void 
dpy_color(rgb_val rgb_cols, char* str) {
    if (!isatty(fileno(stdout))) {
        fprintf(stderr, "You must run the program inside of terminal!\n");
        exit(EXIT_FAILURE);
    }
    printf("fg: ");
    printf(ANSI_START_CODE "38;2;%d;%d;%dm", rgb_cols[0], rgb_cols[1], rgb_cols[2]);
    printf(" %s ", str);
    printf(ANSI_END_CODE);
    printf(" bg: ");
    printf(ANSI_START_CODE "30m");
    printf(ANSI_START_CODE "48;2;%d;%d;%dm", rgb_cols[0], rgb_cols[1], rgb_cols[2]);
    printf(" %s ", str);
    printf(ANSI_END_CODE "\n");
}

char**
sep_str_fields(char* str, char* delim, size_t nmemb, size_t memb_size) {
    char** str_fields = malloc(nmemb * sizeof(char*));
    for (size_t i = 0; i < nmemb; ++i) {
        str_fields[i] = malloc(memb_size * sizeof(char));
    }
    char* token = strtok(str, delim);
    size_t n = 0;
    while (token != NULL) {
        str_fields[n] = strdup(token);
        ++n;
        token = strtok(NULL, delim);
    }
    return str_fields;
}

c_rgb_t 
check_type(char* str) {
    size_t len = strlen(str);
    if (!strncmp(str, "0x", 2) && len == HEX_STR_LEN) {   /* len 10 -> 0x1424fe */
        return HEXADECIMAL;
    } else if (!strncmp(str, "0o", 2) && len >= OCT_STR_LEN_MIN && len <= OCT_STR_LEN_MAX) {  /* len =7 - =13 -> 0o1,3,4 - 0o102,104,203 */
        return OCTAL;
    } else if (!strncmp(str, "0d", 2) && len >= DEC_STR_LEN_MIN && len <= DEC_STR_LEN_MAX) {   /*  len =7 - =12 -> 0d1,2,3 - 0d123,230,124 */
        return DECIMAL;
    } else if (!strncmp(str, "0b", 2) && len == BIN_STR_LEN) { /* len == 28 -> 0b01011010,10110101,10100101*/
        return BINARY;
    }
    return ERROR;
}

void 
free_cols_vals(char** cols_vals) {
    for (int i = 0; i < VALUE_FIELDS; ++i) {
        free(cols_vals[i]);
    }
    free(cols_vals);
}

void usage(char* prg_name) {
    fprintf(stdout, "usage: %s [OPTIONS] <rgb_value> (-p) <str>\n", prg_name);
    fprintf(stdout, "OPTIONS specify to which type of numbering system you want to convert the rgb_value\n\n");
    fprintf(stdout, "OPTIONS:\n");
    fprintf(stdout, "\t-h\tprint this message\n");
    fprintf(stdout, "\t-d\tto decimal numbering system (base 10)\n");
    fprintf(stdout, "\t-x\tto hexadeciam numbering system (base 16)\n");
    fprintf(stdout, "\t-o\tto octal numbering system (base 8)\n");
    fprintf(stdout, "\t-b\tto binary numbering system (base 2)\n\n");
    fprintf(stdout, "-p option is optional\n");
    fprintf(stdout, "\t-p\twill print the str in the color you've passed earlier\n\n");
    fprintf(stdout, "Correct format of <rgb_value>:\n");
    fprintf(stdout, "for -d -> 0dR_value%1$sG_value%1$sB_value (all values must be from 0 to 255)\n", DELIM);
    fprintf(stdout, "for -x -> 0xR_value%1$sG_value%1$sB_value (all values must be from 00 to ff/FF)\n", DELIM);
    fprintf(stdout, "for -o -> 0oR_value%1$sG_value%1$sB_value (all values must be from 0 to 377)\n", DELIM);
    fprintf(stdout, "for -b -> 0bR_value%1$sG_value%1$sB_value (all values must be 00000000 to 11111111)\n", DELIM);
}	

char* 
itoa(int value, char* result, int base) {
    /* check that the base if valid */
    if (base < 2 || base > 36) { *result = '\0'; return result; }
    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while (value);

    /* Apply negative sign */
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}
