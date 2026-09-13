// U15 structural recovery, parent77db5b7fe; not a spelling/TU-state matrix.
// Before, in the packed town class:
// union {
//   SpellType8 m_spells[5][4]; SpellType8 m_spellSlots[20];
//   struct { char m_spellPad[19]; i8 m_spellCounts[6]; };
// };
// After (SpellType8 here denotes the existing H2_ENUM_STORAGE(SpellType,i8)):
// union { SpellType8 m_spells[5][4]; SpellType8 m_spellSlots[20]; };
// i8 m_spellCounts[5];
// Every former count[i] becomes count[i-1], simplified to level where the
// previous expression was level+1. The six exact mappings are in U15.md.
// Both full builds pass. build/u15-all-native-layout.log proves all98 objects,
//1826 emitted function instances, every allocated section and ordered native
//relocation/addend unchanged; standalone VC6 layout assertions pass.
// Five consumers pass the general whole-retail checker. The sixth AI consumer
//passes all1975 ordinary bytes/sites/other targets; its13 private floats pass
//the fixed-site supplement. No fake padding or biased pointer retained.
