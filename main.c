#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "converts.h"

#define PRG_NAME        "cvrgb"
#define VALUE_FIELDS    3

#define DEC_STR_LEN_MIN 7 
#define DEC_STR_LEN_MAX 13
#define HEX_STR_LEN     8
#define OCT_STR_LEN_MIN 7
#define OCT_STR_LEN_MAX 13
#define BIN_STR_LEN     28

#define ANSI_START_CODE "\x1b["
#define ANSI_END_CODE   "\x1b[0m"

c_rgb_t check_type(char* str);
char** sep_str_fields(char *str, char* delim, size_t nmemb, size_t memb_size);
char* strinsrt(char* str, char c, size_t index);
void display_preview(uint8_t* rgb_cols, char* str);
void free_cols_vals(char** cols_vals);
void usage(char* prg_name);

int
main(int argc, char* argv[]) {
    rgb_val rgb_col_stuff;
    char buffer[64];
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
                printf("0d%d,%d,%d \n", rgb_col_stuff[0], rgb_col_stuff[1],
                       rgb_col_stuff[2]);
                break;
            case 'b':
                break;
            case 'x':
                printf("0x");
                sprintf(buffer, "%02x%02x%02x", rgb_col_stuff[0], rgb_col_stuff[1],
                       rgb_col_stuff[2]);
                for (int i = 0; i < strlen(buffer); ++i) {
                    buffer[i] = toupper(buffer[i]);
                }
                printf("%s\n", buffer);
                break;
            case 'o':
                sprintf(buffer, "0o%o,%o,%o", rgb_col_stuff[0], rgb_col_stuff[1],
                        rgb_col_stuff[2]);
                printf("%s\n", buffer);
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
        if (!strncmp(argv[3], "-p", 2) && argv[4]) {
            display_preview(rgb_col_stuff, argv[4]);
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
    char* delim = ",";
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
        char *hex_str = strinsrt(str, *delim, 2);
        char *hex_str2 = strinsrt(hex_str, *delim, 5);
        s_strs = sep_str_fields(hex_str2, delim, VALUE_FIELDS, 8);
        for (int x = 0; x < VALUE_FIELDS; ++x) {
            d_rgb[x] = any_to_dec(s_strs[x], type);
        }
        free(hex_str);
        free(hex_str2);
    } else {
        s_strs = sep_str_fields(str, delim, VALUE_FIELDS, 8);
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
display_preview(rgb_val rgb_cols, char* str) {
    printf("fg: ");
    printf("\x1b[38;2;%d;%d;%dm", rgb_cols[0], rgb_cols[1], rgb_cols[2]);
    printf("%s", str);
    printf("\x1b[0m");
    printf(" bg: ");
    printf("\x1b[48;2;%d;%d;%dm", rgb_cols[0], rgb_cols[1], rgb_cols[2]);
    printf("%s", str);
    printf("\x1b[0m\n");
}

char**
sep_str_fields(char *str, char* delim, size_t nmemb, size_t memb_size) {
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
check_type(char *str) {
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
    fprintf(stdout, "for -d -> 0dR_value,G_value,B_value (all values must be from 0 to 255)\n");
    fprintf(stdout, "for -x -> 0xR_value,G_value,B_value (all values must be from 00 to ff/FF)\n");
    fprintf(stdout, "for -o -> 0oR_value,G_value,B_value (all values must be from 0 to 377)\n");
    fprintf(stdout, "for -b -> 0bR_value,G_value,B_value (all values must be 00000000 to 11111111)\n");
}
