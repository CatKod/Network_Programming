/**
 * utils.c - Small input helpers shared by the whole program.
 */
#include "utils.h"

#include <stdio.h>
#include <string.h>

/**
 * Read one line of text from stdin.
 *
 * The trailing newline character is removed. If the input line is longer
 * than the buffer, the exceeding characters are discarded so they do not
 * affect the next read.
 *
 * @param buffer      Destination buffer for the input line.
 * @param buffer_size Size of the destination buffer in bytes.
 * @return 0 on success, -1 on end-of-file or read error.
 */
int read_line(char *buffer, size_t buffer_size) {
    if (buffer == NULL || buffer_size == 0) {
        return -1;
    }

    fflush(stdout); /* make sure any prompt is displayed before reading */

    if (fgets(buffer, (int)buffer_size, stdin) == NULL) {
        buffer[0] = '\0';
        return -1;
    }

    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0'; /* strip the trailing newline */
    } else {
        /* Line longer than the buffer: discard the rest of it */
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
    }

    return 0;
}
