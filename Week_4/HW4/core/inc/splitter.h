#ifndef STRING_SPLITTER_H
#define STRING_SPLITTER_H

#include <stddef.h>

/*
 * Result codes returned by split_string(). The TCP server forwards the
 * corresponding line to the client so both ends speak the same protocol.
 */
#define SPLIT_OK 0          /* every character is letter or digit */
#define SPLIT_ERR_INVALID 1 /* some character is neither letter nor digit */
#define SPLIT_ERR_NULL 2 /* defensive, never sent over the wire */

/*
 * Split the input line into the letters-only and digits-only substrings.
 *
 * @param input   Null-terminated line received from the client.
 * @param letters Buffer that receives the letter-only substring.
 * @param digits  Buffer that receives the digit-only substring.
 * @return        SPLIT_OK on success, SPLIT_ERR_INVALID otherwise.
 */
int split_string(const char *input, char *letters, char *digits);

#endif /* STRING_SPLITTER_H */