# Jotter

A minimal interactive command-line note-taking application written in C.

Notes are kept in memory and persisted to a plain-text file, one note per
line.

## Requirements

- A C11 compiler (GCC, Clang, or MSVC)
- CMake 3.20+
- Linux, macOS, or Windows (file locking uses `flock` on POSIX and
  `LockFileEx` on Windows)

## Building

```bash
cmake -S . -B build
cmake --build build
```

This produces the `jotter` executable and the `test_notes_handler` test
binary in `build/`.

## Running

```bash
./build/jotter
```

You'll be shown a menu:

```
1) Add a note
2) Read current notes
3) Clean all notes
4) Exit
```

## Configuration

By default, notes are stored in `notes.txt` in the current working
directory. Set the `NOTES_FILE` environment variable to use a different
path:

```bash
NOTES_FILE=/path/to/my_notes.txt ./build/jotter
```

## Testing

```bash
cd build
ctest --output-on-failure
```

Tests run against an isolated file (via `NOTES_FILE`) and never touch your
real notes.

## Known limitations

- No maximum note count or file size — this is a small personal tool, not
  designed for very large note collections.
- File locking guards against corruption from concurrent instances but
  does not provide transactional guarantees across crashes.

## License

[MIT](LICENSE)
