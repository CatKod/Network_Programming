/**
 * IT4062 - Homework 2: domain name / IP address resolver.
 *
 * The program receives one command line parameter:
 *   ./resolver parameter
 *
 * If the parameter is a valid IPv4 address, it prints the official host
 * name and the alias names of that address. Otherwise the parameter is
 * treated as a domain name and the program prints its official IPv4
 * address and the alias addresses. When nothing can be resolved, it
 * prints "Not found information".
 *
 * Author: Hoàng Kim Vĩnh - 20235876
 */
#include <stdio.h>

#include "resolver.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <domain name | IPv4 address>\n", argv[0]);
        return RESOLVER_ERROR;
    }

    return resolve(argv[1]);
}
