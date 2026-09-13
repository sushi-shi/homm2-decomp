// VC6 SP5 source family c76-newgame-initialize; parent f84e0b7c plus U18 incoming packet header.
// Except O07 itself, parent includes O07 recovered outgoing300-byte owner.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state search is claimed.
// build/source-variant-batch/c76-newgame-initialize/results.json; RVA 0x756e8
// Complete 8 arms; elapsed 6.115736s; source restored.
// Disposition: retain all tested destination/promotion cast removals; map domain keeps inner i16 truncation and uses enum constructor.
// Private label/SG/EH counters can change relocation hashes without changing destinations.
// Final native objects/retail sites and targets decide retention, not fuzzy scores.
// Existing four EH/FS model gaps remain open; reviewed _stricmp alias is pinned separately.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 1028 / af3cc992987c1223 / a6e63cfbc9b5371e / 60:60 / 60,0,0,0 / True
// 1 / {'family': 'remove_001', 'candidate': 'baseline'} / 1028 / af3cc992987c1223 / a6e63cfbc9b5371e / 60:60 / 60,0,0,0 / True
// 2 / {'family': 'remove_010', 'candidate': 'baseline'} / 1028 / af3cc992987c1223 / a6e63cfbc9b5371e / 60:60 / 60,0,0,0 / True
// 3 / {'family': 'remove_011', 'candidate': 'baseline'} / 1028 / af3cc992987c1223 / a6e63cfbc9b5371e / 60:60 / 60,0,0,0 / True
// 4 / {'family': 'remove_100', 'candidate': 'baseline'} / 1028 / af3cc992987c1223 / a6e63cfbc9b5371e / 60:60 / 60,0,0,0 / True
// 5 / {'family': 'remove_101', 'candidate': 'baseline'} / 1028 / af3cc992987c1223 / a6e63cfbc9b5371e / 60:60 / 60,0,0,0 / True
// 6 / {'family': 'remove_110', 'candidate': 'baseline'} / 1028 / af3cc992987c1223 / a6e63cfbc9b5371e / 60:60 / 60,0,0,0 / True
// 7 / {'family': 'remove_111', 'candidate': 'baseline'} / 1028 / af3cc992987c1223 / a6e63cfbc9b5371e / 60:60 / 60,0,0,0 / True

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
// ALTERNATIVE remove_001 (unified delta from BEFORE):
// | --- before
// | +++ remove_001
// | @@ -70,7 +70,7 @@
// |
// |          for (player = 0; player < m_mapHeader.playerCount; ++player) {
// |              if (m_setupPlayerType[player] == GAME_NETWORK_PLAYER_NONE)
// | -                m_setupPlayerType[player] = static_cast<i8>(playerType);
// | +                m_setupPlayerType[player] = playerType;
// |          }
// |
// |          for (player = 0; player < m_mapHeader.playerCount; ++player) {
// ALTERNATIVE remove_010 (unified delta from BEFORE):
// | --- before
// | +++ remove_010
// | @@ -29,7 +29,7 @@
// |
// |          for (player = 0; player < MAP_HEADER_PLAYER_COUNT; ++player) {
// |              if (m_mapHeader.playerEnabled[player]) {
// | -                m_setupPlayerColor[activeColorCount] = static_cast<i8>(player);
// | +                m_setupPlayerColor[activeColorCount] = player;
// |                  ++activeColorCount;
// |              }
// |          }
// ALTERNATIVE remove_011 (unified delta from BEFORE):
// | --- before
// | +++ remove_011
// | @@ -29,7 +29,7 @@
// |
// |          for (player = 0; player < MAP_HEADER_PLAYER_COUNT; ++player) {
// |              if (m_mapHeader.playerEnabled[player]) {
// | -                m_setupPlayerColor[activeColorCount] = static_cast<i8>(player);
// | +                m_setupPlayerColor[activeColorCount] = player;
// |                  ++activeColorCount;
// |              }
// |          }
// | @@ -70,7 +70,7 @@
// |
// |          for (player = 0; player < m_mapHeader.playerCount; ++player) {
// |              if (m_setupPlayerType[player] == GAME_NETWORK_PLAYER_NONE)
// | -                m_setupPlayerType[player] = static_cast<i8>(playerType);
// | +                m_setupPlayerType[player] = playerType;
// |          }
// |
// |          for (player = 0; player < m_mapHeader.playerCount; ++player) {
// ALTERNATIVE remove_100 (unified delta from BEFORE):
// | --- before
// | +++ remove_100
// | @@ -21,7 +21,7 @@
// |          }
// |          goto selected_player;
// |      } else {
// | -        m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
// | +        m_newGameHumanCount = giNumHumanPlayers;
// |          if (header != NULL)
// |              m_mapHeader = *header;
// |          else
// ALTERNATIVE remove_101 (unified delta from BEFORE):
// | --- before
// | +++ remove_101
// | @@ -21,7 +21,7 @@
// |          }
// |          goto selected_player;
// |      } else {
// | -        m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
// | +        m_newGameHumanCount = giNumHumanPlayers;
// |          if (header != NULL)
// |              m_mapHeader = *header;
// |          else
// | @@ -70,7 +70,7 @@
// |
// |          for (player = 0; player < m_mapHeader.playerCount; ++player) {
// |              if (m_setupPlayerType[player] == GAME_NETWORK_PLAYER_NONE)
// | -                m_setupPlayerType[player] = static_cast<i8>(playerType);
// | +                m_setupPlayerType[player] = playerType;
// |          }
// |
// |          for (player = 0; player < m_mapHeader.playerCount; ++player) {
// ALTERNATIVE remove_110 (unified delta from BEFORE):
// | --- before
// | +++ remove_110
// | @@ -21,7 +21,7 @@
// |          }
// |          goto selected_player;
// |      } else {
// | -        m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
// | +        m_newGameHumanCount = giNumHumanPlayers;
// |          if (header != NULL)
// |              m_mapHeader = *header;
// |          else
// | @@ -29,7 +29,7 @@
// |
// |          for (player = 0; player < MAP_HEADER_PLAYER_COUNT; ++player) {
// |              if (m_mapHeader.playerEnabled[player]) {
// | -                m_setupPlayerColor[activeColorCount] = static_cast<i8>(player);
// | +                m_setupPlayerColor[activeColorCount] = player;
// |                  ++activeColorCount;
// |              }
// |          }
// ALTERNATIVE remove_111 (unified delta from BEFORE):
// | --- before
// | +++ remove_111
// | @@ -21,7 +21,7 @@
// |          }
// |          goto selected_player;
// |      } else {
// | -        m_newGameHumanCount = static_cast<i8>(giNumHumanPlayers);
// | +        m_newGameHumanCount = giNumHumanPlayers;
// |          if (header != NULL)
// |              m_mapHeader = *header;
// |          else
// | @@ -29,7 +29,7 @@
// |
// |          for (player = 0; player < MAP_HEADER_PLAYER_COUNT; ++player) {
// |              if (m_mapHeader.playerEnabled[player]) {
// | -                m_setupPlayerColor[activeColorCount] = static_cast<i8>(player);
// | +                m_setupPlayerColor[activeColorCount] = player;
// |                  ++activeColorCount;
// |              }
// |          }
// | @@ -70,7 +70,7 @@
// |
// |          for (player = 0; player < m_mapHeader.playerCount; ++player) {
// |              if (m_setupPlayerType[player] == GAME_NETWORK_PLAYER_NONE)
// | -                m_setupPlayerType[player] = static_cast<i8>(playerType);
// | +                m_setupPlayerType[player] = playerType;
// |          }
// |
// |          for (player = 0; player < m_mapHeader.playerCount; ++player) {
