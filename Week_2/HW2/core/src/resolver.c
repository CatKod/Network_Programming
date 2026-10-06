/**
 * IT4062 - Homework 2: domain name / IP address resolver.
 *
 * Implementation of the lookup logic, built on the standard resolver
 * library:
 *   - gethostbyname() resolves a domain name to its IPv4 addresses
 *   - gethostbyaddr() resolves an IPv4 address back to its host names
 *
 * The parameter is treated as an IPv4 address only when inet_pton()
 * accepts it, i.e. it is a full dotted-decimal address a.b.c.d with all
 * octets in range 0-255. Shortened numeric forms such as "1.2.3" are
 * rejected explicitly ("Not found information"), because
 * gethostbyname() would silently interpret them as an address instead
 * of a domain name. Everything else ("259.12.34.12", "google.com", ...)
 * is treated as a domain name; when the name cannot be resolved either,
 * "Not found information" is printed.
 *
 * Author: Hoàng Kim Vĩnh - 20235876
 */
#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600 /* Windows Vista+, needed for inet_pton() */
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <stdio.h>

#include "resolver.h"

static int resolve_ip(const struct in_addr *addr);
static int resolve_name(const char *name);

/**
 * Resolve the command line parameter and print the result.
 *
 * If the parameter is a valid IPv4 address, look up its host names;
 * otherwise treat it as a domain name and look up its IPv4 addresses.
 *
 * Returns RESOLVER_OK, RESOLVER_NOT_FOUND or RESOLVER_ERROR.
 */
int resolve(const char *parameter) {
    struct in_addr addr;
    int status;

#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        fprintf(stderr, "Error: cannot initialize Winsock\n");
        return RESOLVER_ERROR;
    }
#endif

    if (inet_pton(AF_INET, parameter, &addr) == 1) {
        /* Valid full dotted-decimal IPv4 address */
        status = resolve_ip(&addr);
    } else if (inet_addr(parameter) != INADDR_NONE) {
        /* Numeric form that is not a full a.b.c.d address ("1.2.3",
         * "0x7f.1", ...): not a valid IP and gethostbyname() would
         * wrongly treat it as one, so reject it right away. */
        printf("Not found information\n");
        status = RESOLVER_NOT_FOUND;
    } else {
        /* Anything else is treated as a domain name */
        status = resolve_name(parameter);
    }

#ifdef _WIN32
    WSACleanup();
#endif
    return status;
}

/**
 * Reverse lookup: print the official host name of the IPv4 address,
 * followed by the list of its alias names.
 */
static int resolve_ip(const struct in_addr *addr) {
    struct hostent *host =
        gethostbyaddr((const char *)addr, sizeof(*addr), AF_INET);

    if (host == NULL || host->h_name == NULL) {
        printf("Not found information\n");
        return RESOLVER_NOT_FOUND;
    }

    printf("Official name: %s\n", host->h_name);
    printf("Alias name:\n");
    for (char **alias = host->h_aliases; alias != NULL && *alias != NULL;
         ++alias) {
        printf("%s\n", *alias);
    }
    return RESOLVER_OK;
}

/**
 * Forward lookup: print the official IPv4 address of the domain name,
 * followed by the list of its alias (additional) addresses.
 */
static int resolve_name(const char *name) {
    struct hostent *host = gethostbyname(name);

    if (host == NULL || host->h_addrtype != AF_INET ||
        host->h_addr_list == NULL || host->h_addr_list[0] == NULL) {
        printf("Not found information\n");
        return RESOLVER_NOT_FOUND;
    }

    printf("Official IP: %s\n",
           inet_ntoa(*(struct in_addr *)host->h_addr_list[0]));
    printf("Alias IP:\n");
    for (char **ip = host->h_addr_list + 1; *ip != NULL; ++ip) {
        printf("%s\n", inet_ntoa(*(struct in_addr *)*ip));
    }
    return RESOLVER_OK;
}
