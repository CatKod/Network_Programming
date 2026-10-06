/**
 * splitter.c - Separate a string into its letter and digit components.
 *
 * Used by the B1 server to build the two reply strings required by the
 * assignment. Any character that is not A-Z / a-z / 0-9 is considered
 * invalid and the whole request is rejected with SPLIT_ERR_INVALID.
 */
#include "splitter.h"

#include <ctype.h>
#include <stddef.h>

int split_string(const char *input, char *letters, char *digits) {
    if (input == NULL || letters == NULL || digits == NULL) {
        return SPLIT_ERR_NULL;
    }

    size_t li = 0;
    size_t di = 0;

    for (const unsigned char *p = (const unsigned char *)input; *p != '\0';
         ++p) {
        if (isalpha(*p)) {
            letters[li++] = (char)*p;
        } else if (isdigit(*p)) {
            digits[di++] = (char)*p;
        } else {
            return SPLIT_ERR_INVALID;
        }
    }

    letters[li] = '\0';
    digits[di] = '\0';
    return SPLIT_OK;
}