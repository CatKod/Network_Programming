#ifndef ACCOUNT_H
#define ACCOUNT_H

/* Path of the account file, relative to the project root (HW1) */
#define ACCOUNT_FILE_PATH "data/account.txt"

/* Maximum length of a username (including the null terminator) */
#define MAX_USERNAME_LEN 64

/* Possible values of the account status field */
#define ACCOUNT_STATUS_BANNED 0
#define ACCOUNT_STATUS_ACTIVE 1

/* One user account, stored as a node of a singly linked list */
typedef struct account {
    char username[MAX_USERNAME_LEN];
    int status;
    struct account *next;
} Account;

int load_accounts(const char *file_path);
Account *find_account(const char *username);
void free_accounts(void);

#endif /* ACCOUNT_H */
