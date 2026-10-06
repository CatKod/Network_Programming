#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

/*
 * Result codes exchanged by the file-transfer server and client.
 * Every transfer starts with a fixed-size header that carries the file
 * name length, the payload size and the status word below.
 */
#define FT_STATUS_OK 0
#define FT_STATUS_NOT_FOUND 1     /* client could not open the file */
#define FT_STATUS_EXISTS 2        /* file already on the server */
#define FT_STATUS_INTERRUPTED 3   /* transfer did not complete */
#define FT_STATUS_INVALID_NAME 4  /* empty or too long name */
#define FT_STATUS_TOO_BIG 5       /* above the 100 MB limit */
#define FT_STATUS_SERVER_ERROR 6  /* any other server-side failure */

/*
 * Maximum file size the server accepts (100 MB), as required by the
 * assignment. The value is stored in bytes so we do not depend on
 * any floating-point arithmetic on the wire.
 */
#define FT_MAX_FILE_SIZE (100 * 1024 * 1024)

/*
 * Maximum length of the file name field, including the null terminator.
 * 512 bytes is plenty for any sane path on Windows or POSIX.
 */
#define FT_MAX_NAME_LEN 512

/* On-the-wire header shared by both sides. Kept POD and #pragma pack'd. */
#pragma pack(push, 1)
typedef struct {
    unsigned char status;       /* one of FT_STATUS_* */
    unsigned int name_len;      /* bytes used by the name field */
    unsigned long long size;    /* payload size in bytes (0 if status != OK) */
} ft_header_t;
#pragma pack(pop)

#endif /* FILE_TRANSFER_H */