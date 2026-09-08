/**
 * IT4062 - Homework 1: User account management.
 *
 * The program loads user accounts from data/account.txt, then shows a
 * menu that lets the user log in, post a message, log out or exit. Every
 * request is appended to the activity log file in logs/ (see logger.h).
 *
 * Run the executable from the project root (HW1) so the relative paths
 * to data/ and logs/ resolve correctly.
 *
 * Author: Hoàng Kim Vĩnh - 20235876
 */
#include <stdio.h>
#include <stdlib.h>

#include "account.h"
#include "logger.h"
#include "utils.h"

/* Maximum length of a posted message (including the null terminator) */
#define MAX_MESSAGE_LEN 1024

/* Menu function identifiers, also written to the log file */
enum menu_choice {
    MENU_LOG_IN = 1,
    MENU_POST_MESSAGE = 2,
    MENU_LOG_OUT = 3,
    MENU_EXIT = 4
};

/* Currently logged-in account, NULL when nobody is logged in */
static Account *current_user = NULL;

static void print_menu(void);
static void handle_log_in(void);
static void handle_post_message(void);
static void handle_log_out(void);

/**
 * Program entry point: load the account file then run the menu loop.
 */
int main(void) {
    if (load_accounts(ACCOUNT_FILE_PATH) < 0) {
        fprintf(stderr, "Error: cannot read account file '%s'\n", ACCOUNT_FILE_PATH);
        return EXIT_FAILURE;
    }

    int running = 1;
    while (running) {
        print_menu();
        printf("Your choice: ");

        char choice_input[16];
        if (read_line(choice_input, sizeof(choice_input)) != 0) {
            break; /* end of input: stop the program gracefully */
        }

        int choice;
        if (sscanf(choice_input, "%d", &choice) != 1) {
            printf("Invalid choice. Please enter a number from 1 to 4.\n");
            continue;
        }

        switch (choice) {
        case MENU_LOG_IN:
            handle_log_in();
            break;
        case MENU_POST_MESSAGE:
            handle_post_message();
            break;
        case MENU_LOG_OUT:
            handle_log_out();
            break;
        case MENU_EXIT:
            write_log(MENU_EXIT, "", LOG_RESULT_OK);
            running = 0;
            break;
        default:
            printf("Invalid choice. Please enter a number from 1 to 4.\n");
            break;
        }
    }

    free_accounts();
    return EXIT_SUCCESS;
}

/**
 * Print the main menu.
 */
static void print_menu(void) {
    printf("\n----------- MENU -----------\n");
    printf("1. Log in\n");
    printf("2. Post message\n");
    printf("3. Logout\n");
    printf("4. Exit\n");
    printf("----------------------------\n");
}

/**
 * Function 1: log in with a username typed from the keyboard.
 *
 * The login succeeds only when nobody is logged in yet, the account
 * exists in the account file and the account is active.
 */
static void handle_log_in(void) {
    char username[MAX_USERNAME_LEN];

    printf("Username: ");
    if (read_line(username, sizeof(username)) != 0) {
        return;
    }

    if (current_user != NULL) {
        printf("You have already logged in\n");
        write_log(MENU_LOG_IN, username, LOG_RESULT_ERR);
        return;
    }

    Account *account = find_account(username);
    if (account == NULL) {
        printf("Account is not exist\n");
        write_log(MENU_LOG_IN, username, LOG_RESULT_ERR);
        return;
    }

    if (account->status != ACCOUNT_STATUS_ACTIVE) {
        printf("Account is banned\n");
        write_log(MENU_LOG_IN, username, LOG_RESULT_ERR);
        return;
    }

    current_user = account;
    printf("Hello %s\n", current_user->username);
    write_log(MENU_LOG_IN, username, LOG_RESULT_OK);
}

/**
 * Function 2: read a message from the keyboard and post it.
 *
 * The message is always read first; the post only succeeds when the
 * user has already logged in.
 */
static void handle_post_message(void) {
    char message[MAX_MESSAGE_LEN];

    printf("Post message: ");
    if (read_line(message, sizeof(message)) != 0) {
        return;
    }

    if (current_user == NULL) {
        printf("You have not logged in.\n");
        write_log(MENU_POST_MESSAGE, message, LOG_RESULT_ERR);
        return;
    }

    printf("Successful post\n");
    write_log(MENU_POST_MESSAGE, message, LOG_RESULT_OK);
}

/**
 * Function 3: log the current user out.
 */
static void handle_log_out(void) {
    if (current_user == NULL) {
        printf("You have not logged in.\n");
        write_log(MENU_LOG_OUT, "", LOG_RESULT_ERR);
        return;
    }

    current_user = NULL;
    printf("Successful log out\n");
    write_log(MENU_LOG_OUT, "", LOG_RESULT_OK);
}
