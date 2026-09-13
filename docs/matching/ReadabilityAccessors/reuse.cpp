// Readability audit group 2: measured structural alternatives, not build input.
// Each matrix used --min-depth 0 --max-depth 0 --state-trials 0 and a limit
// equal to the complete source product. These are before/after regressions,
// not a new retail-closure campaign. No generated TU-state probes are retained.
// Axes/manifests/results live under build/readability/<id>-{axes,manifest}.json
// and build/readability/<id>-matrix/results.json. All matrices completed.

// h22-morale, SOURCE/ARMYGRP, RVA 0x00021802: both 561 bytes, 11 relocations,
// exact to target. h22-mobility, SOURCE/HERO, RVA 0x00060f87: both 572 bytes,
// 15 relocations, exact. Retain the existing accessor's masked i32 return.
// Morale arms:
//   HAS(armyHero->m_eventFlags, HERO_EVENT_EMBARKED)
//   armyHero->IsEmbarked()
// Mobility arms (before the unchanged movePoints = seaBaseMobility store):
//   if (HAS(m_eventFlags, HERO_EVENT_EMBARKED)) {
//   if (IsEmbarked()) {

// h27-node, SOURCE/SEARCH, RVA 0x000916c0: both 278 bytes, 3 relocations,
// exact to target. Reference type and global MAP_WIDTH stride are unchanged.
//   &GetColumn(destinationX)[MAP_WIDTH * destinationY]
//   &GetNode(destinationX, destinationY)

// h69-name, SOURCE/GAME, RVA 0x00055529: explicit/private versus shared
// uppercase, both 3336 bytes with identical instruction SHA. Existing local
// retail relocation residual remains; neither is claimed as an exact closure.
//   armyName0[0] = ToUpperCp1251(armyName0[0]);
//   armyName0[0] = CyrillicToUpper(static_cast<char>(armyName0[0]));
// h69-combat, SOURCE/ARMY, RVA 0x0001b858: both 1411 bytes, 56 relocations;
// identical instruction/relocation SHA, but the existing target-normalization
// residual makes results.json exact=false despite the rounded 100% display.
//   gText[0] = ToUpperCp1251(gText[0]);
//   gText[0] = CyrillicToUpper(gText[0]);
//   strcpy(textBuf, gText); // stays after either arm

// h69-calendar, SOURCE/GAME, RVA 0x0005e8fe: independent month/week axes,
// complete 2x2 product. Same u8 lowerName19 and lowerFirst in both sites.
// Explicit/explicit: 1277 bytes; shared month: 1282; shared week: 1284;
// both shared: 1289. Retain both explicit staged-byte arms below.
// Shared alternative at each site:
lowerFirst = static_cast<u8>(CyrillicToLower(static_cast<char>(lowerName19[0])));
// Explicit alternative at each site:
if (lowerName19[0] >= 'A' && lowerName19[0] <= 'Z')
    lowerFirst = lowerName19[0] + ('a' - 'A');
else if (lowerName19[0] >= CYRILLIC_CAPITAL_A && lowerName19[0] <= CYRILLIC_CAPITAL_YA)
    lowerFirst = lowerName19[0] + (CYRILLIC_SMALL_A - CYRILLIC_CAPITAL_A);
else if (lowerName19[0] == CYRILLIC_CAPITAL_YO)
    lowerFirst = CYRILLIC_SMALL_YO;
else
    lowerFirst = lowerName19[0];

// h69-filename, SOURCE/GAME, RVA 0x0004d202: complete two-arm product.
// Explicit: 428 bytes; shared: 433. Retain the explicit u8 stage. Filtering,
// filename length and source-dot mutation remain outside both alternatives.
chr = static_cast<u8>(CyrillicToUpper(static_cast<char>(chr)));
// Explicit alternative:
if (chr >= 'a' && chr <= 'z')
    chr = chr - ('a' - 'A');
else if (chr >= CYRILLIC_SMALL_A && chr <= CYRILLIC_SMALL_YA)
    chr = chr - (CYRILLIC_SMALL_A - CYRILLIC_CAPITAL_A);
else if (chr == CYRILLIC_SMALL_YO)
    chr = CYRILLIC_CAPITAL_YO;
else
    chr = chr;

// Final retained group 2 vs the original compiler-object snapshot: all 98
// objects, 1516 non-debug sections and 42433 ordered relocations equivalent.
// Named symbols retain identity, section, offset, type and storage. Only real
// compiler counters may change their numeric suffix at an identical target.
// Reproduce: python3 -m homm2.audit.object_equivalence
//   build/readability/baseline-objects build/objdiff/base
// Exhaustive 256+256 byte behavior: python3 -m homm2.audit.readability_contracts
