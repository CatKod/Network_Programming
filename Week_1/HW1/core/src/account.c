/**
 * account.c - Load and query user accounts stored in account.txt.
 */
#include "account.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Head of the linked list holding every account read from file */
static Account *account_list = NULL;

/**
 * Create a new account node and add it to the account list.
 *
 * @param username Username of the account.
 * @param status   ACCOUNT_STATUS_ACTIVE or ACCOUNT_STATUS_BANNED.
 * @return 0 on success, -1 if memory allocation fails.
 */
static int add_account(const char *username, int status) {
    Account *new_account = (Account *)malloc(sizeof(Account));
    if (new_account == NULL) {
        return -1;
    }

    strncpy(new_account->username, username, MAX_USERNAME_LEN - 1);
    new_account->username[MAX_USERNAME_LEN - 1] = '\0';
    new_account->status = status;
    new_account->next = account_list;
    account_list = new_account;
    return 0;
}

/**
 * Load all accounts from the given file into memory.
 *
 * Each line of the file has the format "<username> <status>", where
 * status is 0 (banned) or 1 (active).
 *
 * @param file_path Path to the account file.
 * @return Number of accounts loaded on success, -1 on file I/O error.
 */
int load_accounts(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        return -1;
    }

    char username[MAX_USERNAME_LEN];
    int status;
    int count = 0;

    /* "%63s" must match MAX_USERNAME_LEN - 1 */
    while (fscanf(file, "%63s %d", username, &status) == 2) {
        if (add_account(username, status) != 0) {
            fclose(file);
            return -1;
        }
        count++;
    }

    if (ferror(file)) {
        fclose(file);
        return -1;
    }

    fclose(file);
    return count;
}

/**
 * Find an account by username.
 *
 * @param username Username to look for (case sensitive).
 * @return Pointer to the matching account, or NULL if not found.
 */
Account *find_account(const char *username) {
    for (Account *account = account_list; account != NULL; account = account->next) {
        if (strcmp(account->username, username) == 0) {
            return account;
        }
    }
    return NULL;
}

/**
 * Free every account node and reset the list.
 */
void free_accounts(void) {
    Account *account = account_list;
    while (account != NULL) {
        Account *next = account->next;
        free(account);
        account = next;
    }
    account_list = NULL;
}
