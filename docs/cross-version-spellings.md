# Cross-version spelling ledger

The 2.1 branch's larger purpose is cross-referencing against the 2.0
reconstruction to recover the one true source tree. Whenever this branch's
byte-matching forces a spelling that DIFFERS from the PoL 2.0 spelling, the
divergence is an **unresolved claim**, not a conclusion: matching Buka's VC6
bytes proves the spelling is *sufficient for 2.1*, never that it is what the
devs wrote. Each entry below must eventually resolve to one of:

- **invariant** - the 2.1 spelling also reproduces 2.0's bytes under MSVC
  4.2 (test in the 2.0 tree). The 2.0 reconstruction then adopts it; PoL's
  spelling was just another member of 4.2's equivalence class.
- **mixed** - neither current spelling satisfies both compilers; search for
  a third that does before concluding anything.
- **dev-change** - no single spelling satisfies both, after a genuine
  search: the devs edited the line between 2.0 and 2.1. Document in
  docs/version-changes.md with the evidence.

Resolution protocol: compile the candidate spelling in the 2.0 tree
(homm2-decomp-master, MSVC 4.2 harness) and byte-compare against the 2.0
target the same way `reloc_donation.masked_equal` does here. Until that run
happens, entries stay OPEN. Timing: the verification runs are deferred until
the 2.1 reconstruction is complete - this branch only records divergences
while chasing exactness; the cross-analysis is a later dedicated phase.

## Open entries (2026-08-02)

