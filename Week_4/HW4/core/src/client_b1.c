/**
 * client_b1.c - TCP client for Bài 1 (string splitter).
 *
 * Usage: ./client_b1 IPAddress PortNumber
 *
 * The program loops, reading a line from stdin, sending it to the
 * server (length header + payload), then printing the two substrings
 * returned by the server. The loop stops when the user types an empty
 * line.
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
#include <sys/types.h>
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
#include "logger.h"
#include "splitter.h"
#include "utils.h"

#define MAX_LINE_LEN 4096

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

    printf("[client_b1] Connected to %s:%d. Empty line to quit.\n", ip, port);
    fflush(stdout);

    char line[MAX_LINE_LEN + 1];
    while (1) {
        printf("Input string: ");
        fflush(stdout);
        if (read_line(line, sizeof(line)) != 0) {
            break;
        }
        if (line[0] == '\0') {
            break;
        }

        /* --- send: 4-byte length header + payload -------------------- */
        unsigned int length = strlen(line);
        unsigned int length_net = htonl(length);
        if (send_all(fd, &length_net, sizeof(length_net)) != 0 ||
            send_all(fd, line, length) != 0) {
            fprintf(stderr, "Error: cannot send request to server\n");
            break;
        }

        /* --- receive: status byte + lengths + payloads ---------------- */
        unsigned char status = 0;
        if (recv_all(fd, &status, 1) != 0) {
            fprintf(stderr, "Error: cannot read status byte\n");
            break;
        }

        if (status == 1 /* REPLY_ERR */) {
            printf("Error: string contains an invalid character\n");
            write_log("B1", line, LOG_RESULT_ERR);
            continue;
        }

        unsigned int ll_net = 0, dl_net = 0;
        if (recv_all(fd, &ll_net, sizeof(ll_net)) != 0) {
            fprintf(stderr, "Error: cannot read letters length\n");
            break;
        }
        unsigned int ll = ntohl(ll_net);
        unsigned int dl = 0;
        if (ll > MAX_LINE_LEN) {
            fprintf(stderr, "Error: server returned an oversized letters string\n");
            break;
        }

        char letters[MAX_LINE_LEN + 1];
        if (ll > 0 && recv_all(fd, letters, ll) != 0) {
            fprintf(stderr, "Error: cannot read letters\n");
            break;
        }
        letters[ll] = '\0';

        if (recv_all(fd, &dl_net, sizeof(dl_net)) != 0) {
            fprintf(stderr, "Error: cannot read digits length\n");
            break;
        }
        dl = ntohl(dl_net);
        if (dl > MAX_LINE_LEN) {
            fprintf(stderr, "Error: server returned an oversized digits string\n");
            break;
        }

        char digits[MAX_LINE_LEN + 1];
        if (dl > 0 && recv_all(fd, digits, dl) != 0) {
            fprintf(stderr, "Error: cannot read digits\n");
            break;
        }
        digits[dl] = '\0';

        printf("Letters: %s\n", letters);
        printf("Digits : %s\n", digits);
        write_log("B1", line, LOG_RESULT_OK);
    }

    printf("[client_b1] Closing connection.\n");
    fflush(stdout);
    CLOSESOCK(fd);
    return APP_OK;
}