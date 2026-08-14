/**
 * Jotter: a minimal interactive CLI for storing plain-text notes,
 * persisted to the file returned by notes_file_path() (see
 * notes_handler.h), which defaults to "notes.txt" in the current
 * working directory and can be overridden via the NOTES_FILE
 * environment variable.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "notes_handler.h"

#define BUFFER_SIZE 1024

enum {
    CREATE_NOTE = '1',
    READ_NOTES = '2',
    CLEAN_NOTES = '3',
    EXIT_APP = '4'
};

int main(void) {
    NotesList* list = malloc(sizeof(NotesList));

    if (list == NULL) {
        perror("malloc");

        return EXIT_FAILURE;
    }

    init_notes(list);
    load_notes(list);

    char input[BUFFER_SIZE];

    printf("Welcome to Jotter!\n");

    bool running = true;

    while (running) {
        char choice[2];

        printf("Choose what you want to do: \n"
               "1) Add a note\n"
               "2) Read current notes\n"
               "3) Clean all notes\n"
               "4) Exit\n\n"
               "Your choice: ");

        if (fgets(choice, sizeof(choice), stdin) == NULL) {
            break;
        }

        int c;

        while ((c = getchar()) != '\n' && c != EOF) {}

        switch (choice[0]) {
            case CREATE_NOTE:
                printf("Enter your note: ");

                if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
                    break;
                }

                size_t input_len = strlen(input);

                if (input_len > 0 && input[input_len - 1] == '\n') {
                    input[input_len - 1] = '\0';
                } else {
                    /* Line was longer than the buffer; drain the rest. */
                    while ((c = getchar()) != '\n' && c != EOF) {}
                }

                write_to_file(input, list);

                printf("New note added!\n");

                break;
            case READ_NOTES:
                printf("Your notes: \n\n");

                print_notes(list);

                break;
            case CLEAN_NOTES:
                clean_notes(list);

                printf("Notes cleaned.\n\n");

                break;
            case EXIT_APP:
                running = false;

                break;
            default:
                printf("Invalid choice.\n\n");

                break;
        }
    }

    free_notes(list);
    free(list);

    return EXIT_SUCCESS;
}
