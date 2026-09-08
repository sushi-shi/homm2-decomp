# Save replacement

`game::SaveGame` writes the unchanged retail save format through a
`platform::FileTransaction`. It resolves the destination once, exclusively
creates a unique `.homm2-save-*.tmp` file in the same directory, writes every
record, checks flush and close, then replaces the destination. Existing
case-insensitive path resolution and UTF-8 filenames are preserved. Player-name
and rumour serialization buffers start zeroed, so unused text padding does not
include uninitialized stack bytes.

A creation, write, flush, close or replacement failure leaves the old save
intact. Abandoned transactions remove their temporary file during normal scope
exit; commit failures also clean it up before reporting the existing file error.
A killed process or fatal game shutdown may leave an unused temporary sibling.
Its `.tmp` extension keeps it out of the save chooser, and a later save uses a
different exclusive filename. The destination is never deleted before rename.

On POSIX hosts, buffered data is flushed and `fsync` succeeds on the temporary
file before close and same-directory rename. The parent directory is then
synced. If that final directory sync fails, replacement has already happened:
the complete new save remains visible and a diagnostic explains that its
directory entry could not be confirmed durable. Windows uses a checked CRT
file-data commit and `MoveFileExW` with replacement and write-through flags.
These guarantees still depend on the host filesystem and storage device.

The Web build flushes and replaces the complete file inside Emscripten's virtual
filesystem. Its existing IDBFS `autoPersist` mount later synchronizes changes
to IndexedDB asynchronously. A successful in-game save therefore means the
complete file is visible in the current session; it is not a synchronous
browser-storage acknowledgment. Closing the tab immediately or a browser quota
failure can lose the latest update. See the upstream
[IDBFS persistence contract](https://emscripten.org/docs/api_reference/Filesystem-API.html#idbfs),
[SDL stream flush contract](https://wiki.libsdl.org/SDL3/SDL_FlushIO), and
[Windows replacement flags](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-movefileexw).

The `file_transaction` CTest uses the production transaction and SDL filesystem
inside an isolated temporary user-data directory. A delegating fault injector
fails at nine write positions, creation, flush, checked close and replacement.
It verifies old-file contents and temporary cleanup, concurrent transactions,
existing filename case, UTF-8 names and reading the committed file back through
the production filesystem. Three subprocesses flush different temporary-file
prefixes and terminate without destructors or commit, verifying that the old
destination remains intact.

```sh
nix develop -c cmake -S . -B build
nix develop -c cmake --build build --target homm2_file_transaction_test
ctest --test-dir build -R '^file_transaction$' --output-on-failure
```

The filesystem round trip checks file replacement and readback. Development
validation also started Broken Alliance with installed retail assets and created
a 108,635-byte `.GM1` save through the game. Two fresh-process load/save/quit
cycles both exited normally and replaced the destination; their final save
files were byte-identical after initializing the text padding. The retail
assets and generated saves are not included in the repository.
