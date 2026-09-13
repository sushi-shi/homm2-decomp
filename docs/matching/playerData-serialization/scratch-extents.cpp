// S34: complete safe layout family, retained original reservation.
// Writer: build/s34-write-{axes,manifest}.json and -batch/results.json.
// Six arms (capacity,clear): (52,48),(52,42),(48,48),(48,42),(44,42),(42,42).
// All571 bytes/27 relocs; only52/48 strict exact. 6/6 in8.493s, restored,
// no truncation. Same one-block/211-instruction shape cannot hide slot drift.
// Reader: build/s34-read-{axes,manifest}.json and -batch/results.json.
// Four capacities52/48/44/42, each reads42; all556 bytes/26 relocs.
// Only52 strict exact; 4/4 in5.609s, restored, no truncation.
// Representative structural arms (complete prefixes in axes artifacts):
void writer_original(int file) {
    char unused[52];
    // unchanged preceding field writes
    memset(unused, 0, 48);
    write(file, unused, 42);
}
void writer_compact(int file) {
    char unused[42];
    // unchanged preceding field writes
    memset(unused, 0, 42);
    write(file, unused, 42);
}
// Intermediate writer arms use capacities52,48,44 and the safe clear lengths
// enumerated above. Reader uses char unused[capacity]; read(file, unused, 42).
// +3 sub esp38 becomes34 for capacity48; this save/loads move ebp-38 to-34.
// Reducing only clear changes immediate+85 from30 to2a. No fake padding kept.
// Independent retained retail bytes/sites/ordered targets PASS for both;
// see docs/reconstruction/S34-C35.md. No inference of original array spelling.
