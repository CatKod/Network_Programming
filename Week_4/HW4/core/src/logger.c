/**
 * logger.c - Append a single activity record to the project log file.
 *
 * The format mirrors the one used in HW1 so that the grading script can
 * reuse the same parser:
 *   [dd/mm/yyyy hh:mm:ss] $ <tag> $ <user value> $ <+OK|-ERR>
 *
 * Example:
 *   [06/10/2026 14:42:11] $ B1 $ 1a2b3cd $ +OK
 */
#include "logger.h"

#include <stdio.h>
#include <time.h>

int write_log(const char *tag, const char *user_value, int result) {
    FILE *file = fopen(LOG_FILE_PATH, "a");
    if (file == NULL) {
        fprintf(stderr, "Error: cannot open log file '%s'\n", LOG_FILE_PATH);
        return -1;
    }

    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);
    if (local_time == NULL) {
        fprintf(stderr, "Error: cannot get current time\n");
        fclose(file);
        return -1;
    }

    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", local_time);

    const char *result_code = (result == LOG_RESULT_OK) ? "+OK" : "-ERR";
    const char *value = (user_value == NULL) ? "" : user_value;

    int written = fprintf(file, "[%s] $ %s $ %s $ %s\n",
                          timestamp, tag, value, result_code);
    if (written < 0) {
        fprintf(stderr, "Error: cannot write to log file '%s'\n",
                LOG_FILE_PATH);
        fclose(file);
        return -1;
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "Error: cannot close log file '%s'\n", LOG_FILE_PATH);
        return -1;
    }
    return 0;
}