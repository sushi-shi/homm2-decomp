#ifndef HOMM2_IMPORT_LEGACYSAVE_H
#define HOMM2_IMPORT_LEGACYSAVE_H

#include <SAVE/Snapshot.h>

namespace legacy_save {

// Legacy files carry no code-page identifier. The importing user chooses it;
// the normal game only accepts native saves with explicit encoding metadata.
bool Decode(std::span<const u8> bytes, localization::TextEncoding encoding,
            savegame::Snapshot& state, std::string& error);

}

#endif
