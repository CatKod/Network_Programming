/**
 * client_b2.c - TCP client for Bài 2 (file transfer).
 *
 * Usage: ./client_b2 IPAddress PortNumber
 *
 * The program repeatedly asks the user for a file path, opens the file,
 * sends a fixed-size header (name length, payload size), then the file
 * name and finally the payload bytes. The empty string stops the program.
 *
 * Author: Hoàng Kim Vĩnh - 20235876
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#define CLOSESOCK closesocket
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
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

static int run_client(const char *ip, int port);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s IPAddress PortNumber\n", argv[0]);
        return APP_USAGE_ERROR;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Error: invalid port number '%s'\n", argv[2]);
        return APP_USAGE_ERROR;
    }

    if (net_init() != 0) {
        return APP_NETWORK_ERROR;
    }

    int exit_code = run_client(ip, port);
    net_cleanup();
    return exit_code;
}

/*
 * Extract the file name (no directory) from a path. Used to send only
 * the basename to the server, even when the user types a full path like
 * "D:/test/testfile.jpg".
 */
static void basename_of(const char *path, char *out, size_t out_size) {
    if (path == NULL || out == NULL || out_size == 0) {
        if (out && out_size > 0) {
            out[0] = '\0';
        }
        return;
    }
    const char *slash = strrchr(path, '/');
    const char *bslash = strrchr(path, '\\');
    const char *last = slash;
    if (bslash != NULL && (last == NULL || bslash > last)) {
        last = bslash;
    }
    const char *name = (last != NULL) ? last + 1 : path;
    strncpy(out, name, out_size - 1);
    out[out_size - 1] = '\0';
}

static int run_client(const char *ip, int port) {
    SOCKET fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == INVALID_SOCKET) {
        fprintf(stderr, "Error: cannot create client socket\n");
        return APP_NETWORK_ERROR;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons((unsigned short)port);
    if (inet_pton(AF_INET, ip, &address.sin_addr) != 1) {
        fprintf(stderr, "Error: invalid IPv4 address '%s'\n", ip);
        CLOSESOCK(fd);
        return APP_USAGE_ERROR;
    }
    if (connect(fd, (struct sockaddr *)&address, sizeof(address)) ==
        SOCKET_ERROR) {
        fprintf(stderr, "Error: cannot connect to %s:%d\n", ip, port);
        CLOSESOCK(fd);
        return APP_NETWORK_ERROR;
    }

    printf("[client_b2] Connected to %s:%d. Empty path to quit.\n", ip, port);
    fflush(stdout);

    char input_path[FT_MAX_NAME_LEN];
    char file_name[FT_MAX_NAME_LEN];
    char buffer[READ_BUFFER];

    while (1) {
        printf("Enter file path: ");
        fflush(stdout);
        if (read_line(input_path, sizeof(input_path)) != 0) {
            break;
        }
        if (input_path[0] == '\0') {
            break;
        }

        basename_of(input_path, file_name, sizeof(file_name));
        if (file_name[0] == '\0') {
            printf("Error: File not found\n");
            write_log("B2", input_path, LOG_RESULT_ERR);
            continue;
        }

        FILE *file = fopen(input_path, "rb");
        if (file == NULL) {
            printf("Error: File not found\n");
            write_log("B2", input_path, LOG_RESULT_ERR);
            continue;
        }

        if (fseek(file, 0, SEEK_END) != 0) {
            fclose(file);
            printf("Error: File tranfering is interupted\n");
            write_log("B2", input_path, LOG_RESULT_ERR);
            continue;
        }
        long size_long = ftell(file);
        rewind(file);
        if (size_long < 0) {
            fclose(file);
            printf("Error: File tranfering is interupted\n");
            write_log("B2", input_path, LOG_RESULT_ERR);
            continue;
        }
        unsigned long long size = (unsigned long long)size_long;
        if (size > FT_MAX_FILE_SIZE) {
            fclose(file);
            printf("Error: file too large (> %d bytes)\n", FT_MAX_FILE_SIZE);
            write_log("B2", input_path, LOG_RESULT_ERR);
            continue;
        }

        /* --- send header --------------------------------------------- */
        ft_header_t header;
        header.status = FT_STATUS_OK;
        header.name_len = (unsigned int)strlen(file_name);
        header.size = size;
        if (send_all(fd, &header, sizeof(header)) != 0) {
            fprintf(stderr, "Error: cannot send header\n");
            fclose(file);
            break;
        }
        if (send_all(fd, file_name, header.name_len) != 0) {
            fprintf(stderr, "Error: cannot send file name\n");
            fclose(file);
            break;
        }

        /* --- send payload -------------------------------------------- */
        size_t remaining = (size_t)size;
        int io_ok = 1;
        while (remaining > 0) {
            size_t want = (remaining > sizeof(buffer)) ? sizeof(buffer)
                                                        : remaining;
            size_t read_bytes = fread(buffer, 1, want, file);
            if (read_bytes == 0) {
                io_ok = 0;
                break;
            }
            if (send_all(fd, buffer, read_bytes) != 0) {
                io_ok = 0;
                break;
            }
            remaining -= read_bytes;
        }
        fclose(file);

        if (!io_ok) {
            printf("Error: File tranfering is interupted\n");
            write_log("B2", input_path, LOG_RESULT_ERR);
            /* We still try to read the server reply so the connection
             * stays in sync with the next request. */
        }

        /* --- receive the server reply ------------------------------- */
        ft_header_t reply;
        if (recv_all(fd, &reply, sizeof(reply)) != 0) {
            fprintf(stderr, "Error: cannot read server reply\n");
            break;
        }

        switch (reply.status) {
        case FT_STATUS_OK:
            printf("Successful transfering\n");
            write_log("B2", input_path, LOG_RESULT_OK);
            break;
        case FT_STATUS_EXISTS:
            printf("Error: File is existent on server\n");
            write_log("B2", input_path, LOG_RESULT_ERR);
            break;
        case FT_STATUS_NOT_FOUND:
            printf("Error: File not found\n");
            write_log("B2", input_path, LOG_RESULT_ERR);
            break;
        case FT_STATUS_INTERRUPTED:
            printf("Error: File tranfering is interupted\n");
            write_log("B2", input_path, LOG_RESULT_ERR);
            break;
        default:
            printf("Error: transfer failed (status=%u)\n",
                   (unsigned)reply.status);
            write_log("B2", input_path, LOG_RESULT_ERR);
            break;
        }
    }

    printf("[client_b2] Closing connection.\n");
    CLOSESOCK(fd);
    return APP_OK;
}