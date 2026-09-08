/**
 * logger.c - Append activity records to the log file.
 */
#include "logger.h"

#include <stdio.h>
#include <time.h>

/**
 * Append one activity record to the log file.
 *
 * Line format:
 *   [dd/mm/yyyy hh:mm:ss] $ <choice> $ <user value> $ <+OK|-ERR>
 * Example:
 *   [31/03/2023 14:46:24] $ 1 $ tungbt $ +OK
 *
 * @param choice     Menu function chosen by the user (1..4).
 * @param user_value Value provided by the user (username, message...),
 *                   NULL or "" when the function has no input value.
 * @param result     LOG_RESULT_OK or LOG_RESULT_ERR.
 * @return 0 on success, -1 if the log file cannot be written.
 */
int write_log(int choice, const char *user_value, int result) {
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

    int written;
    if (user_value == NULL || user_value[0] == '\0') {
        written = fprintf(file, "[%s] $ %d $ $ %s\n", timestamp, choice, result_code);
    } else {
        written = fprintf(file, "[%s] $ %d $ %s $ %s\n", timestamp, choice, user_value,
                          result_code);
    }

    if (written < 0) {
        fprintf(stderr, "Error: cannot write to log file '%s'\n", LOG_FILE_PATH);
        fclose(file);
        return -1;
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "Error: cannot close log file '%s'\n", LOG_FILE_PATH);
        return -1;
    }
    return 0;
}
