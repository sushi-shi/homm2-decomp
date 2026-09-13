// U15: compile-only layout assertions; not linked into reconstructed code.
#include <SOURCE/town.h>
#include <stddef.h>

typedef char TownRecordSize[(sizeof(town) == 100) ? 1 : -1];
typedef char TownSpellOffset[(offsetof(town, m_spells) == 0x3c) ? 1 : -1];
typedef char TownFlatSpellOffset[(offsetof(town, m_spellSlots) == 0x3c) ? 1 : -1];
typedef char TownSpellExtent[(sizeof(static_cast<town*>(0)->m_spells) == 20) ? 1 : -1];
typedef char TownCountOffset[(offsetof(town, m_spellCounts) == 0x50) ? 1 : -1];
typedef char TownCountExtent[(sizeof(static_cast<town*>(0)->m_spellCounts) == 5) ? 1 : -1];
typedef char TownTurnOffset[(offsetof(town, m_turnsOwned) == 0x55) ? 1 : -1];
typedef char TownNameOffset[(offsetof(town, m_name) == 0x57) ? 1 : -1];
typedef char TownNameExtent[(sizeof(static_cast<town*>(0)->m_name) == 13) ? 1 : -1];
