// VC6 SP5 source family c76-newgame-map-domain; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/c76-newgame-map-domain/results.json; RVA 0x75b4b
// Complete 3 arms; elapsed 2.464215s; source restored.
// Disposition: retain all tested destination/promotion cast removals; map domain keeps inner i16 truncation and uses enum constructor.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 2673 / 072be471f71f2c32 / f12e7436ced3db53 / 91:91 / 91,0,0,0 / True
// 1 / {'family': 'direct_enum', 'candidate': 'baseline'} / 2672 / b8bfd6e415047960 / 4e847544bceb5ff7 / 91:91 / 91,0,0,0 / True
// 2 / {'family': 'keep_word', 'candidate': 'baseline'} / 2673 / 072be471f71f2c32 / f7d42f632491ad03 / 91:91 / 91,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// | static_cast<NewGameMapChoice>(static_cast<i16>(gpWindowManager->m_dialogResult))
// ALTERNATIVE direct_enum (unified delta from BEFORE):
// | --- before
// | +++ direct_enum
// | @@ -1 +1 @@
// | -static_cast<NewGameMapChoice>(static_cast<i16>(gpWindowManager->m_dialogResult))
// | +NewGameMapChoice(gpWindowManager->m_dialogResult)
// ALTERNATIVE keep_word (unified delta from BEFORE):
// | --- before
// | +++ keep_word
// | @@ -1 +1 @@
// | -static_cast<NewGameMapChoice>(static_cast<i16>(gpWindowManager->m_dialogResult))
// | +NewGameMapChoice(static_cast<i16>(gpWindowManager->m_dialogResult))
