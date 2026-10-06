/**
 * server_b1.c - TCP server for Bài 1 (string splitter).
 *
 * Workflow per connection:
 *   1. Read a length header (4 bytes, unsigned int, network byte order).
 *   2. Read exactly that many bytes of payload (the user line).
 *   3. Split the line into letters and digits.
 *   4. Send a status byte followed by the two resulting strings.
 *
 * The protocol is fully framed, which satisfies the bonus requirement
 * of "handling the byte stream issue on TCP sockets".
 *
 * Usage: ./server_b1 PortNumber
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

/* Maximum length of the user line (excluding the header). */
#define MAX_LINE_LEN 4096

/* Reply status byte sent to the client. */
#define REPLY_OK 0
#define REPLY_ERR 1

static int handle_client(SOCKET client);
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

    printf("[server_b1] Listening on port %d (student %s)...\n",
           port, STUDENT_ID);
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
        printf("[server_b1] Connection from %s:%d\n", ip,
               ntohs(client_addr.sin_port));
        fflush(stdout);

        /* Service every frame sent by this client until the connection
         * drops. This matches the assignment requirement: a single TCP
         * connection can carry several requests in a row. */
        while (handle_client(client_fd) == 0) {
        }

        CLOSESOCK(client_fd);
        printf("[server_b1] Connection from %s closed\n", ip);
        fflush(stdout);
    }

    CLOSESOCK(listen_fd);
    return APP_OK;
}

/*
 * Service a single client: read a 4-byte length header, then the payload,
 * then reply with the two resulting strings. Returns 0 on success.
 */
static int handle_client(SOCKET client_fd) {
    /* --- 1. read 4-byte length header ---------------------------------- */
    unsigned int len_net = 0;
    if (recv_all(client_fd, &len_net, sizeof(len_net)) != 0) {
        fprintf(stderr, "Error: cannot read length header\n");
        return -1;
    }
    unsigned int length = ntohl(len_net);

    if (length == 0 || length > MAX_LINE_LEN) {
        fprintf(stderr, "Error: invalid length header (%u)\n", length);
        return -1;
    }

    /* --- 2. read the actual payload ----------------------------------- */
    char line[MAX_LINE_LEN + 1];
    if (recv_all(client_fd, line, length) != 0) {
        fprintf(stderr, "Error: cannot read payload\n");
        return -1;
    }
    line[length] = '\0';

    /* --- 3. split the line --------------------------------------------- */
    char letters[MAX_LINE_LEN + 1];
    char digits[MAX_LINE_LEN + 1];
    int rc = split_string(line, letters, digits);

    /* --- 4. send the framed reply -------------------------------------- */
    unsigned char status = (rc == SPLIT_OK) ? REPLY_OK : REPLY_ERR;
    if (send_all(client_fd, &status, 1) != 0) {
        fprintf(stderr, "Error: cannot send status byte\n");
        return -1;
    }

    if (status == REPLY_OK) {
        unsigned int ll = strlen(letters);
        unsigned int dl = strlen(digits);
        unsigned int ll_net = htonl(ll);
        unsigned int dl_net = htonl(dl);

        if (send_all(client_fd, &ll_net, sizeof(ll_net)) != 0 ||
            send_all(client_fd, letters, ll) != 0 ||
            send_all(client_fd, &dl_net, sizeof(dl_net)) != 0 ||
            send_all(client_fd, digits, dl) != 0) {
            fprintf(stderr, "Error: cannot send result strings\n");
            return -1;
        }

        printf("[server_b1] Input: '%s' -> letters='%s', digits='%s'\n",
               line, letters, digits);
        write_log("B1", line, LOG_RESULT_OK);
    } else {
        printf("[server_b1] Input: '%s' -> Error (invalid character)\n", line);
        write_log("B1", line, LOG_RESULT_ERR);
    }
    fflush(stdout);
    return 0;
}