| Site | PoL 2.0 spelling | 2.1 byte-pinned spelling | State |
|---|---|---|---|
| HandleRemoteDeadPlayerExit guard | `giThisGamePos == pos` | `pos == giThisGamePos` (a1-moffs local-first) | OPEN |
| MapExtraPosAndAdjacentsSet reads | `mapExtra[MAP_WIDTH * y + x]`, `mapExtra[checkY * MAP_WIDTH + checkX]` | flat `*(mapExtra + x + MAP_WIDTH * y)` (both); the `if (MAP_HEIGHT <= checkY) {} else` empty-if is REAL (removing it loses 2 bytes) but must read `checkY >= MAP_HEIGHT` | OPEN - 2 jump-distance bytes left: retail places the loop-back jmp BEFORE the body, ours after |
| BlitBitmapToScreenVesa convention | `__fastcall` | `__cdecl` (509 -> 501 vs retail 486; residual is body work) | OPEN |
| a1-moffs compare order (2 sites) | UpdateAppSpecificMenus `hmnuAdv == hMenu`; SetMapSize `h == MAP_HEIGHT && w == MAP_WIDTH` | `hMenu == hmnuAdv` (local-first, -1 byte); `MAP_HEIGHT == h && MAP_WIDTH == w` (global-first, +1 byte) - the class runs BOTH directions, read it off the bytes | OPEN |
| fullMap cell-read sweep (24 sites) | `Row(y)[x]` / `Row(y) + x` across GAME, EVENTS, ADVMGR | `GetCell(x, y)` (the nested-inline shape) | OPEN - same class as the GetCell body change |
| combatManager::SetupAdjacencyArray locals | `sourceHex`; rowIndex/direction declared inside the outer loop | `fromHex`; both hoisted to fn scope (retail's toHex sits at -0x10, only reachable with 4 fn-scope slots). 41 -> 13 diffs, 3-cycle -> 2-cycle. RESIDUAL: rowIndex/direction pair - the bucket model predicts direction@-4 and MATCHES the target, but VC6 compiles the reverse; 4 probes (dir/y/decl-swap) never flipped it. First hard model-vs-VC6 disagreement on an ENUM-typed local | OPEN - permuter identifier_rename queued |
| advManager::SaveAdventureBorder local | `screenPixel` | `screen` (bucket 0 ties savedPixels; later decl wins the 3-cycle) | OPEN |
| advManager::DrawAdventureBorder slots | - | PARKED: the sibling's rename does NOT transfer (Draw declares screen first and wants savedPixels above it); decl-order swap measured 232 diffs, `saved`/`dst`+`src` measured worse - permuter queued | OPEN |
| slot-solve batch 4 (3 fns) | PoL names | GetBitmap/GetMIDIWrap `id`->`fileId`; GiveArmy `tmp`->`swap` | OPEN as slot-name class |
| DDSetFullScreenStatus / SetupHotSeatGame slots | - | PARKED: retail's 5-slot order needs x/y declared ASYMMETRICALLY (868 solver hits, all requiring e.g. `x` + `top`/`y1`) - unnatural for a paired coordinate function; SetupHotSeatGame's `window` slot resists both bucket-1 and bucket-2 names | OPEN - permuter/identifier_rename matrix queued |
| slot-solve batch 3 (7 fns) | PoL names | HandleVictory mapIndex->map; dpSendMessage result->status; DetermineHeroToMove idx/val->best/value; GetTurnAttentionValue factor->scale; Expunge cur/next (ShutDownSystem echo); ValidPath reserved->extra (dead slot); ValidSpellTarget unusedSpellWord5->unused (dead slot) | OPEN as slot-name class |
| slot-solve batch 2 | PoL names | GetOutOfItMask armyIndex2/bit1/mask5/currentArmy10 -> idx/bitMask/result/currentArmy (GetStrength family echo); IsHomogeneous nRaces/last -> numRaces/prev | OPEN as slot-name class |
| slot-solve batch 1 (8 fns) | PoL names | GetMoveShowIt directionX/Y->dx/dy; GetSSLevel bonus/ssLevel; DemobilizeCurrHero hp/cell; StopOnTrigger trigger->special (bucket model mispredicts 'metadata' on VC6); RandomizeBarrier idx->index; CanBuy idx->r; NumOfGivenArtifact jj hoisted to fn top; InitializePasswords jj->j + table-read mirror | OPEN as slot-name class |
| combatManager::EffectSpellCure six sites | `index != targetIndex`; fightValue-cast leading FP chain; `fightValue * m_quantity`; `>= MAX ? MAX : X` duration ternaries; `IDX(m_currentSide) == sideWork` (x2) | `targetIndex != index`; `cast(curePoints) * MODIFIER * fightValue`; `m_quantity * fightValue`; `< MAX ? X : MAX`; `sideWork == IDX(m_currentSide)` | OPEN - slot residual parked |
| combatManager::ProcessCombatMsg switch layout | case order as in source | retail lays inner-switch case bodies in a DIFFERENT order (jump-table targets differ; first body is the m_?f2c3 member call, ours is the 0/1 local store) + one member/local mirror | OPEN - PARKED for body-diff pass: needs jump-table target mapping |
| combatManager::Resurrect five sites | `m_initialQuantity < m_quantity`; `count == index+1`; `A<B?A:B` min-ternary; two FP/index orders | `m_quantity > m_initialQuantity`; `index+1 == count`; `REVERSE<frames?REVERSE:frames`; FIZZLE_DELAY const-first | OPEN - slot residual parked |
| combatManager::VaporizeCreature stripe writes | `gyModify[row*W + firstY + top]`; `gyModify[lastY + (row*-W - bottom)]` | `*(row*W + gyModify + firstY + top)`; `*(gyModify - row*W - bottom + lastY)` (computed-term-first flat sums; simple loads canonicalize, muls do not) | OPEN - slot residual parked |
| combatManager::TurnToStone limit increment | `++m_limitCreatureCount[side][index]` | retail reads m_index FIRST - no plain respelling reproduces it (flat + and commuted-bracket both canonicalize identically); PERMUTER matrix queued (accessor-introduction arm) | OPEN - parked |
| fullMap::GetCell inline body | `cells + y * width + x` | `&Column(x)[y * width]` (nested-inline: y*width via this-temp first, x*12, cells - the retail shape at ~119 call sites; fuzzy +0.3% TU-wide, 0 drops) | OPEN - test under 4.2 |
| game::HasLateOverlay cell fetch | `WORLDMAP->Row(row) + col` | `WORLDMAP->GetCell(col, row)` | OPEN |
| ComputeUALoc probe reads | `gpGame->m_worldMap.Row(y)[x].f` (x4) | `gpGame->m_worldMap.GetCell(x, y)->f` | OPEN - slot residual parked |
| combatManager::SummonElemental hex index | `summonHexes_l[side*3 + (rand+off)%3]` | flat `*(summonHexes_l + side*3 + (rand+off)%3)` | OPEN - slot residual parked |
| game::GetLuck block order | clamps, Battle Garb, then Rainbow | Rainbow BEFORE clamps (VALUE - see version-changes) | dev-change candidate |
| strip::DrawIcons X exprs + type compare | `m_x + slot * STEP + FIRST_X` (5 sites); `m_army->types[slot] != cached[slot]` | `m_x + FIRST_X + slot * STEP` (NEW CLASS: const after member steers member-first eval); `cached[slot] != army types` | OPEN |
| listBoxWidget::listBoxWidget(void) store order | `items, scrollbar, selectedIndex, itemCount, lastSelected` | `itemCount, items, selectedIndex, lastSelected, scrollbar` | OPEN |
| button::button(void) store order | `normalFrame, pressedFrame, iconId, selectMode, hotkey, icon` | `iconId, icon, normalFrame, pressedFrame, selectMode, hotkey` | OPEN |
| combatManager::ShowMassSpell frame clamps | `effectFrames < member` (x2); `returnFrames7 < member` | member-first `> effectFrames` / `> returnFrames7` | OPEN - slot residual parked |
| combatManager::Fireball FP factor | `mod[speed] * SPELL_AREA_ANIMATION_DELAY` | `DELAY * mod[speed]` | OPEN - slot residual parked |
| combatManager::DoVictory compares | `winningSide == Opposite(...)`; `fadeCount > fadeIndex26`; `emptySlots > iMaxTransferArtifacts`; `!= CREATURE_GHOST` | `Opposite(...) == winningSide`; `fadeIndex26 < fadeCount`; `iMaxTransferArtifacts < emptySlots`; `!= CREATURE_SKELETON` (VALUE - see version-changes) | OPEN - slot residual parked |
| mouseManager::Open store order | `cursorL/T, mouseX, sizeIdx x2, cursorBottom, mouseY`; `m_active` before `m_priority` | R/B then L/T then X/Y pairs, size indexes last; `m_priority` before `m_active` | OPEN |
| executive::ShutDownSystem manager walk | `next = head; while ((cur = next) != NULL) { next = cur->m_next; ... }` | `baseManager* next; baseManager* cur = head; while (cur != NULL) { next = ...; ...; cur = next; }` (decl order flips the 14/14 tie) | OPEN |
| game::SetVisibility mapExtra writes + distance terms + locals | `mapExtra[MAP_WIDTH * row + col] \|=`; x-term first; `col/row/visibility`, distance in-loop | `*(mapExtra + col + MAP_WIDTH * row) \|=` (RMW emits base-first load, index-first store); y-term first; `i/j/vis`, distance hoisted to fn scope | OPEN |
| game::MakeAllWaterVisible map access | `WORLDMAP->Row(y)[x]`; `mapExtra[y * MAP_WIDTH + x]` | in-loop `fullMap* map = WORLDMAP;` + `map->Column(x)[y * map->width]`; `*(mapExtra + x + MAP_WIDTH * y)` | OPEN - third Column-class site (GetNewCellExtra pair, now this) |
| philAI::RVConversion resource sum order | ORE, GEMS, MERCURY, GOLD, WOOD, SULFUR, CRYSTAL | GOLD, WOOD, ORE, CRYSTAL, SULFUR, MERCURY, GEMS | OPEN |
| advManager::ScreenScroll locals + 4 compares | `originX/originY`; bound-first clamps; `m_mapOriginX != originX \|\| ...` | `xOrigin/yOrigin` (yOrigin 2 < xOrigin 11 flips the pair); local-first clamps and != compares | OPEN |
| combatManager::UpdateMouseGrid 5 compares | `m_mouseGridHex == hexIndex`; extent updates expr-first | `hexIndex == m_mouseGridHex`; `giMinExtent* > expr`, `giMaxExtent* < expr` (global-left) | OPEN - slot residual parked |
| game::DoKnob knob clamp compare | `floatChain < cast(mouse.y)` | `cast(mouse.y) > floatChain` (fild-first) | OPEN - slot residual parked |
| philAI::GetBestCreature random compare | `weakestArmyValue < table.randomValue` | `table.randomValue > weakestArmyValue` | OPEN - slot residual parked |
| playerData::NextHero local + hero compare | `current`; `m_heroIds[i] == m_currentHero` | `curHero`; `m_currentHero == m_heroIds[i]` | OPEN |
| textWidget::textWidget(void) store order | `color, alignment, font, text, kind` | `font, text, color, alignment, kind` | OPEN |
| dropListWidget::dropListWidget(void) store order | `items, savedBackground, itemCount, selectedIndex` | `itemCount, items, selectedIndex, savedBackground` | OPEN |
| combatManager::DoBlast FP factor | `gfCombatSpeedMod[speed] * BLAST_FRAME_DELAY` | `BLAST_FRAME_DELAY * gfCombatSpeedMod[speed]` | OPEN - same class as combatManager::Main |
| heroWindow::MoveWindow clamp sums | `m_winWidth + newX`; `m_winHeight + newY` | `newX + m_winWidth`; `newY + m_winHeight` | OPEN - slot 6-perm residual parked (no symmetric rename satisfies newX<newY<=oldH<=oldX<oldY<oldW) |
| army::LoadResources dead locals + loopCount | eight `unusedLoadWord*` decls; `loopCount = 1` | no dead locals (frame 0x08); `loopCount = 0` | OPEN - loopCount is VALUE-LEVEL (see version-changes) |
| swapManager::SplitMons type compare | `selectedArmy->types[selSlot] == targetArmy->types[tgtSlot]` | target-first mirror | OPEN |
| combatManager::ShotIsThroughWall trace compares | `iWallToHexCell[i] == traceHex11`; `iTowerToHexCell[i] == traceHex11` | `traceHex11 == table[i]` (both) | OPEN - slot 14-perm residual parked |
| CreateJoinFile diff loop | `while (diffSize1 > position1)` | `while (position1 < diffSize1)` | OPEN - slot residual parked |
| recruitUnit::Update dead buffer | `char text[NAME_SIZE];` declared (PoL) | ABSENT - retail frame is 0x20, exactly NAME_SIZE smaller; sprintf targets gText | OPEN - check whether 2.0 bytes carry the 0x34 frame |
| army::AttackTo standing if/else + two compares | `if (pI == 1 \|\| steps >= speed) fS=1; else fS=0;`; `m_hex == m_moveTargetHex`; `pathLength - 1 != pI` | De Morgan: `if (pI != 1 && steps < speed) fS=0; else fS=1;`; `m_moveTargetHex == m_hex`; `pI != pathLength - 1` | OPEN - residual: retail slots finishStanding BELOW the this-spill; no rename/scope/register spelling reproduces it (3 probes measured); parked as slot-wall with GetBestDirection |
| game::ExperienceValueOfStack factors | `hitPoints * m_quantities[i]` | `m_quantities[i] * hitPoints` | OPEN |
| town::Deallocate locals + owner compare | `playerRecord/position`; `m_id == ->m_currentTown` | `pp/pos`; `pp->m_currentTown == m_id` | OPEN |
| armyGroup::DamageGroup locals + kill compare | `percentChance/numKilled`; `m_creatureCounts[i] == killed` | `killChance/killed`; `killed == m_creatureCounts[i]` | OPEN |
| combatManager::EffectSpellResurrect locals | `quantityResult[COMBAT_SIDE_COUNT]` array + `targetStack/resurrectPowerWork/armyIndexWork` | scalar `count` + dead `float workChance` (slot -16 unreferenced, bucket 15) + `target/resurrectPower/armyIndex`; two operand mirrors | OPEN - VALUE-SHAPE: 2.1 frame cannot host the [0]-at-fc array; check what 2.0 bytes hold |
| widget::widget(void) store order | `m_id` first | `m_id` after `m_prev` | OPEN |
| widget::widget(x,y,w,h,id,kind) store order | `m_id` after `m_zOrder` | `m_id` before `m_flags` | OPEN |
| army::LeftX else-branch addends | `->w + ->x` | `->x + ->w` | OPEN |
| combatManager::SetupGridForArmy locals + hex compare | `savedTargetSide/targetIndexSave/hexIndex`; `m_hex == hexIndex` | `oldSide/oldIndex/j`; `j == armyPtr->m_hex` | OPEN |
| hero::DoSSLevelDialog local + icon-frame expr | `skillLevelText`; `skills[...] + skill * STRIDE - BASE` | `skillText`; `skill * STRIDE + skills[...] - BASE` | OPEN |
| combatManager::EffectSpellCreateCreature locals + imul | `spellPowerValue/creatureType`; `fightValue * IDX(type)` | `spellPower/monType`; `IDX(monType) * fightValue` | OPEN |
| combatManager::Main timers + FP factor + army decl | `KBTickCount() > glTimers[...]` (x2); `mod[speed] * DELAY`; `army* currentArmy` in bare block | `glTimers[...] < KBTickCount()`; `DELAY * mod[speed]`; `army* currentArmy;` hoisted to fn top (goto forbids initialized decl; hoist also moves it into the outer frame group - inner-block locals slot AFTER outer ones) | OPEN |
| advManager::SetTownContext locals + two compares | `currentTownValue/contextValue7`; `m_currentTerrain != cast(ctx)`; `page + 3 - 1 < ctx` | `tp/townNo`; `cast(townNo) != m_currentTerrain`; `townNo > page + 3 - 1` | OPEN |
| dpnet_init locals + guest loop | `enumerateFunction/result`; `giNumHumanPlayers > guestIndex` | `dpEnumerate/rc`; `guestIndex < giNumHumanPlayers` | OPEN |
| army::GetBestDirection compares | source-first (`sCC < tC` etc.); `leftFlag == isMovingRight` | target-first (`tC > sCC`, `tC != sCC`, `tRV > sR`, `tRV != sR`); `isMovingRight == leftFlag` | OPEN - slot 8-perm residual parked: no natural rename satisfies b(down)<=b(tCol)<=b(sCol)<b(left)<=b(up)<=b(tRow)<=b(sRow)<b(right) |
| border::border(void) store order | `bitmap, icon, fillColor` | `fillColor, bitmap, icon` | OPEN |
| iconWidget::iconWidget(void) store order | `frame, fillColor, icon, orientation, iconId` | `icon, frame, fillColor, orientation, iconId` | OPEN |
| resourceManager::PointToFile locals | `isFound/entryIndex/aggregateIndex/ignoredPosition` | `found/entry/i/position` | OPEN - 2.0 names read PoL-invented |
| resourceManager::GetFileSize locals | `isFound/entryIndex/matchedAggregate/fileIndex` | `found/entry/matched/i` | OPEN |
| ComputeAdvNetControl locals + while compares | `selectedPlayer/player/currentPlayer(dead)`; `giCurPlayer != player` | `selected/player/myPlayer(dead)`; `player != giCurPlayer` (x2) | OPEN |
| resourceManager::GetBackdropAtLoc locals + loop bound | `backdropIcon/dataWidth/imageHeight/row`; `row < imageHeight + destinationY` | `width`/`curRow` renames; `curRow < destinationY + imageHeight` | OPEN |
| hero::GetLevel local + while compare | `increment`; `experience < experienceValue` | `growth`; `experienceValue > experience` | OPEN |
| advManager::IsCrystalBallInEffect locals + distance terms | `heroIndex/range`; `(m_y-y)^2 + (m_x-x)^2` | `i/distance`; `(m_x-x)^2 + (m_y-y)^2` | OPEN |
| GetMonType threshold compares | `table[...] >= score`; `table[...] <= score` | `score <= table[...]`; `score >= table[...]` | OPEN |
| philAI::NetValueOfArtifact addend order | `a4 * cost[a3] + a2 * cost[GOLD]` | `a2 * cost[GOLD] + a4 * cost[a3]` | OPEN |
| hero::HeroScreenUpdate | `index`; `index == giHeroScreenSrcIndex` | `i`; `giHeroScreenSrcIndex == i` | OPEN |
| advManager::CheckScreenScroll locals + origin compares | `mouseX6/mouseY1/oldOriginX9/oldOriginY3`; `m_mapOriginX == old...` | `mouseX/mouseY/oldMapX/oldMapY`; `oldMapX == m_mapOriginX` (both axes) | OPEN - 2.0 names are PoL auto-suffixed |
| textEntryWidget::textEntryWidget(void) store order | `cursor, icon, kind, maxLength, iconFrame, displayOffset` | `cursor, maxLength, icon, iconFrame, displayOffset, kind` | OPEN |
| DimensionDoorHandler mouse fields + flag | `payload.mouse.x/.y` (+4/+8); `handled` | `payload.mouse.screenX/.screenY` (+0x10/+0x14); `result` | OPEN - VALUE-SEMANTIC: different union fields read; check what 2.0's bytes read before classifying |
| game::IsMobile locals + return compare | `cp`; `CalcTerrainCost(...) <= m_remainingMobility` | `cell`; `m_remainingMobility >= CalcTerrainCost(...)` | OPEN |
| combatManager::GetStrength locals + and-mask | `armyIndex4/bit36/strength7/currentArmy8`; `(bit36 & mask)` | `idx/bitMask/totalStrength/currentArmy`; `(mask & bitMask)` | OPEN - 2.0 names are PoL auto-suffixed; dev names unknown on 4.2 side too |
| advManager::CheckSetEvilInterface | `m_evilInterface != gbUseEvilInterface`; `interfaceMessage` | `gbUseEvilInterface != m_evilInterface`; `message` | OPEN |
| resource::resource(void) store order | `m_id = 0;` before `m_refCount = ...` | `m_refCount` before `m_id` (header order) | OPEN |
| wsnet_init locals + strlen guard | `socketMode`, `player`; `strlen(gcTCPAddress) != 0` | `blockMode`, `plr`; `strlen(gcTCPAddress) > 0` | OPEN |
| philAI::CanBuyBHC locals + two compares | `jb/idx`; `garrison[what] < num`; `cost[idx] * num > m_resources[idx]` | `j/index`; `num > garrison[what]`; `m_resources[index] < cost[index] * num` | OPEN |
| fullMap::GetNewCellExtra{Overlay,Object} reads | `Row(y)[x]` | `Column(x)[y * width]` (+ new `Column` accessor) | OPEN - test Column form under 4.2; only ~25 arms tried, uniqueness unproven |
| fullMap::Cell body / cell assigns | `cells + width * y + x` | `cells + x + y * width` | OPEN |
| fullMap::Read memcpy args | `cells + width * y + x` etc. | `cells + x + y * width` | OPEN |
| fullMap::GetNewCellExtraIndex locals | `nb, i, j` | `n, i, j` | OPEN - 4.2 hash differs from VC6; both may hold per-compiler with dev rename in either direction |
| fullMap::Read locals | `tmp` | `tmp1` | OPEN (same class) |
| fullMap::ChangeTilesetIndex head | if/else | zero-arm ternary `t = index != MAPCELL_SPRITE_NONE ? tileset : TILESET_NONE;` | OPEN |
| fullMap::ChangeTilesetIndex unused local | `dummy` | `a` (bucket-1 pin) | OPEN |
| mouseManager::Close head | `if (m_active == 1) { ... }` | `if (m_active != 1) return;` unwrap | OPEN - 4.2 lowering of wrap-vs-guard untested |
| inputManager::Close head | wrap | guard-clause unwrap | OPEN |
| heroWindowManager::Close head | wrap | guard-clause unwrap | OPEN |
| AiPrint head | wrap | guard-clause unwrap | OPEN |
| soundManager::AdjustSoundVolumes | n/a (Buka-era body) | `&&` split into guard + if | 2.1-only body, no 2.0 counterpart |
| UpdateSystemOptionsMenu guard | (compare order unrecorded in PoL) | `hmnuApp != hmnuAdv` (reloc-pinned) | OPEN - check 2.0 bytes' operand order |
| ws/dpWaitForExtraGuests first compare | `iWSLastMsg... != giNum...` | `giNumHumanPlayers != i...LastMsg...` | OPEN (same check possible in 2.0) |
| ws/dpnet_rcv first compare | `Tail == Head` | `Head == Tail` | OPEN |
| REQUEST GetMapHeader CRT calls | n/a | `open/read/close` (oldnames) | matches 2.0 house style; uncontroversial |
| Bzip spotBlock locals | upstream `pos, delta, newdelta` | `index, delta, updatedDelta2` | OPEN - upstream names cannot give VC6's slots; either Buka renamed (dev-change) or the 4.2/VC6 hash split resolves it; test upstream names in 2.0 tree |
| PATH FindPath / ValidRange, HERO GetNumSpells / GetExperience, KB CanBuild, TOWN GiveSpells, CURSOR pair, REMOTE calc_crc_long, Newgame GetLossConditionText, wingraph DDInitializePalette locals | PoL names | bucket-pinned renames (see f5d5b4c3) | OPEN as a class - slot names are per-compiler hash pins; resolving needs the 4.2-vs-VC6 hash comparison per name |

| TeleportTo telescope bonus | `>= 1` u32 cast (PoL-era spelling) | `HasArtifact(...) != 0` | OPEN |
| TeleportTo terrain compare | `m_currentTerrain != terrain` | `terrain != m_currentTerrain` | OPEN |
| TeleportTo locals | savedShow11/terrain5/oldCell2/unused47 | savedShow/terrain/cellOld2/unused (bucket pins) | OPEN as slot-name class |
| InsertSound bounds guard spelling | `MAP_WIDTH <= x \|\| MAP_HEIGHT <= mapY` | param-first, axes swapped (see Resolved) | - |
| WGAppPaint scroll adds | `giScrollX + x` (a1-form) | `destX7 + giScrollX` etc. (local first) | OPEN |
| WGAppPaint locals | padding8/destY1/sourceX7/sourceY7/destWidth0 | decl swap + destW/fromY/xSource/destY0 (bucket pins) | OPEN as slot-name class |
| InitCampaignMap priority compare | `bestLocal < best` | `best > bestLocal` (mirror) | OPEN |
| InitCampaignMap faction switch | BARBARIAN, WARLOCK, NECROMANCER | WARLOCK, BARBARIAN, NECROMANCER (je-distance pinned) | OPEN |
| InitCampaignMap carryover product | ternary * counts | counts * ternary (permuter trial 135) | OPEN |
| ElementalStorm delay product + storm frame index | `speedMod * DELAY`, `col*STEP + frame + row` | `DELAY * speedMod`, `frame + col*STEP + row` (matrix winners) | OPEN |
| m_armies indexing (SPELLS + AI, 19 sites) | flattened `m_armies[0] + side*21 + idx` | 2D `m_armies[side] + idx` (single-imul fold, byte-pinned) | OPEN |
| ElementalStorm locals | *_suffixed epoch names | hit/whichSide/c/baseDam/dmg2/stack/iter/member (bucket pins) | OPEN as slot-name class |
| Random head compare | `low == high` | `high == low` | OPEN |
| netwin done-callbacks compare | `&gNbSessNcb[i] == ncb` | `ncb == &gNbSessNcb[i]` | OPEN |
| townObject::Draw frame sum | `m_animationFrame + baseFrame + 1` | `baseFrame + m_animationFrame + 1` | OPEN |
| Relational/commutative mirror class (AddWidget, DeleteItem, SetSpellInfluence, DrawWindow, GetNthSpell, recruitUnit::Open, wsWaitForHost, game::Scan, Random, netwin callbacks, townObject::Draw) | PoL-order spellings | mirrored operand order, byte-pinned per fn | OPEN as a class |
| GetCategoryStats rare-resource sum | mercury+sulfur+crystal+gems | gems+crystal+sulfur+mercury (reversed, byte-pinned) | OPEN - PoL sits at 99.05 with the forward order; likely the same residue there |

## Resolved

| Site | Resolution | Evidence |
|---|---|---|
| ValidHex bound (125 vs <117) | **dev-change** | PoL ledger banks ValidHex 100.0000 with `<= 125`; this image compares `< 117` |
| TransferArtifacts bound (WAND vs GOOSE) | **dev-change** | PoL ledger banks TransferArtifacts 100.0000 with `<= WAND(3)`; this image compares `<= 7` |
| InsertSound loopCount 0 vs 1 | **dev-change** | both values byte-pinned (PoL 2.0 = 0, Buka = 1); ledgered |
| InsertSound bounds guard | **dev-change (or 2.0 recheck)** | retail 2.1 swaps the axes (x vs HEIGHT); verify what 2.0's bytes compare before classifying further |
| In-code UI strings (29 literals) + SExecutiveText | **dev-change (Buka)** | retail CP1251 content differs from 2.0's English; ledgered in version-changes.md |
| EDITOR/mapcell KB.h include (STL pair) | **dev-change (2.1)** | the two new functions exist only in the 2.1 image |
