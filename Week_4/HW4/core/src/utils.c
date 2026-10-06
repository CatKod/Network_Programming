/**
 * utils.c - Small helpers shared by the TCP client and server programs.
 */
#include "utils.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#endif

int read_line(char *buffer, size_t buffer_size) {
    if (buffer == NULL || buffer_size == 0) {
        return -1;
    }

    fflush(stdout);

    if (fgets(buffer, (int)buffer_size, stdin) == NULL) {
        buffer[0] = '\0';
        return -1;
    }

    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    } else {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
    }
    return 0;
}

/*
 * Keep calling send() until every byte of `buffer` has been pushed to
 * the kernel. TCP does not guarantee that a single send() transfers the
 * whole payload, so this loop is required to honour the assignment's
 * "byte stream" bonus.
 */
int send_all(int socket_fd, const void *buffer, size_t count) {
    const char *cursor = (const char *)buffer;
    size_t remaining = count;

    while (remaining > 0) {
#ifdef _WIN32
        int sent = send(socket_fd, cursor, (int)remaining, 0);
#else
        ssize_t sent = send(socket_fd, cursor, remaining, 0);
#endif
        if (sent < 0) {
#ifdef _WIN32
            int err = WSAGetLastError();
            if (err == WSAEINTR || err == WSAEWOULDBLOCK) {
                continue;
            }
#else
            if (errno == EINTR) {
                continue;
            }
#endif
            return -1;
        }
        if (sent == 0) {
            return -1;
        }
        cursor += sent;
        remaining -= (size_t)sent;
    }
    return 0;
}

/*
 * Same idea as send_all(), but for the receiving side. The caller knows
 * how many bytes to expect (typically because a fixed header has been
 * exchanged first), so the function refuses to return until that exact
 * amount has been read or an error occurs.
 */
int recv_all(int socket_fd, void *buffer, size_t count) {
    char *cursor = (char *)buffer;
    size_t remaining = count;

    while (remaining > 0) {
#ifdef _WIN32
        int received = recv(socket_fd, cursor, (int)remaining, 0);
#else
        ssize_t received = recv(socket_fd, cursor, remaining, 0);
#endif
        if (received < 0) {
#ifdef _WIN32
            int err = WSAGetLastError();
            if (err == WSAEINTR || err == WSAEWOULDBLOCK) {
                continue;
            }
#else
            if (errno == EINTR) {
                continue;
            }
#endif
            return -1;
        }
        if (received == 0) {
            return -1; /* peer closed the connection */
        }
        cursor += received;
        remaining -= (size_t)received;
    }
    return 0;
}

int net_init(void) {
#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        fprintf(stderr, "Error: cannot initialize Winsock\n");
        return -1;
    }
#endif
    return 0;
}

void net_cleanup(void) {
#ifdef _WIN32
    WSACleanup();
#else
    /* nothing to do on POSIX */
#endif
}