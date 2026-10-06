/**
 * server_b2.c - TCP server for Bài 2 (file transfer).
 *
 * Workflow per connection:
 *   1. Read a fixed-size ft_header_t.
 *   2. Read the file name (header.name_len bytes).
 *   3. If the file already exists in SERVER_STORAGE_DIR, send back
 *      FT_STATUS_EXISTS and close.
 *   4. Otherwise read `header.size` bytes of payload and write them to
 *      disk. Any read/write error triggers FT_STATUS_INTERRUPTED.
 *
 * The size limit of 100 MB is enforced both client- and server-side.
 *
 * Usage: ./server_b2 PortNumber
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define MKDIR(path) _mkdir(path)
#define CLOSESOCK closesocket
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define MKDIR(path) mkdir(path, 0777)
#define CLOSESOCK close
#define SOCKET int
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#endif

#include "common.h"
#include "file_transfer.h"
#include "logger.h"
#include "utils.h"

#define READ_BUFFER 64 * 1024

static int handle_client(SOCKET client_fd);
static int run_server(int port);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s PortNumber\n", argv[0]);
        return APP_USAGE_ERROR;
    }

    int port = atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Error: invalid port number '%s'\n", argv[1]);
        return APP_USAGE_ERROR;
    }

    if (net_init() != 0) {
        return APP_NETWORK_ERROR;
    }

    int exit_code = run_server(port);
    net_cleanup();
    return exit_code;
}

static int run_server(int port) {
    SOCKET listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == INVALID_SOCKET) {
        fprintf(stderr, "Error: cannot create listening socket\n");
        return APP_NETWORK_ERROR;
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR,
               (const char *)&opt, sizeof(opt));

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons((unsigned short)port);

    if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) ==
        SOCKET_ERROR) {
        fprintf(stderr, "Error: cannot bind to port %d\n", port);
        CLOSESOCK(listen_fd);
        return APP_NETWORK_ERROR;
    }
    if (listen(listen_fd, 8) == SOCKET_ERROR) {
        fprintf(stderr, "Error: cannot listen on port %d\n", port);
        CLOSESOCK(listen_fd);
        return APP_NETWORK_ERROR;
    }

    MKDIR(SERVER_STORAGE_DIR);
    printf("[server_b2] Listening on port %d, storing files in '%s'\n",
           port, SERVER_STORAGE_DIR);
    fflush(stdout);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        SOCKET client_fd =
            accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == INVALID_SOCKET) {
            fprintf(stderr, "Error: cannot accept new connection\n");
            continue;
        }

        char ip[INET_ADDRSTRLEN] = "?";
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
        printf("[server_b2] Connection from %s:%d\n", ip,
               ntohs(client_addr.sin_port));
        fflush(stdout);

        /* Service every file transfer sent by this client until the
         * connection drops. This matches the assignment requirement:
         * one TCP connection can carry several uploads in a row. */
        while (handle_client(client_fd) == 0) {
        }

        CLOSESOCK(client_fd);
        printf("[server_b2] Connection from %s closed\n", ip);
        fflush(stdout);
    }

    CLOSESOCK(listen_fd);
    return APP_OK;
}

/*
 * Sanity-check a file name sent by a remote client. Only basename-like
 * names are accepted: no path separators, no "..", not too long.
 */
static int is_safe_name(const char *name) {
    if (name == NULL || name[0] == '\0') {
        return 0;
    }
    size_t length = strlen(name);
    if (length >= FT_MAX_NAME_LEN) {
        return 0;
    }
    for (size_t i = 0; i < length; ++i) {
        char ch = name[i];
        if (ch == '/' || ch == '\\' || ch == ':' || ch == '*' ||
            ch == '?' || ch == '"' || ch == '<' || ch == '>' || ch == '|') {
            return 0;
        }
    }
    return 1;
}

/*
 * Tell the client whether the transfer succeeded. We always reply with
 * a complete header so the client can rely on its framing logic.
 */
static int send_reply(SOCKET fd, unsigned char status,
                      const char *name, unsigned long long size) {
    ft_header_t h;
    h.status = status;
    h.name_len = (unsigned int)strlen(name);
    h.size = size;
    return send_all(fd, &h, sizeof(h));
}

static int handle_client(SOCKET client_fd) {
    ft_header_t header;
    if (recv_all(client_fd, &header, sizeof(header)) != 0) {
        fprintf(stderr, "Error: cannot read transfer header\n");
        return -1;
    }

    if (header.name_len == 0 || header.name_len >= FT_MAX_NAME_LEN) {
        fprintf(stderr, "Error: invalid file name length (%u)\n",
                header.name_len);
        send_reply(client_fd, FT_STATUS_INVALID_NAME, "", 0);
        return -1;
    }

    char name[FT_MAX_NAME_LEN];
    if (recv_all(client_fd, name, header.name_len) != 0) {
        fprintf(stderr, "Error: cannot read file name\n");
        return -1;
    }
    name[header.name_len] = '\0';

    if (!is_safe_name(name)) {
        fprintf(stderr, "Error: unsafe file name '%s'\n", name);
        send_reply(client_fd, FT_STATUS_INVALID_NAME, "", 0);
        write_log("B2", name, LOG_RESULT_ERR);
        return -1;
    }

    if (header.size > FT_MAX_FILE_SIZE) {
        fprintf(stderr, "Error: file '%s' too large (%llu bytes)\n",
                name, header.size);
        send_reply(client_fd, FT_STATUS_TOO_BIG, "", 0);
        write_log("B2", name, LOG_RESULT_ERR);
        return -1;
    }

    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", SERVER_STORAGE_DIR, name);

    FILE *file = fopen(path, "rb");
    if (file != NULL) {
        fclose(file);
        printf("[server_b2] File '%s' already exists -> reject\n", name);
        send_reply(client_fd, FT_STATUS_EXISTS, name, 0);
        write_log("B2", name, LOG_RESULT_ERR);
        return 0;
    }

    file = fopen(path, "wb");
    if (file == NULL) {
        fprintf(stderr, "Error: cannot create '%s'\n", path);
        send_reply(client_fd, FT_STATUS_SERVER_ERROR, "", 0);
        write_log("B2", name, LOG_RESULT_ERR);
        return -1;
    }

    unsigned long long remaining = header.size;
    char buffer[READ_BUFFER];
    int transfer_ok = 1;

    while (remaining > 0) {
        size_t want = (remaining > sizeof(buffer)) ? sizeof(buffer)
                                                    : (size_t)remaining;
#ifdef _WIN32
        int received = recv(client_fd, buffer, (int)want, 0);
#else
        ssize_t received = recv(client_fd, buffer, want, 0);
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
            transfer_ok = 0;
            break;
        }
        if (received == 0) {
            transfer_ok = 0; /* peer closed before all bytes received */
            break;
        }
        if (fwrite(buffer, 1, received, file) != (size_t)received) {
            transfer_ok = 0;
            break;
        }
        remaining -= (unsigned long long)received;
    }

    fclose(file);

    if (!transfer_ok || remaining != 0) {
        printf("[server_b2] Transfer of '%s' interrupted (%llu bytes left)\n",
               name, remaining);
        remove(path);
        send_reply(client_fd, FT_STATUS_INTERRUPTED, name, 0);
        write_log("B2", name, LOG_RESULT_ERR);
        return -1;
    }

    printf("[server_b2] File '%s' received (%llu bytes) -> OK\n",
           name, header.size);
    send_reply(client_fd, FT_STATUS_OK, name, header.size);
    write_log("B2", name, LOG_RESULT_OK);
    fflush(stdout);
    return 0;
}