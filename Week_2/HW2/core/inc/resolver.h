#ifndef RESOLVER_H
#define RESOLVER_H

/* Exit codes of the program */
#define RESOLVER_OK 0        /* information found and printed */
#define RESOLVER_NOT_FOUND 1 /* no information for the parameter */
#define RESOLVER_ERROR 2     /* wrong usage / internal error */

int resolve(const char *parameter);

#endif /* RESOLVER_H */
