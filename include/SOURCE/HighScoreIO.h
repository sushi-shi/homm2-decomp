#ifndef HOMM2_SOURCE_HIGHSCOREIO_H
#define HOMM2_SOURCE_HIGHSCOREIO_H

#include <H2/Ints.h>

struct HighScoreEntry;

typedef enum HighScoreReadResult {
    // The record was read; empty slots are complete records too.
    HIGH_SCORE_READ_COMPLETE  = 0,
    // All 100 bytes were consumed but a name is unterminated. The entry is
    // left empty and the following records can still be read.
    HIGH_SCORE_READ_INVALID   = 1,
    // The file ended inside the record; no later record can be read.
    HIGH_SCORE_READ_TRUNCATED = 2
} HighScoreReadResult;

// A single 100-byte retail record. Unless the result is complete, the entry
// is initialized empty, so callers retain every other valid record.
HighScoreReadResult ReadHighScoreEntry(i32 file, HighScoreEntry& entry);
bool WriteHighScoreEntry(i32 file, const HighScoreEntry& entry);

#endif
