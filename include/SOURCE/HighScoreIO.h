#ifndef HOMM2_SOURCE_HIGH_SCORE_IO_H
#define HOMM2_SOURCE_HIGH_SCORE_IO_H

#include <Ints.h>

struct HighScoreEntry;

// A single 100-byte retail record. Reads initialize an empty entry on failure;
// the caller can retain preceding complete records and stop at a short tail.
bool ReadHighScoreEntry(i32 file, HighScoreEntry& entry);
bool WriteHighScoreEntry(i32 file, const HighScoreEntry& entry);

#endif
