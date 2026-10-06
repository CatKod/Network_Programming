#ifndef LOGGER_H
#define LOGGER_H

#include "common.h"

/* Result of a client request, written at the end of each log line */
#define LOG_RESULT_OK 1
#define LOG_RESULT_ERR 0

int write_log(const char *tag, const char *user_value, int result);

#endif /* LOGGER_H */