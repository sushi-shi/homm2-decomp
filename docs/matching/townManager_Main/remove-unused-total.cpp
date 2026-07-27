// Structural attempt: remove the unused top-level `i32 total`.
//
// MSVC reserved a frame slot even though the local had no reads or writes.
// Removing it made the frame exactly 592 bytes and raised clean state to
// 99.327360%, while retaining exact size and 215/215 relocations.
// Matrix: build/match-variants/townmain-unused-total-20260727

// No replacement: the declaration was not part of the function semantics.
