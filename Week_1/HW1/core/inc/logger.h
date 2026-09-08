#ifndef LOGGER_H
#define LOGGER_H

#define STUDENT_ID "20235876"
#define LOG_FILE_PATH "logs/log_" STUDENT_ID ".txt"

/* Result of a user request, written at the end of each log line */
#define LOG_RESULT_OK 1
#define LOG_RESULT_ERR 0

int write_log(int choice, const char *user_value, int result);

#endif /* LOGGER_H */
