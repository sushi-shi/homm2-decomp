// VC6 SP5 source family g97-newgame-map-retry; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/g97-newgame-map-retry/results.json; RVA 0x75b4b
// Complete 2 arms; elapsed 1.868026s; source restored.
// Disposition: retain structured retry loop; combined full-object and ordered retail checks required.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 2673 / 072be471f71f2c32 / f12e7436ced3db53 / 91:91 / 91,0,0,0 / True
// 1 / {'family': 'structured', 'candidate': 'baseline'} / 2673 / 072be471f71f2c32 / 643e75251776ad37 / 91:91 / 91,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// |     pick_map:
// |         wrongExpansionType = false;
// |         mapExt = FindLastToken(m_mapFilename, '.');
// |         if (mapExt != NULL) {
// |             if (StrEqNoCase(mapExt, ".MX2") && xIsExpansionMap)
// |                 wrongExpansionType = true;
// |             if (StrEqNoCase(mapExt, ".MP2") && !xIsExpansionMap)
// |                 wrongExpansionType = true;
// |         }
// |         if (!wrongExpansionType) {
// |             if (xIsExpansionMap)
// |                 strcpy(gpGame->m_mapFilename, "arrax.mx2");
// |             else
// |                 strcpy(gpGame->m_mapFilename, "brokena.mp2");
// |             m_newGameInitialized = false;
// |             m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
// |         }
// |         if (giNumHumanPlayers > BROKENA_MAX_HUMAN_PLAYERS
// |             && stricmp(gpGame->m_mapFilename, "brokena.mp2") == 0)
// |             strcpy(gpGame->m_mapFilename, "slugfest.mp2");
// |         if (giNumHumanPlayers > 1 && stricmp(gpGame->m_mapFilename, "arrax.mx2") == 0)
// |             strcpy(gpGame->m_mapFilename, "fullhse.mx2");
// |
// |         strcpy(gMapName, m_mapFilename);
// |         mapHeaderRead = GetMapHeader(m_mapFilename, &m_mapHeader);
// |         if (!mapHeaderRead || giNumHumanPlayers < m_mapHeader.minHumanPlayers
// |             || giNumHumanPlayers > m_mapHeader.maxHumanPlayers) {
// |             gpGame->GetMap();
// |             goto pick_map;
// |         }
// ALTERNATIVE structured (unified delta from BEFORE):
// | --- before
// | +++ structured
// | @@ -1,4 +1,4 @@
// | -    pick_map:
// | +        for (;;) {
// |          wrongExpansionType = false;
// |          mapExt = FindLastToken(m_mapFilename, '.');
// |          if (mapExt != NULL) {
// | @@ -26,5 +26,8 @@
// |          if (!mapHeaderRead || giNumHumanPlayers < m_mapHeader.minHumanPlayers
// |              || giNumHumanPlayers > m_mapHeader.maxHumanPlayers) {
// |              gpGame->GetMap();
// | -            goto pick_map;
// | +            continue;
// |          }
// | +
// | +        break;
// | +        }
