#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../notes_handler.h"

static const char* TEST_FILE = "test_notes.txt";

static void remove_test_file(void) {
    remove(TEST_FILE);
}

static void test_init_notes_starts_empty(void) {
    NotesList list;
    init_notes(&list);

    assert(list.count == 0);
    assert(list.capacity > 0);
    assert(list.notes != NULL);

    free_notes(&list);
}

static void test_write_and_load_round_trip(void) {
    remove_test_file();

    NotesList list;
    init_notes(&list);

    write_to_file("first note", &list);
    write_to_file("second note", &list);

    assert(list.count == 2);
    assert(strcmp(list.notes[0], "first note") == 0);
    assert(strcmp(list.notes[1], "second note") == 0);

    free_notes(&list);

    NotesList reloaded;
    init_notes(&reloaded);
    load_notes(&reloaded);

    assert(reloaded.count == 2);
    assert(strcmp(reloaded.notes[0], "first note") == 0);
    assert(strcmp(reloaded.notes[1], "second note") == 0);

    free_notes(&reloaded);
    remove_test_file();
}

static void test_write_grows_capacity_past_initial(void) {
    remove_test_file();

    NotesList list;
    init_notes(&list);

    int initial_capacity = list.capacity;

    for (int i = 0; i < initial_capacity + 3; i++) {
        write_to_file("note", &list);
    }

    assert(list.count == initial_capacity + 3);
    assert(list.capacity >= list.count);

    free_notes(&list);
    remove_test_file();
}

static void test_clean_notes_empties_list_and_file(void) {
    remove_test_file();

    NotesList list;
    init_notes(&list);

    write_to_file("temporary note", &list);
    assert(list.count == 1);

    clean_notes(&list);

    assert(list.count == 0);

    FILE* file = fopen(TEST_FILE, "r");
    assert(file != NULL);

    char buffer[16];
    assert(fgets(buffer, sizeof(buffer), file) == NULL);

    fclose(file);
    free_notes(&list);
    remove_test_file();
}

static void test_load_notes_missing_file_is_noop(void) {
    remove_test_file();

    NotesList list;
    init_notes(&list);

    load_notes(&list);

    assert(list.count == 0);

    free_notes(&list);
}

static void test_notes_file_path_defaults_and_honors_env(void) {
    unsetenv("NOTES_FILE");
    assert(strcmp(notes_file_path(), "notes.txt") == 0);

    setenv("NOTES_FILE", "/tmp/custom_notes.txt", 1);
    assert(strcmp(notes_file_path(), "/tmp/custom_notes.txt") == 0);

    unsetenv("NOTES_FILE");
}

int main(void) {
    setenv("NOTES_FILE", TEST_FILE, 1);

    test_init_notes_starts_empty();
    test_write_and_load_round_trip();
    test_write_grows_capacity_past_initial();
    test_clean_notes_empties_list_and_file();
    test_load_notes_missing_file_is_noop();

    unsetenv("NOTES_FILE");
    test_notes_file_path_defaults_and_honors_env();

    printf("All tests passed.\n");

    return EXIT_SUCCESS;
}
