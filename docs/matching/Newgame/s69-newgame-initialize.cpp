// VC6 SP5 source family s69-newgame-initialize; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/s69-newgame-initialize/results.json; RVA 0x756e8
// Complete 3 arms; elapsed 2.541858s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 1028 / af3cc992987c1223 / a6e63cfbc9b5371e / 60:60 / 60,0,0,0 / True
// 1 / {'family': 'drop_unread_local', 'candidate': 'baseline'} / 1021 / 2a736819dd7098e2 / 46d39ece961f28fd / 60:60 / 59,1,0,0 / True
// 2 / {'family': 'drop_unread_store', 'candidate': 'baseline'} / 1021 / 9a82901f17e5e052 / 46d39ece961f28fd / 60:60 / 59,1,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// |  0x404)
// | void game::InitNewGame(struct SMapHeader* header) {
// |     i32 humanCount;
// |     NewGamePlayerSetupType playerType;
// |     i32 player;
// |     i32 H2_UNUSED(unusedTally);
// |     i32 activeColorCount;
// |     i32 computerCount;
// |
// |     activeColorCount = 0;
// |     unusedTally = 0;
// |     humanCount = 0;
// |     computerCount = 0;
// |
// |     if (m_newGameInitialized && m_newGameHumanCount == giNumHumanPlayers) {
// |         for (player = 0; player < m_mapHeader.playerCount; ++player) {
// |             if (m_setupPlayerNetworkId[player] == GAME_COMPUTER_PLAYER
// |                 || m_setupPlayerNetworkId[player] >= giNumHumanPlayers) {
// |                 m_setupPlayerRace[player] = m_mapHeader.playerRace[m_setupPlayerColor[player]];
// |             }
// |         }
// |         goto selected_player;
// |     } else {
// |         m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
// |         if (header != NULL)
// |             m_mapHeader = *header;
// |         else
// |             GetMapHeader(m_mapFilename, &m_mapHeader);
// |
// |         for (player = 0; player < MAP_HEADER_PLAYER_COUNT; ++player) {
// |             if (m_mapHeader.playerEnabled[player]) {
// |                 m_setupPlayerColor[activeColorCount] = static_cast<i8>(player);
// |                 ++activeColorCount;
// |             }
// |         }
// |
// |         for (player = 0; player < MAP_HEADER_PLAYER_COUNT; ++player) {
// |             if (player >= m_mapHeader.playerCount) {
// |                 m_setupPlayerType[player] = GAME_NETWORK_PLAYER_NONE;
// |                 m_setupPlayerNetworkId[player] = GAME_NETWORK_PLAYER_NONE;
// |                 m_setupPlayerRace[player] = FACTION_ANY;
// |                 m_playerHandicap[player] = static_cast<PlayerHandicap>(-1);
// |             } else {
// |                 m_playerHandicap[player] = PLAYER_HANDICAP_NONE;
// |                 m_setupPlayerRace[player] = m_mapHeader.playerRace[m_setupPlayerColor[player]];
// |                 m_setupPlayerNetworkId[player] = GAME_NETWORK_PLAYER_NONE;
// |                 m_setupPlayerType[player] = GAME_NETWORK_PLAYER_NONE;
// |             }
// |         }
// |
// |         for (player = 0; player < m_mapHeader.playerCount; ++player) {
// |             if (m_mapHeader.playerCanHuman[m_setupPlayerColor[player]]
// |                 && !m_mapHeader.playerCanComputer[m_setupPlayerColor[player]]) {
// |                 m_setupPlayerType[player] = GAME_PLAYER_DEFAULT;
// |                 m_setupPlayerNetworkId[player] = static_cast<i8>(humanCount);
// |                 ++humanCount;
// |             } else if (!m_mapHeader.playerCanHuman[m_setupPlayerColor[player]]
// |                        && m_mapHeader.playerCanComputer[m_setupPlayerColor[player]]) {
// |                 m_setupPlayerNetworkId[player] = GAME_COMPUTER_PLAYER;
// |                 m_setupPlayerType[player] = GAME_PLAYER_DEFAULT;
// |                 ++computerCount;
// |             }
// |         }
// |
// |         if (humanCount < giNumHumanPlayers
// |             && computerCount < m_mapHeader.playerCount - giNumHumanPlayers)
// |             playerType = GAME_PLAYER_FLEXIBLE;
// |         else
// |             playerType = GAME_PLAYER_DEFAULT;
// |
// |         for (player = 0; player < m_mapHeader.playerCount; ++player) {
// |             if (m_setupPlayerType[player] == GAME_NETWORK_PLAYER_NONE)
// |                 m_setupPlayerType[player] = static_cast<i8>(playerType);
// |         }
// |
// |         for (player = 0; player < m_mapHeader.playerCount; ++player) {
// |             if (m_setupPlayerNetworkId[player] != GAME_NETWORK_PLAYER_NONE)
// |                 continue;
// |             if (humanCount < giNumHumanPlayers
// |                 && m_mapHeader.playerCanHuman[m_setupPlayerColor[player]]) {
// |                 m_setupPlayerNetworkId[player] = static_cast<i8>(humanCount);
// |                 ++humanCount;
// |             } else {
// |                 m_setupPlayerNetworkId[player] = GAME_COMPUTER_PLAYER;
// |             }
// |         }
// |         m_difficulty = DIFFICULTY_NORMAL;
// |         m_newGameInitialized = true;
// |     }
// |
// | selected_player:
// |     m_selectedSetupPlayer = GAME_NETWORK_PLAYER_NONE;
// | }
// |
// ALTERNATIVE drop_unread_local (unified delta from BEFORE):
// | --- before
// | +++ drop_unread_local
// | @@ -3,12 +3,10 @@
// |      i32 humanCount;
// |      NewGamePlayerSetupType playerType;
// |      i32 player;
// | -    i32 H2_UNUSED(unusedTally);
// |      i32 activeColorCount;
// |      i32 computerCount;
// |
// |      activeColorCount = 0;
// | -    unusedTally = 0;
// |      humanCount = 0;
// |      computerCount = 0;
// |
// ALTERNATIVE drop_unread_store (unified delta from BEFORE):
// | --- before
// | +++ drop_unread_store
// | @@ -8,7 +8,6 @@
// |      i32 computerCount;
// |
// |      activeColorCount = 0;
// | -    unusedTally = 0;
// |      humanCount = 0;
// |      computerCount = 0;
// |
