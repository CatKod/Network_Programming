#ifndef COMMON_H
#define COMMON_H

/* Student identifier used in log filenames and personal markings */
#define STUDENT_ID "20235876"
#define STUDENT_NAME "HoangKimVinh"

/* Exit codes shared by every program */
#define APP_OK 0
#define APP_USAGE_ERROR 1
#define APP_NETWORK_ERROR 2
#define APP_FILE_ERROR 3

/* Path used to log every client request (relative to the project root) */
#define LOG_FILE_PATH "logs/hw4_" STUDENT_ID ".log"

/* Path used by the file-transfer server to keep uploaded files */
#define SERVER_STORAGE_DIR "server_storage"

#endif /* COMMON_H */