#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <sys/file.h>
#endif

#include "jotter.h"

#define BUFFER_SIZE 1024
#define DEFAULT_NOTES_FILE "notes.txt"

typedef enum {
    NOTES_LOCK_SHARED,
    NOTES_LOCK_EXCLUSIVE
} NotesLockMode;

/** Locks the whole file backing `file`. Returns 0 on success, -1 on failure. */
static int lock_file(FILE* file, NotesLockMode mode) {
#ifdef _WIN32
    HANDLE handle = (HANDLE)_get_osfhandle(_fileno(file));
    OVERLAPPED overlapped = {0};
    DWORD flags = (mode == NOTES_LOCK_EXCLUSIVE) ? LOCKFILE_EXCLUSIVE_LOCK : 0;

    return LockFileEx(handle, flags, 0, MAXDWORD, MAXDWORD, &overlapped) ? 0 : -1;
#else
    int operation = (mode == NOTES_LOCK_EXCLUSIVE) ? LOCK_EX : LOCK_SH;

    return flock(fileno(file), operation);
#endif
}

/** Unlocks the whole file backing `file`. */
static void unlock_file(FILE* file) {
#ifdef _WIN32
    HANDLE handle = (HANDLE)_get_osfhandle(_fileno(file));
    OVERLAPPED overlapped = {0};

    UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &overlapped);
#else
    flock(fileno(file), LOCK_UN);
#endif
}

/** @see jotter.h */
const char* notes_file_path(void) {
    const char* path = getenv("NOTES_FILE");

    return path != NULL ? path : DEFAULT_NOTES_FILE;
}

/** @see jotter.h */
void init_notes(NotesList* list) {
    list->count = 0;
    list->capacity = 2;
    list->notes = malloc(list->capacity * sizeof(char*));

    if (list->notes == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
}

/** @see jotter.h */
void write_to_file(const char* text, NotesList* list) {
    if (list->count == list->capacity) {
        list->capacity *= 2;

        char** temp = realloc(list->notes, list->capacity * sizeof(char*));

        if (temp == NULL) {
            perror("realloc");

            return;
        }

        list->notes = temp;
    }

    list->notes[list->count] = malloc(strlen(text) + 1); // adding one because of the null-terminator
                                                             // example: "hello" in memory: [ h | e | l | l | o | \0 ] <- absence of null-terminator triggers SIGSEGV or UB

    if (list->notes[list->count] == NULL) {
        perror("malloc");

        return;
    }

    strcpy(list->notes[list->count], text);
    list->count++;

    FILE* file = fopen(notes_file_path(), "a");

    if (file == NULL) {
        perror("fopen");

        return;
    }

    if (lock_file(file, NOTES_LOCK_EXCLUSIVE) != 0) {
        perror("lock_file");
        fclose(file);

        return;
    }

    size_t text_len = strlen(text);

    if (fwrite(text, sizeof(char), text_len, file) != text_len ||
        fputc('\n', file) == EOF) {
        perror("fwrite");
    }

    unlock_file(file);
    fclose(file);
}

/** @see jotter.h */
void load_notes(NotesList* list) {
    FILE* file = fopen(notes_file_path(), "r");

    if (file == NULL) {
        return;
    }

    if (lock_file(file, NOTES_LOCK_SHARED) != 0) {
        perror("lock_file");
        fclose(file);

        return;
    }

    char buffer[BUFFER_SIZE];

    while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
        size_t buffer_len = strlen(buffer);

        if (buffer_len > 0 && buffer[buffer_len - 1] == '\n') {
            buffer[buffer_len - 1] = '\0';
        }

        if (list->count == list->capacity) {
            list->capacity *= 2;

            char** temp = realloc(list->notes, list->capacity * sizeof(char*));

            if (temp == NULL) {
                perror("realloc");

                return;
            }

            list->notes = temp;
        }

        list->notes[list->count] = malloc(strlen(buffer) + 1);

        if (list->notes[list->count] == NULL) {
            perror("malloc");
            fclose(file);

            return;
        }

        strcpy(list->notes[list->count], buffer);
        list->count++;
    }

    unlock_file(file);
    fclose(file);
}

/** @see jotter.h */
void print_notes(const NotesList* list) {
    for (int i = 0; i < list->count; i++) {
        printf("[%d] %s\n", i, list->notes[i]);
    }
}

/** @see jotter.h */
void clean_notes(NotesList* list) {
    for (int i = 0; i < list->count; i++) {
        free(list->notes[i]);
    }

    free(list->notes);
    list->notes = NULL;
    list->count = 0;
    list->capacity = 0;

    init_notes(list);

    FILE* file = fopen(notes_file_path(), "w");

    if (file == NULL) {
        perror("fopen");

        return;
    }

    if (lock_file(file, NOTES_LOCK_EXCLUSIVE) != 0) {
        perror("lock_file");
    }

    unlock_file(file);
    fclose(file);
}

/** @see jotter.h */
void free_notes(NotesList* list) {
    for (int i = 0; i < list->count; i++) {
        free(list->notes[i]);
    }

    free(list->notes);
    list->notes = NULL;
    list->count = 0;
    list->capacity = 0;
}
