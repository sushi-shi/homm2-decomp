// VC6 SP5 Castle source family r22-castle-setup-roles; parent f8b6c956.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/r22-castle-setup-roles/results.json; RVA 0x24320
// Complete 4 arms; elapsed 2.409943s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 3856 / c3f48f8d1e052b80 / dcfacc22e7679c2b / 138:138 / 138,0,0,0 / True
// 1 / {'source_family': 'heroSlot', 'candidate': 'baseline'} / 3856 / 54ba93f8df932691 / b47da0b60b6738b0 / 138:138 / 138,0,0,0 / True
// 2 / {'source_family': 'statIndex', 'candidate': 'baseline'} / 3886 / e956ea631a7ceedf / a391563705402ead / 138:138 / 138,0,0,0 / True
// 3 / {'source_family': 'statIndex_heroSlot', 'candidate': 'baseline'} / 3886 / 06088297f3123079 / aa63325a05362b98 / 138:138 / 138,0,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// | void townManager::SetupCastle(heroWindow* window, i32 updateOnly) {
// |     u32l captainQuarters;
// |     i32 H2_UNUSED(tileX);
// |     i32 H2_UNUSED(tileY);
// |     i32 terrainIconFrame;
// |     i32 column;
// |     i32 rowPos;
// |     widget* backgroundWidget;
// |     i32 raceBase;
// |     i16 H2_UNUSED(builtFrame);
// |     i32 slotNum;
// |     char icnName[TOWN_OBJECT_FILENAME_SIZE];
// |     i32 backFrame;
// |     i16 H2_UNUSED(noBuildFrame);
// |     char captainStatLine[CAPTAIN_STAT_LINE_CAPACITY];
// |     i32 stateFrame;
// |     tag_message msg;
// |     i16 H2_UNUSED(cannotAffordIcon);
// |
// |     casWin = window;
// |     builtFrame = IDX(FRAME_BUILT);
// |     noBuildFrame = IDX(FRAME_CANNOT_BUILD);
// |     cannotAffordIcon = IDX(FRAME_CANNOT_AFFORD);
// |     for (slotNum = 0; slotNum < CASTLE_SLOT_COUNT; ++slotNum) {
// |         castleSlotsUse[slotNum] = castleSlotsBase[slotNum];
// |         if (castleSlotsBase[slotNum] >= BUILDING_SLOT_DWELLING_SECOND
// |             && castleSlotsBase[slotNum] <= BUILDING_SLOT_DWELLING_SIXTH
// |             && ((m_town->m_buildings & (1L << IDX(castleSlotsBase[slotNum])))
// |                 || (m_town->m_buildings
// |                     & (1L << (IDX(castleSlotsBase[slotNum]) + CASTLE_UPGRADE_OFFSET)))
// |                 || (castleSlotsBase[slotNum] == BUILDING_SLOT_DWELLING_SIXTH
// |                     && m_town->m_type == FACTION_WARLOCK
// |                     && (m_town->m_buildings & IDX(TOWN_BUILDING_ALTERNATE_UPGRADED_DWELLING_6))))
// |             && (gTownEligibleBuildMask[IDX(m_town->m_type)]
// |                 & (1L << (IDX(castleSlotsBase[slotNum]) + CASTLE_UPGRADE_OFFSET)))) {
// |             if (castleSlotsBase[slotNum] == BUILDING_SLOT_DWELLING_SIXTH
// |                 && m_town->m_type == FACTION_WARLOCK
// |                 && ((m_town->m_buildings & IDX(TOWN_BUILDING_UPGRADED_DWELLING_6))
// |                     || (m_town->m_buildings & IDX(TOWN_BUILDING_ALTERNATE_UPGRADED_DWELLING_6)))) {
// |                 castleSlotsUse[slotNum] = BUILDING_SLOT_DWELLING_LAST;
// |             } else {
// |                 castleSlotsUse[slotNum] = castleSlotsBase[slotNum] + CASTLE_UPGRADE_OFFSET;
// |             }
// |         }
// |     }
// |
// |     m_affordableBuildings = m_buildableBuildings = 0;
// |     for (slotNum = 0; slotNum < CASTLE_SLOT_COUNT; ++slotNum) {
// |         if (CanBuy(m_town, castleSlotsUse[slotNum]))
// |             m_affordableBuildings |= 1L << IDX(castleSlotsUse[slotNum]);
// |         if (CanBuild(m_town, castleSlotsUse[slotNum]))
// |             m_buildableBuildings |= 1L << IDX(castleSlotsUse[slotNum]);
// |     }
// |
// |     msg.type = MESSAGE_WIDGET;
// |     msg.payload.widget.command = CASTLE_WIDGET_FRAME;
// |     for (slotNum = 0; slotNum < CASTLE_SLOT_COUNT; ++slotNum) {
// |         msg.payload.widget.id = CONTROL_BUILDING_ICON_FIRST + slotNum;
// |         msg.payload.widget.data.value = IDX(castleSlotsUse[slotNum]);
// |         casWin->BroadcastMessage(msg);
// |     }
// |
// |     msg.payload.widget.command = CASTLE_WIDGET_ICON_FILE;
// |     sprintf(
// |         icnName,
// |         "cstl%s.icn",
// |         cHeroTypeShortName[IDX(m_town->m_type)]
// |     );
// |     msg.payload.widget.data.text = icnName;
// |     for (slotNum = 0; slotNum < CASTLE_SLOT_COUNT; ++slotNum) {
// |         msg.payload.widget.id = CONTROL_BUILDING_ICON_FIRST + slotNum;
// |         casWin->BroadcastMessage(msg);
// |     }
// |
// |     msg.payload.widget.command = CASTLE_WIDGET_TEXT;
// |     for (slotNum = 0; slotNum < CASTLE_SLOT_COUNT; ++slotNum) {
// |         msg.payload.widget.id = CONTROL_BUILDING_NAME_FIRST + slotNum;
// |         if (castleSlotsUse[slotNum] == CASTLE_MAGE_GUILD) {
// |             sprintf(
// |                 gText,
// |                 "%d \xfd\xf2\xe0\xe6 \xc3\xe8\xeb\xfc\xe4\xe8\xe8 \xec\xe0\xe3\xee\xe2"
// |                     /* "%d этаж Гильдии магов" */,
// |                 m_town->m_buildState + 1 < TOWN_MAGE_GUILD_MAX_LEVEL ? m_town->m_buildState + 1
// |                                                                      : TOWN_MAGE_GUILD_MAX_LEVEL
// |             );
// |             msg.payload.widget.data.text = gText;
// |         } else {
// |             msg.payload.widget.data.text = GetBuildingName(
// |                 m_town->m_type,
// |                 castleSlotsUse[slotNum]
// |             );
// |         }
// |         casWin->BroadcastMessage(msg);
// |     }
// |
// |     for (slotNum = 0; slotNum < CASTLE_SLOT_COUNT; ++slotNum) {
// |         stateFrame = FRAME_NONE;
// |         if ((m_town->m_buildings & (1L << IDX(castleSlotsUse[slotNum])))
// |             && (castleSlotsUse[slotNum] != CASTLE_MAGE_GUILD
// |                 || m_town->m_buildState == TOWN_MAGE_GUILD_MAX_LEVEL)) {
// |             stateFrame = FRAME_BUILT;
// |         } else {
// |             if (!(m_buildableBuildings & (1L << IDX(castleSlotsUse[slotNum]))))
// |                 stateFrame = FRAME_CANNOT_BUILD;
// |             else if (!(m_affordableBuildings & (1L << IDX(castleSlotsUse[slotNum]))))
// |                 stateFrame = FRAME_CANNOT_AFFORD;
// |         }
// |
// |         if (stateFrame != FRAME_NONE) {
// |             msg.payload.widget.command = CASTLE_WIDGET_ENABLE;
// |             msg.payload.widget.id = CONTROL_BUILDING_BUTTON_FIRST + slotNum;
// |             msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
// |             casWin->BroadcastMessage(msg);
// |             msg.payload.widget.command = CASTLE_WIDGET_FRAME;
// |             msg.payload.widget.data.value = stateFrame;
// |             casWin->BroadcastMessage(msg);
// |         } else {
// |             msg.payload.widget.command = CASTLE_WIDGET_DISABLE;
// |             msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
// |             msg.payload.widget.id = CONTROL_BUILDING_BUTTON_FIRST + slotNum;
// |             casWin->BroadcastMessage(msg);
// |         }
// |
// |         if (stateFrame == FRAME_BUILT) {
// |             msg.payload.widget.command = CASTLE_WIDGET_DISABLE;
// |             msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
// |             msg.payload.widget.id = CONTROL_BUILDING_OVERLAY_FIRST + slotNum;
// |             casWin->BroadcastMessage(msg);
// |         } else {
// |             msg.payload.widget.command = CASTLE_WIDGET_ENABLE;
// |             msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
// |             msg.payload.widget.id = CONTROL_BUILDING_OVERLAY_FIRST + slotNum;
// |             casWin->BroadcastMessage(msg);
// |             msg.payload.widget.command = CASTLE_WIDGET_FRAME;
// |             msg.payload.widget.data.value =
// |                 stateFrame == FRAME_NONE ? IDX(FRAME_AVAILABLE) : FRAME_UNAVAILABLE;
// |             casWin->BroadcastMessage(msg);
// |         }
// |     }
// |
// |     captainQuarters = m_town->m_buildings & IDX(TOWN_BUILDING_CAPTAIN_QUARTERS);
// |     msg.payload.widget.command =
// |         captainQuarters != 0 ? CASTLE_WIDGET_DISABLE : CASTLE_WIDGET_ENABLE;
// |     msg.payload.widget.id = CONTROL_CAPTAIN_OVERLAY;
// |     msg.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
// |     casWin->BroadcastMessage(msg);
// |     msg.payload.widget.command = CASTLE_WIDGET_FRAME;
// |     msg.payload.widget.id = CONTROL_CAPTAIN_ICON;
// |     msg.payload.widget.data.value = captainQuarters != 0;
// |     casWin->BroadcastMessage(msg);
// |     sprintf(
// |         gText,
// |         "CSTLCAP%c.ICN",
// |         cHeroTypeInitial[IDX(m_town->m_type)]
// |     );
// |     msg.payload.widget.command = CASTLE_WIDGET_ICON_FILE;
// |     msg.payload.widget.id = CONTROL_CAPTAIN_ICON;
// |     msg.payload.widget.data.text = gText;
// |     casWin->BroadcastMessage(msg);
// |     msg.payload.widget.command =
// |         captainQuarters != 0 ? CASTLE_WIDGET_ENABLE : CASTLE_WIDGET_DISABLE;
// |     msg.payload.widget.id = CONTROL_CAPTAIN_FLAG;
// |     msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
// |     casWin->BroadcastMessage(msg);
// |     if (captainQuarters != 0) {
// |         msg.payload.widget.command = CASTLE_WIDGET_FRAME;
// |         msg.payload.widget.id = CONTROL_CAPTAIN_FLAG;
// |         msg.payload.widget.data.value = gpCurPlayer->m_color;
// |         casWin->BroadcastMessage(msg);
// |     }
// |
// |     stateFrame = FRAME_NONE;
// |     if (captainQuarters != 0) {
// |         msg.payload.widget.command = CASTLE_WIDGET_TEXT;
// |         msg.payload.widget.data.text = gText;
// |         sprintf(
// |             gText,
// |             ""
// |         );
// |         for (slotNum = 0; slotNum < HERO_PRIMARY_STAT_COUNT; ++slotNum) {
// |             sprintf(
// |                 captainStatLine,
// |                 "%s\n",
// |                 gStatNames[slotNum]
// |             );
// |             strcat(gText, captainStatLine);
// |         }
// |         msg.payload.widget.id = CONTROL_CAPTAIN_STATS;
// |         casWin->BroadcastMessage(msg);
// |         sprintf(
// |             gText,
// |             ""
// |         );
// |         for (slotNum = 0; slotNum < HERO_PRIMARY_STAT_COUNT; ++slotNum) {
// |             sprintf(
// |                 captainStatLine,
// |                 "%d\n",
// |                 captainStats[IDX(m_town->m_type)][slotNum]
// |             );
// |             strcat(gText, captainStatLine);
// |         }
// |         msg.payload.widget.id = CONTROL_CAPTAIN_VALUES;
// |         casWin->BroadcastMessage(msg);
// |         msg.payload.widget.command = m_town->m_formation != TOWN_FORMATION_SPREAD
// |                                               ? CASTLE_WIDGET_DISABLE
// |                                               : CASTLE_WIDGET_ENABLE;
// |         msg.payload.widget.id = CONTROL_CAPTAIN_FORMATION_SPREAD_INACTIVE;
// |         msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
// |         casWin->BroadcastMessage(msg);
// |         msg.payload.widget.command = m_town->m_formation == TOWN_FORMATION_SPREAD
// |                                               ? CASTLE_WIDGET_DISABLE
// |                                               : CASTLE_WIDGET_ENABLE;
// |         msg.payload.widget.id = CONTROL_CAPTAIN_FORMATION_GROUPED_INACTIVE;
// |         msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
// |         casWin->BroadcastMessage(msg);
// |     } else {
// |         if (!CanBuild(m_town, CASTLE_CAPTAIN))
// |             stateFrame = FRAME_CANNOT_BUILD;
// |         else if (!CanBuy(m_town, CASTLE_CAPTAIN))
// |             stateFrame = FRAME_CANNOT_AFFORD;
// |         if (CanBuild(m_town, CASTLE_CAPTAIN))
// |             m_buildableBuildings |= IDX(TOWN_BUILDING_CAPTAIN_QUARTERS);
// |         if (CanBuy(m_town, CASTLE_CAPTAIN))
// |             m_affordableBuildings |= IDX(TOWN_BUILDING_CAPTAIN_QUARTERS);
// |     }
// |
// |     msg.payload.widget.command =
// |         stateFrame == FRAME_NONE ? CASTLE_WIDGET_DISABLE : CASTLE_WIDGET_ENABLE;
// |     msg.payload.widget.id = CONTROL_CAPTAIN_BUTTON;
// |     msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
// |     casWin->BroadcastMessage(msg);
// |     if (stateFrame != FRAME_NONE) {
// |         msg.payload.widget.command = CASTLE_WIDGET_FRAME;
// |         msg.payload.widget.data.value = stateFrame;
// |         casWin->BroadcastMessage(msg);
// |     }
// |
// |     if (gpCurPlayer->m_resources[IDX(RES_GOLD)] < gHeroGoldCost)
// |         stateFrame = FRAME_CANNOT_AFFORD;
// |     else if (gpCurPlayer->m_heroCount == PLAYER_HERO_CAPACITY || m_town->m_occupyingHeroId != -1)
// |         stateFrame = FRAME_CANNOT_BUILD;
// |     else if (m_recruitResult != 0)
// |         stateFrame = FRAME_BUILT;
// |     else
// |         stateFrame = FRAME_NONE;
// |
// |     for (slotNum = 0; slotNum < PLAYER_AVAILABLE_HERO_COUNT; ++slotNum) {
// |         msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
// |         msg.payload.widget.id = CONTROL_RECRUIT_FIRST + slotNum;
// |         if (stateFrame != FRAME_NONE) {
// |             msg.payload.widget.command = CASTLE_WIDGET_ENABLE;
// |             casWin->BroadcastMessage(msg);
// |             msg.payload.widget.command = CASTLE_WIDGET_FRAME;
// |             msg.payload.widget.data.value = stateFrame;
// |             casWin->BroadcastMessage(msg);
// |         } else {
// |             msg.payload.widget.command = CASTLE_WIDGET_DISABLE;
// |             casWin->BroadcastMessage(msg);
// |         }
// |         msg.payload.widget.command = CASTLE_WIDGET_ICON_FILE;
// |         sprintf(
// |             icnName,
// |             "port%04d.icn",
// |             IDX(gpGame->m_heroRecs[gpCurPlayer->AvailableHeroId(slotNum)].m_portrait)
// |         );
// |         msg.payload.widget.data.text = icnName;
// |         msg.payload.widget.id = CONTROL_HERO_FIRST + slotNum;
// |         casWin->BroadcastMessage(msg);
// |     }
// |
// |     tileX = BACKGROUND_LEFT;
// |     tileY = BACKGROUND_TOP;
// |     terrainIconFrame = (IDX(giGroundToTerrain
// |                                   [gpGame->m_worldMap.GetCell(m_town->m_x, m_town->m_y)
// |                                        ->m_terrainImageIndex])
// |                           - 1)
// |                          * (TERRAIN_ICON_COLUMNS * TERRAIN_ICON_FRAMES);
// |     raceBase = IDX(m_town->m_type) * RACE_ICON_FRAMES;
// |     if (updateOnly == 0) {
// |         backFrame = 0;
// |         for (rowPos = BACKGROUND_TERRAIN_FIRST_ROW; rowPos <= BACKGROUND_TERRAIN_LAST_ROW; ++rowPos) {
// |             for (column = BACKGROUND_FIRST_COLUMN; column <= BACKGROUND_LAST_COLUMN; ++column) {
// |                 backgroundWidget = new iconWidget(
// |                     static_cast<i16>(
// |                         (column - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT
// |                     ),
// |                     static_cast<i16>((rowPos - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE),
// |                     BACKGROUND_TILE_SIZE,
// |                     BACKGROUND_TILE_SIZE,
// |                     "objntwba.icn",
// |                     static_cast<i16>(terrainIconFrame + backFrame),
// |                     ICON_DRAW_NORMAL,
// |                     TOWN_WIDGET_ID_NONE,
// |                     WIDGET_KIND_ICON_DIRECT,
// |                     BACKGROUND_FILL_COLOR
// |                 );
// |                 if (backgroundWidget == NULL)
// |                     MemError();
// |                 casWin->AddWidget(backgroundWidget, TOWN_WIDGET_INSERT_DEFAULT);
// |                 ++backFrame;
// |             }
// |         }
// |         backFrame = 0;
// |         for (rowPos = BACKGROUND_TOWN_FIRST_ROW; rowPos <= BACKGROUND_TOWN_LAST_ROW; ++rowPos) {
// |             for (column = BACKGROUND_FIRST_COLUMN; column <= BACKGROUND_LAST_COLUMN; ++column) {
// |                 if (rowPos == BACKGROUND_TOWN_FIRST_ROW && column != BACKGROUND_TOP_CENTER_COLUMN) {
// |                     continue;
// |                 }
// |                 backgroundWidget = new iconWidget(
// |                     static_cast<i16>(
// |                         (column - BACKGROUND_FIRST_COLUMN) * BACKGROUND_TILE_SIZE + BACKGROUND_LEFT
// |                     ),
// |                     static_cast<i16>((rowPos - BACKGROUND_TOWN_FIRST_ROW) * BACKGROUND_TILE_SIZE),
// |                     BACKGROUND_TILE_SIZE,
// |                     BACKGROUND_TILE_SIZE,
// |                     "objntown.icn",
// |                     static_cast<i16>(raceBase + backFrame),
// |                     ICON_DRAW_NORMAL,
// |                     TOWN_WIDGET_ID_NONE,
// |                     WIDGET_KIND_ICON_DIRECT,
// |                     BACKGROUND_FILL_COLOR
// |                 );
// |                 if (backgroundWidget == NULL)
// |                     MemError();
// |                 casWin->AddWidget(backgroundWidget, TOWN_WIDGET_INSERT_DEFAULT);
// |                 ++backFrame;
// |             }
// |         }
// |         if (xIsExpansionMap == 0 && m_town->m_type == FACTION_NECROMANCER) {
// |             backgroundWidget = new iconWidget(
// |                 EXPANSION_OVERLAY_X,
// |                 EXPANSION_OVERLAY_Y,
// |                 EXPANSION_OVERLAY_WIDTH,
// |                 EXPANSION_OVERLAY_HEIGHT,
// |                 "caslxtra.icn",
// |                 0,
// |                 ICON_DRAW_NORMAL,
// |                 TOWN_WIDGET_ID_NONE,
// |                 WIDGET_KIND_ICON_DIRECT,
// |                 BACKGROUND_FILL_COLOR
// |             );
// |             if (backgroundWidget == NULL)
// |                 MemError();
// |             casWin->AddWidget(backgroundWidget, TOWN_WIDGET_INSERT_DEFAULT);
// |         }
// |     }
// | }
// ALTERNATIVE heroSlot (unified source delta from BEFORE):
// | --- before
// | +++ heroSlot
// | @@ -5,6 +5,7 @@
// |      i32 terrainIconFrame;
// |      i32 column;
// |      i32 rowPos;
// | +    i32 heroSlot;
// |      widget* backgroundWidget;
// |      i32 raceBase;
// |      i16 H2_UNUSED(builtFrame);
// | @@ -244,9 +245,9 @@
// |      else
// |          stateFrame = FRAME_NONE;
// |
// | -    for (slotNum = 0; slotNum < PLAYER_AVAILABLE_HERO_COUNT; ++slotNum) {
// | +    for (heroSlot = 0; heroSlot < PLAYER_AVAILABLE_HERO_COUNT; ++heroSlot) {
// |          msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
// | -        msg.payload.widget.id = CONTROL_RECRUIT_FIRST + slotNum;
// | +        msg.payload.widget.id = CONTROL_RECRUIT_FIRST + heroSlot;
// |          if (stateFrame != FRAME_NONE) {
// |              msg.payload.widget.command = CASTLE_WIDGET_ENABLE;
// |              casWin->BroadcastMessage(msg);
// | @@ -261,10 +262,10 @@
// |          sprintf(
// |              icnName,
// |              "port%04d.icn",
// | -            IDX(gpGame->m_heroRecs[gpCurPlayer->AvailableHeroId(slotNum)].m_portrait)
// | +            IDX(gpGame->m_heroRecs[gpCurPlayer->AvailableHeroId(heroSlot)].m_portrait)
// |          );
// |          msg.payload.widget.data.text = icnName;
// | -        msg.payload.widget.id = CONTROL_HERO_FIRST + slotNum;
// | +        msg.payload.widget.id = CONTROL_HERO_FIRST + heroSlot;
// |          casWin->BroadcastMessage(msg);
// |      }
// |
// ALTERNATIVE statIndex (unified source delta from BEFORE):
// | --- before
// | +++ statIndex
// | @@ -4,6 +4,7 @@
// |      i32 H2_UNUSED(tileY);
// |      i32 terrainIconFrame;
// |      i32 column;
// | +    i32 statIndex;
// |      i32 rowPos;
// |      widget* backgroundWidget;
// |      i32 raceBase;
// | @@ -177,11 +178,11 @@
// |              gText,
// |              ""
// |          );
// | -        for (slotNum = 0; slotNum < HERO_PRIMARY_STAT_COUNT; ++slotNum) {
// | +        for (statIndex = 0; statIndex < HERO_PRIMARY_STAT_COUNT; ++statIndex) {
// |              sprintf(
// |                  captainStatLine,
// |                  "%s\n",
// | -                gStatNames[slotNum]
// | +                gStatNames[statIndex]
// |              );
// |              strcat(gText, captainStatLine);
// |          }
// | @@ -191,11 +192,11 @@
// |              gText,
// |              ""
// |          );
// | -        for (slotNum = 0; slotNum < HERO_PRIMARY_STAT_COUNT; ++slotNum) {
// | +        for (statIndex = 0; statIndex < HERO_PRIMARY_STAT_COUNT; ++statIndex) {
// |              sprintf(
// |                  captainStatLine,
// |                  "%d\n",
// | -                captainStats[IDX(m_town->m_type)][slotNum]
// | +                captainStats[IDX(m_town->m_type)][statIndex]
// |              );
// |              strcat(gText, captainStatLine);
// |          }
// ALTERNATIVE statIndex_heroSlot (unified source delta from BEFORE):
// | --- before
// | +++ statIndex_heroSlot
// | @@ -4,7 +4,9 @@
// |      i32 H2_UNUSED(tileY);
// |      i32 terrainIconFrame;
// |      i32 column;
// | +    i32 statIndex;
// |      i32 rowPos;
// | +    i32 heroSlot;
// |      widget* backgroundWidget;
// |      i32 raceBase;
// |      i16 H2_UNUSED(builtFrame);
// | @@ -177,11 +179,11 @@
// |              gText,
// |              ""
// |          );
// | -        for (slotNum = 0; slotNum < HERO_PRIMARY_STAT_COUNT; ++slotNum) {
// | +        for (statIndex = 0; statIndex < HERO_PRIMARY_STAT_COUNT; ++statIndex) {
// |              sprintf(
// |                  captainStatLine,
// |                  "%s\n",
// | -                gStatNames[slotNum]
// | +                gStatNames[statIndex]
// |              );
// |              strcat(gText, captainStatLine);
// |          }
// | @@ -191,11 +193,11 @@
// |              gText,
// |              ""
// |          );
// | -        for (slotNum = 0; slotNum < HERO_PRIMARY_STAT_COUNT; ++slotNum) {
// | +        for (statIndex = 0; statIndex < HERO_PRIMARY_STAT_COUNT; ++statIndex) {
// |              sprintf(
// |                  captainStatLine,
// |                  "%d\n",
// | -                captainStats[IDX(m_town->m_type)][slotNum]
// | +                captainStats[IDX(m_town->m_type)][statIndex]
// |              );
// |              strcat(gText, captainStatLine);
// |          }
// | @@ -244,9 +246,9 @@
// |      else
// |          stateFrame = FRAME_NONE;
// |
// | -    for (slotNum = 0; slotNum < PLAYER_AVAILABLE_HERO_COUNT; ++slotNum) {
// | +    for (heroSlot = 0; heroSlot < PLAYER_AVAILABLE_HERO_COUNT; ++heroSlot) {
// |          msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
// | -        msg.payload.widget.id = CONTROL_RECRUIT_FIRST + slotNum;
// | +        msg.payload.widget.id = CONTROL_RECRUIT_FIRST + heroSlot;
// |          if (stateFrame != FRAME_NONE) {
// |              msg.payload.widget.command = CASTLE_WIDGET_ENABLE;
// |              casWin->BroadcastMessage(msg);
// | @@ -261,10 +263,10 @@
// |          sprintf(
// |              icnName,
// |              "port%04d.icn",
// | -            IDX(gpGame->m_heroRecs[gpCurPlayer->AvailableHeroId(slotNum)].m_portrait)
// | +            IDX(gpGame->m_heroRecs[gpCurPlayer->AvailableHeroId(heroSlot)].m_portrait)
// |          );
// |          msg.payload.widget.data.text = icnName;
// | -        msg.payload.widget.id = CONTROL_HERO_FIRST + slotNum;
// | +        msg.payload.widget.id = CONTROL_HERO_FIRST + heroSlot;
// |          casWin->BroadcastMessage(msg);
// |      }
// |
