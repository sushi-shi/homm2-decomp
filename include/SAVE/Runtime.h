#ifndef HOMM2_SAVE_RUNTIME_H
#define HOMM2_SAVE_RUNTIME_H

#include <SAVE/Snapshot.h>

namespace savegame {

bool Capture(const game& source, Snapshot& state, std::string& error);
// The caller must validate the snapshot before applying it. Parsing never
// changes the current game, including when an input file is damaged.
void Apply(game& destination, Snapshot state);
bool ReadFile(const std::string& path, Snapshot& state, std::string& error);
bool WriteFile(const std::string& path, const Snapshot& state, std::string& error);

}

#endif
