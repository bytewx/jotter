#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

/**
 * In-memory list of notes, backed by a dynamically growing array of
 * heap-allocated, null-terminated strings.
 *
 * Must be initialized with init_notes() before use, and released with
 * free_notes() (or clean_notes()) when no longer needed. Every entry in
 * `notes[0..count-1]` is owned by the list and must not be freed by callers.
 */
typedef struct {
    char **notes;
    int count;
    int capacity;
} NotesList;

/**
 * Initializes a NotesList to an empty state and allocates its backing
 * storage. Must be called once before any other function is used on `list`.
 *
 * Terminates the process via exit(EXIT_FAILURE) if the initial allocation
 * fails.
 *
 * @param list Pointer to an uninitialized NotesList. Must not be NULL.
 */
void init_notes(NotesList* list);

/**
 * Appends a note to the in-memory list and persists it by appending to
 * the file returned by notes_file_path().
 *
 * On allocation or file I/O failure, the error is reported via perror() and
 * the function returns without modifying the list further; a partial write
 * to the file may still have occurred.
 *
 * @param text Null-terminated note content to store. Must not be NULL.
 * @param list Initialized NotesList to append to. Must not be NULL.
 */
void write_to_file(const char* text, NotesList* list);

/**
 * Loads previously saved notes from the file returned by notes_file_path()
 * into `list`, appending them to any notes already present.
 *
 * If the file does not exist, the list is left unchanged. On allocation
 * failure, the error is reported via perror() and loading stops early,
 * leaving `list` with whatever notes were read so far.
 *
 * @param list Initialized NotesList to load notes into. Must not be NULL.
 */
void load_notes(NotesList* list);

/**
 * Prints all notes currently held in `list` to stdout, one per line,
 * prefixed with their zero-based index.
 *
 * @param list Initialized NotesList to print. Must not be NULL.
 */
void print_notes(const NotesList* list);

/**
 * Frees all notes currently held in `list`, resets it back to a fresh
 * empty state (as if init_notes() had just been called), and truncates
 * the file returned by notes_file_path().
 *
 * @param list Initialized NotesList to clear. Must not be NULL.
 */
void clean_notes(NotesList* list);

/**
 * Frees all notes currently held in `list` and its backing storage,
 * leaving `list` in a zeroed, unusable state. Does not touch the notes
 * file on disk. Call this before discarding a NotesList; init_notes()
 * must be called again before the list can be reused.
 *
 * @param list Initialized NotesList to release. Must not be NULL.
 */
void free_notes(NotesList* list);

/**
 * Returns the path to the notes storage file: the value of the NOTES_FILE
 * environment variable if set, otherwise "notes.txt" in the current
 * working directory.
 *
 * @return Null-terminated path string. Never NULL. Owned by the
 *         environment or a static default; must not be freed.
 */
const char* notes_file_path(void);

#endif /** FILE_HANDLER_H */
