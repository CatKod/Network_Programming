#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

/* Read one line from stdin; returns 0 on success, -1 on EOF/error. */
int read_line(char *buffer, size_t buffer_size);

/*
 * Robust wrappers around send()/recv() that keep reading/writing until
 * `count` bytes have been transferred, even when the kernel splits a
 * message into several chunks (which is exactly what the bonus "byte
 * stream" issue in the assignment describes).
 */
int send_all(int socket_fd, const void *buffer, size_t count);
int recv_all(int socket_fd, void *buffer, size_t count);

/*
 * Initialise / tear down the Winsock library on Windows. On POSIX
 * systems the macros expand to no-ops.
 */
int net_init(void);
void net_cleanup(void);

#endif /* UTILS_H */