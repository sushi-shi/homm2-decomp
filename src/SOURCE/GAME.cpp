// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\GAME.OBJ   from: (directly linked into exe)
// functions: 99   data: 26
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/philAI.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/GAME.h>
#include <BASE/Icon2b.h>
#include <BASE/Misc.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/FINDPATH.h>
#include <SOURCE/HERO.h>
#include <SOURCE/KB.h>
#include <SOURCE/REMOTE.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <SOURCE/game.h>
#include <SOURCE/playerData.h>
#include <SOURCE/town.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/advManager.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/army.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/Campaign.h>
#include <SOURCE/townManager.h>
#include <SOURCE/TOWNMGR.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/EVENTS.h>
#include <EDITOR/mapcell.h>
#include <EDITOR/fullMap.h>
#include <BASE/soundManager.h>
#include <BASE/resourceManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/heroWindow.h>
#include <BASE/font.h>
#include <BASE/iconWidget.h>
#include <BASE/BITS.h>
#include <BASE/Bzip.h>
#include <BASE/INPUTMGR.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <SOURCE/ARMY.h>
#include <SOURCE/kbwin.h>

// EventExtra/SThievesData live in SOURCE/GAME.h.

// GAME's BaseAlloc/BaseFree pass __FILE__ + a source line number. The retail
// encodes the base line as a 2-byte string read via movswl, then adds a per-call
// delta; reproduce byte-exactly. __FILE__ is the original build path (reloc-masked).
// @data-layout-note Retail's initialized GAME contribution is
// 0xf70e0..0xf80b8 (0xfd8); candidate .data is 0xfd6. Its 204 definitions are
// closed one-to-one as 17 source DATA owners plus 187 private allocations. Every
// private payload is byte-exact, has one candidate code reference, and maps to a
// disjoint retail extent. Fourteen typed short line-base owners leave their
// natural two-byte alignment halves unowned; bMapInitialized leaves three bytes,
// and retail has a six-byte terminal zero tail. Those 0x25 bytes are the complete
// initialized-storage residual. ViewArmy and CreateJoinFile require distinct
// local bases at 0xf7388 and 0xf7bc4; sharing the save/diff bases incorrectly hid
// two referenced retail owners. The 0xb8 rdata group and all 23 BSS owners are
// also closed. Do not add padding, aliases, synthetic owners, or section pragmas
// to force physical allocation order.
DATA(0x004f70e0) b32 gbGameOver = false;
// PROVEN /Gi artifact: the retail TU was built with MSVC 4.2 incremental
// compilation, under which __LINE__ is not an immediate - the compiler emits a
// per-function static i16 __LINE__Var (movsx + add offset at each use) so
// incremental rebuilds can patch line numbers in data. The original source was
// plain __LINE__; these statics pin retail's anchor values because our file's
// physical line numbers differ. Do not fold the base+offset sums into literals -
// the memory reference is part of the byte proof. (Probe: CL /Od /Gi vs /Od.)
DATA(0x004f70e4) static i16 gSaveSourceLine = 0x294;
DATA(0x004f71a8) static i16 gLoadSourceLine = 0x44f;
DATA(0x004f7274) static i16 gMapSourceLine = 0xaf4;
DATA(0x004f75c4) static i16 gTransmitSourceLine = 0x1a4e;
DATA(0x004f77b8) static i16 gReceiveSourceLine = 0x1b2d;
DATA(0x004f7a60) static i16 gDiffSourceLine = 0x1d66;
DATA(0x004f7e90) static i16 gCompressTest2SourceLine = 0x1f72;
DATA(0x004f7f84) static i16 gCompressTestSourceLine = 0x1f95;

#define RETAIL_FILE const_cast<char*>("I:\\Projects\\Heroes\\Prog\\SOURCE\\GAME.CPP")

// GAME-private tuning and encoding constants.
HOMM2_ENUM_VALUES_BEGIN(GameTuningConstant)
    RANDOM_SCAN_RETRY_LIMIT = 10000,
    EXPERIENCE_HERO_PRESENCE_BONUS = 500,
    MINE_FLAG_OVERWRITE_LIMIT = 0x30,   // highest passive object id a mine flag may cover
    RANDOM_MONSTER_SPRITE_FIRST = 0x43, // MONS32 placeholder frames for random monsters 1-4
    RANDOM_MONSTER_SPRITE_LAST = 0x46,
    RANDOM_MONSTER_SPRITE_TO_TRIGGER =
        0x70,                     // sprite index + 0x70 == its MAP_TRIGGER_RANDOM_MONSTER_LEVEL_*
    BANK_GUARDIAN_FLAG = 0x100,   // creature-bank metadata: defenders present
    TOWN_RECORD_TYPE_MASK = 0x7f, // saved town record: low bits carry the race
    COMPRESS_TEST_ITERATIONS = 100
HOMM2_ENUM_VALUES_END(GameTuningConstant)

HOMM2_ENUM_BEGIN(ViewArmyControlId)
    VIEW_ARMY_QUICK_VIEW_ID = 0x7800,
    VIEW_ARMY_UPGRADE_ID = 0x7803
HOMM2_ENUM_END(ViewArmyControlId)

// Retail's original source passed plain __FILE__/__LINE__ here: under /Od /Gi,
// MSVC 4.2 lowers __LINE__ to a compiler-synthesized per-function static i16
// anchor (?__LINE__Var@...) loaded with movsx and offset with add — that is the
// entire origin of the retail "line base" words. The DATA statics above stand
// in for that synthesized storage; call sites pass the resulting retail line
// numbers as literals.
// Retail folds the embedded member offset into Row/Extra accesses after inlining.
#define WORLDMAP (&m_worldMap)

// Inline accessors that reference gpGame directly (the retail emits `add [gpGame]`
// + a per-call `jmp $+0`), so they are free inline helpers, not game methods.
inline town* GetCastle(i32 idx) {
    return gpGame->GetTown(idx);
}
inline i8 PlayerEventByte(i8 color) {
    return gpGame->m_players[color].m_color;
}

VA(0x004708b0, 0x23d)
void playerData::Write(i32 file) {
    char unused[52];

    write(file, &m_color, 1);
    write(file, &m_heroCount, 1);
    write(file, &m_currentHero, 1);
    write(file, &m_heroLocatorPage, 1);
    write(file, m_heroIds, 8);
    write(file, m_availableHeroIds, 2);
    memset(unused, 0, 48);
    write(file, unused, 42);
    write(file, &gpGame->m_cheated, 1);
    write(file, &m_cheatValue, 1);
    write(file, &m_aiDifficulty, 4);
    write(file, &m_minimumHeroCount, 1);
    write(file, &m_evilInterface, 1);
    write(file, &m_ultimateArtifactHintChance, 1);
    write(file, &m_ultimateArtifactHintX, 1);
    write(file, &m_ultimateArtifactHintY, 1);
    write(file, &m_daysLeft, 1);
    write(file, &m_townCount, 1);
    write(file, &m_currentTown, 1);
    write(file, &m_townLocatorPage, 1);
    write(file, m_townIds, 72);
    write(file, m_resources, 28);
    write(file, m_income, 28);
    write(file, &m_barrierTents, 1);
    write(file, &m_barrierTents, 1);
    write(file, m_unknownad, 6);
}

VA(0x00470aed, 0x22d)
void playerData::Read(i32 file) {
    char unused[52];

    read(file, &m_color, 1);
    read(file, &m_heroCount, 1);
    read(file, &m_currentHero, 1);
    read(file, &m_heroLocatorPage, 1);
    read(file, m_heroIds, 8);
    read(file, m_availableHeroIds, 2);
    read(file, unused, 42);
    read(file, &gpGame->m_cheated, 1);
    read(file, &m_cheatValue, 1);
    read(file, &m_aiDifficulty, 4);
    read(file, &m_minimumHeroCount, 1);
    read(file, &m_evilInterface, 1);
    read(file, &m_ultimateArtifactHintChance, 1);
    read(file, &m_ultimateArtifactHintX, 1);
    read(file, &m_ultimateArtifactHintY, 1);
    read(file, &m_daysLeft, 1);
    read(file, &m_townCount, 1);
    read(file, &m_currentTown, 1);
    read(file, &m_townLocatorPage, 1);
    read(file, m_townIds, 72);
    read(file, m_resources, 28);
    read(file, m_income, 28);
    read(file, &m_barrierTents, 1);
    read(file, &m_barrierTents, 1);
    read(file, m_unknownad, 6);
}

VA(0x00470d1a, 0x12d)
i32 playerData::NextHero(i32) {
    i32 current = -1;
    i32 i;

    if (gpCurPlayer->m_currentHero != -1) {
        for (i = 0; i < gpCurPlayer->m_heroCount; i++) {
            if (gpCurPlayer->m_heroIds[i] == gpCurPlayer->m_currentHero)
                current = i;
        }
    }

    for (i = current + 1; i < gpCurPlayer->m_heroCount; i++) {
        if (gpGame->IsMobile(gpCurPlayer->m_heroIds[i]))
            return m_heroIds[i];
    }
    for (i = 0; i < current + 1; i++) {
        if (gpGame->IsMobile(gpCurPlayer->m_heroIds[i]))
            return m_heroIds[i];
    }
    return -1;
}

VA(0x00470e47, 0x65)
i32 playerData::HasMobileHero(void) {
    i32 i;
    for (i = 0; i < m_heroCount; i++) {
        if (gpGame->IsMobile(m_heroIds[i]))
            return 1;
    }
    return 0;
}

VA(0x00470eac, 0x64)
i32 GetNumObelisks(i32 color) {
    i32 count = 0;
    i32 i;
    for (i = 0; i < GAME_BOAT_COUNT; i++) {
        if (gpGame->m_obeliskVisitors[i] & (1 << color))
            count++;
    }
    return count;
}

VA(0x00470f10, 0xca)
i32 playerData::BuildingsOwned(i32 townType, BuildingSlotType buildingIndex, i32 buildState) {
    i32 count = 0;
    i32 i;
    for (i = 0; i < m_townCount; i++) {
        town* ownedTown = &gpGame->m_castleRecs[m_townIds[i]];
        if (buildingIndex < BUILDING_SLOT_DWELLING_FIRST || ownedTown->m_type == townType) {
            if (buildingIndex == BUILDING_SLOT_MAGE_GUILD) {
                if (ownedTown->m_buildings & IDX(TOWN_BUILDING_MAGE_GUILD)) {
                    if (ownedTown->m_buildState == buildState)
                        count++;
                }
            } else {
                if (ownedTown->m_buildings & (1 << buildingIndex))
                    count++;
            }
        }
    }
    return count;
}

VA(0x00470fda, 0x97)
i32 playerData::NumOfGivenArtifact(ArtifactType artifact) {
    i32 count = 0;
    i32 i;
    for (i = 0; i < m_heroCount; i++) {
        i32 j;
        for (j = 0; j < HERO_ARTIFACT_SLOT_COUNT; j++) {
            if (gpGame->m_heroRecs[m_heroIds[i]].m_artifacts[j] == artifact)
                count++;
        }
    }
    return count;
}

VA(0x00471071, 0x82)
i32 game::MineTypesOwned(i32 owner, i32 resourceType) {
    i32 num = 0;
    i32 i;
    for (i = 0; i < GAME_MINE_COUNT; i++) {
        if (m_mines[i].owner == owner && m_mines[i].resourceType == resourceType)
            num++;
    }
    return num;
}

// @early-stop
// Complete semantics, CFG, frame/slots, and all 36 ordered relocation
// sites/targets agree. The only unmasked bytes are branch displacements +0xa0
// and +0x181: retail's early exits route through the +0x3fe return trampoline,
// while base branches directly to the +0x408 epilogue. Revisit after a relevant
// GAME predecessor/header change alters this local trampoline placement.
VA(0x004710f3, 0x40d)
void ComputeUALoc(i32 player) {
    i32 result = gpGame->SetupPuzzlePieces(player, 1);
    if (result < 8 || gpGame->m_ultimateArtifactId == IDX(ARTIFACT_NONE)) {
        gpGame->m_players[player].m_ultimateArtifactHintChance = 0;
        gpGame->m_players[player].m_ultimateArtifactHintX = -1;
        gpGame->m_players[player].m_ultimateArtifactHintY = -1;
        return;
    }

    i32 probability = (result - 8) * 4;
    if (probability > 100)
        probability = 100;
    if (probability < 1)
        probability = 1;
    gpGame->m_players[player].m_ultimateArtifactHintChance = static_cast<i8>(probability);

    if (Random(1, 100) <= gpGame->m_players[player].m_ultimateArtifactHintChance) {
        gpGame->m_players[player].m_ultimateArtifactHintX = gpGame->m_ultimateArtifactX;
        gpGame->m_players[player].m_ultimateArtifactHintY = gpGame->m_ultimateArtifactY;
        return;
    }

    i32 x = -1;
    i32 y = -1;
    i32 direction = 0;
    i32 tries = 0;
    while (
        !(x >= 0 && (&x)[0] < MAP_WIDTH && y >= 0 && (&y)[0] < MAP_HEIGHT
          && gpGame->m_worldMap.Row(y)[x].m_triggerType == 0
          && gpGame->m_worldMap.Row(y)[x].m_objectIndex == IDX(MAPCELL_SPRITE_NONE)
          && gpGame->m_worldMap.Row(y)[x].m_overlayIndex == IDX(MAPCELL_SPRITE_NONE)
          && giGroundToTerrain[gpGame->m_worldMap.Row(y)[x].m_terrainImageIndex] != 0)
    ) {
        tries++;
        direction = 0;
        while (direction == 0)
            direction = 3 - Random(0, 2) - Random(0, 2) - Random(0, 2);
        x = gpGame->m_ultimateArtifactX + direction;
        direction = 0;
        while (direction == 0)
            direction = 3 - Random(0, 2) - Random(0, 2) - Random(0, 2);
        y = gpGame->m_ultimateArtifactY + direction;
        if (tries >= 200) {
            x = gpGame->m_ultimateArtifactX;
            y = gpGame->m_ultimateArtifactY;
            goto saveLocation;
        }
    }
saveLocation:
    gpGame->m_players[player].m_ultimateArtifactHintX = static_cast<i8>(x);
    gpGame->m_players[player].m_ultimateArtifactHintY = static_cast<i8>(y);
}

// @early-stop
// @early-stop-reloc-only
// All 0x2ac relocation-masked bytes, the frame/slots/CFG, and all 25 ordered
// relocation sites/effective targets agree. Objdiff spells retail's
// __adjust_fdiv address as iLeftRightSave+0x10; both resolve to RVA 0x12126c.
VA(0x00471500, 0x2ac)
i32 game::SetupPuzzlePieces(i32 player, i32 justCount) {
    i32 pieceCountTotal = GetNumObelisks(player);
    i32 unvisitedObelisks = 48 - m_obeliskCount;
    float ratio = static_cast<float>(GetNumObelisks(player)) / m_obeliskCount;
    float interpolation = (ratio * ratio + ratio) / 2.0f;
    pieceCountTotal = static_cast<i32>(pieceCountTotal + unvisitedObelisks * interpolation);

    if (GetNumObelisks(player) == m_obeliskCount)
        pieceCountTotal = 48;
    pieceCountTotal += m_players[player].m_cheatValue;
    if (pieceCountTotal > 48)
        pieceCountTotal = 48;
    if (justCount)
        return pieceCountTotal;

    memset(puzzlePiecesRemoved, 0, 6);
    SRand(m_players[player].m_color + m_players[player].m_evilInterface * 3);
    i32 tries;
    i32 fallbackNum;
    i32 pieceValue;
    i32 i;
    for (i = 0; (&i)[0] < pieceCountTotal; i++) {
        for (pieceValue = 0; pieceValue < 48; pieceValue += SRandom(1, 5)) {
            if (!BitTest(puzzlePiecesRemoved, pieceValue))
                break;
        }

        for (tries = 0; tries < 100; tries++) {
            fallbackNum = SRandom(0, 47);
            if (!BitTest(puzzlePiecesRemoved, fallbackNum))
                break;
        }
        if (tries >= 100) {
            for (fallbackNum = 0; fallbackNum < 48; fallbackNum++) {
                if (!BitTest(puzzlePiecesRemoved, fallbackNum))
                    break;
            }
        }
        if (fallbackNum >= 48)
            fallbackNum = 0;
        if (pieceValue < 48)
            BitSet(puzzlePiecesRemoved, pieceValue);
        else
            BitSet(puzzlePiecesRemoved, fallbackNum);
    }
    return pieceCountTotal;
}

VA(0x004717ac, 0xb5)
i32 game::IsMobile(i32 heroId) {
    if (heroId == -1)
        return 0;
    hero* mobileHero = &m_heroRecs[heroId];
    mapCell* cp = gpAdvManager->GetCell(mobileHero->m_x, mobileHero->m_y);
    return CalcTerrainCost(
               giGroundToTerrain[cp->m_terrainImageIndex],
               1,
               mobileHero->m_remainingMobility,
               mobileHero->m_secondarySkills[IDX(HERO_SKILL_PATHFINDING)],
               cp->m_isRoad,
               0
           )
           <= mobileHero->m_remainingMobility;
}

VA(0x00471861, 0x1e)
fullMap* game::GetWorldMapData(void) {
    return &m_worldMap;
}

// @semantic
// The 0x11e frame, CFG, and all three ordered relocations match. Only 12 bytes at
// +0xf5..+0x104 differ: retail preserves the packed cell word before shifting boatIdx,
// while MSVC emits the equivalent RHS-first order here. Whole-word, cast, and |0 variants
// did not steer it; revisit only after GAME's cumulative declaration state changes.
VA(0x0047187f, 0x11e)
i32 game::CreateBoat(i32 x, i32 y, i32 notify) {
    i32 boatIdx = Scan(m_boatSlots, 0, GAME_BOAT_COUNT);
    if (boatIdx != -1) {
        if (notify == 0)
            SendMapChange(4, 0, x, y, -999, 0, 0);
        m_boatSlots[boatIdx] = static_cast<i8>(boatIdx);
        boatRecord* boat = &m_boats[boatIdx];
        boat->id = static_cast<i8>(boatIdx);
        boat->x = static_cast<i8>(x);
        boat->y = static_cast<i8>(y);
        boat->direction = 2;
        boat->owner = static_cast<i8>(giCurPlayer);
        mapCell* cell = WORLDMAP->Row(y) + x;
        boat->savedTriggerType = cell->m_triggerType;
        boat->savedEventData = static_cast<u8>(cell->m_objectMetadata);
        cell->m_triggerType = IDX(MAP_TRIGGER_BOAT);
        cell->m_objectMetadata = boatIdx;
    }
    return boatIdx;
}

VA(0x0047199d, 0x5a)
i32 game::Scan(i8* array, i32 start, i32 length) {
    i32 i;
    for (i = start; i < length + start; i++) {
        if (array[i] == -1)
            return i;
    }
    return -1;
}

VA(0x004719f7, 0x76)
i32 game::RandomScan(i8* array, i32 start, i32 range, i32 unused, i8 target) {
    i32 idx = target;
    i32 i;
    for (i = 0; i < RANDOM_SCAN_RETRY_LIMIT; i++) {
        idx = start + Random(0, range - 1);
        if (array[idx] == target)
            return idx;
    }
    return -1;
}

VA(0x00471a6d, 0x213)
i32 game::GetNewHeroId(i32, i32 heroClass, i32 requireExperienced) {
    i32 result = -1;
    i32 previousHero;
    i32 heroId = -1;
    i32 attempts = 0;
    i32 oldHeroId;
    while (attempts < 2000) {
        attempts++;
        heroId = Random(0, 53);
        if (m_availableHeroes[heroId] != -1 && m_availableHeroes[heroId] != 64)
            continue;
        if (m_availableHeroes[heroId] == 64 && attempts < 1500)
            continue;
        if (heroClass >= 0 && heroClass <= 5 && attempts < 100
            && m_heroRecs[heroId].m_cursorType != heroClass)
            continue;
        if (requireExperienced && attempts < 40 && m_heroRecs[heroId].m_experience < 1000
            && (m_heroRecs[heroId].m_artifacts[0] == IDX(ARTIFACT_NONE)
                || m_heroRecs[heroId].m_artifacts[0] == IDX(ARTIFACT_MAGIC_BOOK))
            && (m_heroRecs[heroId].m_artifacts[1] == IDX(ARTIFACT_NONE)
                || m_heroRecs[heroId].m_artifacts[1] == IDX(ARTIFACT_MAGIC_BOOK)))
            continue;
        if (gbInCampaign && attempts < 500 && m_heroRecs[heroId].m_portrait >= 54
            && m_heroRecs[heroId].m_portrait <= 59)
            continue;
        break;
    }
    return heroId;
}

VA(0x00471c80, 0x85)
i32 game::GetTownId(i32 col, i32 row) {
    i32 i;
    for (i = 0; i < GAME_TOWN_COUNT; i++) {
        if (m_castleRecs[i].m_x == col && m_castleRecs[i].m_y == row)
            return i;
    }
    return -1;
}

VA(0x00471d05, 0x84)
i32 game::GetMineId(i32 col, i32 row) {
    i32 i;
    for (i = 0; i < GAME_MINE_COUNT; i++) {
        if (m_mines[i].x == col && m_mines[i].y == row)
            return i;
    }
    return -1;
}

VA(0x00471d89, 0x12e)
void GenerateStandardFileName(char* source, char* destination) {
    char* extension = FindLastToken(source, '.');
    if (extension == 0) {
        strcpy(destination, source);
        return;
    }

    *extension = 0;
    i32 indexOut = 0;
    i32 sourceLength = strlen(source);
    i32 i;
    char c;
    for (i = 0; i < (&sourceLength)[0]; i++) {
        c = source[i];
        if (c >= 'a' && c <= 'z')
            c -= 'a' - 'A';
        if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
            destination[indexOut] = c;
            indexOut++;
        }
        if (indexOut >= 8)
            i = 999;
    }
    *extension = '.';
    strcpy(destination + indexOut, extension);
}

// @early-stop
// reloc-masked: identical 0xbc4-byte code/frame; 135/135 targets agree, only compiler literal/constant symbol identities differ
VA(0x00471eb7, 0xbc4)
i32 game::SaveGame(char* filename, i32 generateName, i8 expansionFormat) {
    void* emptyPayload = H2_ALLOC(GAME_SAVE_BUFFER_SIZE, 670);
    memset(emptyPayload, 0, GAME_SAVE_BUFFER_SIZE);
    if (!xIsExpansionMap)
        expansionFormat = 1;
    gpAdvManager->DemobilizeCurrHero();

    char savePathValue[452];
    char generatedNameStorage[452];
    i32 humanPlayersVar;
    i32 indexFile;
    i32 unusedTemp;
    i32 oldFlag;
    i32 saveValue;
    i32 filePadding;
    i32 compatibilityReserved;
    if (generateName) {
        if (gbInCampaign) {
            sprintf(generatedNameStorage, "%s.%s", filename, "GMC");
        } else if (xIsPlayingExpansionCampaign) {
            sprintf(generatedNameStorage, "%s.%s", filename, "GXC");
        } else {
            humanPlayersVar = 0;
            for (indexFile = 0; indexFile < GAME_PLAYER_COUNT; indexFile++) {
                if (m_playerDead[indexFile] == 0 && gbHumanPlayer[indexFile])
                    humanPlayersVar++;
            }
            if (xIsExpansionMap && !expansionFormat)
                sprintf(generatedNameStorage, "%s.GX%d", filename, humanPlayersVar);
            else
                sprintf(generatedNameStorage, "%s.GM%d", filename, humanPlayersVar);
        }
    } else {
        sprintf(generatedNameStorage, filename);
    }

    if (strnicmp(generatedNameStorage, "RMT", 3) == 0) {
        sprintf(savePathValue, "%s%s", ".\\DATA\\", generatedNameStorage);
    } else {
        sprintf(savePathValue, "%s%s", gcGamePath, generatedNameStorage);
        if (strnicmp(generatedNameStorage, "AUTOSAVE", 8) != 0
            && strnicmp(generatedNameStorage, "PLYREXIT", 8) != 0)
            strcpy(gpGame->m_saveName, filename);
    }

    i32 fileInfo = open(savePathValue, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (fileInfo == -1)
        FileError(savePathValue);

    i32 legacyMarkerTemp = -1;
    if (!expansionFormat)
        write(fileInfo, &legacyMarkerTemp, 4);
    write(fileInfo, &m_worldMap.width, 4);
    write(fileInfo, &m_worldMap.height, 4);
    write(fileInfo, &m_mapHeader, sizeof(m_mapHeader));
    write(fileInfo, m_setupPlayerColor, CAMPAIGN_SETUP_RESET_SIZE);
    write(fileInfo, &gbIAmGreatest, 1);
    write(fileInfo, this, 2);
    write(fileInfo, &giMonthType, 1);
    write(fileInfo, &giMonthTypeExtra, 1);
    write(fileInfo, &giWeekType, 1);
    write(fileInfo, &giWeekTypeExtra, 1);
    write(fileInfo, cPlayerNames, 126);

    char legacyData[100];
    memset(legacyData, 0, 40);
    write(fileInfo, legacyData, 36);
    if (xIsPlayingExpansionCampaign) {
        i32 campaignTypeInfo = 2;
        write(fileInfo, &campaignTypeInfo, 4);
        write(fileInfo, &xCampaign, EXPANSION_CAMPAIGN_SAVE_PREFIX_SIZE);
    } else {
        write(fileInfo, &gbInCampaign, 4);
        if (gbInCampaign)
            write(fileInfo, &m_campaignType, CAMPAIGN_STATE_RESET_SIZE);
    }
    if (!expansionFormat)
        write(fileInfo, &xIsExpansionMap, 1);

    gpAdvManager->PurgeMapChangeQueue();
    write(fileInfo, &giMapChangeCtr, 4);
    GenerateStandardFileName(m_saveName, legacyData);
    write(fileInfo, legacyData, 14);
    write(fileInfo, &m_playerCount, 1);
    char currentPlayerInfo[4];
    currentPlayerInfo[0] = static_cast<char>(giCurPlayer);
    write(fileInfo, currentPlayerInfo, 1);
    write(fileInfo, &m_deadPlayerCount, 1);
    write(fileInfo, m_playerDead, 6);

    char humanFlagsLocal[8];
    for (indexFile = 0; indexFile < GAME_PLAYER_COUNT; indexFile++) {
        humanFlagsLocal[indexFile] = static_cast<char>(gbHumanPlayer[indexFile]);
        if (m_playerDead[indexFile] != 0)
            humanFlagsLocal[indexFile] = 0;
    }
    write(fileInfo, humanFlagsLocal, 6);
    write(fileInfo, &m_day, 2);
    write(fileInfo, &m_week, 2);
    write(fileInfo, &m_month, 2);
    for (indexFile = 0; indexFile < GAME_PLAYER_COUNT; indexFile++)
        m_players[indexFile].Write(fileInfo);

    write(fileInfo, &m_obeliskCount, 1);
    for (indexFile = 0; indexFile < GAME_HERO_COUNT; indexFile++)
        m_heroRecs[indexFile].Write(fileInfo, !expansionFormat);
    write(fileInfo, m_availableHeroes, GAME_HERO_COUNT);
    write(fileInfo, m_castleRecs, sizeof(m_castleRecs));
    write(fileInfo, m_castleOwners, sizeof(m_castleOwners));
    write(fileInfo, m_dailyEventFlags, sizeof(m_dailyEventFlags));
    write(fileInfo, m_mines, sizeof(m_mines));
    write(fileInfo, m_mineOwners, sizeof(m_mineOwners));
    if (!expansionFormat)
        write(fileInfo, m_randomArtifacts, ARTIFACT_COUNT);
    else
        write(fileInfo, m_randomArtifacts, RANDOM_ARTIFACT_BASE_TABLE_SIZE);
    write(fileInfo, m_boats, sizeof(m_boats));
    write(fileInfo, m_boatSlots, sizeof(m_boatSlots));
    write(fileInfo, m_obeliskVisitors, sizeof(m_obeliskVisitors));
    write(fileInfo, &m_ultimateArtifactX, 1);
    write(fileInfo, &m_ultimateArtifactY, 1);
    write(fileInfo, &m_ultimateArtifactId, 1);
    write(fileInfo, m_rumour, sizeof(m_rumour));
    write(fileInfo, m_defaultPlayerNames, sizeof(m_defaultPlayerNames));
    write(fileInfo, &m_rumourEventCount, 4);
    write(fileInfo, m_rumourEventIndices, m_rumourEventCount * 2);
    write(fileInfo, &m_timeEventCount, 4);
    write(fileInfo, m_timeEventIndices, m_timeEventCount * 2);
    write(fileInfo, &m_mapEventCount, 4);
    write(fileInfo, m_mapEventIndices, m_mapEventCount * 2);

    i32 markerBuffer[3];
    markerBuffer[0] = GAME_FILE_MARKER;
    i32 unusedMarkerInfo = GAME_UNUSED_FILE_MARKER;
    write(fileInfo, markerBuffer, 4);
    write(fileInfo, &iMaxMapExtra, 4);
    write(fileInfo, markerBuffer, 4);
    for (indexFile = 1; indexFile < iMaxMapExtra; indexFile++) {
        write(fileInfo, markerBuffer, 4);
        write(fileInfo, pwSizeOfMapExtra + indexFile, 2);
        if (ppMapExtra[indexFile] != 0)
            write(fileInfo, ppMapExtra[indexFile], pwSizeOfMapExtra[indexFile]);
        else
            write(fileInfo, emptyPayload, pwSizeOfMapExtra[indexFile]);
    }
    write(fileInfo, markerBuffer, 4);
    write(fileInfo, mapExtra, MAP_WIDTH * MAP_HEIGHT);
    write(fileInfo, markerBuffer, 4);
    m_worldMap.Write(fileInfo);
    write(fileInfo, markerBuffer, 4);
    close(fileInfo);
    H2_FREE(emptyPayload, 897);
    return 1;
}

// @early-stop
// Complete semantics, frame, CFG, and all 75 external relocations agree after fixing
// Sorceress Navigation and Warlock Scouting initialization. The only residual is six
// instructions in the starting-stat loop: retail loads j before scaling i*100, while
// this TU scales i before adding j. Split 4+1 fields, the contiguous five-byte array,
// and `j[m_primaryStats]` all emit the same sequence; revisit on cumulative TU changes.
VA(0x00472a7b, 0xb44)
void game::SetupOrigData(void) {
    ClearMapExtra();
    gbIAmGreatest = false;
    m_difficultyRating = 1;
    giMonthType = 0;
    giMonthTypeExtra = 0;
    giWeekType = 0;
    giWeekTypeExtra = 0;
    m_cheated = 0;
    gpAdvManager->PurgeMapChangeQueue();
    giMapChangeCtr = 1;
    strcpy(m_saveName, "NEWGAME");
    m_playerCount = 4;
    m_deadPlayerCount = 0;
    memset(m_playerDead, 0, GAME_PLAYER_COUNT);
    m_month = 1;
    m_week = m_month;
    m_day = m_week;
    giCurTurn = 1;

    i32 i;
    i32 j;
    for (i = 0; i < GAME_PLAYER_COUNT; i++) {
        strcpy(m_defaultPlayerNames + i * 4, "");
        if (i < (&giNumHumanPlayers)[0]) {
            if (i == 0 || iMPBaseType == MULTIPLAYER_BASE_HOT_SEAT)
                gbThisNetHumanPlayer[i] = 1;
            else
                gbThisNetHumanPlayer[i] = 0;
            gbHumanPlayer[i] = 1;
        } else {
            gbThisNetHumanPlayer[i] = 0;
            gbHumanPlayer[i] = 0;
        }
        memset(&m_players[i], 0, sizeof(m_players[i]));
        m_players[i].m_color = static_cast<i8>(i);
        m_players[i].m_heroCount = 0;
        m_players[i].m_townCount = 0;
        m_players[i].m_daysLeft = -1;
        m_players[i].m_cheatValue = 0;
        memset(m_players[i].m_availableHeroIds, -1, 2);
        memset(m_players[i].m_heroIds, -1, 8);
        memset(m_players[i].m_townIds, -1, GAME_TOWN_COUNT);
    }

    m_obeliskCount = 0;
    gpAdvManager->m_heroContextLocked = 0;
    memset(m_availableHeroes, -1, GAME_HERO_COUNT);
    for (i = 0; i < GAME_HERO_COUNT; i++) {
        memset(&m_heroRecs[i], 0, sizeof(m_heroRecs[i]));
        memset(m_heroRecs[i].m_spells, 0, sizeof(m_heroRecs[i].m_spells));
        memset(m_heroRecs[i].m_artifacts, ARTIFACT_NONE, sizeof(m_heroRecs[i].m_artifacts));
        m_heroRecs[i].m_patrolY = -1;
        m_heroRecs[i].m_patrolX = m_heroRecs[i].m_patrolY;
        m_heroRecs[i].m_id = static_cast<i8>(i);
        m_heroRecs[i].m_portrait = static_cast<u8>(i);
        m_heroRecs[i].m_owner = -1;
        m_heroRecs[i].m_direction = 2;
        strcpy(m_heroRecs[i].m_name, gHeroDefaultNames[i]);
        m_heroRecs[i].m_cursorType = static_cast<u8>(i / 9);
        for (j = 0; j < HERO_STARTING_STAT_COUNT; j++)
            m_heroRecs[i].m_primaryStats[j] = gStartingHeroStats[m_heroRecs[i].m_cursorType][j];
        for (j = 0; j < ARMY_GROUP_SLOT_COUNT; j++)
            m_heroRecs[i].m_army.m_creatureTypes[j] = ARMY_GROUP_EMPTY_SLOT;
        m_heroRecs[i].m_destinationY = -1;
        m_heroRecs[i].m_destinationX = m_heroRecs[i].m_destinationY;
        m_heroRecs[i].m_level = 1;
        m_heroRecs[i].m_spellPoints = m_heroRecs[i].Stats(HeroPrimaryStat(3)) * 10;
        m_heroRecs[i].m_secondarySkillCount = 0;
        for (j = 0; j < 14; j++) {
            m_heroRecs[i].m_secondarySkills[j] = 0;
            m_heroRecs[i].m_secondarySkillOrder[j] = 0;
        }
        if (m_heroRecs[i].m_cursorType == 0) {
            m_heroRecs[i].GiveSS(IDX(HERO_SKILL_LEADERSHIP), 1);
            m_heroRecs[i].GiveSS(IDX(HERO_SKILL_BALLISTICS), 1);
        }
        if (m_heroRecs[i].m_cursorType == 2) {
            m_heroRecs[i].m_artifacts[0] = IDX(ARTIFACT_MAGIC_BOOK);
            m_heroRecs[i].GiveSS(IDX(HERO_SKILL_NAVIGATION), 2);
            m_heroRecs[i].GiveSS(IDX(HERO_SKILL_WISDOM), 1);
        }
        if (m_heroRecs[i].m_cursorType == 1)
            m_heroRecs[i].GiveSS(IDX(HERO_SKILL_PATHFINDING), 2);
        if (m_heroRecs[i].m_cursorType == 3) {
            m_heroRecs[i].m_artifacts[0] = IDX(ARTIFACT_MAGIC_BOOK);
            m_heroRecs[i].GiveSS(IDX(HERO_SKILL_SCOUTING), 2);
            m_heroRecs[i].GiveSS(IDX(HERO_SKILL_WISDOM), 1);
        }
        if (m_heroRecs[i].m_cursorType == 4) {
            m_heroRecs[i].m_artifacts[0] = IDX(ARTIFACT_MAGIC_BOOK);
            m_heroRecs[i].GiveSS(IDX(HERO_SKILL_WISDOM), 2);
        }
        if (m_heroRecs[i].m_cursorType == 5) {
            m_heroRecs[i].m_artifacts[0] = IDX(ARTIFACT_MAGIC_BOOK);
            m_heroRecs[i].GiveSS(IDX(HERO_SKILL_WISDOM), 1);
            m_heroRecs[i].GiveSS(IDX(HERO_SKILL_NECROMANCY), 1);
        }
    }

    memset(m_castleOwners, TOWN_OWNER_NONE, sizeof(m_castleOwners));
    for (i = 0; i < GAME_TOWN_COUNT; i++) {
        memset(&m_castleRecs[i], 0, sizeof(m_castleRecs[i]));
        m_castleRecs[i].m_onMap = 0;
        m_castleRecs[i].m_id = static_cast<u8>(i);
        m_castleRecs[i].m_owner = TOWN_OWNER_NONE;
        m_castleRecs[i].m_type = static_cast<u8>(i / 9);
        m_castleRecs[i].m_occupyingHeroId = TOWN_OCCUPYING_HERO_NONE;
        for (j = 0; j < ARMY_GROUP_SLOT_COUNT; j++)
            m_castleRecs[i].m_army.m_creatureTypes[j] = ARMY_GROUP_EMPTY_SLOT;
    }
    for (i = 0; i < GAME_MINE_COUNT; i++)
        memset(&m_mines[i], -1, sizeof(m_mines[i]));
    memset(m_mineOwners, -1, GAME_MINE_COUNT);
    for (i = 0; i < GAME_BOAT_COUNT; i++) {
        memset(&m_boats[i], 0, sizeof(m_boats[i]));
        m_boats[i].id = static_cast<i8>(i);
        m_boats[i].heroId = -1;
    }
    memset(m_dailyEventFlags, 0, sizeof(m_dailyEventFlags));
    memset(m_boatSlots, -1, sizeof(m_boatSlots));
    m_ultimateArtifactY = -1;
    m_ultimateArtifactX = m_ultimateArtifactY;
    memset(m_obeliskVisitors, 0, 48);
    strcpy(gpGame->m_saveName, "NEWGAME");
    giCurPlayer = 0;
    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    giCurPlayerBit = static_cast<u8>(1 << giCurPlayer);
    giCurWatchPlayer = giCurPlayer;
    while (!gbThisNetHumanPlayer[giCurWatchPlayer])
        giCurWatchPlayer = (giCurWatchPlayer + 1) % m_playerCount;
    giCurWatchPlayerBit = static_cast<u8>(1 << giCurWatchPlayer);
    gpAdvManager->CheckSetEvilInterface(0, -1);
    bShowIt = gbThisNetHumanPlayer[giCurPlayer];
}

VA(0x004735bf, 0xc27)
void game::LoadGame(char* filename, i32 loadFromFile, i32) {
    LogStr("LG1");
    if (loadFromFile) {
        SetupOrigData();
        return;
    }
    LogStr("LG2");
    i32 humansLoaded3 = 0;
    gbGameOver = false;
    m_gameLoaded = 1;

    char path28[452];
    if (loadFromFile || strnicmp(filename, "RMT", 3) == 0)
        sprintf(path28, "%s%s", ".\\DATA\\", filename);
    else
        sprintf(path28, "%s%s", gcGamePath, filename);

    i32 file0 = open(path28, _O_BINARY);
    if (file0 == -1)
        FileError(path28);
    ClearMapExtra();

    i8 expansionMarker0 = 0;
    i32 width8;
    i32 height9[11];
    read(file0, &width8, 4);
    if (width8 == -1) {
        expansionMarker0 = 1;
        read(file0, &width8, 4);
    }
    read(file0, height9, 4);
    SetMapSize(width8, height9[0]);
    read(file0, &m_mapHeader, sizeof(m_mapHeader));
    read(file0, m_setupPlayerColor, CAMPAIGN_SETUP_RESET_SIZE);
    read(file0, &gbIAmGreatest, 1);
    read(file0, this, 2);
    read(file0, &giMonthType, 1);
    read(file0, &giMonthTypeExtra, 1);
    read(file0, &giWeekType, 1);
    read(file0, &giWeekTypeExtra, 1);
    read(file0, cPlayerNames, 126);

    char oldData3[40];
    read(file0, oldData3, 36);
    read(file0, &gbInCampaign, 4);
    if (gbInCampaign == 1) {
        read(file0, &m_campaignType, CAMPAIGN_STATE_RESET_SIZE);
    } else if (gbInCampaign == 2) {
        xIsPlayingExpansionCampaign = 1;
        gbInCampaign = false;
        read(file0, &xCampaign, EXPANSION_CAMPAIGN_SAVE_PREFIX_SIZE);
    }
    if (expansionMarker0)
        read(file0, &xIsExpansionMap, 1);

    gpAdvManager->PurgeMapChangeQueue();
    read(file0, &giMapChangeCtr, 4);
    read(file0, oldData3, 14);
    if (strnicmp(filename, "RMT", 3) != 0)
        sprintf(gpGame->m_saveName, filename);
    read(file0, &m_playerCount, 1);

    char currentPlayer6[8];
    read(file0, currentPlayer6, 1);
    giCurPlayer = currentPlayer6[0];
    read(file0, &m_deadPlayerCount, 1);
    read(file0, m_playerDead, 6);

    char humanFlags1[8];
    read(file0, humanFlags1, 6);
    i32 i29;
    for (i29 = 0; i29 < GAME_PLAYER_COUNT; i29++) {
        if (humanFlags1[i29] && humansLoaded3 < (&giNumHumanPlayers)[0]) {
            humansLoaded3++;
            gbHumanPlayer[i29] = 1;
        } else {
            gbHumanPlayer[i29] = 0;
        }
    }
    for (i29 = 0; i29 < GAME_PLAYER_COUNT; i29++) {
        if (gbHumanPlayer[i29]) {
            if (!gbRemoteOn || (&i29)[0] == giThisGamePos)
                gbThisNetHumanPlayer[i29] = 1;
            else
                gbThisNetHumanPlayer[i29] = 0;
        } else {
            gbThisNetHumanPlayer[i29] = 0;
        }
    }

    read(file0, &m_day, 2);
    read(file0, &m_week, 2);
    read(file0, &m_month, 2);
    giCurTurn = (m_week - 1) * 7 + (m_month - 1) * 28 + m_day;
    for (i29 = 0; i29 < GAME_PLAYER_COUNT; i29++)
        m_players[i29].Read(file0);

    read(file0, &m_obeliskCount, 1);
    for (i29 = 0; i29 < GAME_HERO_COUNT; i29++)
        m_heroRecs[i29].Read(file0, expansionMarker0);
    read(file0, m_availableHeroes, GAME_HERO_COUNT);
    read(file0, m_castleRecs, sizeof(m_castleRecs));
    read(file0, m_castleOwners, sizeof(m_castleOwners));
    read(file0, m_dailyEventFlags, sizeof(m_dailyEventFlags));
    read(file0, m_mines, sizeof(m_mines));
    read(file0, m_mineOwners, sizeof(m_mineOwners));
    if (expansionMarker0)
        read(file0, m_randomArtifacts, ARTIFACT_COUNT);
    else
        read(file0, m_randomArtifacts, RANDOM_ARTIFACT_BASE_TABLE_SIZE);
    read(file0, m_boats, sizeof(m_boats));
    read(file0, m_boatSlots, sizeof(m_boatSlots));
    read(file0, m_obeliskVisitors, sizeof(m_obeliskVisitors));
    read(file0, &m_ultimateArtifactX, 1);
    read(file0, &m_ultimateArtifactY, 1);
    read(file0, &m_ultimateArtifactId, 1);
    read(file0, m_rumour, sizeof(m_rumour));
    read(file0, m_defaultPlayerNames, sizeof(m_defaultPlayerNames));
    read(file0, &m_rumourEventCount, 4);
    read(file0, m_rumourEventIndices, m_rumourEventCount * 2);
    read(file0, &m_timeEventCount, 4);
    read(file0, m_timeEventIndices, m_timeEventCount * 2);
    read(file0, &m_mapEventCount, 4);
    read(file0, m_mapEventIndices, m_mapEventCount * 2);

    char marker0[8];
    read(file0, marker0, 4);
    read(file0, &iMaxMapExtra, 4);
    read(file0, marker0, 4);
    ppMapExtra = reinterpret_cast<void**>(H2_ALLOC(iMaxMapExtra * 4, 1306));
    pwSizeOfMapExtra = reinterpret_cast<i16*>(H2_ALLOC(iMaxMapExtra * 2, 1307));
    memset(ppMapExtra, 0, iMaxMapExtra * 4);
    memset(pwSizeOfMapExtra, 0, iMaxMapExtra * 2);
    for (i29 = 1; (&i29)[0] < iMaxMapExtra; i29++) {
        read(file0, marker0, 4);
        read(file0, pwSizeOfMapExtra + i29, 2);
        ppMapExtra[i29] = H2_ALLOC(pwSizeOfMapExtra[i29], 1316);
        read(file0, ppMapExtra[i29], pwSizeOfMapExtra[i29]);
    }
    read(file0, marker0, 4);
    read(file0, mapExtra, MAP_WIDTH * MAP_HEIGHT);
    read(file0, marker0, 4);
    m_worldMap.Read(file0, 0);
    read(file0, marker0, 4);
    close(file0);

    gpAdvManager->m_heroContextLocked = 0;
    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    giCurPlayerBit = static_cast<u8>(1 << giCurPlayer);
    giCurWatchPlayer = giCurPlayer;
    while (!gbThisNetHumanPlayer[giCurWatchPlayer])
        giCurWatchPlayer = (giCurWatchPlayer + 1) % m_playerCount;
    giCurWatchPlayerBit = static_cast<u8>(1 << giCurWatchPlayer);
    bShowIt = gbThisNetHumanPlayer[giCurPlayer];
    SetupAdjacentMons();
    LogStr("LG3");
    gpAdvManager->CheckSetEvilInterface(0, -1);
}

VA(0x004741e6, 0x3ee)
void game::GiveTroopsToNeutralTown(i32 townId) {
    i32 kn;
    i32 jb;
    i32 idx;
    i32 random;
    i32 cnt;
    i32 divisor;

    if ((m_castleRecs[townId].m_x > 0 || m_castleRecs[townId].m_y > 0)
        && m_castleRecs[townId].m_owner < 0) {
        random = Random(1, 15);
        divisor = giCurTurn / 10;
        if (divisor != 0)
            random += Random(0, divisor);

        if (random <= 5) {
            jb = 10;
            cnt = Random(8, 15);
        } else if (random <= 10) {
            jb = 20;
            cnt = Random(5, 7);
        } else if (random <= 13) {
            jb = 30;
            cnt = Random(3, 5);
        } else if (random <= 15) {
            jb = 40;
            cnt = Random(1, 3);
        } else {
            jb = 50;
            cnt = 1;
        }

        cnt += giCurTurn / 20;
        switch (m_castleRecs[townId].m_type + jb) {
            case 10:
                idx = 0;
                break;
            case 20:
                idx = 1;
                break;
            case 30:
                idx = 3;
                break;
            case 40:
                idx = 5;
                break;
            case 50:
                idx = 7;
                break;
            case 11:
                idx = 11;
                break;
            case 21:
                idx = 12;
                break;
            case 31:
                idx = 14;
                break;
            case 41:
                idx = 15;
                break;
            case 51:
                idx = 17;
                break;
            case 12:
                idx = 20;
                break;
            case 22:
                idx = 21;
                break;
            case 32:
                idx = 23;
                break;
            case 42:
                idx = 25;
                break;
            case 52:
                idx = 27;
                break;
            case 13:
                idx = 29;
                break;
            case 23:
                idx = 30;
                break;
            case 33:
                idx = 31;
                break;
            case 43:
                idx = 32;
                break;
            case 53:
                idx = 34;
                break;
            case 14:
                idx = 38;
                break;
            case 24:
                idx = 39;
                break;
            case 34:
                idx = 40;
                break;
            case 44:
                idx = 42;
                break;
            case 54:
                idx = 43;
                break;
            case 15:
                idx = 47;
                break;
            case 25:
                idx = 48;
                break;
            case 35:
                idx = 50;
                break;
            case 45:
                idx = 52;
                break;
            case 55:
                idx = 54;
                break;
            default:;
        }
        GiveArmy(&m_castleRecs[townId].m_army, idx, cnt, -1);
    }
}

VA(0x004745d4, 0xa4)
void game::GiveTroopsToNeutralTowns(void) {
    i32 i;
    for (i = 0; i < GAME_TOWN_COUNT; i++) {
        GiveTroopsToNeutralTown(i);
        if (m_castleRecs[i].m_buildings & IDX(TOWN_BUILDING_CASTLE)) {
            if (Random(0, 100) < NEUTRAL_TOWN_CASTLE_REINFORCE_CHANCE)
                GiveTroopsToNeutralTown(i);
        } else {
            if (Random(0, 100) < NEUTRAL_TOWN_REINFORCE_CHANCE)
                GiveTroopsToNeutralTown(i);
        }
    }
}

// @early-stop
// reloc-masked: identical 0x1dd0-byte instruction stream; 126/126 relocations, residual is compiler literal/global symbol identity
VA(0x00474678, 0x1dd0)
void game::NewMap(char* filename) {
    char* extension0;
    i32 randomColor2;
    i32 nextHuman6;
    i32 player2;
    i32 townIndex9;
    i32 heroIndex1;
    i32 pass27;
    i32 selectedTown14;
    i32 ultimateDistance5;
    i32 ultimateTries4;
    i32 campaignHero15;
    i32 heroClass5;
    i32 heroX6;
    i32 heroY16;
    i8 setupClass12;
    i32 specialPortrait6;
    char* specialName3;
    i32 specialClass6;
    i32 resource13;

    extension0 = FindLastToken(gMapName, '.');
    if (extension0 != 0 && StrEqNoCase(extension0 + 1, "MX2"))
        xIsExpansionMap = 1;
    if (xIsExpansionMap)
        gTownEligibleBuildMask[IDX(FACTION_NECROMANCER)] |= 4;
    else
        gTownEligibleBuildMask[IDX(FACTION_NECROMANCER)] &= ~4;

    gbInNewGameSetup = true;
    giCurPlayer = 0;
    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    giCurPlayerBit = static_cast<u8>(1 << giCurPlayer);
    giCurWatchPlayerBit = giCurPlayerBit;
    giCurWatchPlayer = giCurPlayer;
    randomColor2 = Random(0, 5);
    nextHuman6 = giNumHumanPlayers;

    for (player2 = 0; player2 < GAME_PLAYER_COUNT; player2++) {
        if (player2 >= static_cast<u8>(gpGame->m_mapHeader.playerCount)) {
            gbSetupGamePosToRealGamePos[player2] = -1;
        } else {
            if (m_setupPlayerNetworkId[player2] == 10)
                gbSetupGamePosToRealGamePos[player2] = static_cast<i8>(nextHuman6++);
            else
                gbSetupGamePosToRealGamePos[player2] = m_setupPlayerNetworkId[player2];
        }
    }
    for (player2 = 0; player2 < GAME_PLAYER_COUNT; player2++) {
        m_players[player2].m_color = -1;
        gcColorToPlayerPos[player2] = -1;
        gcColorToSetupPos[player2] = -1;
        if (gpGame->m_setupPlayerRace[player2] == 7)
            gpGame->m_setupPlayerRace[player2] = static_cast<i8>(randomColor2);
        randomColor2 = (randomColor2 + 1) % GAME_PLAYER_COUNT;
    }
    for (player2 = 0; player2 < m_playerCount; player2++)
        gcColorToSetupPos[m_setupPlayerColor[player2]] = static_cast<i8>(player2);
    for (player2 = 0; player2 < m_playerCount; player2++)
        m_players[gbSetupGamePosToRealGamePos[player2]].m_color = m_setupPlayerColor[player2];
    for (player2 = 0; player2 < m_playerCount; player2++)
        gcColorToPlayerPos[m_players[player2].m_color] = static_cast<i8>(player2);
    for (player2 = 0; player2 < m_playerCount; player2++) {
        m_players[player2].m_townCount = 0;
        m_players[player2].m_townLocatorPage = 0;
        m_players[player2].m_currentTown = -1;
        m_players[player2].m_heroCount = 0;
        m_players[player2].m_heroLocatorPage = 0;
        m_players[player2].m_currentHero = -1;
    }

    RandomizeHeroPool();
    strcpy(gMapName, filename);
    LoadMap(gMapName);
    InitRandomArtifacts();
    ProcessRandomObjects();
    ProcessMapExtra();
    SetupTowns();
    InitializePasswords();
    for (player2 = 0; player2 < GAME_PLAYER_COUNT; player2++)
        m_players[player2].m_barrierTents = 0;
    RandomizeEvents();
    ProcessOnMapHeroes();
    m_deadPlayerCount = 0;
    for (player2 = m_playerCount; player2 < GAME_PLAYER_COUNT; player2++)
        m_playerDead[player2] = 1;

    if (m_mapHeader.victoryCondition == 4 || m_mapHeader.victoryCondition == 2) {
        m_mapHeader.computerAlsoWins = 1;
        m_mapHeader.allowNormalVictory = 0;
    }
    if (m_mapHeader.victoryCondition == 4) {
        townIndex9 = 0;
        for (player2 = 0; player2 < GAME_PLAYER_COUNT; player2++) {
            if (m_mapHeader.playerEnabled[player2] != 0)
                townIndex9++;
            if (m_mapHeader.victoryConditionValue + 1 == townIndex9) {
                m_mapHeader.victorySideThreshold = static_cast<u16>(player2);
                player2 = 99;
            }
        }
    }
    if (m_mapHeader.victoryCondition == 3)
        m_mapHeader.computerAlsoWins = 1;

    for (player2 = 0; player2 < m_playerCount; player2++) {
        m_players[player2].m_ultimateArtifactHintChance = 0;
        m_players[player2].m_ultimateArtifactHintX = -1;
        m_players[player2].m_ultimateArtifactHintY = -1;
        heroIndex1 = 0;
        selectedTown14 = -1;
        if (m_mapHeader.unknown25 == 0 && m_players[player2].m_townCount > 0) {
            for (pass27 = 0; pass27 < 2; pass27++) {
                for (townIndex9 = 0; townIndex9 < m_players[player2].m_townCount; townIndex9++) {
                    if (selectedTown14 == -1
                        && m_castleRecs[(m_players + player2)->m_townIds[townIndex9]]
                                   .m_occupyingHeroId
                               == -1
                        && ((m_castleRecs[(m_players + player2)->m_townIds[townIndex9]].m_buildings
                             & IDX(TOWN_BUILDING_CASTLE))
                                != 0
                            || pass27 == 1))
                        selectedTown14 = townIndex9;
                }
            }
        }
        if (selectedTown14 != -1) {
            m_players[player2].m_heroIds[m_players[player2].m_heroCount] =
                static_cast<i8>(GetNewHeroId(
                    player2,
                    m_castleRecs[m_players[player2].m_townIds[selectedTown14]].m_type,
                    0
                ));
            m_availableHeroes[m_players[player2].m_heroIds[m_players[player2].m_heroCount]] =
                static_cast<i8>(player2);
            m_heroRecs[m_players[player2].m_heroIds[m_players[player2].m_heroCount]].m_owner =
                static_cast<i8>(player2);
            m_heroRecs[m_players[player2].m_heroIds[m_players[player2].m_heroCount]].m_x =
                m_castleRecs[m_players[player2].m_townIds[selectedTown14]].m_x;
            m_heroRecs[m_players[player2].m_heroIds[m_players[player2].m_heroCount]].m_y =
                m_castleRecs[m_players[player2].m_townIds[selectedTown14]].m_y;
            m_castleRecs[m_players[player2].m_townIds[selectedTown14]].m_occupyingHeroId =
                m_players[player2].m_heroIds[m_players[player2].m_heroCount];
            SetVisibility(
                m_heroRecs[m_players[player2].m_heroIds[m_players[player2].m_heroCount]].m_x,
                m_heroRecs[m_players[player2].m_heroIds[m_players[player2].m_heroCount]].m_y,
                player2,
                giVisRange[static_cast<i8>(m_heroRecs[m_players[player2].m_heroIds[0]]
                                               .m_cursorType)]
            );
            m_players[player2].m_heroCount++;
        }
    }

    for (player2 = 0; player2 < m_playerCount; player2++) {
        if (player2 == 0 && gbInCampaign
            && (m_campaignAwards[IDX(CAMPAIGN_AWARD_SORCERESS_GUILD)] != 0
                || m_campaignAwards[IDX(CAMPAIGN_AWARD_DWARFBANE)] != 0)) {
            if (m_campaignAwards[IDX(CAMPAIGN_AWARD_SORCERESS_GUILD)] != 0)
                specialPortrait6 = 2;
            else
                specialPortrait6 = 5;
            for (campaignHero15 = 0; campaignHero15 < GAME_HERO_COUNT; campaignHero15++) {
                if (m_heroRecs[campaignHero15].m_cursorType == specialPortrait6
                    && m_availableHeroes[campaignHero15] == -1)
                    break;
            }
            if (campaignHero15 < GAME_HERO_COUNT) {
                if (m_campaignAwards[IDX(CAMPAIGN_AWARD_SORCERESS_GUILD)] != 0) {
                    m_heroRecs[campaignHero15].m_experience += 5000;
                    m_heroRecs[campaignHero15].CheckLevel();
                    strcpy(m_heroRecs[campaignHero15].m_name, "Sister Eliza");
                    m_heroRecs[campaignHero15].m_portrait = 56;
                } else {
                    m_heroRecs[campaignHero15].m_experience += 5000;
                    m_heroRecs[campaignHero15].CheckLevel();
                    strcpy(m_heroRecs[campaignHero15].m_name, "Brother Brax");
                    m_heroRecs[campaignHero15].m_portrait = 59;
                }
                m_players[player2].m_availableHeroIds[0] = static_cast<char>(campaignHero15);
                m_availableHeroes[m_players[player2].m_availableHeroIds[0]] = 64;
                heroClass5 = m_heroRecs[campaignHero15].m_cursorType;
                goto secondHero;
            }
        }
        {
            if (xIsPlayingExpansionCampaign && player2 == 0) {
                specialClass6 = GAME_SPECIAL_HERO_CLASS_NONE;
                if (xCampaign.HasAward(6)) {
                    specialClass6 = 4;
                    specialName3 = xCampaign.JosephName();
                    specialPortrait6 = 64;
                } else if (xCampaign.HasAward(7)) {
                    specialClass6 = 1;
                    specialName3 = xCampaign.IvanName();
                    specialPortrait6 = 63;
                }
                if (specialClass6 != static_cast<u32>(GAME_SPECIAL_HERO_CLASS_NONE)) {
                    for (campaignHero15 = 0; campaignHero15 < GAME_HERO_COUNT; campaignHero15++) {
                        if (m_heroRecs[campaignHero15].m_cursorType == specialClass6
                            && m_availableHeroes[campaignHero15] == -1)
                            break;
                    }
                    if (campaignHero15 < GAME_HERO_COUNT) {
                        m_heroRecs[campaignHero15].m_experience = 5000;
                        m_heroRecs[campaignHero15].CheckLevel();
                        strcpy(m_heroRecs[campaignHero15].m_name, specialName3);
                        m_heroRecs[campaignHero15].m_portrait = specialPortrait6;
                        m_players[player2].m_availableHeroIds[0] =
                            static_cast<char>(campaignHero15);
                        m_availableHeroes[m_players[player2].m_availableHeroIds[0]] = 64;
                        heroClass5 = m_heroRecs[campaignHero15].m_cursorType;
                        goto secondHero;
                    }
                }
            }
            heroClass5 = Random(0, 5);
            if (m_setupPlayerRace[gcColorToSetupPos[m_players[player2].m_color]] < 6)
                heroClass5 = m_setupPlayerRace[gcColorToSetupPos[m_players[player2].m_color]];
            m_players[player2].m_availableHeroIds[0] =
                static_cast<char>(GetNewHeroId(player2, heroClass5, 0));
            m_availableHeroes[m_players[player2].m_availableHeroIds[0]] = 64;
        }
    secondHero:
        heroClass5 = (Random(1, 5) + heroClass5) % 6;
        m_players[player2].m_availableHeroIds[1] =
            static_cast<char>(GetNewHeroId(player2, heroClass5, 0));
        m_availableHeroes[m_players[player2].m_availableHeroIds[1]] = 64;
    }

    for (player2 = 0; player2 < m_playerCount; player2++) {
        for (campaignHero15 = 0; campaignHero15 < m_players[player2].m_heroCount;
             campaignHero15++) {
            heroX6 = m_heroRecs[m_players[player2].m_heroIds[campaignHero15]].m_x;
            heroY16 = m_heroRecs[m_players[player2].m_heroIds[campaignHero15]].m_y;
            m_heroRecs[m_players[player2].m_heroIds[campaignHero15]].m_locationType =
                m_worldMap.GetCell(heroX6, heroY16)->m_triggerType;
            m_heroRecs[m_players[player2].m_heroIds[campaignHero15]].m_occupiedTown =
                m_worldMap.GetCell(heroX6, heroY16)->m_objectMetadata;
            m_worldMap.GetCell(heroX6, heroY16)->m_triggerType = IDX(MAP_TRIGGER_HERO);
            m_worldMap.GetCell(heroX6, heroY16)->m_objectMetadata =
                m_players[player2].m_heroIds[campaignHero15];
        }
        if (m_players[player2].m_heroCount > 0)
            m_players[player2].m_currentHero = m_players[player2].m_heroIds[0];
        else if (m_players[player2].m_townCount > 0)
            m_players[player2].m_currentTown = m_players[player2].m_townIds[0];
    }

    player2 = -1;
    townIndex9 = -1;
    ultimateTries4 = 0;
    ultimateDistance5 = Random(1, 20) + Random(1, 20) + Random(1, 30);
    while (player2 < 9 || townIndex9 < 9 || player2 > MAP_WIDTH - 10 || townIndex9 > MAP_HEIGHT - 10
           || m_worldMap.GetCell(player2, townIndex9)->m_objectIndex != IDX(MAPCELL_SPRITE_NONE)
           || m_worldMap.GetCell(player2, townIndex9)->m_overlayIndex != IDX(MAPCELL_SPRITE_NONE)
           || giGroundToTerrain[m_worldMap.GetCell(player2, townIndex9)->m_terrainImageIndex] == 0
           || (giNumHumanPlayers == 1 && ultimateTries4 < 200
               && ultimateDistance5
                      >= abs(player2 - m_heroRecs[m_players[0].m_heroIds[0]].m_x)
                             + abs(townIndex9 - m_heroRecs[m_players[0].m_heroIds[0]].m_y))) {
        if (ultimateTries4 < 400 && giUABaseX > 0) {
            player2 = giUABaseX + (giUARadius != 0 ? Random(-giUARadius, giUARadius) : 0);
            townIndex9 = giUABaseY + (giUARadius != 0 ? Random(-giUARadius, giUARadius) : 0);
        } else {
            player2 = Random(9, MAP_WIDTH - 10);
            townIndex9 = Random(9, MAP_HEIGHT - 10);
        }
        ultimateDistance5 = Random(1, 20) + Random(1, 20) + Random(1, 30);
        ultimateTries4++;
    }
    m_ultimateArtifactX = static_cast<i8>(player2);
    m_ultimateArtifactY = static_cast<i8>(townIndex9);
    m_ultimateArtifactId = static_cast<i8>(Random(ARTIFACT_ULTIMATE_BOOK, ARTIFACT_GOLDEN_GOOSE));
    if (gbInCampaign
        && ((m_campaignType == 0 && static_cast<i8>(m_campaignScenario) + 1 == 8)
            || (m_campaignType == 1 && static_cast<i8>(m_campaignScenario) + 1 == 9)))
        m_ultimateArtifactId = IDX(ARTIFACT_ULTIMATE_CROWN);
    for (player2 = 0; player2 < m_playerCount; player2++) {
        if (gbHumanPlayer[player2]) {
            m_players[player2].m_aiDifficulty = 3;
            memcpy(m_players[player2].m_resources, gInitResourcesHuman[m_difficulty], 28);
            if (m_playerHandicap[player2] != 0) {
                for (townIndex9 = 0; townIndex9 < 7; townIndex9++) {
                    double resourceScale;
                    if (m_playerHandicap[player2] == 1)
                        resourceScale = 0.85;
                    else
                        resourceScale = 0.7;
                    (m_players + player2)->m_resources[townIndex9] = static_cast<i32>(
                        (m_players + player2)->m_resources[townIndex9] * resourceScale
                    );
                }
            }
        } else {
            m_players[player2].m_aiDifficulty = Random(0, 2);
            memcpy(m_players[player2].m_resources, gInitResourcesComputer[m_difficulty], 28);
        }
    }
    SetupAdjacentMons();
    if (m_mapHeader.lossCondition == 2) {
        ultimateDistance5 = m_mapHeader.lossConditionValue;
        ultimateTries4 = m_mapHeader.lossTownY;
        m_mapHeader.lossConditionValue = 0;
        if (m_worldMap.GetCell(ultimateDistance5, ultimateTries4)->m_triggerType
            == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERMAID))
            m_mapHeader.lossConditionValue =
                m_worldMap.GetCell(ultimateDistance5, ultimateTries4)->m_objectMetadata;
        else {
            if (m_worldMap.GetCell(ultimateDistance5, ultimateTries4 - 1)->m_triggerType
                == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERMAID))
                m_mapHeader.lossConditionValue =
                    m_worldMap.GetCell(ultimateDistance5, ultimateTries4 - 1)->m_objectMetadata;
            else
                m_mapHeader.lossCondition = 0;
        }
    }
    if (m_mapHeader.victoryCondition == 2) {
        ultimateDistance5 = m_mapHeader.victoryConditionValue;
        ultimateTries4 = m_mapHeader.victoryTownY;
        m_mapHeader.victoryConditionValue = 0;
        if (m_worldMap.GetCell(ultimateDistance5, ultimateTries4)->m_triggerType
            == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERMAID))
            m_mapHeader.victoryConditionValue =
                m_worldMap.GetCell(ultimateDistance5, ultimateTries4)->m_objectMetadata;
        else {
            if (m_worldMap.GetCell(ultimateDistance5, ultimateTries4 - 1)->m_triggerType
                == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERMAID))
                m_mapHeader.victoryConditionValue =
                    m_worldMap.GetCell(ultimateDistance5, ultimateTries4 - 1)->m_objectMetadata;
            else
                m_mapHeader.victoryCondition = 0;
        }
    }
    for (player2 = 0; player2 < m_playerCount; player2++) {
        heroClass5 = 0;
        if (m_setupPlayerRace[gcColorToSetupPos[m_players[player2].m_color]] >= 0
            && m_setupPlayerRace[gcColorToSetupPos[m_players[player2].m_color]] < 6) {
            heroClass5 = m_setupPlayerRace[gcColorToSetupPos[m_players[player2].m_color]];
        } else {
            if (!!m_players[player2].m_townCount) {
                heroClass5 = gpGame->m_castleRecs[m_players[player2].TownId(0)].m_type;
            } else if (!!m_players[player2].m_heroCount) {
                heroClass5 = gpGame->m_heroRecs[m_players[player2].HeroId(0)].m_cursorType;
            }
        }
        m_players[player2].m_evilInterface = heroClass5 == 1 || heroClass5 == 3 || heroClass5 == 5;
        if (gbInCampaign && player2 == 0)
            m_players[player2].m_evilInterface = m_campaignType == 1;
        for (townIndex9 = 0; townIndex9 < gpGame->m_players[player2].m_townCount; townIndex9++)
            GetCastle(gpGame->m_players[player2].m_townIds[townIndex9])->GiveSpells(0);
        gpGame->m_players[player2].m_minimumHeroCount = gpGame->m_players[player2].m_heroCount;
    }
    gpPhilAI->GetGameAIVars();
    gbInNewGameSetup = false;
    SetupNewRumour();
    gpAdvManager->CheckSetEvilInterface(0, -1);
    return;
}

inline town* GetCastleSlot(game* instance, i32 index) {
    return &instance->m_castleRecs[index];
}

// @semantic
// soft/TU-cumulative: frame 0x1d8 exact; base 0x25fe vs retail 0x2601. Residual is three MAP_WIDTH/xPos2 commutative compare encodings (+0x9d, +0x1db7, +0x1ee4; one byte each) and equal-length packed-m_objectMetadata RHS/cell-word evaluation order at +0x2e1; val|0, setter, union, cast, and bitfield-type spellings do not steer MSVC 4.2.
VA(0x00476448, 0x2601)
void game::RandomizeEvents(void) {
    i32 shrineId8 = 1;
    i32 bottleId11 = 1;
    i32 jailId28 = 1;
    i32 sphinxId26 = 1;
    i32 tentId10 = 1;
    i32 hutId11 = 1;
    i32 eyeId13 = 1;
    i32 row18;
    i32 signId4 = 1;
    u32 extraIndex3;
    i32 yPos19;
    i32 xPos2;
    i32 j9;
    i32 value26;
    i32 randomValue7;
    i32 mineId2;
    i32 column1;
    i32 upperCount;
    i32 lowerCount16;
    i32 upperTilesets29[5];
    i32 upperIndexes1[5];
    i32 lowerTilesets4[5];
    i32 lowerIndexes7[5];
    i32 artifactChoices17[10];
    EventExtra* mapEvent1;
    mapCell* townEntrance;
    mapCell* cell2;
    mapCellExtra* extra15;
    town* townRec4;
    mapEventExtra* eventData16;
    i32 valid27;

    m_mapEventCount = 0;
    memset(m_mapEventIndices, 0, sizeof(m_mapEventIndices));

    for (yPos19 = 0; yPos19 < MAP_HEIGHT; yPos19++) {
        for (xPos2 = 0; (xPos2 | 0) < MAP_WIDTH; xPos2++) {
            cell2 = m_worldMap.Row(yPos19) + xPos2;
            switch (cell2->m_triggerType) {
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WITCH_HUT:
                    cell2->m_objectMetadata = 12;
                    while (cell2->m_objectMetadata == 12 || cell2->m_objectMetadata == 6)
                        cell2->m_objectMetadata = Random(0, 13);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT:
                    cell2->m_objectTileset = 0;
                    cell2->m_objectIndex = IDX(MAPCELL_SPRITE_NONE);
                    cell2->m_objectMetadata = 0;
                    cell2->m_triggerType = 0;
                    CreateBoat(xPos2, yPos19, 1);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SPHINX:
                    eventData16 =
                        reinterpret_cast<mapEventExtra*>(ppMapExtra[cell2->m_objectMetadata]);
                    if (strlen(eventData16->riddle) > 1 && eventData16->answerCount >= 1)
                        eventData16->active = 1;
                    else
                        eventData16->active = 0;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MAP_EVENT:
                    m_mapEventIndices[m_mapEventCount] = cell2->m_objectMetadata;
                    mapEvent1 = reinterpret_cast<EventExtra*>(ppMapExtra[cell2->m_objectMetadata]);
                    mapEvent1->x = static_cast<i16>(xPos2);
                    mapEvent1->y = static_cast<i16>(yPos19);
                    mapEvent1->active = 1;
                    cell2->m_objectMetadata = 0;
                    cell2->m_triggerType = 0;
                    cell2->m_objectIndex = IDX(MAPCELL_SPRITE_NONE);
                    cell2->m_objectTileset = 0;
                    m_mapEventCount++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_GAZEBO:
                    cell2->m_objectMetadata = bottleId11++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_FORT:
                    cell2->m_objectMetadata = jailId28++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WITCH_DOCTOR_HUT:
                    cell2->m_objectMetadata = sphinxId26;
                    sphinxId26++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERCENARY_CAMP:
                    cell2->m_objectMetadata = tentId10++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_STANDING_STONES:
                    if (xPos2 <= 0
                        || m_worldMap.GetCell(xPos2 - 1, yPos19)->m_triggerType
                               != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_STANDING_STONES))
                        cell2->m_objectMetadata = hutId11++;
                    else
                        cell2->m_objectMetadata =
                            m_worldMap.GetCell(xPos2 - 1, yPos19)->m_objectMetadata;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_XANADU:
                    cell2->m_objectMetadata = signId4++;
                    break;
                case MAP_OBJECT_WHIRLPOOL:
                    cell2->m_triggerType |= MAP_TRIGGER_ACTION_FLAG;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_OBELISK:
                    cell2->m_objectMetadata = shrineId8++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_FLOTSAM:
                    cell2->m_objectMetadata = Random(0, 3);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SKELETON:
                    if (!HasObjectTilesetIndex(xPos2, yPos19, IDX(TILESET_OBJNDSRT), 84)) {
                        cell2->m_triggerType &= MAP_TRIGGER_TYPE_MASK;
                    } else if (Random(0, 9) > 2) {
                        cell2->m_objectMetadata = 1;
                    } else {
                        cell2->m_objectMetadata = GetRandomArtifactId(14, 1) + 2;
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WAGON:
                    randomValue7 = Random(0, 100);
                    if (randomValue7 < 40)
                        cell2->m_objectMetadata = 0;
                    else if (randomValue7 < 50)
                        cell2->m_objectMetadata =
                            GetRandomArtifactId(12, 1) | MAP_EVENT_ARTIFACT_CONDITION_FLAG;
                    else
                        cell2->m_objectMetadata = Random(0, 5) + (Random(2, 5) << 4) + 1;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_LEAN_TO:
                    cell2->m_objectMetadata = Random(0, 5) + (Random(1, 4) << 4) + 1;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DAEMON_CAVE:
                    switch (Random(0, 99) % 10) {
                        case 0:
                        case 1:
                        case 2:
                            cell2->m_objectMetadata = 2;
                            break;
                        case 3:
                            cell2->m_objectMetadata = 3;
                            break;
                        case 4:
                        case 5:
                        case 6:
                            cell2->m_objectMetadata = 4;
                            break;
                        case 7:
                        case 8:
                        case 9:
                            cell2->m_objectMetadata = 5;
                            break;
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREASURE_CHEST:
                    if (giGroundToTerrain[cell2->m_terrainImageIndex] == 0) {
                        cell2->m_triggerType = IDX(MAP_TRIGGER_SEA_CHEST);
                        randomValue7 = Random(0, 100);
                        if (randomValue7 < 20)
                            cell2->m_objectMetadata = 0;
                        else if (randomValue7 < 90)
                            cell2->m_objectMetadata = 1;
                        else
                            cell2->m_objectMetadata =
                                GetRandomArtifactId(8, 1) | MAP_EVENT_ARTIFACT_GUARD_FLAG;
                    } else {
                        randomValue7 = Random(0, 100);
                        if (randomValue7 < 32)
                            cell2->m_objectMetadata = 2;
                        else if (randomValue7 < 64)
                            cell2->m_objectMetadata = 3;
                        else if (randomValue7 < 95)
                            cell2->m_objectMetadata = 4;
                        else
                            cell2->m_objectMetadata =
                                GetRandomArtifactId(8, 1) | MAP_EVENT_ARTIFACT_GUARD_FLAG;
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CAMPFIRE:
                    cell2->m_objectMetadata = Random(4, 6) << 4;
                    cell2->m_objectMetadata |= Random(0, 5);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ANCIENT_LAMP:
                    cell2->m_objectMetadata = Random(0, 2) + 2;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHIPWRECK_SURVIVOR:
                    randomValue7 = Random(0, 100);
                    if (randomValue7 < 60)
                        cell2->m_objectMetadata = GetRandomArtifactId(8, 1);
                    else if (randomValue7 < 80)
                        cell2->m_objectMetadata = GetRandomArtifactId(4, 1);
                    else
                        cell2->m_objectMetadata = GetRandomArtifactId(2, 1);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_GRAVEYARD:
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHIPWRECK:
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DERELICT_SHIP:
                    switch (Random(0, 99) % 10) {
                        case 0:
                        case 1:
                        case 2:
                            cell2->m_objectMetadata = 2;
                            break;
                        case 3:
                        case 4:
                        case 5:
                            cell2->m_objectMetadata = 3;
                            break;
                        case 6:
                        case 7:
                        case 8:
                            cell2->m_objectMetadata = 4;
                            break;
                        case 9:
                            cell2->m_objectMetadata = 5;
                            break;
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARCHER_HOUSE:
                    cell2->m_objectMetadata = Random(10, 25);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_GOBLIN_HUT:
                    cell2->m_objectMetadata = Random(15, 40);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DWARF_COTTAGE:
                    cell2->m_objectMetadata = Random(0, 20) + 1;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_PEASANT_HUT:
                    cell2->m_objectMetadata = Random(0, 40) + 1;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_LOG_CABIN:
                    cell2->m_objectMetadata = Random(20, 50);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WATER_WHEEL:
                    cell2->m_objectMetadata = 1;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTESIAN_SPRING:
                    cell2->m_objectMetadata = 1;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MAGIC_GARDEN:
                    cell2->m_objectMetadata = Random(0, 1) == 0 ? 6 : 7;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREE_OF_KNOWLEDGE:
                    cell2->m_objectMetadata = (Random(1, 3) << 6) | eyeId13++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER:
                    if (cell2->m_objectMetadata == 0) {
                        cell2->m_objectMetadata = GetRandomNumTroops(cell2->m_objectIndex);
                        if (cell2->m_objectIndex != 59 && cell2->m_objectIndex != 62
                            && cell2->m_objectIndex != 63 && cell2->m_objectIndex != 64
                            && cell2->m_objectIndex != 65 && Random(0, 100) < 20)
                            cell2->m_objectMetadata |= IDX(MAP_MONSTER_GUARD_FLAG);
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RESOURCE:
                    cell2->m_objectMetadata = cell2->m_objectIndex >> 1;
                    switch (cell2->m_objectMetadata) {
                        case 0:
                        case 2:
                            cell2->m_objectMetadata = Random(5, 10);
                            break;
                        case 6:
                            cell2->m_objectMetadata = Random(5, 10);
                            break;
                        default:
                            cell2->m_objectMetadata = Random(3, 6);
                            break;
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHRINE_FIRST_CIRCLE:
                    cell2->m_objectMetadata = Random(0, 64) + 1;
                    while (gsSpellInfo[cell2->m_objectMetadata - 1].level != 1) {
                        cell2->m_objectMetadata = Random(0, 64) + 1;
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHRINE_SECOND_CIRCLE:
                    cell2->m_objectMetadata = Random(0, 64) + 1;
                    while (gsSpellInfo[cell2->m_objectMetadata - 1].level != 2) {
                        cell2->m_objectMetadata = Random(0, 64) + 1;
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHRINE_THIRD_CIRCLE:
                    cell2->m_objectMetadata = Random(0, 64) + 1;
                    while (gsSpellInfo[cell2->m_objectMetadata - 1].level != 3) {
                        cell2->m_objectMetadata = Random(0, 64) + 1;
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_PYRAMID:
                    cell2->m_objectMetadata = Random(0, 64) + 1;
                    while (gsSpellInfo[cell2->m_objectMetadata - 1].level != 5) {
                        cell2->m_objectMetadata = Random(0, 64) + 1;
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREE_HOUSE:
                    cell2->m_objectMetadata = Random(15, 25);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SIRENS:
                    cell2->m_objectMetadata = Random(10, 20);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WATCH_TOWER:
                    cell2->m_objectMetadata = Random(7, 10);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RUINS:
                    cell2->m_objectMetadata = Random(3, 5);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREE_CITY:
                    cell2->m_objectMetadata = Random(20, 40);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HALFLING_HOLE:
                    cell2->m_objectMetadata = Random(20, 40);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TROLL_BRIDGE:
                    cell2->m_objectMetadata = Random(4, 6) | BANK_GUARDIAN_FLAG;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CITY_OF_DEAD:
                    cell2->m_objectMetadata = Random(4, 6) | BANK_GUARDIAN_FLAG;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DRAGON_CITY:
                    cell2->m_objectMetadata = BANK_GUARDIAN_FLAG | 2;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CAVE:
                    cell2->m_objectMetadata = Random(10, 20);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EXCAVATION:
                    cell2->m_objectMetadata = Random(10, 25);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DESERT_TENT:
                    cell2->m_objectMetadata = Random(10, 20);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WAGON_CAMP:
                    if (!HasObjectTilesetIndex(xPos2, yPos19, IDX(TILESET_OBJNMUL2), 129))
                        cell2->m_triggerType &= MAP_TRIGGER_TYPE_MASK;
                    else
                        cell2->m_objectMetadata = Random(30, 50);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTIFACT:
                    randomValue7 = Random(0, 99);
                    value26 = cell2->m_objectIndex >> 1;
                    if (value26 != IDX(ARTIFACT_SPELL_SCROLL)) {
                        if (randomValue7 < 60) {
                            if (randomValue7 % 10 == 1)
                                cell2->m_objectMetadata = 4;
                            else if (randomValue7 % 10 == 2)
                                cell2->m_objectMetadata = 5;
                            else
                                cell2->m_objectMetadata = 1;
                        } else if (randomValue7 < 80) {
                            if (gArtifactLevel[value26] == 8)
                                cell2->m_objectMetadata = 3;
                            else if (gArtifactLevel[value26] == 4)
                                cell2->m_objectMetadata = (Random(0, 5) << 4) | 6;
                            else if (gArtifactLevel[value26] == 2)
                                cell2->m_objectMetadata = (Random(0, 5) << 4) | 7;
                        } else {
                            artifactChoices17[6] = CREATURE_PALADIN;
                            artifactChoices17[7] = CREATURE_CRUSADER;
                            artifactChoices17[8] = CREATURE_CYCLOPS;
                            artifactChoices17[9] = CREATURE_GENIE;
                            artifactChoices17[0] = CREATURE_GREEN_DRAGON;
                            artifactChoices17[1] = CREATURE_RED_DRAGON;
                            artifactChoices17[2] = CREATURE_BLACK_DRAGON;
                            artifactChoices17[3] = CREATURE_BONE_DRAGON;
                            artifactChoices17[4] = CREATURE_GIANT;
                            artifactChoices17[5] = CREATURE_TITAN;
                            cell2->m_objectMetadata = 1;
                            if (gArtifactLevel[value26] == 8)
                                cell2->m_objectMetadata |= IDX(CREATURE_ROGUE);
                            else if (gArtifactLevel[value26] == 4)
                                cell2->m_objectMetadata |= artifactChoices17[Random(0, 3) + 6];
                            else
                                cell2->m_objectMetadata |= artifactChoices17[Random(0, 5)];
                        }
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE:
                    mineId2 = GetTownId(xPos2, yPos19);
                    for (row18 = yPos19 - 2; row18 <= yPos19 + 1; row18++) {
                        for (column1 = xPos2 - 2; column1 <= xPos2 + 2; column1++) {
                            if (m_worldMap.GetCell(column1, row18)->m_objectMetadata != 0)
                                continue;
                            m_worldMap.GetCell(column1, row18)->m_objectMetadata = mineId2;
                        }
                    }
                    townRec4 = GetCastleSlot(this, mineId2);
                    townRec4->m_boatY = -1;
                    townRec4->m_boatX = townRec4->m_boatY;
                    if (yPos19 <= MAP_HEIGHT - 3) {
                        townEntrance = gpAdvManager->GetCell(xPos2 - 1, yPos19 + 2);
                        if (giGroundToTerrain[townEntrance->m_terrainImageIndex] == 0) {
                            townRec4->m_boatX = static_cast<i8>(xPos2 - 1);
                            townRec4->m_boatY = static_cast<i8>(yPos19 + 2);
                        } else {
                            townEntrance = gpAdvManager->GetCell(xPos2 + 1, yPos19 + 2);
                            if (giGroundToTerrain[townEntrance->m_terrainImageIndex] == 0) {
                                townRec4->m_boatX = static_cast<i8>(xPos2 + 1);
                                townRec4->m_boatY = static_cast<i8>(yPos19 + 2);
                            }
                        }
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_LIGHTHOUSE: {
                    m_worldMap.GetCell(xPos2, yPos19)->m_objectMetadata = GetMineId(xPos2, yPos19);
                    break;
                }
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ABANDONED_MINE:
                    mineId2 = GetMineId(xPos2, yPos19);
                    m_mines[mineId2].guardianType = 59;
                    m_mines[mineId2].guardianCount = static_cast<u8>(Random(30, 60));
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ALCHEMIST_LAB:
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE:
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SAWMILL:
                    mineId2 = GetMineId(xPos2, yPos19);
                    for (row18 = yPos19 - 1; row18 <= yPos19; row18++) {
                        for (column1 = xPos2 - 2; column1 <= xPos2 + 1; column1++) {
                            if (column1 == xPos2 - 2
                                && cell2->m_triggerType
                                       != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ALCHEMIST_LAB))
                                continue;
                            if (m_worldMap.GetCell(column1, row18)->m_objectMetadata == 0
                                || ((m_worldMap.GetCell(column1, row18)->m_triggerType
                                     & MAP_TRIGGER_TYPE_MASK)
                                    == (cell2->m_triggerType & MAP_TRIGGER_TYPE_MASK)))
                                m_worldMap.GetCell(column1, row18)->m_objectMetadata = mineId2;
                        }
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WINDMILL:
                    cell2->m_objectMetadata = Random(1, 5);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BARRIER:
                    RandomizeBarrier(cell2);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TRAVELER_TENT:
                    RandomizePassword(cell2);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EXPANSION_OBJECT:
                    WeeklyGenericSite(cell2);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EXPANSION_DWELLING:
                    WeeklyRecruitSite(cell2);
                    break;
            }
        }
    }

    for (yPos19 = 0; yPos19 < MAP_HEIGHT; yPos19++) {
        for (xPos2 = 0; (xPos2 | 0) < MAP_WIDTH; xPos2++) {
            cell2 = m_worldMap.Row(yPos19) + xPos2;
            if (cell2->m_objectIndex != IDX(MAPCELL_SPRITE_NONE) && cell2->m_objectLayerBit1) {
                valid27 = 1;
                extraIndex3 = cell2->m_extraIndex;
                while (extraIndex3 != 0) {
                    extra15 = m_worldMap.Extra(extraIndex3);
                    if (extra15->objectIndex != IDX(MAPCELL_SPRITE_NONE) && !extra15->objectLayerBit1)
                        valid27 = 0;
                    extraIndex3 = extra15->nextIndex;
                }
                if (valid27)
                    cell2->m_flags |= IDX(MAP_CELL_OBJECT_SHADOW_ONLY);
            }
        }
    }

    for (yPos19 = 0; yPos19 < MAP_HEIGHT; yPos19++) {
        for (xPos2 = 0; (xPos2 | 0) < MAP_WIDTH; xPos2++) {
            cell2 = m_worldMap.Row(yPos19) + xPos2;
            if ((cell2->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_ROCK
                && cell2->m_objectTileset == IDX(TILESET_X_LOC2))
                cell2->m_flags |= 8;
            if (cell2->m_objectIndex != IDX(MAPCELL_SPRITE_NONE)
                && !(cell2->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                && !(cell2->m_flags & IDX(MAP_CELL_OBJECT_SHADOW_ONLY))
                && cell2->m_overlayIndex != IDX(MAPCELL_SPRITE_NONE))
                cell2->m_flags |= 8;
            upperCount = 0;
            lowerCount16 = 0;
            if (!(cell2->m_flags & 8) && yPos19 < MAP_HEIGHT - 1
                && cell2->m_objectIndex != IDX(MAPCELL_SPRITE_NONE)
                && !(cell2->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                && !(cell2->m_flags & IDX(MAP_CELL_OBJECT_SHADOW_ONLY))) {
                mapCell* below0;
                if (m_worldMap.GetCell(xPos2, yPos19 + 1)->m_objectIndex != IDX(MAPCELL_SPRITE_NONE)
                    && !(
                        m_worldMap.GetCell(xPos2, yPos19 + 1)->m_triggerType
                        & MAP_TRIGGER_ACTION_FLAG
                    )
                    && !(
                        m_worldMap.GetCell(xPos2, yPos19 + 1)->m_flags & IDX(MAP_CELL_OBJECT_SHADOW_ONLY)
                    )) {
                    if (!cell2->m_objectLayerBit1) {
                        upperTilesets29[upperCount] = cell2->m_objectTileset;
                        upperIndexes1[upperCount] = cell2->m_objectIndex;
                        upperCount++;
                    }
                    if (cell2->m_extraIndex != 0)
                        extra15 = m_worldMap.Extra(cell2->m_extraIndex);
                    else
                        extra15 = 0;
                    while (upperCount < 5 && extra15 != 0) {
                        if (extra15->objectIndex != IDX(MAPCELL_SPRITE_NONE)
                            && !extra15->objectLayerBit1) {
                            upperTilesets29[upperCount] = extra15->objectTileset;
                            upperIndexes1[upperCount] = extra15->objectIndex;
                            upperCount++;
                        }
                        if (extra15->nextIndex != 0)
                            extra15 = m_worldMap.Extra(extra15->nextIndex);
                        else
                            extra15 = 0;
                    }
                    below0 = m_worldMap.GetCell(xPos2, yPos19 + 1);
                    if (!below0->m_objectLayerBit1) {
                        lowerTilesets4[lowerCount16] = below0->m_objectTileset;
                        lowerIndexes7[lowerCount16] = below0->m_objectIndex;
                        lowerCount16++;
                    }
                    if (below0->m_extraIndex != 0)
                        extra15 = m_worldMap.Extra(below0->m_extraIndex);
                    else
                        extra15 = 0;
                    while (lowerCount16 < 5 && extra15 != 0) {
                        if (extra15->objectIndex != IDX(MAPCELL_SPRITE_NONE)
                            && !extra15->objectLayerBit1) {
                            lowerTilesets4[lowerCount16] = extra15->objectTileset;
                            lowerIndexes7[lowerCount16] = extra15->objectIndex;
                            lowerCount16++;
                        }
                        if (extra15->nextIndex != 0)
                            extra15 = m_worldMap.Extra(extra15->nextIndex);
                        else
                            extra15 = 0;
                    }
                    for (randomValue7 = 0; randomValue7 < upperCount; randomValue7++) {
                        for (j9 = 0; lowerCount16 > j9; j9++) {
                            if (lowerTilesets4[j9] == upperTilesets29[randomValue7]
                                || (upperTilesets29[randomValue7] >= 35
                                    && upperTilesets29[randomValue7] <= 38
                                    && lowerTilesets4[j9] >= 35 && lowerTilesets4[j9] <= 38))
                                cell2->m_flags |= 8;
                        }
                    }
                }
            }
            if (yPos19 < MAP_HEIGHT - 1) {
                if (m_worldMap.GetCell(xPos2, yPos19 + 1)->m_triggerType
                        == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)
                    || m_worldMap.GetCell(xPos2, yPos19 + 1)->m_triggerType
                           == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_TOWN)
                    || m_worldMap.GetCell(xPos2, yPos19 + 1)->m_triggerType
                           == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_CASTLE))
                    cell2->m_flags |= 8;
            }
            if (cell2->m_objectIndex != IDX(MAPCELL_SPRITE_NONE)
                && !(cell2->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                && !(cell2->m_flags & IDX(MAP_CELL_OBJECT_SHADOW_ONLY))
                && (yPos19 == MAP_HEIGHT - 1 || (m_worldMap.Row(yPos19 + 1)[xPos2].m_flags & 4)))
                cell2->m_flags |= 8;
        }
    }
}

VA(0x00478a49, 0xa1)
void game::InitializePasswords(void) {
    char flag;
    i32 i;
    i32 j;
    for (i = 0; i < 8; i++) {
        flag = 0;
        while (flag == 0) {
            xPasswordStringsIndex[i] = Random(0, 210);
            flag = 1;
            for (j = 0; j < i; j++) {
                if (xPasswordStringsIndex[j] == xPasswordStringsIndex[i])
                    flag = 0;
            }
        }
    }
}

VA(0x00478aea, 0x64)
void game::RandomizeBarrier(mapCell* cell) {
    i32 idx = cell->m_objectMetadata;
    idx &= 7;
    i32 pass = xPasswordStringsIndex[idx];
    i32 color = (pass << 3) | idx;
    cell->m_objectMetadata = color | 0;
}

VA(0x00478b4e, 0x24)
void game::RandomizePassword(mapCell* cell) {
    RandomizeBarrier(cell);
}

VA(0x00478b72, 0x478)
i32 game::LoadMap(char* filename) {
    char column5[4];
    i32 i37;
    i32 file2;
    char row9[4];
    char type5[8];
    char trailer15[8];

    sprintf(gText, "%s%s", gcMapPath, filename);
    file2 = open(gText, _O_BINARY);
    if (file2 == -1)
        FileError(gText);
    read(file2, &m_mapHeader, sizeof(m_mapHeader));
    m_worldMap.Read(file2, 1);
    SetMapSize(m_worldMap.width, m_worldMap.height);

    for (i37 = 0; i37 < GAME_TOWN_COUNT; i37++) {
        read(file2, column5, 1);
        read(file2, row9, 1);
        read(file2, type5, 1);
        if (static_cast<u8>(column5[0]) != IDX(GAME_SAVED_TOWN_OFF_MAP)) {
            m_castleRecs[i37].m_onMap = 1;
            m_castleRecs[i37].m_x = static_cast<u8>(column5[0]);
            m_castleRecs[i37].m_y = static_cast<u8>(row9[0]);
            m_castleRecs[i37].m_type = static_cast<i8>(type5[0] & TOWN_RECORD_TYPE_MASK);
            if (type5[0] < 0)
                m_castleRecs[i37].m_buildings |= IDX(TOWN_BUILDING_CASTLE);
            else
                m_castleRecs[i37].m_buildings |= IDX(TOWN_BUILDING_TENT);
        }
    }

    for (i37 = 0; i37 < GAME_MINE_COUNT; i37++) {
        if (m_mapHeader.magic == MAP_HEADER_MAGIC_BASE_GAME && i37 >= GAME_TOWN_COUNT) {
            column5[0] = -1;
            row9[0] = -1;
            type5[0] = -1;
        } else {
            read(file2, column5, 1);
            read(file2, row9, 1);
            read(file2, type5, 1);
        }
        if (static_cast<u8>(column5[0]) != IDX(GAME_SAVED_TOWN_OFF_MAP)) {
            m_mines[i37].guardianType = -1;
            m_mines[i37].x = static_cast<u8>(column5[0]);
            m_mines[i37].y = static_cast<u8>(row9[0]);
            m_mines[i37].resourceType = type5[0];
        }
    }

    m_mapHeader.magic = MAP_HEADER_MAGIC_EXPANSION_GAME;
    read(file2, &m_obeliskCount, 1);
    read(file2, m_rumourEventIndices, m_mapHeader.rumourCount * 2);
    m_rumourEventCount = m_mapHeader.rumourCount;
    read(file2, m_timeEventIndices, m_mapHeader.timeEventCount * 2);
    m_timeEventCount = m_mapHeader.timeEventCount;
    read(file2, &iMaxMapExtra, 4);
    ppMapExtra = reinterpret_cast<void**>(H2_ALLOC(iMaxMapExtra * 4, 2893));
    pwSizeOfMapExtra = reinterpret_cast<i16*>(H2_ALLOC(iMaxMapExtra * 2, 2894));
    memset(ppMapExtra, 0, iMaxMapExtra * 4);
    memset(pwSizeOfMapExtra, 0, iMaxMapExtra * 2);
    for (i37 = 1; (&i37)[0] < iMaxMapExtra; i37++) {
        read(file2, pwSizeOfMapExtra + i37, 2);
        ppMapExtra[i37] = H2_ALLOC(pwSizeOfMapExtra[i37], 2902);
        read(file2, ppMapExtra[i37], pwSizeOfMapExtra[i37]);
    }
    read(file2, trailer15, 2);
    close(file2);
    return 0;
}

// @early-stop
// reloc-masked: only two retail /Ob1 GetCell continuation jmps differ (5 bytes each)
VA(0x00478fea, 0x3aa)
void game::ClaimTown(i32 townId, i32 player, i32 suppressVisibility) {
    i32 i;
    town* townRec;
    mapCell* cell;

    if (!gbInNewGameSetup)
        SendMapChange(7, static_cast<i8>(townId), 0, 0, player, 0, 0);
    townRec = &m_castleRecs[townId];
    if (townRec->m_owner == player)
        return;
    townRec->m_formation = 0;
    if (m_castleOwners[townId] != -1)
        GetCastle(townId)->Deallocate();
    for (i = 0; i < 5; i++) {
        townRec->m_army.m_creatureTypes[i] = -1;
        townRec->m_army.m_creatureCounts[i] = 0;
    }
    m_castleRecs[townId].m_turnsOwned = m_castleRecs[townId].m_owner == -1 ? 2 : 0;
    m_castleRecs[townId].m_owner = static_cast<i8>(player);
    m_castleOwners[townId] = static_cast<i8>(player);
    m_players[player].m_townIds[m_players[player].m_townCount] = static_cast<i8>(townId);
    m_players[player].m_townCount++;

    cell = m_worldMap.GetCell(m_castleRecs[townId].m_x - 1, m_castleRecs[townId].m_y);
    m_worldMap.ChangeTilesetIndex(
        cell,
        m_castleRecs[townId].m_x - 1,
        m_castleRecs[townId].m_y,
        14,
        m_players[static_cast<i8>(player)].m_color * 2,
        1,
        -1
    );
    cell = m_worldMap.GetCell(m_castleRecs[townId].m_x + 1, m_castleRecs[townId].m_y);
    m_worldMap.ChangeTilesetIndex(
        cell,
        m_castleRecs[townId].m_x + 1,
        m_castleRecs[townId].m_y,
        14,
        m_players[static_cast<i8>(player)].m_color * 2 + 1,
        1,
        -1
    );
    if (suppressVisibility != 0)
        return;
    SetVisibility(m_castleRecs[townId].m_x, m_castleRecs[townId].m_y, player, giVisRangeTown);
    CheckEndGame(0, 0);
}

// @early-stop
// reloc-masked: residual is the /Ob1 Row continuation plus delinked switch local-label identity
VA(0x00479394, 0x4c2)
void game::ClaimMine(i32 mineId, i32 player) {
    mapCell* acc;
    i32 flag;
    u32 x;
    u32 y;

    SendMapChange(8, static_cast<i8>(mineId), 0, 0, player, 0, 0);
    m_mines[mineId].owner = static_cast<i8>(player);
    m_mineOwners[mineId] = static_cast<i8>(player);
    switch (m_mines[mineId].resourceType) {
        case 101:
            flag = 35;
            break;
        case 100:
            flag = 42;
            break;
        case 0:
            flag = 28;
            break;
        case 1:
            flag = 21;
            break;
        default:
            flag = 14;
            break;
    }
    switch (m_mines[mineId].resourceType) {
        case 1:
            x = m_mines[mineId].x;
            y = m_mines[mineId].y - 1;
            break;
        case 0:
            x = m_mines[mineId].x + 1;
            y = m_mines[mineId].y - 1;
            break;
        case 101:
            x = m_mines[mineId].x - 1;
            y = m_mines[mineId].y - 3;
            break;
        case 100:
            x = m_mines[mineId].x;
            y = m_mines[mineId].y;
            break;
        default:
            x = m_mines[mineId].x;
            y = m_mines[mineId].y;
            break;
    }
    acc = m_worldMap.Row(y) + x;
    if (player == -1) {
        m_worldMap.ChangeTilesetIndex(acc, x, y, 14, 255, 1, -1);
    } else {
        m_worldMap.ChangeTilesetIndex(
            acc,
            x,
            y,
            14,
            m_players[static_cast<i8>(player)].m_color + flag,
            1,
            -1
        );
        if (m_mines[mineId].resourceType == 1) {
            ConvertFlagToLateOverlay(x, y);
        } else if (y > 0
                   && (m_mines[mineId].resourceType == 2 || m_mines[mineId].resourceType == 6
                       || m_mines[mineId].resourceType == 5 || m_mines[mineId].resourceType == 3
                       || m_mines[mineId].resourceType == 4)
                   && HasLateOverlay(x, y - 1)) {
            ConvertFlagToLateOverlay(x, y);
        }
    }
}

// @early-stop
// Relocation-masked comparison is identical for the full 0x1e2-byte span;
// both objects contain the same 16 relocation sites and objdiff reports 100%.
VA(0x00479856, 0x1e2)
SpellType
game::ViewSpells(hero* spellHero, i32 spellType, i32 (*callback)(tag_message&), i32 readOnly) {
    tag_message message;

    viewSpellsHero = spellHero;
    m_viewSpell = -1;
    if (spellHero->GetNumSpells(spellType) == 0) {
        NormalDialog(const_cast<char*>("No spells to cast."), 1, -1, -1, -1, 0, -1, 0, -1, 0);
    } else {
        m_viewSpellsCallback = callback;
        m_viewSpellsReadOnly = static_cast<i8>(readOnly);
        m_viewSpellsHero = spellHero;
        if (spellType == 2)
            m_viewSpellsType = 1;
        else
            m_viewSpellsType = spellType;
        m_viewSpellsTop[0] = 0;
        m_viewSpellsCount[0] = spellHero->GetNumSpells(0);
        m_viewSpellsTop[1] = 0;
        m_viewSpellsCount[1] = spellHero->GetNumSpells(1);
        m_viewSpellsWindow = new heroWindow(86, 87, const_cast<char*>("spellwin.bin"));
        if (m_viewSpellsWindow == 0)
            MemError();
        if (spellType != 2) {
            message.type = MESSAGE_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            if (spellType == 0)
                message.payload.widget.id = 4;
            else
                message.payload.widget.id = 5;
            message.payload.widget.data.value = 6;
            m_viewSpellsWindow->BroadcastMessage(message);
        }
        UpdateSpellWidgets();
        gpWindowManager->DoDialog(m_viewSpellsWindow, ViewSpellsHandler, 0);
        delete m_viewSpellsWindow;
    }
    return m_viewSpell;
}

// @early-stop
// Relocation-masked comparison is identical for the full 0x403-byte span;
// both objects contain the same 42 relocation sites and objdiff reports 100%.
VA(0x00479a38, 0x403)
void game::UpdateSpellWidgets(void) {
    tag_message message9;
    i32 spellSlot6;
    i32 spellPoints0;
    i32 spell2;
    i32 lineLength0;

    message9.type = MESSAGE_WIDGET;
    spellPoints0 = m_viewSpellsHero->m_spellPoints;
    if (spellPoints0 > 999)
        spellPoints0 = 999;
    message9.payload.widget.data.value = 6;
    if (spellPoints0 > 99)
        message9.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
    else
        message9.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    message9.payload.widget.id = 7;
    m_viewSpellsWindow->BroadcastMessage(message9);
    if (spellPoints0 > 9)
        message9.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
    else
        message9.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    message9.payload.widget.id = 8;
    m_viewSpellsWindow->BroadcastMessage(message9);

    sprintf(gText, "%d", (spellPoints0 / 100) % 10);
    message9.payload.widget.data.text = gText;
    message9.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message9.payload.widget.id = 7;
    m_viewSpellsWindow->BroadcastMessage(message9);
    sprintf(gText, "%d", (spellPoints0 / 10) % 10);
    message9.payload.widget.data.text = gText;
    message9.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message9.payload.widget.id = 8;
    m_viewSpellsWindow->BroadcastMessage(message9);
    sprintf(gText, "%d", spellPoints0 % 10);
    message9.payload.widget.data.text = gText;
    message9.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message9.payload.widget.id = 9;
    m_viewSpellsWindow->BroadcastMessage(message9);

    for (spellSlot6 = 0; spellSlot6 < 12; spellSlot6++) {
        if (m_viewSpellsTop[m_viewSpellsType] + spellSlot6 >= m_viewSpellsCount[m_viewSpellsType]) {
            message9.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message9.payload.widget.id = spellSlot6 + 100;
            message9.payload.widget.data.value = 6;
            m_viewSpellsWindow->BroadcastMessage(message9);
            message9.payload.widget.id = spellSlot6 + 30;
            m_viewSpellsWindow->BroadcastMessage(message9);
        } else {
            spell2 = m_viewSpellsHero->GetNthSpell(
                m_viewSpellsType,
                m_viewSpellsTop[m_viewSpellsType] + spellSlot6 + 1
            );
            message9.payload.widget.command = WIDGET_COMMAND_SET_FILL_COLOR;
            message9.payload.widget.id = spellSlot6 + 30;
            if (GetManaCost(spell2, m_viewSpellsHero) > m_viewSpellsHero->m_spellPoints)
                message9.payload.widget.data.value = 3;
            else
                message9.payload.widget.command = 1;
            m_viewSpellsWindow->BroadcastMessage(message9);
            message9.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message9.payload.widget.data.value = 6;
            m_viewSpellsWindow->BroadcastMessage(message9);
            message9.payload.widget.id = spellSlot6 + 100;
            m_viewSpellsWindow->BroadcastMessage(message9);
            if (m_viewSpellsReadOnly) {
                message9.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
                message9.payload.widget.data.value = 2;
                m_viewSpellsWindow->BroadcastMessage(message9);
            }
            message9.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message9.payload.widget.data.value = gsSpellInfo[spell2].iconIndex;
            m_viewSpellsWindow->BroadcastMessage(message9);
            lineLength0 = smallFont->LineLength(gSpellNames[spell2], 78);
            if (lineLength0 == 1) {
                sprintf(
                    gText,
                    "%s\n[%d]",
                    gSpellNames[spell2],
                    GetManaCost(spell2, m_viewSpellsHero)
                );
            } else {
                sprintf(
                    gText,
                    "%s [%d]",
                    gSpellNames[spell2],
                    GetManaCost(spell2, m_viewSpellsHero)
                );
            }
            message9.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            message9.payload.widget.id = spellSlot6 + 30;
            message9.payload.widget.data.text = gText;
            m_viewSpellsWindow->BroadcastMessage(message9);
        }
    }
}

// @early-stop
// Complete semantics, 0x18 frame/slots, CFG, and all 101 relocation
// identities/counts/effective targets agree. At +0x27 retail uses the five-byte
// absolute-EAX load for gpWindowManager before loading the hover id; base loads
// the hover id first and needs a six-byte absolute-ECX load, making its body one
// byte longer and shifting later relocation sites/local switch-table addends.
// Reversing the source equality is byte-neutral. Revisit after a relevant GAME
// predecessor/header change alters this TU-cumulative commutative load order.
VA(0x00479e3b, 0x692)
i32 ViewSpellsHandler(tag_message& msg) {
    SpellType spell;

    if (msg.type == MESSAGE_MOUSE_MOVE) {
        gpWindowManager->ConvertToHover(msg);
        if (msg.payload.hover.id == gpWindowManager->m_lastHoverId) {
            return 1;
        } else {
            return gpGame->m_viewSpellsCallback(msg);
        }
    }
    if (msg.type == MESSAGE_WIDGET) {
        switch (msg.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                if (msg.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT
                    || (msg.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0)
                    break;
                {
                    switch (msg.payload.widget.id) {
                        case 6:
                        case 7:
                        case 8:
                        case 9:
                            sprintf(gText, cSpellHelp[8], viewSpellsHero->m_spellPoints);
                            NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                            break;
                        case 2:
                            if (gpGame->m_viewSpellsTop[gpGame->m_viewSpellsType] == 0)
                                break;
                            gpGame->m_viewSpellsTop[gpGame->m_viewSpellsType] -= 12;
                            if (gpGame->m_viewSpellsTop[gpGame->m_viewSpellsType] < 0)
                                gpGame->m_viewSpellsTop[gpGame->m_viewSpellsType] = 0;
                            gpGame->UpdateSpellWidgets();
                            gpGame->m_viewSpellsWindow->MoveWindow(0, 0);
                            break;
                        case 3:
                            if (gpGame->m_viewSpellsTop[gpGame->m_viewSpellsType] + 12
                                < gpGame->m_viewSpellsCount[gpGame->m_viewSpellsType])
                                gpGame->m_viewSpellsTop[gpGame->m_viewSpellsType] += 12;
                            gpGame->UpdateSpellWidgets();
                            gpGame->m_viewSpellsWindow->MoveWindow(0, 0);
                            break;
                        case 4:
                            gpGame->m_viewSpellsType = 1;
                            gpGame->UpdateSpellWidgets();
                            gpGame->m_viewSpellsWindow->MoveWindow(0, 0);
                            break;
                        case 5:
                            gpGame->m_viewSpellsType = 0;
                            gpGame->UpdateSpellWidgets();
                            gpGame->m_viewSpellsWindow->MoveWindow(0, 0);
                            break;
                        case EVENT_WINDOW_FIRST_BUTTON:
                            msg.payload.widget.id = 10;
                            break;
                    }
                }
                break;
            case WIDGET_COMMAND_SELECT:
            case WIDGET_COMMAND_ALTERNATE_SELECT:
                if (msg.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT
                    || (msg.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0) {
                    switch (msg.payload.widget.id) {
                        case 100:
                        case 101:
                        case 102:
                        case 103:
                        case 104:
                        case 105:
                        case 106:
                        case 107:
                        case 108:
                        case 109:
                        case 110:
                        case 111:
                            spell = gpGame->m_viewSpellsHero->GetNthSpell(
                                gpGame->m_viewSpellsType,
                                gpGame->m_viewSpellsTop[gpGame->m_viewSpellsType]
                                    + (msg.payload.widget.id - 100) + 1
                            );
                            NormalDialog(gSpellDesc[IDX(spell)], 4, -1, -1, 8, spell, -1, 0, -1, 0);
                            break;
                        case 2:
                            NormalDialog(cSpellHelp[0], 4, -1, -1, -1, 0, -1, 0, -1, 0);
                            break;
                        case 3:
                            NormalDialog(cSpellHelp[1], 4, -1, -1, -1, 0, -1, 0, -1, 0);
                            break;
                        case 4:
                            NormalDialog(cSpellHelp[2], 4, -1, -1, -1, 0, -1, 0, -1, 0);
                            break;
                        case 5:
                            NormalDialog(cSpellHelp[3], 4, -1, -1, -1, 0, -1, 0, -1, 0);
                            break;
                        case 6:
                        case 7:
                        case 8:
                        case 9:
                            sprintf(gText, cSpellHelp[8], viewSpellsHero->m_spellPoints);
                            NormalDialog(gText, 4, -1, -1, -1, 0, -1, 0, -1, 0);
                            break;
                    }
                } else {
                    switch (msg.payload.widget.id) {
                        case 100:
                        case 101:
                        case 102:
                        case 103:
                        case 104:
                        case 105:
                        case 106:
                        case 107:
                        case 108:
                        case 109:
                        case 110:
                        case 111:
                            spell = gpGame->m_viewSpellsHero->GetNthSpell(
                                gpGame->m_viewSpellsType,
                                gpGame->m_viewSpellsTop[gpGame->m_viewSpellsType]
                                    + (msg.payload.widget.id - 100) + 1
                            );
                            if (gpGame->m_viewSpellsReadOnly) {
                                NormalDialog(gSpellDesc[IDX(spell)], 1, -1, -1, 8, spell, -1, 0, -1, 0);
                                return 1;
                            }
                            if (GetManaCost(spell, viewSpellsHero)
                                > viewSpellsHero->m_spellPoints) {
                                sprintf(
                                    gText,
                                    "That spell costs %d mana.  You only have %d mana, so you "
                                    "can't cast the spell.",
                                    GetManaCost(spell, viewSpellsHero),
                                    viewSpellsHero->m_spellPoints
                                );
                                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                                return 0;
                            }
                            gpGame->m_viewSpell = spell;
                            msg.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
                            return 2;
                    }
                }
                break;
            default:
                break;
        }

        if (msg.payload.widget.id == 10) {
            msg.payload.widget.command = msg.payload.widget.id;
            return 2;
        }
    }
    return 1;
}

// @semantic
// Manual relocation audit finds 35 sites in both objects. This body is 0x17d versus
// retail's 0x17c solely because the commutative hover equality loads msg first into eax
// (15 bytes) instead of the window manager first into eax (14 bytes); all later blocks
// and the jump table realign. Both operand orders and exhaustive AST variants emit alike.
VA(0x0047a4cd, 0x17c)
i32 ViewSpecialHandler(tag_message& msg) {
    if (msg.type == MESSAGE_MOUSE_MOVE) {
        if (gpWindowManager->m_lastHoverId == msg.payload.hover.id)
            return 1;
        gpWindowManager->m_lastHoverId = msg.payload.hover.id;
        switch (msg.payload.hover.id) {
            case 2:
                strcpy(gText, cSpellHelp[0]);
                break;
            case 3:
                strcpy(gText, cSpellHelp[1]);
                break;
            case 4:
                strcpy(gText, cSpellHelp[2]);
                break;
            case 5:
                strcpy(gText, cSpellHelp[3]);
                break;
            case EVENT_WINDOW_FIRST_BUTTON:
                strcpy(gText, cSpellHelp[4]);
                break;
            case 6:
            case 7:
            case 8:
            case 9:
                sprintf(gText, cSpellHelp[8], viewSpellsHero->m_spellPoints);
                break;
            default:
                strcpy(gText, cSpellHelp[5]);
                break;
        }
        HeroMessageUpdate(gText);
        return 1;
    }
    return 1;
}

VA(0x0047a649, 0xc86)
void game::ViewArmy(
    i32 x,
    i32 y,
    i32 monsterType,
    i32 numTroops,
    town* castle,
    i32 disableUpgrade,
    i32 facing,
    i32 quickView,
    hero* theHero,
    class army* theArmy,
    armyGroup* theGroup,
    i32 groupIndex
) {
    DATA(0x004f7388) static i16 viewArmySourceLineBase = 0x0dd1;
    i16 baseX7 = 86;
    i16 quickBaseY3 = 164;
    i16 blankWidget3 = 1;
    i16 numWidget15 = 2;
    i16 titleMessage15 = 3;
    i16 detailMessage2 = 4;
    i16 frame18 = 5;
    i32 loopIndex0;
    tag_message message6;
    message6.type = MESSAGE_WIDGET;

    iViewArmyFrame = 0;
    iViewArmyType = monsterType;
    iViewArmyNumTroops = numTroops;
    gbAllowUpgrade = false;

    if (castle && (gpAdvManager->m_active || gpTownManager->m_active)) {
        for (loopIndex0 = 20; loopIndex0 <= 24; loopIndex0++) {
            if (gDwellingType[static_cast<i8>(castle->m_type)][loopIndex0 - 19] == monsterType
                && (castle->m_buildings & (1 << (loopIndex0 + 5)))) {
                gbAllowUpgrade = true;
                iViewArmyUpgradeToType = static_cast<CreatureType>(IDX(monsterType) + 1);
            }
        }
        if ((monsterType == 35 || monsterType == 36)
            && (castle->m_buildings & IDX(KB_DWELLING_UPGRADE_SIXTH_FLAG))) {
            gbAllowUpgrade = true;
            iViewArmyUpgradeToType = 37;
        }
    }

    tag_monsterInfo* monster8 = &gMonsterDatabase[monsterType];
    tag_monsterInfo* armyMonster11;
    if (theArmy)
        armyMonster11 = &theArmy->m_monster;
    else
        armyMonster11 = &gMonsterDatabase[monsterType];

    x = 19;
    y = 75;
    m_viewArmyWindow = new heroWindow(x, y, const_cast<char*>("armywin.bin"));
    if (!m_viewArmyWindow)
        MemError();

    viewArmyFacingWIPXMod = facing == 1 ? -1 : 1;
    gpResourceManager->PointToFile(gpResourceManager->MakeId(cArmyFrameFileNames[monsterType], 1));
    gpResourceManager->ReadBlock(
        reinterpret_cast<i8*>(&sViewArmyMonFrameInfo),
        sizeof(sViewArmyMonFrameInfo)
    );
    ModifyFrameInfo(&sViewArmyMonFrameInfo, monsterType);
    BuildTempWalkSeq(&sViewArmyMonFrameInfo, 0, 1);

    viewArmyBaseX = 167;
    char filename4[16];
    if (gbLowMemory)
        sprintf(filename4, "monh%04d.icn", monsterType);
    else
        strcpy(filename4, cMonFilename[monsterType]);

    icon* monsterIcon5 = gpResourceManager->GetIcon(filename4);
    i32 iconFrame15 = sViewArmyMonFrameInfo.animationFrames[IDX(ARMY_ANIMATION_WALK)][0];
    viewArmyBaseX += (GetIconEntry(monsterIcon5, iconFrame15)->w / 2) * viewArmyFacingWIPXMod;
    viewArmyBaseX += GetIconEntry(monsterIcon5, iconFrame15)->x * viewArmyFacingWIPXMod
                     + sViewArmyMonFrameInfo.walkXOffsets[0] * viewArmyFacingWIPXMod;
    viewArmyBaseY = 138;
    viewArmyBaseY += GetIconEntry(monsterIcon5, iconFrame15)->h / 2;
    if (gbLowMemory) {
        viewArmyBaseX = 126;
        viewArmyBaseY = 93;
    }

    iconWidget* monsterWidget7 = new iconWidget(
        static_cast<i16>(viewArmyBaseX),
        static_cast<i16>(viewArmyBaseY),
        86,
        149,
        filename4,
        gbLowMemory ? 0 : sViewArmyMonFrameInfo.animationFrames[IDX(ARMY_ANIMATION_WALK)][0],
        facing == 0,
        5,
        16,
        1
    );
    if (!monsterWidget7)
        MemError();
    m_viewArmyWindow->AddWidget(monsterWidget7, -1);
    gpResourceManager->Dispose(monsterIcon5);

    char armyName8[32];
    strcpy(armyName8, gArmyNames[monsterType]);
    armyName8[0] -= 32;
    message6.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message6.payload.widget.id = 3;
    message6.payload.widget.data.text = armyName8;
    m_viewArmyWindow->BroadcastMessage(message6);

    char* details9 = static_cast<char*>(H2_ALLOC(550, 3684));
    i32 morale2 = theGroup ? theGroup->GetMorale(theHero, castle, 0) : 0;
    if (HAS(monster8->flags.all, MONSTER_FLAGS_NO_MORALE))
        morale2 = 0;

    sprintf(details9, "");
    i32 modifier15 = 0;
    sprintf(gText, "%s%d", cArmyDetail[0], static_cast<i32>(monster8->attack));
    strcat(details9, gText);
    if (theHero)
        modifier15 += theHero->Stats(HeroPrimaryStat(0));
    if (theArmy)
        modifier15 = theArmy->m_monster.attack - monster8->attack;
    if (modifier15) {
        sprintf(gText, " (%d)", monster8->attack + modifier15);
        strcat(details9, gText);
    }

    modifier15 = 0;
    sprintf(gText, "\n%s%d", cArmyDetail[1], static_cast<i32>(monster8->defense));
    strcat(details9, gText);
    if (theHero)
        modifier15 += theHero->Stats(HeroPrimaryStat(1));
    if (theArmy)
        modifier15 = theArmy->m_monster.defense - monster8->defense;
    if (modifier15) {
        sprintf(gText, " (%d)", monster8->defense + modifier15);
        strcat(details9, gText);
    }

    if (HAS(monster8->flags.all, MONSTER_FLAGS_SHOOTER)) {
        i32 shots8 = armyMonster11->shots;
        if (shots8 > 0) {
            if (gpCombatManager->m_active)
                sprintf(gText, "\n%s%d", cArmyDetail[2], shots8);
            else
                sprintf(gText, "\n%s%d", cArmyDetail[8], shots8);
            strcat(details9, gText);
        }
    }

    sprintf(gText, "\n%s%d", cArmyDetail[3], static_cast<i32>(monster8->damageMin));
    strcat(details9, gText);
    if (monster8->damageMin != monster8->damageMax) {
        sprintf(gText, "-%d", static_cast<i32>(monster8->damageMax));
        strcat(details9, gText);
    }
    sprintf(gText, "\n%s%d", cArmyDetail[4], static_cast<u32>(monster8->hitPoints));
    strcat(details9, gText);
    if (gpCombatManager->m_active) {
        sprintf(
            gText,
            "\n%s%d",
            "Hit Points Left: ",
            static_cast<u32>(monster8->hitPoints) - theArmy->m_hitPointsLost
        );
        strcat(details9, gText);
    }
    sprintf(gText, "\n%s%s", cArmyDetail[5], speedText[armyMonster11->speed]);
    strcat(details9, gText);
    sprintf(gText, "\n%s%s", cArmyDetail[6], gMoraleText[morale2 + 3]);
    strcat(details9, gText);
    i32 luck1 = GetLuck(theHero, theArmy, castle);
    sprintf(gText, "\n%s%s", cArmyDetail[7], gLuckText[luck1 + 3]);
    strcat(details9, gText);

    message6.payload.widget.id = 4;
    message6.payload.widget.data.text = details9;
    m_viewArmyWindow->BroadcastMessage(message6);
    if (!gbAllowUpgrade) {
        message6.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message6.payload.widget.data.text = reinterpret_cast<char*>(6);
        message6.payload.widget.id = 500;
        m_viewArmyWindow->BroadcastMessage(message6);
    }
    if (disableUpgrade) {
        message6.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message6.payload.widget.data.text = reinterpret_cast<char*>(6);
        message6.payload.widget.id = VIEW_ARMY_UPGRADE_ID;
        m_viewArmyWindow->BroadcastMessage(message6);
    }
    if (quickView) {
        message6.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message6.payload.widget.data.text = reinterpret_cast<char*>(6);
        message6.payload.widget.id = VIEW_ARMY_QUICK_VIEW_ID;
        m_viewArmyWindow->BroadcastMessage(message6);
    }
    if (numTroops < 1) {
        message6.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message6.payload.widget.data.text = reinterpret_cast<char*>(6);
        message6.payload.widget.id = 1;
        m_viewArmyWindow->BroadcastMessage(message6);
        message6.payload.widget.id = 2;
        m_viewArmyWindow->BroadcastMessage(message6);
    } else {
        char countText[12];
        sprintf(countText, "%d", numTroops);
        message6.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        message6.payload.widget.id = 2;
        message6.payload.widget.data.text = countText;
        m_viewArmyWindow->BroadcastMessage(message6);
    }

    if (theArmy) {
        i32 spellY3 = 169;
        i32 spellCenterX8 = 420;
        if (quickView)
            spellY3 += 12;
        i32 spacing0 = 44 - theArmy->m_spellCount;
        i32 spellX3 =
            10 - theArmy->m_spellCount + spellCenterX8 - (theArmy->m_spellCount * spacing0) / 2;
        i32 spellIndex9 = -1;
        for (loopIndex0 = 0; loopIndex0 < (theArmy->m_spellCount < 6 ? theArmy->m_spellCount : 6);
             loopIndex0++) {
            spellIndex9++;
            for (; spellIndex9 < 15; spellIndex9++) {
                if (theArmy->m_spellInfluence[spellIndex9])
                    break;
            }
            iconWidget* spellWidget = new iconWidget(
                static_cast<i16>((&loopIndex0)[0] * spacing0 + spellX3),
                static_cast<i16>(spellY3 + 14),
                0,
                0,
                const_cast<char*>("spellinl.icn"),
                static_cast<i16>(spellIndex9),
                0,
                static_cast<i16>(loopIndex0 + 200),
                16,
                1
            );
            if (!spellWidget)
                MemError();
            m_viewArmyWindow->AddWidget(spellWidget, -1);
        }
    }

    glTimers[0] = KBTickCount() + 90;
    m_viewArmyResult = 0;
    if (quickView) {
        gpWindowManager->AddWindow(m_viewArmyWindow, -1, 1);
        QuickViewWait();
        gpWindowManager->RemoveWindow(m_viewArmyWindow);
    } else {
        gpWindowManager->DoDialog(m_viewArmyWindow, ViewArmyHandler, 0);
        if (gbDismissArmy && theGroup) {
            theGroup->m_troopTypes[groupIndex] = -1;
            theGroup->m_troopCounts[groupIndex] = 0;
        }
        if (gbUpgradeArmy && theGroup)
            theGroup->m_troopTypes[groupIndex] = static_cast<i8>(iViewArmyUpgradeToType);
    }
    H2_FREE(details9, 3893);
    delete m_viewArmyWindow;
}

// @early-stop
// @early-stop-reloc-only: retail spells __adjust_fdiv as iLeftRightSave+0x10;
// both resolve to RVA 0x12126c.
VA(0x0047b2cf, 0x3f5)
i32 ViewArmyHandler(tag_message& msg) {
    i32 goldCost6;
    ResourceType resourceType0;
    i32 resourceCost5;

    gbDismissArmy = false;
    gbUpgradeArmy = false;
    // Retail reserves a second short word before the aligned upgrade-cost locals.
    i16 frameDelay0 = 5;
    i16 frameOffset1;

    if (msg.type == MESSAGE_WIDGET) {
        switch (msg.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                switch (msg.payload.widget.id) {
                    case EVENT_WINDOW_FIRST_BUTTON:
                    case EVENT_WINDOW_SECOND_BUTTON:
                        gpWindowManager->m_dialogResult = msg.payload.widget.id;
                        msg.payload.widget.id = 10;
                        msg.payload.widget.command = msg.payload.widget.id;
                        return 2;
                    case EVENT_WINDOW_FOURTH_BUTTON:
                        NormalDialog(
                            const_cast<char*>("Are you sure you want to dismiss this army?"),
                            2,
                            -1,
                            -1,
                            -1,
                            0,
                            -1,
                            0,
                            -1,
                            0
                        );
                        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
                            gbDismissArmy = true;
                            msg.payload.widget.id = 10;
                            msg.payload.widget.command = msg.payload.widget.id;
                            return 2;
                        }
                        break;
                    case 500:
                        goldCost6 = (gMonsterDatabase[IDX(iViewArmyUpgradeToType)].cost
                                     - gMonsterDatabase[iViewArmyType].cost)
                                    * iViewArmyNumTroops * 2;
                        if (iViewArmyUpgradeToType == CREATURE_BLACK_DRAGON) {
                            resourceType0 = RES_SULFUR;
                            resourceCost5 = iViewArmyNumTroops * 2;
                        } else if (iViewArmyUpgradeToType == CREATURE_TITAN) {
                            resourceType0 = RES_GEMS;
                            resourceCost5 = iViewArmyNumTroops * 2;
                        } else {
                            resourceType0 = -1;
                            resourceCost5 = 0;
                        }
                        if (gpCurPlayer->m_resources[IDX(RES_GOLD)] >= goldCost6
                            && (resourceType0 == -1
                                || gpCurPlayer->m_resources[IDX(resourceType0)] >= resourceCost5)) {
                            NormalDialog(
                                const_cast<char*>(
                                    "Your troops can be upgraded, but it will cost you dearly.  "
                                    "Do you wish to upgrade them?"
                                ),
                                2,
                                -1,
                                -1,
                                6,
                                goldCost6,
                                resourceType0,
                                resourceCost5,
                                -1,
                                0
                            );
                            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
                                gpCurPlayer->m_resources[IDX(RES_GOLD)] -= goldCost6;
                                if (resourceType0 != -1)
                                    gpCurPlayer->m_resources[IDX(resourceType0)] -= resourceCost5;
                                gbUpgradeArmy = true;
                                msg.payload.widget.id = 10;
                                msg.payload.widget.command = msg.payload.widget.id;
                                return 2;
                            }
                        } else {
                            NormalDialog(
                                const_cast<char*>("You can't afford to upgrade your troops!"),
                                1,
                                -1,
                                -1,
                                6,
                                goldCost6,
                                resourceType0,
                                resourceCost5,
                                -1,
                                0
                            );
                        }
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    if (!gbLowMemory && KBTickCount() > glTimers[0]) {
        msg.type = MESSAGE_WIDGET;
        msg.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        msg.payload.widget.id = 5;
        iViewArmyFrame = (iViewArmyFrame + 1)
                         % sViewArmyMonFrameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK)];
        msg.payload.widget.data.value =
            sViewArmyMonFrameInfo.animationFrames[IDX(ARMY_ANIMATION_WALK)][iViewArmyFrame];
        gpGame->m_viewArmyWindow->BroadcastMessage(msg);
        msg.payload.widget.command = 52;
        msg.payload.widget.data.value =
            sViewArmyMonFrameInfo.walkXOffsets[iViewArmyFrame] * viewArmyFacingWIPXMod
            + viewArmyBaseX;
        gpGame->m_viewArmyWindow->BroadcastMessage(msg);
        gpGame->m_viewArmyWindow->DrawWindow(1, 0, WINDOW_DRAW_ID_LIMIT);
        glTimers[0] = static_cast<i32>(
            KBTickCount()
            + sViewArmyMonFrameInfo.walkDuration * GAME_VIEW_ARMY_FRAME_DELAY_SCALE
                  / sViewArmyMonFrameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK)]
        );
    }
    return 1;
}

// @early-stop
// Relocation-masked comparison is identical for all 0x671 bytes (133 relocation
// sites in both objects). The objdiff residual is delinked local-label identity.
VA(0x0047b6c4, 0x671)
i32 game::GetRandomNumTroops(i32 monsterType) {
    switch (monsterType) {
        case 0:
            return Random(40, 80);
        case 1:
            return Random(20, 30);
        case 2:
            return Random(20, 30);
        case 3:
            return Random(20, 30);
        case 4:
            return Random(20, 30);
        case 5:
            return Random(12, 25);
        case 6:
            return Random(12, 25);
        case 7:
            return Random(10, 18);
        case 8:
            return Random(8, 16);
        case 9:
            return Random(6, 12);
        case 10:
            return Random(6, 10);
        case 11:
            return Random(25, 40);
        case 12:
            return Random(15, 30);
        case 13:
            return Random(15, 30);
        case 14:
            return Random(20, 35);
        case 15:
            return Random(12, 25);
        case 16:
            return Random(10, 20);
        case 17:
            return Random(7, 10);
        case 18:
            return Random(7, 10);
        case 19:
            return Random(5, 7);
        case 20:
            return Random(25, 45);
        case 21:
            return Random(12, 25);
        case 22:
            return Random(10, 22);
        case 23:
            return Random(15, 30);
        case 24:
            return Random(12, 28);
        case 25:
            return Random(10, 25);
        case 26:
            return Random(10, 20);
        case 27:
            return Random(8, 15);
        case 28:
            return Random(7, 12);
        case 29:
            return Random(20, 50);
        case 30:
            return Random(15, 30);
        case 31:
            return Random(12, 25);
        case 32:
            return Random(10, 16);
        case 33:
            return Random(9, 16);
        case 34:
            return Random(7, 10);
        case 35:
            return Random(4, 7);
        case 36:
            return Random(3, 7);
        case 37:
            return Random(3, 7);
        case 38:
            return Random(20, 50);
        case 39:
            return Random(15, 30);
        case 40:
            return Random(10, 25);
        case 41:
            return Random(10, 22);
        case 42:
            return Random(10, 16);
        case 43:
            return Random(8, 12);
        case 44:
            return Random(7, 11);
        case 45:
            return Random(5, 8);
        case 46:
            return Random(3, 7);
        case 47:
            return Random(20, 50);
        case 48:
            return Random(15, 30);
        case 49:
            return Random(15, 30);
        case 50:
            return Random(10, 25);
        case 51:
            return Random(10, 25);
        case 52:
            return Random(8, 12);
        case 53:
            return Random(8, 12);
        case 54:
            return Random(6, 10);
        case 55:
            return Random(6, 10);
        case 56:
            return Random(4, 8);
        case 57:
            return Random(20, 40);
        case 58:
            return Random(12, 25);
        case 59:
            return Random(10, 20);
        case 60:
            return Random(5, 10);
        case 61:
            return Random(12, 20);
        case 62:
            return Random(13, 25);
        case 63:
            return Random(13, 25);
        case 64:
            return Random(13, 25);
        case 65:
            return Random(13, 25);
        default:
            return 3;
    }
}

VA(0x0047bd35, 0x3f)
void game::TurnOnAIMusic(void) {
    gpSoundManager->StopAllSamples(1);
    gpSoundManager->SwitchAmbientMusic(28);
    gpSoundManager->m_samplesReady = 0;
}

VA(0x0047bd74, 0x25)
void game::TurnOffAIMusic(void) {
    gpSoundManager->m_samplesReady = 1;
}

// @semantic
// The reconstructed body realigns after each of the first two flag clears and thereafter
// matches instruction-for-instruction. Retail expands each clear into a 0x1e-byte longer
// address/load/and/address/store sequence (114 relocations versus 112); direct, bitfield,
// accessor, and volatile spellings either collapse it or over-expand it under this /Od TU.
VA(0x0047bd99, 0x596)
void game::NextPlayer(void) {
    i32 humanCount;
    i32 index;

    m_heroRecs[gpCurPlayer->m_availableHeroIds[0]].m_eventFlags =
        m_heroRecs[gpCurPlayer->m_availableHeroIds[0]].m_eventFlags & ~HERO_EVENT_WEEKLY_VISIT;
    m_heroRecs[gpCurPlayer->m_availableHeroIds[1]].m_eventFlags =
        m_heroRecs[gpCurPlayer->m_availableHeroIds[1]].m_eventFlags & ~HERO_EVENT_WEEKLY_VISIT;
    iCurHourGlassPhase = 0;

    if (gbThisNetHumanPlayer[giCurPlayer] && gConfig.autosave) {
        humanCount = 0;
        for (index = 0; index < 6; index++) {
            if (m_playerDead[index] == 0 && gbHumanPlayer[index])
                humanCount++;
        }
        SaveGame(const_cast<char*>("AUTOSAVE"), 1, 0);
    }

    gpAdvManager->m_identifyHeroActive = 0;
    if (gpGame->m_players[giCurPlayer].m_daysLeft > 0)
        gpGame->m_players[giCurPlayer].m_daysLeft--;
    CheckEndGame(0, 0);
    gpAdvManager->DeactivateCurrTown();
    gpAdvManager->DeactivateCurrHero();

    do {
        giCurPlayer++;
        if (giCurPlayer >= m_playerCount) {
            giCurPlayer = 0;
            PerDay();
        }
    } while (gpGame->m_playerDead[giCurPlayer]);

    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    giCurPlayerBit = static_cast<u8>(1 << giCurPlayer);
    for (index = 0; index < m_players[giCurPlayer].m_heroCount; index++) {
        hero* currentHero = &m_heroRecs[m_players[giCurPlayer].m_heroIds[index]];
        currentHero->m_mobility = currentHero->CalcMobility();
        currentHero->m_remainingMobility = currentHero->m_mobility;
    }

    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        gpMouseManager->SetPointer(1);
        gpAdvManager->HideRoute(1, 0, 1);
        gpAdvManager->CheckDimNextHeroBut();
        TurnOnAIMusic();
        SetNoDialogMenus(0);
        giBottomViewOverride = 6;
        ShowComputerScreen();
        bShowIt = 0;
        if (gbRemoteOn && gbHumanPlayer[giCurPlayer]) {
            gbThisNetGotAdventureControl = false;
            i32 remotePlayer = gbGamePosToNetPos[giCurPlayer];
            if (!gpGame->TransmitSaveGame(remotePlayer, 0, 0))
                ShutDown(0);
        }
        if (giBottomViewOverride == 6)
            giBottomViewOverride = 0;
    } else {
        SetNoDialogMenus(1);
        gpInputManager->Flush();
        gbAllBlack = true;
        gpAdvManager->CheckSetEvilInterface(1, giCurPlayer);
        gbAllBlack = false;
        if (gbBlackoutPlayer && giNumHumanPlayers > 1) {
            sprintf(gText, "%s's turn.", cPlayerNames[giCurPlayer]);
            WaitForPlayer(gText, giCurPlayer);
        }
        if (gbThisNetHumanPlayer[giCurPlayer])
            CancelComputerScreen();
        giCurWatchPlayerBit = giCurPlayerBit;
        giCurWatchPlayer = giCurPlayer;
    }

    if (gbThisNetHumanPlayer[giCurPlayer] && gbRemoteOn && m_day != 1 && giForceSwitchMusic == -1) {
        gpSoundManager->SwitchAmbientMusic(21);
        giForceSwitchMusic = KBTickCount();
        gpSoundManager->m_samplesReady = 0;
    }
    if (m_day == 1 && giCurTurn != 1)
        gpSoundManager->m_samplesReady = 0;

    DoNewTurn();
    CheckEndGame(0, 0);
    if (gbThisNetHumanPlayer[giCurPlayer] && gpSoundManager->m_samplesReady == 0
        && giForceSwitchMusic == -1) {
        gpSoundManager->m_samplesReady = 1;
        gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[gpAdvManager->m_currentTerrain]);
        gpAdvManager->SetEnvironmentOrigin(
            gpAdvManager->m_mapOriginX + 7,
            gpAdvManager->m_mapOriginY + 7,
            1
        );
    }
    if (gbThisNetHumanPlayer[giCurPlayer])
        gpAdvManager->ForceNewHover();
}

// @early-stop
// Logic, frame, slots, CFG, and all 25 relocations match. At +0x2b9 retail forms the
// commutative hero-record address as `add eax,this; mov ecx,heroIndex`; this TU swaps
// those two loads. `heroIndex[m_heroIds]` produced the same bytes; revisit only after
// a cumulative GAME/header-state change.
VA(0x0047c32f, 0x432)
i32 game::ComputeDailyGold(i32 player) {
    i32 heroIndex;
    i32 gold = 0;
    i32 index;

    for (index = 0; index < 144; index++) {
        if (m_mines[index].owner == player) {
            if (m_mines[index].resourceType == 6)
                gold += 1000;
            if (m_mines[index].resourceType == 101)
                gold += 1000;
        }
    }

    for (index = 0; index < 72; index++) {
        if (m_castleRecs[index].m_owner == player) {
            if (m_castleRecs[index].m_buildings & BIT(BUILDING_SLOT_UPGRADE_CASTLE))
                gold += 250;
            else
                gold += 1000;
            if (m_castleRecs[index].m_buildings & BIT(BUILDING_SLOT_SPECIAL_SEVEN))
                gold += 250;
            if (m_castleRecs[index].m_type == 3
                && (m_castleRecs[index].m_buildings & BIT(BUILDING_SLOT_SPECIAL)))
                gold += 500;
        }
    }

    gold += m_players[player].NumOfGivenArtifact(30) * 1000;
    gold += m_players[player].NumOfGivenArtifact(31) * 750;
    gold += m_players[player].NumOfGivenArtifact(32) * 500;
    gold += m_players[player].NumOfGivenArtifact(7) * 10000;
    gold += m_players[player].NumOfGivenArtifact(69) * -250;

    for (heroIndex = 0; heroIndex < m_players[player].m_heroCount; heroIndex++) {
        gold += gEstatesGoldLevel[gpGame->m_heroRecs[m_players[player].m_heroIds[heroIndex]]
                                      .m_secondarySkills[IDX(HERO_SKILL_ESTATES)]];
    }

    if (!gbHumanPlayer[player]) {
        if (gpGame->m_difficulty == 0)
            gold = static_cast<i32>(gold * 0.75);
        if (gpGame->m_difficulty == 1) {
        }
        if (gpGame->m_difficulty == 2)
            gold = static_cast<i32>(gold * 1.29);
        if (gpGame->m_difficulty == 3)
            gold = static_cast<i32>(gold * 1.45);
        if (gpGame->m_difficulty == 4)
            gold = static_cast<i32>(gold * 1.6);
    }

    if (m_playerHandicap[player] == 1)
        gold = static_cast<i32>(gold * 0.15);
    else if (m_playerHandicap[player] == 2)
        gold = static_cast<i32>(gold * 0.3);
    return gold;
}

// @semantic
// Frame/slots and 30 relocations are exact. The only residual is target 0x47ce94..0x47cec2:
// the commutative handicap sum loads gpGame secondary income before this->primary income;
// this partial /Od TU loads the same two operands in the opposite order, then realigns at fild.
VA(0x0047c761, 0x9aa)
void game::PerDay(void) {
    i32 maxSpellPoints9;
    i32 player;
    i32 resource8;
    i32 income13;
    i32 dailyIncome0;
    i32 resourceType1;
    hero* currentHero6;
    i32 restoredSpellPoints13;
    hero* townHero12;
    town* currentTown4;
    double penaltyRate9;

    for (player = 0; player < gpGame->m_playerCount; player++) {
        for (resource8 = 0; resource8 < 7; resource8++) {
            gpGame->m_players[player].m_income[resource8] =
                -m_players[player].m_resources[resource8];
        }
    }

    memset(m_dailyEventFlags, 0, sizeof(m_dailyEventFlags));

    for (player = 0; player < GAME_MINE_COUNT; player++) {
        if (m_mines[player].owner != -1) {
            resourceType1 = m_mines[player].resourceType;
            dailyIncome0 = 0;
            if (resourceType1 == RES_ORE)
                dailyIncome0 = 2;
            else if (resourceType1 == RES_WOOD)
                dailyIncome0 = 2;
            else if (resourceType1 != RES_GOLD)
                dailyIncome0 = 1;

            if (resourceType1 != RES_GOLD && resourceType1 <= RES_GOLD)
                m_players[m_mines[player].owner].m_resources[resourceType1] += dailyIncome0;
        }
    }

    for (player = 0; player < GAME_TOWN_COUNT; player++)
        m_castleRecs[player].m_turnsOwned++;

    for (player = 0; player < m_playerCount; player++) {
        m_players[player].m_resources[IDX(RES_SULFUR)] +=
            m_players[player].NumOfGivenArtifact(IDX(ARTIFACT_ENDLESS_POUCH_SULFUR));
        m_players[player].m_resources[IDX(RES_MERCURY)] +=
            m_players[player].NumOfGivenArtifact(IDX(ARTIFACT_ENDLESS_VIAL_MERCURY));
        m_players[player].m_resources[IDX(RES_GEMS)] +=
            m_players[player].NumOfGivenArtifact(IDX(ARTIFACT_ENDLESS_POUCH_GEMS));
        m_players[player].m_resources[IDX(RES_WOOD)] +=
            m_players[player].NumOfGivenArtifact(IDX(ARTIFACT_ENDLESS_CORD_WOOD));
        m_players[player].m_resources[IDX(RES_ORE)] +=
            m_players[player].NumOfGivenArtifact(IDX(ARTIFACT_ENDLESS_CART_ORE));
        m_players[player].m_resources[IDX(RES_CRYSTAL)] +=
            m_players[player].NumOfGivenArtifact(IDX(ARTIFACT_ENDLESS_POUCH_CRYSTAL));
        m_players[player].m_resources[IDX(RES_GOLD)] += ComputeDailyGold(player);
    }

    if (xIsPlayingExpansionCampaign && xCampaign.HasAward(2))
        m_players[0].m_resources[IDX(RES_WOOD)] += 2;

    for (player = 0; player < m_playerCount; player++) {
        if (!gbHumanPlayer[player]) {
            if (gpGame->m_difficulty >= GAME_DIFFICULTY_HARD) {
                m_players[player].m_resources[IDX(RES_WOOD)]++;
                m_players[player].m_resources[IDX(RES_ORE)]++;
            }
            if (gpGame->m_difficulty >= GAME_DIFFICULTY_EXPERT && m_day >= 1 && m_day <= 6)
                m_players[player].m_resources[m_day - 1]++;
            if (gpGame->m_difficulty >= GAME_DIFFICULTY_IMPOSSIBLE && m_day >= 1 && m_day <= 6)
                m_players[player].m_resources[m_day - 1]++;
            if (gpGame->m_players[player].m_aiDifficulty == 1 && m_day >= 1 && m_day <= 6)
                m_players[player].m_resources[m_day - 1]++;
        }
    }

    m_day++;
    giCurTurn = (m_week - 1) * 7 + (m_month - 1) * 28 + m_day;
    if (!gbGameOver) {
        if (m_day > 7) {
            m_day = 1;
            PerWeek();
        }
        if (m_week > 4) {
            m_week = 1;
            PerMonth();
        }
    }

    for (player = 0; player < GAME_HERO_COUNT; player++)
        m_heroRecs[player].m_eventFlags =
            m_heroRecs[player].m_eventFlags & ~WEEKLY_HERO_RESERVED_FLAG;

    for (player = 0; player < gpGame->m_playerCount; player++) {
        for (resource8 = 0; resource8 < 6; resource8++) {
            if (m_playerHandicap[player]) {
                if (m_playerHandicap[player] == 1)
                    penaltyRate9 = 0.15;
                else
                    penaltyRate9 = 0.30;
                m_players[player].m_resources[resource8] -= static_cast<i32>(
                    (gpGame->m_players[player].m_income[resource8]
                     + m_players[player].m_resources[resource8])
                    * penaltyRate9
                );
            }
        }
    }

    for (player = 0; player < gpGame->m_playerCount; player++) {
        for (resource8 = 0; resource8 < 7; resource8++) {
            gpGame->m_players[player].m_income[resource8] +=
                m_players[player].m_resources[resource8];
        }
    }

    for (player = 0; player < GAME_HERO_COUNT; player++) {
        currentHero6 = &m_heroRecs[player];
        restoredSpellPoints13 = currentHero6->m_spellPoints;
        maxSpellPoints9 = currentHero6->Stats(HeroPrimaryStat(3)) * 10;
        restoredSpellPoints13 += currentHero6->m_secondarySkills[8] + 1;
        if (currentHero6->HasArtifact(ARTIFACT_POWER_RING))
            restoredSpellPoints13 += 2;
        if (restoredSpellPoints13 > maxSpellPoints9)
            restoredSpellPoints13 = maxSpellPoints9;
        if (currentHero6->m_spellPoints < restoredSpellPoints13)
            currentHero6->m_spellPoints = static_cast<i16>(restoredSpellPoints13);
        if (HAS(currentHero6->m_eventFlags, HERO_EVENT_MAGIC_WELL))
            currentHero6->m_eventFlags = currentHero6->m_eventFlags - HERO_EVENT_MAGIC_WELL;
    }

    for (player = 0; player < GAME_TOWN_COUNT; player++) {
        currentTown4 = GetTown(player);
        if (!(currentTown4->m_buildings & 1))
            continue;
        if (currentTown4->m_occupyingHeroId != -1) {
            townHero12 = GetHero(currentTown4->m_occupyingHeroId);
            maxSpellPoints9 = townHero12->Stats(HeroPrimaryStat(3)) * 10;
            if (townHero12->m_spellPoints < maxSpellPoints9)
                townHero12->m_spellPoints = static_cast<i16>(maxSpellPoints9);
        }
    }
}

// @semantic
// 99.09%: frame and control flow are byte-exact. The six hero-slot index residuals
// differ only in when a commutative subtraction is issued, and the map-height loop
// differs only in compare operand order; both are the documented TU-cumulative /Od
// operand-order pattern. The other residuals are delinked switch labels/constants;
// the full function has the retail 95 relocations and matching external targets.
VA(0x0047d10b, 0x199d)
void game::PerWeek(void) {
    i32 heroClass18 = 0;
    i32 outerIndex5;
    i32 innerIndex3;
    i32 mapY5;
    i32 mapX8;
    town* castle37;
    i32 growth13;
    i32 desiredClass1;
    i32 monsterIncrease16;
    i32 monsterCount36;
    hero* weeklyHero4;

    giWeekType = 0;
    giWeekTypeExtra = Random(0, 14);
    if (m_week != 4) {
        outerIndex5 = Random(1, 4);
        if (outerIndex5 == 1) {
            giWeekType = 1;
            giWeekTypeExtra = Random(0, 56);
        }
    }

    for (outerIndex5 = 0; outerIndex5 < GAME_TOWN_COUNT; outerIndex5++) {
        castle37 = GetTown(outerIndex5);
        for (innerIndex3 = WEEKLY_FIRST_DWELLING; innerIndex3 <= WEEKLY_LAST_DWELLING;
             innerIndex3++) {
            if (castle37->m_buildings & (1 << innerIndex3)) {
                growth13 = gMonsterDatabase[gDwellingType[castle37->m_type]
                                                         [innerIndex3 - WEEKLY_FIRST_DWELLING]]
                               .growth;
                if (castle37->m_buildings & BIT(BUILDING_SLOT_SPECIAL_FOUR))
                    growth13 += 2;
                if (innerIndex3 == WEEKLY_FIRST_DWELLING
                    && (castle37->m_buildings & BIT(BUILDING_SLOT_WELL_EXTRA)))
                    growth13 += 8;
                if (castle37->m_owner == -1)
                    growth13 /= 2;
                if (castle37->m_owner >= 0
                    && castle37->m_garrison[innerIndex3 - WEEKLY_FIRST_DWELLING] == 0
                    && !gbHumanPlayer[castle37->m_owner]) {
                    if (gpGame->m_difficulty == GAME_DIFFICULTY_HARD)
                        growth13 = static_cast<i32>(growth13 * 1.20);
                    if (gpGame->m_difficulty == GAME_DIFFICULTY_EXPERT)
                        growth13 = static_cast<i32>(growth13 * 1.32);
                    if (gpGame->m_difficulty == GAME_DIFFICULTY_IMPOSSIBLE)
                        growth13 = static_cast<i32>(growth13 * 1.44);
                }
                if (giWeekType == 1
                    && gDwellingType[castle37->m_type][innerIndex3 - WEEKLY_FIRST_DWELLING]
                           == giWeekTypeExtra)
                    growth13 += 5;
                castle37->m_garrison[innerIndex3 - WEEKLY_FIRST_DWELLING] += growth13;
            }
        }
    }

    for (outerIndex5 = 0; outerIndex5 < GAME_PLAYER_COUNT; outerIndex5++) {
        for (innerIndex3 = 0; innerIndex3 < 2; innerIndex3++) {
            if (innerIndex3 == 1) {
                heroClass18 =
                    m_heroRecs[gpGame->m_players[outerIndex5].m_availableHeroIds[0]].m_cursorType;
            }
            heroClass18 = (Random(1, 5) + heroClass18) % 6;
            desiredClass1 = heroClass18;
            if (innerIndex3 == 0
                && m_setupPlayerRace[gcColorToSetupPos[m_players[outerIndex5].m_color]]
                       < GAME_PLAYER_COUNT) {
                desiredClass1 =
                    m_setupPlayerRace[gcColorToSetupPos[m_players[outerIndex5].m_color]];
            }

            if (gpGame->m_availableHeroes[(
                    innerIndex3 - outerIndex5 + outerIndex5 * (sizeof(playerData) + 1)
                )[gpGame->m_players[0].m_availableHeroIds]]
                == WEEKLY_AVAILABLE_HERO) {
                if (gpGame
                        ->m_heroRecs[(
                            innerIndex3 - outerIndex5 + outerIndex5 * (sizeof(playerData) + 1)
                        )[gpGame->m_players[0].m_availableHeroIds]]
                        .m_eventFlags
                    & WEEKLY_HERO_RESERVED_FLAG)
                    continue;
            }
            {
                if (gpGame->m_availableHeroes[(
                        innerIndex3 - outerIndex5 + outerIndex5 * (sizeof(playerData) + 1)
                    )[gpGame->m_players[0].m_availableHeroIds]]
                    == WEEKLY_AVAILABLE_HERO)
                    gpGame->m_availableHeroes[(
                        innerIndex3 - outerIndex5 + outerIndex5 * (sizeof(playerData) + 1)
                    )[gpGame->m_players[0].m_availableHeroIds]] = -1;
                if (innerIndex3 == 1 && !gbHumanPlayer[outerIndex5])
                    desiredClass1 = -1;
                i32 useDifficultyBonus3 = !gbHumanPlayer[outerIndex5] && gpGame->m_difficulty > 0;
                (
                    innerIndex3 - outerIndex5 + outerIndex5 * (sizeof(playerData) + 1)
                )[gpGame->m_players[0].m_availableHeroIds] =
                    static_cast<i8>(
                        gpGame->GetNewHeroId(outerIndex5, desiredClass1, useDifficultyBonus3)
                    );
                m_availableHeroes[(
                    innerIndex3 - outerIndex5 + outerIndex5 * (sizeof(playerData) + 1)
                )[gpGame->m_players[0].m_availableHeroIds]] = WEEKLY_AVAILABLE_HERO;
            }
        }
    }

    for (mapY5 = 0; MAP_HEIGHT > mapY5; mapY5++) {
        for (mapX8 = 0; mapX8 < MAP_WIDTH; mapX8++) {
            switch (WORLDMAP->Row(mapY5)[mapX8].m_triggerType) {
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER: {
                    monsterCount36 =
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata & IDX(MAP_MONSTER_COUNT_MASK);
                    monsterIncrease16 = monsterCount36 / 7;
                    if (Random(1, 7) <= static_cast<i32>(monsterCount36 % 7))
                        monsterIncrease16++;
                    monsterCount36 += monsterIncrease16;
                    if (monsterCount36 > WEEKLY_MONSTER_LIMIT)
                        monsterCount36 = WEEKLY_MONSTER_LIMIT;
                    WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata =
                        (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata & IDX(MAP_MONSTER_GUARD_FLAG))
                        | monsterCount36;
                    break;
                }
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTESIAN_SPRING:
                    WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata = 1;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WATER_WHEEL:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                        != WEEKLY_WATER_WHEEL_EMPTY)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata = 2;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MAGIC_GARDEN:
                    WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata = Random(0, 1) ? 7 : 6;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WINDMILL:
                    WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata = Random(1, 5);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARCHER_HOUSE:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(2, 4);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_GOBLIN_HUT:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(3, 6);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DWARF_COTTAGE:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(2, 4);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_PEASANT_HUT:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(5, 10);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_LOG_CABIN:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(5, 10);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DESERT_TENT:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(1, 3);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WAGON_CAMP:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(3, 6);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREE_HOUSE:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(4, 8);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SIRENS:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(3, 6);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WATCH_TOWER:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(1, 4);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RUINS:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(1, 3);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREE_CITY:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                        < WEEKLY_MONSTER_POPULATION_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(10, 20);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CAVE:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(3, 6);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EXCAVATION:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(4, 8);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HALFLING_HOLE:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(5, 10);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TROLL_BRIDGE:
                    if (!(WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                          & WEEKLY_DWELLING_NO_GROWTH_FLAG)
                        && WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                               < WEEKLY_DRAGON_CITY_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(1, 3);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CITY_OF_DEAD:
                    if (!(WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                          & WEEKLY_DWELLING_NO_GROWTH_FLAG)
                        && WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                               < WEEKLY_DRAGON_CITY_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(1, 3);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DRAGON_CITY:
                    if (!(WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                          & WEEKLY_DWELLING_NO_GROWTH_FLAG)
                        && WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                               < WEEKLY_DRAGON_CITY_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += 1;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EXPANSION_DWELLING:
                    WeeklyRecruitSite(WORLDMAP->GetCell(mapX8, mapY5));
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EXPANSION_OBJECT:
                    WeeklyGenericSite(WORLDMAP->GetCell(mapX8, mapY5));
                    break;
                default:
                    break;
            }
        }
    }

    for (outerIndex5 = 0; outerIndex5 < GAME_HERO_COUNT; outerIndex5++) {
        weeklyHero4 = &m_heroRecs[outerIndex5];
        if (weeklyHero4->m_eventFlags & WEEKLY_HERO_VISIT_FLAG)
            weeklyHero4->m_eventFlags =
                static_cast<u32>(weeklyHero4->m_eventFlags) - WEEKLY_HERO_VISIT_FLAG;
    }

    m_week++;
    SetupNewRumour();
    GiveTroopsToNeutralTowns();
}

// @early-stop
// Relocation-masked raw bytes are exact across the 0x12d-byte body. All five
// Random relocations agree; objdiff's 99.77% is only the six jump-table entries,
// whose retail local labels are delinked as the containing function.
VA(0x0047eaa8, 0x12d)
void game::WeeklyRecruitSite(mapCell* cell) {
    i32 type = cell->m_objectMetadata;
    type &= WEEKLY_RECRUIT_TYPE_MASK;
    i32 recruitCount = cell->m_objectMetadata;
    recruitCount >>= WEEKLY_RECRUIT_COUNT_SHIFT;
    i32 packed;

    switch (type) {
        case 0:
            recruitCount += Random(WEEKLY_RECRUIT_MIN_GROWTH, WEEKLY_RECRUIT_MAX_GROWTH);
            break;
        case 1:
            recruitCount += Random(WEEKLY_RECRUIT_MIN_GROWTH, WEEKLY_RECRUIT_MAX_GROWTH);
            break;
        case 2:
            recruitCount += Random(WEEKLY_RECRUIT_MIN_GROWTH, WEEKLY_RECRUIT_MAX_GROWTH);
            break;
        case 3:
            recruitCount += Random(WEEKLY_RECRUIT_MIN_GROWTH, WEEKLY_RECRUIT_MAX_GROWTH);
            break;
        case 4:
            recruitCount += Random(WEEKLY_RECRUIT_MIN_GROWTH, WEEKLY_RECRUIT_MAX_GROWTH);
            break;
    }

    if (recruitCount > WEEKLY_RECRUIT_LIMIT)
        recruitCount = WEEKLY_RECRUIT_LIMIT;
    packed = (recruitCount << WEEKLY_RECRUIT_COUNT_SHIFT) | type;
    cell->m_objectMetadata = packed | 0;
}

// @semantic
// The complete 0x6f frame/CFG matches and neither side has relocations. The 12 bytes at
// +0x30..+0x3f only reverse the equivalent packed-word/type evaluation order in case 4.
// Whole-word, cast, and |0 variants did not steer it; revisit with new GAME TU state.
VA(0x0047ebd5, 0x6f)
void game::WeeklyGenericSite(mapCell* cell) {
    i32 type = cell->m_objectMetadata;
    type &= WEEKLY_SITE_TYPE_MASK;
    switch (type) {
        case 4:
            cell->m_objectMetadata = type;
            break;
    }
}

// @early-stop
// Exact 0x375-byte body and 30 relocation sites. The typed dwelling/monster
// references resolve to retail's folded 0x4fca35 and 0x4faeb7 local symbols.
VA(0x0047ec44, 0x375)
void game::PerMonth(void) {
    mapCell* cell0;
    i32 mapX8;
    i32 mapY5;
    i32 townIndex0;
    i32 building4;
    i32 growth9;
    town* castle10;
    i32 firstCount5;
    i32 secondCount4;

    m_month++;
    townIndex0 = Random(MONTH_ROLL_MIN, MONTH_ROLL_MAX);
    if (townIndex0 <= MONTH_NORMAL_ROLL_MAX) {
        giMonthType = MONTH_TYPE_NORMAL;
        giMonthTypeExtra = Random(MONTH_NORMAL_NAME_MIN, MONTH_NORMAL_NAME_MAX);
    } else if (townIndex0 <= MONTH_CREATURE_ROLL_MAX) {
        giMonthType = MONTH_TYPE_CREATURE;
        giMonthTypeExtra = giMonType[Random(MONTH_CREATURE_LIST_MIN, MONTH_CREATURE_LIST_MAX)];
    } else {
        giMonthType = MONTH_TYPE_PLAGUE;
    }

    for (townIndex0 = 0; townIndex0 < GAME_TOWN_COUNT; townIndex0++) {
        for (building4 = WEEKLY_FIRST_DWELLING; building4 <= WEEKLY_LAST_DWELLING; building4++) {
            castle10 = GetTown(townIndex0);
            if (castle10->m_buildings & (1 << building4)) {
                growth9 = gMonsterDatabase[gDwellingType[castle10->m_type]
                                                        [building4 - WEEKLY_FIRST_DWELLING]]
                              .growth;
                if (castle10->m_buildings & MONTH_WELL_BUILDING)
                    growth9 += MONTH_WELL_GROWTH;
                if (building4 == WEEKLY_FIRST_DWELLING
                    && (castle10->m_buildings & MONTH_FIRST_DWELLING_BONUS_BUILDING))
                    growth9 += MONTH_FIRST_DWELLING_GROWTH;

                if (giMonthType == MONTH_TYPE_CREATURE
                    && gDwellingType[castle10->m_type][building4 - WEEKLY_FIRST_DWELLING]
                           == giMonthTypeExtra)
                    castle10->m_garrison[building4 - WEEKLY_FIRST_DWELLING] *= 2;

                if (giMonthType == MONTH_TYPE_PLAGUE) {
                    castle10->m_garrison[building4 - WEEKLY_FIRST_DWELLING] -= growth9;
                    if (castle10->m_garrison[building4 - WEEKLY_FIRST_DWELLING] < 0)
                        castle10->m_garrison[building4 - WEEKLY_FIRST_DWELLING] = 0;
                    castle10->m_garrison[building4 - WEEKLY_FIRST_DWELLING] =
                        castle10->m_garrison[building4 - WEEKLY_FIRST_DWELLING] >> 1;
                }
            }
        }
    }

    if (giMonthType == MONTH_TYPE_CREATURE) {
        for (mapX8 = 0; mapX8 < MAP_WIDTH; mapX8++) {
            for (mapY5 = 0; mapY5 < MAP_HEIGHT; mapY5++) {
                cell0 = gpAdvManager->GetCell(mapX8, mapY5);
                if (cell0->m_triggerType == 0 && !cell0->m_objectLayerBit1
                    && !cell0->m_objectLayerBit0
                    && giGroundToTerrain[cell0->m_terrainImageIndex] != 0) {
                    if (Random(MONTH_MONSTER_SPAWN_MIN, MONTH_MONSTER_SPAWN_MAX)
                        == MONTH_MONSTER_SPAWN_ROLL) {
                        cell0->m_triggerType = MONTH_MONSTER_TRIGGER;
                        cell0->m_objectTileset = MONTH_MONSTER_TILESET;
                        cell0->m_objectIndex = static_cast<u8>(giMonthTypeExtra);
                        firstCount5 = GetRandomNumTroops(giMonthTypeExtra);
                        secondCount4 = GetRandomNumTroops(giMonthTypeExtra);
                        cell0->m_objectMetadata = ((firstCount5 | 0) + secondCount4) | 0;
                        if (Random(MONTH_MONSTER_SPAWN_MIN, MONTH_MONSTER_GUARD_ROLL_MAX)
                            < MONTH_MONSTER_GUARD_CHANCE)
                            cell0->m_objectMetadata |= IDX(MAP_MONSTER_GUARD_FLAG);
                    }
                }
            }
        }
    }

    gpAdvManager->CompleteDraw(0);
}

// @semantic
// The complete control flow and both relocations align. The AST-permuted bounds spelling
// improves the match to 99.19%: this 0x472-byte body materializes y + 1 with a one-byte inc,
// while retail's 0x476-byte body compares y directly and retains a five-byte continuation.
VA(0x0047efb9, 0x476)
void game::ConvertObject(
    i32 left,
    i32 top,
    i32 right,
    i32 bottom,
    TilesetId oldTileset,
    i32 oldFirstIndex,
    i32 oldLastIndex,
    i32 newTileset,
    i32 newFirstIndex,
    i32 oldTrigger,
    i32 newTrigger
) {
    i32 x;
    i32 y;
    mapCell* cell;
    mapCellExtra* extra;

    for (x = left; right >= x; x++) {
        for (y = top; bottom >= y; y++) {
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && MAP_HEIGHT >= y + 1) {
                cell = WORLDMAP->GetCell(x, y);
                if (cell->m_objectIndex != static_cast<u8>(-1)
                    && cell->m_objectTileset == oldTileset && cell->m_objectIndex >= oldFirstIndex
                    && cell->m_objectIndex <= oldLastIndex) {
                    cell->m_objectTileset = static_cast<u8>(newTileset);
                    cell->m_objectIndex =
                        static_cast<u8>(cell->m_objectIndex - oldFirstIndex + newFirstIndex);
                }
                if ((cell->m_triggerType & MAP_TRIGGER_TYPE_MASK) == oldTrigger)
                    cell->m_triggerType = static_cast<u8>(
                        (cell->m_triggerType & MAP_TRIGGER_ACTION_FLAG) | newTrigger
                    );

                if (cell->m_extraIndex != 0
                    && WORLDMAP->Extra(cell->m_extraIndex)->objectIndex != static_cast<u8>(-1))
                    extra = WORLDMAP->Extra(cell->m_extraIndex);
                else
                    extra = 0;
                while (extra != 0) {
                    if (extra->objectTileset == oldTileset && extra->objectIndex >= oldFirstIndex
                        && extra->objectIndex <= oldLastIndex) {
                        extra->objectTileset = static_cast<u8>(newTileset);
                        extra->objectIndex =
                            static_cast<u8>(extra->objectIndex - oldFirstIndex + newFirstIndex);
                    }
                    if (extra->nextIndex != 0
                        && WORLDMAP->Extra(extra->nextIndex)->objectIndex != static_cast<u8>(-1))
                        extra = WORLDMAP->Extra(extra->nextIndex);
                    else
                        extra = 0;
                }

                if (cell->m_overlayIndex != static_cast<u8>(-1)
                    && cell->m_overlayTileset == oldTileset && cell->m_overlayIndex >= oldFirstIndex
                    && cell->m_overlayIndex <= oldLastIndex) {
                    cell->m_overlayTileset = static_cast<u8>(newTileset);
                    cell->m_overlayIndex =
                        static_cast<u8>(cell->m_overlayIndex - oldFirstIndex + newFirstIndex);
                }
                if (cell->m_extraIndex != 0
                    && WORLDMAP->Extra(cell->m_extraIndex)->overlayIndex != static_cast<u8>(-1))
                    extra = WORLDMAP->Extra(cell->m_extraIndex);
                else
                    extra = 0;
                while (extra != 0) {
                    if (extra->overlayTileset == oldTileset && extra->overlayIndex >= oldFirstIndex
                        && extra->overlayIndex <= oldLastIndex) {
                        extra->overlayTileset = static_cast<u8>(newTileset);
                        extra->overlayIndex =
                            static_cast<u8>(extra->overlayIndex - oldFirstIndex + newFirstIndex);
                    }
                    if (extra->nextIndex != 0
                        && WORLDMAP->Extra(extra->nextIndex)->overlayIndex != static_cast<u8>(-1))
                        extra = WORLDMAP->Extra(extra->nextIndex);
                    else
                        extra = 0;
                }
            }
        }
    }
}

VA(0x0047f42f, 0x1c2)
void game::RandomizeTown(i32 x, i32 y, i32) {
    i32 unused6[2]; // Two unaddressed words are present in the retail /Od frame.
    i32 townId0 = GetTownId(x, y);
    town* castle0 = GetTown(townId0);
    mapTownExtra* extra =
        reinterpret_cast<mapTownExtra*>(ppMapExtra[WORLDMAP->GetCell(x, y)->m_objectMetadata]);
    i32 race0;

    if (extra->color == RANDOM_TOWN_UNOWNED_COLOR)
        race0 = Random(RANDOM_TOWN_RACE_MIN, RANDOM_TOWN_RACE_MAX);
    else
        race0 = m_setupPlayerRace[gcColorToSetupPos[extra->color]];

    castle0->m_turnsOwned = RANDOM_TOWN_AGE;
    ConvertObject(
        x + RANDOM_TOWN_LEFT,
        y + RANDOM_TOWN_TOP,
        x + RANDOM_TOWN_RIGHT,
        y + RANDOM_TOWN_BOTTOM,
        RANDOM_TOWN_SOURCE_TILESET,
        RANDOM_TOWN_OBJECT_SOURCE_FIRST,
        RANDOM_TOWN_OBJECT_SOURCE_LAST,
        RANDOM_TOWN_OBJECT_TILESET,
        race0 << RANDOM_TOWN_RACE_FRAME_SHIFT,
        RANDOM_TOWN_FIRST_TRIGGER,
        RANDOM_TOWN_TRIGGER
    );
    ConvertObject(
        x + RANDOM_TOWN_LEFT,
        y + RANDOM_TOWN_TOP,
        x + RANDOM_TOWN_RIGHT,
        y + RANDOM_TOWN_BOTTOM,
        RANDOM_TOWN_SOURCE_TILESET,
        RANDOM_TOWN_OVERLAY_SOURCE_FIRST,
        RANDOM_TOWN_OVERLAY_SOURCE_LAST,
        RANDOM_TOWN_OVERLAY_TILESET,
        race0 << RANDOM_TOWN_RACE_FRAME_SHIFT,
        RANDOM_TOWN_FIRST_TRIGGER,
        RANDOM_TOWN_TRIGGER
    );
    ConvertObject(
        x + RANDOM_TOWN_LEFT,
        y + RANDOM_TOWN_TOP,
        x + RANDOM_TOWN_RIGHT,
        y + RANDOM_TOWN_BOTTOM,
        RANDOM_TOWN_SOURCE_TILESET,
        RANDOM_TOWN_OBJECT_SOURCE_FIRST,
        RANDOM_TOWN_OBJECT_SOURCE_LAST,
        RANDOM_TOWN_OBJECT_TILESET,
        race0 << RANDOM_TOWN_RACE_FRAME_SHIFT,
        RANDOM_TOWN_SECOND_TRIGGER,
        RANDOM_TOWN_TRIGGER
    );
    ConvertObject(
        x + RANDOM_TOWN_LEFT,
        y + RANDOM_TOWN_TOP,
        x + RANDOM_TOWN_RIGHT,
        y + RANDOM_TOWN_BOTTOM,
        RANDOM_TOWN_SOURCE_TILESET,
        RANDOM_TOWN_OVERLAY_SOURCE_FIRST,
        RANDOM_TOWN_OVERLAY_SOURCE_LAST,
        RANDOM_TOWN_OVERLAY_TILESET,
        race0 << RANDOM_TOWN_RACE_FRAME_SHIFT,
        RANDOM_TOWN_SECOND_TRIGGER,
        RANDOM_TOWN_TRIGGER
    );
    m_castleRecs[townId0].m_type = static_cast<i8>(race0);
}

// @semantic
// Exact 0x619-byte span and 22 relocation sites. The remaining /Od differences
// are operand evaluation in the inlined GetCell(x + 1, ...) accessors and the
// columnOffset + x expressions. Both operand spellings were tested unchanged;
// the residual is TU-cumulative compiler state, not a behavioral difference.
VA(0x0047f5f1, 0x619)
void game::RandomizeMine(i32 x, i32 y) {
    u8 objectFrame1;
    i32 mineId;
    i32 mineType29;
    i32 terrain3 = giGroundToTerrain[WORLDMAP->GetCell(x, y)->m_terrainImageIndex];
    i32 columnOffset4;
    i32 retry4;
    i32 rowOffset0;
    u8 mineFrame36;
    i32 triggerType19;

    for (retry4 = 0; retry4 < 30; retry4++) {
        switch (terrain3) {
            case 1:
            case 6:
                mineType29 = Random(1, 6);
                if (mineType29 == 1)
                    mineType29 = 0;
                break;
            case 2:
                mineType29 = Random(2, 6);
                break;
            case 3:
                mineType29 = Random(0, 6);
                break;
            case 4:
                mineType29 = 1;
                break;
            default:
                mineType29 = Random(1, 6);
                break;
        }
        if (RandMineQty[mineType29] == 0)
            retry4 = 30;
    }
    RandMineQty[mineType29]++;

    switch (mineType29) {
        case 0:
            mineFrame36 = 5;
            break;
        case 1:
            mineFrame36 = 25;
            break;
        default:
            switch (terrain3) {
                case 1:
                    mineFrame36 = 15;
                    break;
                case 2:
                    mineFrame36 = 19;
                    break;
                default:
                    mineFrame36 = 9;
                    break;
            }
            break;
    }

    switch (mineType29) {
        case 0:
            objectFrame1 = 7;
            break;
        case 1:
            switch (terrain3) {
                case 3:
                    objectFrame1 = 43;
                    break;
                case 4:
                    objectFrame1 = 35;
                    break;
                default:
                    objectFrame1 = 27;
                    break;
            }
            break;
        default:
            switch (terrain3) {
                case 1:
                    objectFrame1 = 17;
                    break;
                case 2:
                    objectFrame1 = 21;
                    break;
                case 3:
                    objectFrame1 = 23;
                    break;
                case 5:
                    objectFrame1 = 13;
                    break;
                default:
                    objectFrame1 = 11;
                    break;
            }
            break;
    }

    WORLDMAP->GetCell(x, y)->m_objectIndex = objectFrame1;
    WORLDMAP->GetCell(x + 1, y)->m_objectIndex = objectFrame1 + 1;
    WORLDMAP->GetCell(x, y - 1)->m_overlayIndex = mineFrame36;
    WORLDMAP->GetCell(x + 1, y - 1)->m_overlayIndex = mineFrame36 + 1;

    if (mineType29 == 1) {
        WORLDMAP->GetCell(x + 1, y)->m_objType |= 1;
        triggerType19 = 1;
    } else if (mineType29 == 0) {
        triggerType19 = 29;
    } else {
        m_worldMap
            .ChangeTilesetIndex(WORLDMAP->GetCell(x + 1, y), x + 1, y, 29, mineType29 - 2, 0, -1);
        triggerType19 = 23;
    }

    mineId = GetMineId(x, y);
    for (rowOffset0 = 0; rowOffset0 < 2; rowOffset0++) {
        for (columnOffset4 = 0; columnOffset4 < 2; columnOffset4++) {
            if ((WORLDMAP->GetCell(columnOffset4 + x, y - rowOffset0)->m_triggerType
                 & MAP_TRIGGER_TYPE_MASK)
                > 0)
                if ((WORLDMAP->GetCell(columnOffset4 + x, y - rowOffset0)->m_triggerType
                     & MAP_TRIGGER_TYPE_MASK)
                    <= MINE_FLAG_OVERWRITE_LIMIT)
                    continue;
            WORLDMAP->GetCell(columnOffset4 + x, y - rowOffset0)->m_objectMetadata = mineId;
            WORLDMAP->GetCell(columnOffset4 + x, y - rowOffset0)->m_triggerType = triggerType19;
        }
    }
    WORLDMAP->GetCell(x, y)->m_triggerType |= MAP_TRIGGER_ACTION_FLAG;
    m_mines[mineId].resourceType = static_cast<i8>(mineType29);
}

// @semantic: Complete semantics/frame/CFG and all three external relocation
// identities agree. The only structural residual is the outer width guard:
// retail loads x then compares MAP_WIDTH with jle; base loads MAP_WIDTH then
// compares x with jge. Reversed relational and OD_STEER(x) barrier forms were neutral,
// and ten bounded TU-state probes did not close it. Revisit after a material
// GAME predecessor/header or comparison-tool change.
VA(0x0047fc0a, 0xc6)
void game::InitRandomArtifacts(void) {
    i32 xx;
    memset(m_randomArtifacts, 0, sizeof(m_randomArtifacts));
    i32 x;
    for (x = 0; x < MAP_WIDTH; x++) {
        for (i32 y = 0; y < MAP_HEIGHT; y++) {
            mapCell* cell = WORLDMAP->Row(y) + x;
            if (cell->m_triggerType == IDX(MAP_TRIGGER_ARTIFACT))
                m_randomArtifacts[cell->m_objectIndex >> 1] = 1;
        }
    }
}

VA(0x0047fcd0, 0x17f)
i32 game::GetRandomArtifactId(i32 levelMask, i32 allowCursed) {
    i32 attempts = 0;
    i32 artifact;

    while (1) {
        if (xIsExpansionMap)
            artifact = Random(RANDOM_ARTIFACT_FIRST, RANDOM_ARTIFACT_EXPANSION_LAST);
        else
            artifact = Random(RANDOM_ARTIFACT_FIRST, RANDOM_ARTIFACT_BASE_LAST);

        if (!(levelMask & gArtifactLevel[artifact]))
            continue;
        if (artifact == ARTIFACT_EDITOR_ANY_ULTIMATE || artifact == ARTIFACT_EDITOR_UNUSED_84
            || artifact == ARTIFACT_EDITOR_UNUSED_85 || artifact == ARTIFACT_EDITOR_UNUSED_86
            || artifact == ARTIFACT_SPELL_SCROLL || artifact == ARTIFACT_BREASTPLATE_ANDURAN
            || artifact == ARTIFACT_BATTLE_GARB || artifact == ARTIFACT_HELMET_ANDURAN
            || artifact == ARTIFACT_SWORD_ANDURAN || artifact == ARTIFACT_SPHERE_NEGATION)
            continue;
        if (attempts++ < RANDOM_ARTIFACT_UNIQUE_RETRIES && m_randomArtifacts[artifact])
            continue;
        if (IsCursedItem(artifact)) {
            if (!allowCursed)
                continue;
            if (Random(RANDOM_ARTIFACT_FIRST, RANDOM_ARTIFACT_CURSED_ROLL_MAX)
                < RANDOM_ARTIFACT_CURSED_REJECT_CHANCE)
                continue;
        }
        if (m_mapHeader.victoryCondition != IDX(MAP_VICTORY_FIND_ARTIFACT)
            || m_mapHeader.victoryConditionValue - VICTORY_ARTIFACT_ID_OFFSET != artifact)
            break;
    }

    m_randomArtifacts[artifact] = 1;
    return artifact;
}

VA(0x0047fe4f, 0x68)
i32 IsCursedItem(i32 item) {
    if (item == ARTIFACT_FIZBIN_OF_MISFORTUNE || item == ARTIFACT_HIDEOUS_MASK
        || item == ARTIFACT_TAX_LIEN || item == ARTIFACT_ARM_OF_MARTYR
        || item == ARTIFACT_BROACH_SHIELDING || item == ARTIFACT_HEART_FIRE
        || item == ARTIFACT_HEART_ICE)
        return 1;
    return 0;
}

VA(0x0047feb7, 0x1ef)
void game::RandomizeHeroPool(void) {
    for (i32 heroId = 0; heroId < RANDOM_HERO_COUNT; heroId++) {
        m_heroRecs[heroId].m_experience =
            Random(RANDOM_HERO_EXPERIENCE_MIN, RANDOM_HERO_EXPERIENCE_MAX)
            + RANDOM_HERO_EXPERIENCE_BASE;
        SetRandomHeroArmies(heroId, RANDOM_HERO_NORMAL_ARMY);
        m_heroRecs[heroId].m_remainingMobility = m_heroRecs[heroId].CalcMobility();
        m_heroRecs[heroId].m_mobility = m_heroRecs[heroId].m_remainingMobility;
        m_heroRecs[heroId].m_randomSeed =
            static_cast<u8>(Random(RANDOM_HERO_SEED_MIN, RANDOM_HERO_SEED_MAX));
        m_heroRecs[heroId].m_enabled = RANDOM_HERO_ENABLED;

        if (m_heroRecs[heroId].m_cursorType == IDX(FACTION_SORCERESS))
            m_heroRecs[heroId].m_spells[IDX(SPELL_BLESS)] = RANDOM_HERO_STARTING_SPELL_KNOWN;
        else if (m_heroRecs[heroId].m_cursorType == IDX(FACTION_WARLOCK))
            m_heroRecs[heroId].m_spells[IDX(SPELL_CURSE)] = RANDOM_HERO_STARTING_SPELL_KNOWN;
        else if (m_heroRecs[heroId].m_cursorType == IDX(FACTION_NECROMANCER))
            m_heroRecs[heroId].m_spells[IDX(SPELL_HASTE)] = RANDOM_HERO_STARTING_SPELL_KNOWN;
        else if (m_heroRecs[heroId].m_cursorType == IDX(FACTION_WIZARD))
            m_heroRecs[heroId].m_spells[IDX(SPELL_STONE_SKIN)] = RANDOM_HERO_STARTING_SPELL_KNOWN;
    }
}

// @semantic: Complete army-table semantics, frame/slots, CFG, and all three
// external relocation identities agree. At +0x111 retail computes the strong-army
// threshold before Random and compares threshold<=roll; base calls Random first
// and emits the equivalent roll>=threshold. Reversing the source comparison was
// byte-neutral; ten bounded TU-state probes found only disposable closures.
// Revisit after a material GAME predecessor/header or comparison-tool change.
VA(0x004800a6, 0x378)
void game::SetRandomHeroArmies(i32 heroId, i32 strongArmy) {
    armyGroup* army2 = &m_heroRecs[heroId].m_army;
    i32 armySlot7 = 0;
    RandomHeroArmyRange armyTable7[IDX(FACTION_COUNT)][RANDOM_HERO_ARMY_OPTION_COUNT] = {
        {{IDX(CREATURE_PEASANT), 30, 50}, {IDX(CREATURE_ARCHER), 3, 5}, {IDX(CREATURE_PIKEMAN), 2, 4}},
        {{IDX(CREATURE_GOBLIN), 15, 25}, {IDX(CREATURE_ORC), 3, 5}, {IDX(CREATURE_WOLF), 2, 3}},
        {{IDX(CREATURE_SPRITE), 10, 20}, {IDX(CREATURE_DWARF), 2, 4}, {IDX(CREATURE_ELF), 1, 2}},
        {{IDX(CREATURE_CENTAUR), 6, 10}, {IDX(CREATURE_GARGOYLE), 2, 4}, {IDX(CREATURE_GRIFFIN), 1, 2}},
        {{IDX(CREATURE_HALFLING), 6, 10}, {IDX(CREATURE_BOAR), 2, 4}, {IDX(CREATURE_IRON_GOLEM), 1, 2}},
        {{IDX(CREATURE_SKELETON), 6, 10}, {IDX(CREATURE_ZOMBIE), 2, 4}, {IDX(CREATURE_MUMMY), 1, 2}}
    };
    i32 selected9[3];
    i32 index9;
    i32 minimum5;
    i32 maximum5;

    selected9[0] = RANDOM_HERO_STACK_SELECTED;
    selected9[1] =
        RANDOM_HERO_FIRST_STACK_CHANCE + (strongArmy ? RANDOM_HERO_FIRST_STACK_BONUS_CHANCE : 0)
        > Random(RANDOM_HERO_PERCENT_MIN, RANDOM_HERO_PERCENT_MAX);
    selected9[2] = Random(RANDOM_HERO_PERCENT_MIN, RANDOM_HERO_PERCENT_MAX)
                   < RANDOM_HERO_SECOND_STACK_CHANCE
                         + (strongArmy ? RANDOM_HERO_SECOND_STACK_BONUS_CHANCE : 0);
    if (!selected9[2])
        selected9[1] = RANDOM_HERO_STACK_SELECTED;

    for (index9 = 0; index9 < RANDOM_HERO_ARMY_SLOT_COUNT; index9++) {
        army2->m_creatureTypes[index9] = IDX(CREATURE_NONE);
        army2->m_creatureCounts[index9] = RANDOM_HERO_EMPTY_COUNT;
    }

    for (index9 = 0; index9 < RANDOM_HERO_ARMY_SELECTION_COUNT; index9++) {
        if (selected9[index9]) {
            army2->m_creatureTypes[armySlot7] =
                static_cast<i8>(armyTable7[m_heroRecs[heroId].m_cursorType][index9].creature);
            minimum5 = armyTable7[m_heroRecs[heroId].m_cursorType][index9].minimum
                       * RANDOM_HERO_COUNT_SCALE;
            maximum5 = armyTable7[m_heroRecs[heroId].m_cursorType][index9].maximum
                           * RANDOM_HERO_COUNT_SCALE
                       + RANDOM_HERO_COUNT_ROUNDING;
            if (strongArmy)
                minimum5 = ((minimum5 | 0) + maximum5) / 2;
            army2->m_creatureCounts[armySlot7] =
                static_cast<i16>(Random(minimum5, maximum5) / RANDOM_HERO_COUNT_SCALE);
            armySlot7++;
        }
    }
}

// @semantic
// The recovered 0x40 frame, switch bodies, CFG, and all 56 relocation semantics match.
// Retail's MAP_HEIGHT and MAP_WIDTH loop tests are each one byte shorter: mov global;
// cmp local; jge at +0x61/+0x7e, versus mov local; cmp global; jle here, making this body
// 0x748 bytes. Reversed comparisons and |0 bounds did not steer either test.
VA(0x0048041e, 0x746)
void game::ProcessRandomObjects(void) {
    i32 maxValue17;
    i32 x10;
    i32 mineIndex8;
    i32 y8;
    i32 artifactId18;
    i32 minValue7;
    mapCell* cell6;
    i32 randomType0;
    i32 randomObjectType3;

    giUABaseX = -1;
    giUABaseY = -1;
    giUARadius = 0;
    for (mineIndex8 = 0; mineIndex8 < 7; mineIndex8++)
        RandMineQty[mineIndex8] = 0;

    for (y8 = 0; MAP_HEIGHT > y8; y8++) {
        for (x10 = 0; x10 < MAP_WIDTH; x10++) {
            cell6 = WORLDMAP->GetCell(x10, y8);
            switch (cell6->m_triggerType) {
                case IDX(MAP_TRIGGER_RANDOM_ULTIMATE_ARTIFACT):
                    giUABaseX = static_cast<i16>(x10);
                    giUABaseY = static_cast<i16>(y8);
                    giUARadius = static_cast<i16>(cell6->m_objectMetadata);
                    cell6->m_triggerType = 0;
                    cell6->m_objectTileset = 0;
                    cell6->m_objectIndex = -1;
                    break;
                case IDX(MAP_TRIGGER_RANDOM_TOWN):
                    RandomizeTown(x10, y8, 0);
                    break;
                case IDX(MAP_TRIGGER_RANDOM_CASTLE):
                    RandomizeTown(x10, y8, 1);
                    break;
                case IDX(MAP_TRIGGER_RANDOM_MONSTER):
                    minValue7 = 80;
                    maxValue17 = 2000;
                    goto randomMonster;
                case IDX(MAP_TRIGGER_RANDOM_MONSTER_LEVEL_1):
                    minValue7 = 0;
                    maxValue17 = 400;
                    goto randomMonster;
                case IDX(MAP_TRIGGER_RANDOM_MONSTER_LEVEL_2):
                    minValue7 = 400;
                    maxValue17 = 1000;
                    goto randomMonster;
                case IDX(MAP_TRIGGER_RANDOM_MONSTER_LEVEL_3):
                    minValue7 = 1000;
                    maxValue17 = 2500;
                    goto randomMonster;
                case IDX(MAP_TRIGGER_RANDOM_MONSTER_LEVEL_4):
                    minValue7 = 2500;
                    maxValue17 = 100000;
                    goto randomMonster;
                randomMonster:
                    if (cell6->m_objectTileset == IDX(TILESET_MONS32)
                        && cell6->m_objectIndex >= RANDOM_MONSTER_SPRITE_FIRST
                        && cell6->m_objectIndex <= RANDOM_MONSTER_SPRITE_LAST) {
                        randomObjectType3 = cell6->m_objectIndex + RANDOM_MONSTER_SPRITE_TO_TRIGGER;
                        switch (randomObjectType3) {
                            case IDX(MAP_TRIGGER_RANDOM_MONSTER_LEVEL_1):
                                minValue7 = 0;
                                maxValue17 = 400;
                                goto monsterBoundsReady;
                            case IDX(MAP_TRIGGER_RANDOM_MONSTER_LEVEL_2):
                                minValue7 = 400;
                                maxValue17 = 1000;
                                goto monsterBoundsReady;
                            case IDX(MAP_TRIGGER_RANDOM_MONSTER_LEVEL_3):
                                minValue7 = 1000;
                                maxValue17 = 2500;
                                goto monsterBoundsReady;
                            case IDX(MAP_TRIGGER_RANDOM_MONSTER_LEVEL_4):
                                minValue7 = 2500;
                                maxValue17 = 100000;
                                goto monsterBoundsReady;
                        }
                    }
                monsterBoundsReady:
                    cell6->m_triggerType = IDX(MAP_TRIGGER_MONSTER);
                    cell6->m_objectIndex = static_cast<u8>(Random(0, 65));
                    while (gMonsterDatabase[cell6->m_objectIndex].randomValue <= minValue7
                           || gMonsterDatabase[cell6->m_objectIndex].randomValue >= maxValue17)
                        cell6->m_objectIndex = static_cast<u8>(Random(0, 65));
                    break;
                case IDX(MAP_TRIGGER_RANDOM_RESOURCE):
                    cell6->m_triggerType = IDX(MAP_TRIGGER_RESOURCE);
                    randomType0 = Random(0, 6);
                    ConvertObject(
                        x10 - 1,
                        y8,
                        x10 - 1,
                        y8,
                        IDX(TILESET_OBJNRSRC),
                        16,
                        16,
                        IDX(TILESET_OBJNRSRC),
                        randomType0 * 2,
                        -1,
                        -1
                    );
                    ConvertObject(
                        x10,
                        y8,
                        x10,
                        y8,
                        IDX(TILESET_OBJNRSRC),
                        17,
                        17,
                        IDX(TILESET_OBJNRSRC),
                        randomType0 * 2 + 1,
                        -1,
                        -1
                    );
                    switch (randomType0) {
                        case 0:
                        case 2:
                            cell6->m_objectMetadata = Random(8, 16) | 0;
                            break;
                        case 6:
                            cell6->m_objectMetadata = Random(5, 10) | 0;
                            break;
                        default:
                            cell6->m_objectMetadata = Random(3, 7) | 0;
                            break;
                    }
                    break;
                case IDX(MAP_TRIGGER_RANDOM_ARTIFACT):
                    artifactId18 = GetRandomArtifactId(14, 0);
                    cell6->m_triggerType = IDX(MAP_TRIGGER_ARTIFACT);
                    ConvertObject(
                        x10 - 1,
                        y8,
                        x10 - 1,
                        y8,
                        11,
                        162,
                        162,
                        11,
                        artifactId18 * 2,
                        -1,
                        -1
                    );
                    ConvertObject(x10, y8, x10, y8, 11, 163, 163, 11, artifactId18 * 2 + 1, -1, -1);
                    break;
                case IDX(MAP_TRIGGER_RANDOM_ARTIFACT_LEVEL_1):
                    artifactId18 = GetRandomArtifactId(8, 0);
                    cell6->m_triggerType = IDX(MAP_TRIGGER_ARTIFACT);
                    ConvertObject(
                        x10 - 1,
                        y8,
                        x10 - 1,
                        y8,
                        11,
                        166,
                        166,
                        11,
                        artifactId18 * 2,
                        -1,
                        -1
                    );
                    ConvertObject(x10, y8, x10, y8, 11, 167, 167, 11, artifactId18 * 2 + 1, -1, -1);
                    break;
                case IDX(MAP_TRIGGER_RANDOM_ARTIFACT_LEVEL_2):
                    artifactId18 = GetRandomArtifactId(4, 0);
                    cell6->m_triggerType = IDX(MAP_TRIGGER_ARTIFACT);
                    ConvertObject(
                        x10 - 1,
                        y8,
                        x10 - 1,
                        y8,
                        11,
                        168,
                        168,
                        11,
                        artifactId18 * 2,
                        -1,
                        -1
                    );
                    ConvertObject(x10, y8, x10, y8, 11, 169, 169, 11, artifactId18 * 2 + 1, -1, -1);
                    break;
                case IDX(MAP_TRIGGER_RANDOM_ARTIFACT_LEVEL_3):
                    artifactId18 = GetRandomArtifactId(2, 0);
                    cell6->m_triggerType = IDX(MAP_TRIGGER_ARTIFACT);
                    ConvertObject(
                        x10 - 1,
                        y8,
                        x10 - 1,
                        y8,
                        11,
                        170,
                        170,
                        11,
                        artifactId18 * 2,
                        -1,
                        -1
                    );
                    ConvertObject(x10, y8, x10, y8, 11, 171, 171, 11, artifactId18 * 2 + 1, -1, -1);
                    break;
                case IDX(MAP_TRIGGER_RANDOM_MINE):
                    RandomizeMine(x10, y8);
                    break;
            }
        }
    }
}

// @semantic
// Logic and frame slots are byte-exact. The only residual is the TU-cumulative /Od
// polarity/load order of cutoff <= visibility: retail emits cmp cutoff,visibility; jg,
// while this partial TU emits the relationally equivalent cmp visibility,cutoff; jl.
// Relational swaps, negation, an empty else arm, |0 steering, and the fixed AST permuter
// do not alter it; the other loop/index order differences disappeared as header state landed.
VA(0x00480b64, 0x230)
void game::SetVisibility(i32 x, i32 y, i32 player, i32 radius) {
    i32 col;
    i32 cutoff;
    i32 row;
    u8 mask = static_cast<u8>(1 << player);
    i32 visibility;

    if (!gbHumanPlayer[player]) {
        if (giCurTurn > VISIBILITY_MIDDLE_TURN_LAST) {
            radius += VISIBILITY_LATE_AI_BONUS;
        } else {
            if (giCurTurn > VISIBILITY_EARLY_TURN_LAST)
                radius += VISIBILITY_MIDDLE_AI_BONUS;
            else
                radius += VISIBILITY_EARLY_AI_BONUS;
        }
    }

    if (radius >= VISIBILITY_SMALL_RADIUS_LIMIT)
        cutoff = VISIBILITY_LARGE_RADIUS_THRESHOLD;
    else
        cutoff = VISIBILITY_SMALL_RADIUS_THRESHOLD;

    if (radius >= VISIBILITY_RADIAL_RADIUS_LIMIT) {
        for (row = 0; row < MAP_HEIGHT; row++) {
            for (col = 0; col < MAP_WIDTH; col++) {
                i32 distance = static_cast<i32>(
                    sqrt(static_cast<double>((x - col) * (x - col) + (y - row) * (y - row)))
                );
                if (distance < radius) {
                    mapExtra[(MAP_WIDTH | 0) * row + col] |= mask;
                }
            }
        }
    } else {
        for (row = y - radius; row <= y + radius; row++) {
            for (col = x - radius; col <= x + radius; col++) {
                visibility = radius - abs(y - row) + radius - abs(x - col);
                if (visibility >= cutoff && col >= 0 && row >= 0 && col < MAP_WIDTH
                    && row < MAP_HEIGHT) {
                    mapExtra[(MAP_WIDTH | 0) * row + col] |= mask;
                }
            }
        }
    }
}

// @semantic
// Logic and frame slots are byte-exact; residual is three commutative operand-load swaps (the
// inner-loop test y<MAP_HEIGHT and the two y*MAP_WIDTH index multiplies load the OTHER
// operand into eax first). Not source-steerable (operand order / reversed compare /
// extra temp all tested with no effect). Revisit after later GAME/header reconstruction
// changes cumulative compiler state; do not retry those spellings before 95% total fuzzy.
VA(0x00480d94, 0xd8)
void game::MakeAllWaterVisible(i32 player) {
    char mask = static_cast<char>(1 << player);
    i32 x;
    i32 y;
    for (x = 0; x < MAP_WIDTH; x++) {
        for (y = 0; y < MAP_HEIGHT; y++) {
            if (giGroundToTerrain[WORLDMAP->Row(y)[x].m_terrainImageIndex] == 0)
                mapExtra[y * MAP_WIDTH + x] |= mask;
        }
    }
}

VA(0x00480e6c, 0xfc)
void game::GiveArmy(armyGroup* group, i32 type, i32 count, i32 slot) {
    i32 tmp;
    i32 i;
    if (slot >= 0) {
        i = slot;
        group->m_creatureTypes[i] = static_cast<i8>(type);
        group->m_creatureCounts[i] = 0;
    } else {
        for (i = 0; i < 5; i++) {
            if (group->m_creatureTypes[i] == type)
                break;
        }
        if (i >= 5) {
            for (i = 0; i < 5; i++) {
                if (group->m_creatureTypes[i] < 0) {
                    group->m_creatureCounts[i] = 0;
                    break;
                }
            }
        }
        if (i >= 5)
            return;
    }
    group->m_creatureTypes[i] = static_cast<i8>(type);
    group->m_creatureCounts[i] += count;
}

// @semantic
// The 0x91 frame/CFG and gMonsterDatabase relocation match. Only the two ModRM bytes at
// +0x41/+0x44 differ because retail loads i before group for the creature-type subscript.
// Direct, commuted i[array], and earlier-expression steering did not hold in canonical TU state.
VA(0x00480f68, 0x91)
i32 game::ExperienceValueOfStack(armyGroup* group, hero* h) {
    i32 exp = 0;
    i32 i;
    for (i = 0; i < 5; i++) {
        if (group->m_quantities[i] > 0) {
            exp += gMonsterDatabase[group->m_creatureTypes[i]].hitPoints * group->m_quantities[i];
        }
    }
    if (h != 0)
        exp += EXPERIENCE_HERO_PRESENCE_BONUS;
    return exp;
}

VA(0x00480ff9, 0x126)
i32 game::GetLuck(hero* h, class army*, town* castle) {
    i32 luck;
    if (h == 0)
        return LUCK_NEUTRAL;
    luck = LUCK_NEUTRAL;
    if (h->HasArtifact(ARTIFACT_RABBIT_FOOT))
        luck++;
    if (h->HasArtifact(ARTIFACT_GOLDEN_HORSESHOE))
        luck++;
    if (h->HasArtifact(ARTIFACT_GAMBLERS_COIN))
        luck++;
    if (h->HasArtifact(ARTIFACT_FOUR_LEAF_CLOVER))
        luck++;
    if (h->HasArtifact(ARTIFACT_MASTHEAD) && HAS(h->m_eventFlags, HERO_EVENT_EMBARKED)) {
        luck++;
    }
    luck += h->m_luck;
    luck += h->m_secondarySkills[IDX(HERO_SKILL_LUCK)];
    if (luck < LUCK_MINIMUM)
        luck = LUCK_MINIMUM;
    if (luck > LUCK_MAXIMUM)
        luck = LUCK_MAXIMUM;
    if (h->HasArtifact(ARTIFACT_BATTLE_GARB))
        luck = LUCK_MAXIMUM;
    if (castle != 0 && castle->m_type == IDX(FACTION_SORCERESS)
        && (castle->m_buildings & IDX(TOWN_BUILDING_RAINBOW))) {
        luck += LUCK_RAINBOW_BONUS;
    }
    return luck;
}

// @semantic
// Logic + frame slots byte-exact (col/row/mask + nested x/y land on retail's -0x4..-0x14
// via the {} block); residual is the same TU-cumulative /Od eval-order parity as
// MakeAllWaterVisible - the inner-loop test and the y*MAP_WIDTH multiplies load the other
// operand first. Aligns when GAME is fuller.
VA(0x0048111f, 0xf1)
void game::SetupAdjacentMons(void) {
    i32 col;
    i32 row;
    u8 mask = MAP_EXTRA_ADJACENT_CLEAR_MASK;
    {
        i32 x;
        i32 y;
        for (x = 0; x < MAP_WIDTH; x++) {
            for (y = 0; y < MAP_HEIGHT; y++) {
                if (gpAdvManager->FindAdjacentMonster(x, y, &col, &row, -1, -1))
                    mapExtra[y * MAP_WIDTH + x] |= IDX(MAP_EXTRA_ADJACENT_MONSTER);
                else
                    mapExtra[y * MAP_WIDTH + x] &= mask;
            }
        }
    }
}

VA(0x00481210, 0x61)
void game::CancelComputerScreen(void) {
    TurnOffAIMusic();
    bShowIt = 1;
    i32 i;
    for (i = 1; i <= 6; i++) {
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET,
            WIDGET_COMMAND_CLEAR_FLAGS,
            i,
            WIDGET_FLAG_UPDATE | WIDGET_FLAG_DIMMED
        );
    }
}

VA(0x00481271, 0xed)
void game::ShowComputerScreen(void) {
    if (gConfig.blackoutComputer) {
        i32 saved = gbThisNetHumanPlayer[giCurPlayer];
        gbThisNetHumanPlayer[giCurPlayer] = 1;
        i32 i;
        for (i = 1; i <= 6; i++)
            gpWindowManager->BroadcastMessage(
                MESSAGE_WIDGET,
                WIDGET_COMMAND_SET_FLAGS,
                i,
                WIDGET_FLAG_UPDATE | WIDGET_FLAG_DIMMED
            );
        gbAllBlack = true;
        gpAdvManager->CompleteDraw(1);
        gpAdvManager->UpdateHeroLocators(1, 1);
        gpAdvManager->UpdateTownLocators(1, 1);
        gpAdvManager->UpdBottomView(1, 1, 1);
        gpAdvManager->UpdateScreen(0, 1);
        gbAllBlack = false;
        gbThisNetHumanPlayer[giCurPlayer] = static_cast<i8>(saved);
    }
    ShowHeroesLogo();
}

VA(0x0048135e, 0xa0)
void game::ShowHeroesLogo(void) {
    if (gpAdvManager->m_openState == 0) {
        gpAdvManager->m_openState = 1;
        icon* theIcon = gpResourceManager->GetIcon("herologo.icn");
        IconToBitmap(theIcon, gpWindowManager->m_screen, 480, 16, 0, 0, 0, 0, 640, 480, 0);
        gpWindowManager->UpdateScreenRegion(480, 16, 144, 144);
        gpResourceManager->Dispose(static_cast<resource*>(theIcon));
    }
}

VA(0x004813fe, 0x143)
void game::WaitForPlayer(char* text, i32 player) {
    if (gbBlackoutPlayer && giNumHumanPlayers > 1 && !gbRemoteOn) {
        gpMouseManager->SetPointer(0);
        gbAllBlack = true;
        giBottomViewOverrideEndTime = KBTickCount() + WAIT_BOTTOM_VIEW_TIMEOUT;
        giBottomViewOverride = gbThisNetHumanPlayer[giCurPlayer] != 0;
        gpSoundManager->m_samplesReady = 1;
        gpSoundManager->SwitchAmbientMusic(WAIT_AMBIENT_MUSIC);
        gpAdvManager->CompleteDraw(1);
        gpAdvManager->UpdateHeroLocators(1, 1);
        gpAdvManager->UpdateTownLocators(1, 1);
        gpAdvManager->UpdateScreen(0, 1);
        ShowHeroesLogo();
        gbAllBlack = false;
        NormalDialog(
            text,
            1,
            -1,
            -1,
            WAIT_DIALOG_TYPE,
            static_cast<i8>(gpGame->m_players[player].m_color),
            -1,
            0,
            -1,
            0
        );
        gpSoundManager->SwitchAmbientMusic(-1);
    }
}

// @early-stop
// Computation byte-exact; residual is 2 inline-accessor jmp$+0 brackets the /Ob1
// expander places leading (after the ternary test) where retail places them trailing
// (after the Extra() body) - the documented /Od /Ob1 block-boundary artifact, identical
// in kind to EDITOR/mapcell GetNewCellExtra*. See docs/patterns/inline-accessors.md.
VA(0x00481541, 0x104)
i32 game::HasLateOverlay(i32 col, i32 row) {
    mapCell* cell = WORLDMAP->Row(row) + col;
    if (cell->m_drawOverlayOnTop)
        return 1;
    mapCellExtra* extra = cell->m_extraIndex ? WORLDMAP->Extra(cell->m_extraIndex) : 0;
    while (extra) {
        if (extra->drawOverlayOnTop)
            return 1;
        extra = extra->nextIndex ? WORLDMAP->Extra(extra->nextIndex) : 0;
    }
    return 0;
}

// @early-stop
// Instruction stream, frame slots, and traversal are byte-exact after excluding two
// five-byte Extra() inline continuation jumps. Both jumps exist on each side, but C2
// places ours trailing and retail's leading; there are no relocations in either function.
VA(0x00481645, 0x120)
void game::ConvertFlagToLateOverlay(i32 col, i32 row) {
    mapCell* cell = WORLDMAP->Row(row) + col;
    if (cell->m_overlayTileset == MAP_TILESET_FLAG)
        cell->m_drawOverlayOnTop = 1;
    mapCellExtra* extra = cell->m_extraIndex ? WORLDMAP->Extra(cell->m_extraIndex) : 0;
    while (extra) {
        if (extra->overlayTileset == MAP_TILESET_FLAG)
            extra->drawOverlayOnTop = 1;
        extra = extra->nextIndex ? WORLDMAP->Extra(extra->nextIndex) : 0;
    }
}

// @early-stop
// Instruction stream, frame slots, and traversal are byte-exact after excluding two
// five-byte Extra() inline continuation jumps. Both jumps exist on each side, but C2
// places ours trailing and retail's leading; there are no relocations in either function.
VA(0x00481765, 0x13b)
i32 game::HasObjectTilesetIndex(i32 col, i32 row, i32 tileset, i32 index) {
    mapCell* cell = WORLDMAP->Row(row) + col;
    if (cell->m_objectTileset == tileset && cell->m_objectIndex == index)
        return 1;
    mapCellExtra* extra = cell->m_extraIndex ? WORLDMAP->Extra(cell->m_extraIndex) : 0;
    while (extra) {
        if (extra->objectTileset == tileset && extra->objectIndex == index)
            return 1;
        extra = extra->nextIndex ? WORLDMAP->Extra(extra->nextIndex) : 0;
    }
    return 0;
}

// @early-stop
// Twin of HasLateOverlay: computation byte-exact; only the 2 inline-accessor jmp$+0
// brackets (Extra()) are placed leading vs retail's trailing - the same /Od /Ob1
// block-boundary artifact. See docs/patterns/inline-accessors.md.
VA(0x004818a0, 0x112)
void game::ConvertAllToLateOverlay(i32 col, i32 row) {
    mapCell* cell = WORLDMAP->Row(row) + col;
    if (cell->m_overlayIndex != IDX(MAPCELL_SPRITE_NONE))
        cell->m_drawOverlayOnTop = 1;
    mapCellExtra* extra = cell->m_extraIndex ? WORLDMAP->Extra(cell->m_extraIndex) : 0;
    while (extra) {
        if (extra->overlayIndex != IDX(MAPCELL_SPRITE_NONE))
            extra->drawOverlayOnTop = 1;
        extra = extra->nextIndex ? WORLDMAP->Extra(extra->nextIndex) : 0;
    }
}

// @semantic
// Logic is complete. The residual is one coupled TU-cumulative /Od lowering choice:
// retail evaluates the packed m_objectMetadata lvalue first and reserves two hidden temporary words;
// this partial TU evaluates townId first and omits them. The same parity flips the three
// inner MAP_WIDTH comparisons. Direct packed-word spellings, |0 steering, relational
// swaps, and the fixed AST permuter did not reproduce the retail field-first lowering;
// anonymous source padding would conceal rather than reconstruct this compiler state.
VA(0x004819b2, 0x295)
void game::ProcessMapExtra(void) {
    i32 row;
    i32 col;
    i32 townId;
    mapCell* cell;

    for (row = 0; row < MAP_HEIGHT; row++) {
        for (col = 0; MAP_WIDTH > col; col++) {
            cell = WORLDMAP->Row(row) + col;
            switch (cell->m_triggerType) {
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE:
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_TOWN:
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_CASTLE:
                    townId = GetTownId(col, row);
                    m_castleRecs[townId].m_extraIndex = cell->m_objectMetadata;
                    cell->m_objectMetadata = townId;
                    break;
            }
        }
    }

    for (row = 0; row < MAP_HEIGHT; row++) {
        for (col = 0; MAP_WIDTH > col; col++) {
            cell = WORLDMAP->Row(row) + col;
            if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE) && row > 0
                && HasLateOverlay(col, row - 1)) {
                ConvertFlagToLateOverlay(col, row);
            }
            if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ALCHEMIST_LAB)) {
                if (row > 0)
                    ConvertFlagToLateOverlay(col, row - 1);
                if (row > 1)
                    ConvertFlagToLateOverlay(col, row - 2);
            }
        }
    }

    for (row = 0; row < MAP_HEIGHT; row++) {
        for (col = 0; MAP_WIDTH > col; col++) {
            if (HasLateOverlay(col, row))
                ConvertAllToLateOverlay(col, row);
        }
    }
}

// @early-stop
// reloc-masked: identical frame/instruction stream and all 39 relocation sites align.
// Retail folds gDwellingType, gMonsterDatabase, gSpellLimits, and gsSpellInfo to
// field/addend aliases; their effective addresses are identical.
VA(0x00481c47, 0x900)
void game::SetupTowns(void) {
    DATA(0x004f756c) static i16 setupTownsSourceLineBase = 0x17f9;
    char defaultDwellingRoll[12];
    i8 usedSpells[IDX(SPELL_COUNT)];
    i32 spellsPerLevel[5];
    i32 townIndex;
    i32 slot;
    i32 owner;
    i32 building;
    i32 spellLevel;
    i32 spellSlot;
    SpellType spell;
    i32 roll;
    i32 attempts;
    i32 spellValue;
    i32 combatSpells;
    i32 dwellingCount;
    u32 extraIndex;
    town* castle;
    mapTownExtra* extra;

    for (townIndex = 0; townIndex < GAME_TOWN_COUNT; townIndex++) {
        if (!m_castleRecs[townIndex].m_onMap)
            continue;
        castle = GetTown(townIndex);

        extraIndex = castle->m_extraIndex;
        extra = reinterpret_cast<mapTownExtra*>(ppMapExtra[extraIndex]);
        if (extra->color == -1)
            owner = -1;
        else
            owner = gcColorToPlayerPos[extra->color];
        ClaimTown(townIndex, owner, 0);
        castle->m_originalOwner = static_cast<i8>(owner);

        if (extra->hasCustomArmy) {
            for (slot = 0; slot < 5; slot++) {
                castle->m_army.m_troopCounts[slot] = extra->troopCounts[slot];
                if (static_cast<i16>(castle->m_army.m_troopCounts[slot]) > 0)
                    castle->m_army.m_troopTypes[slot] = extra->troopTypes[slot];
                else
                    castle->m_army.m_troopTypes[slot] = -1;
            }
        } else {
            for (slot = 0; slot < 5; slot++) {
                castle->m_army.m_troopCounts[slot] = 0;
                castle->m_army.m_troopTypes[slot] = -1;
            }
            GiveTroopsToNeutralTown(townIndex);
            GiveTroopsToNeutralTown(townIndex);
            GiveTroopsToNeutralTown(townIndex);
            GiveTroopsToNeutralTown(townIndex);
        }

        if (extra->hasCustomBuildings) {
            castle->m_buildings =
                (gTownEligibleBuildMask[castle->m_type] & extra->buildings)
                | (castle->m_buildings & (IDX(TOWN_BUILDING_CASTLE) | IDX(TOWN_BUILDING_TENT)));
            castle->m_buildState = extra->mageGuildLevel;
        } else {
            defaultDwellingRoll[0] = 1;
            defaultDwellingRoll[1] = 1;
            defaultDwellingRoll[2] = 1;
            defaultDwellingRoll[3] = 2;
            defaultDwellingRoll[4] = 1;
            defaultDwellingRoll[5] = 1;
            defaultDwellingRoll[6] = 1;
            defaultDwellingRoll[7] = 2;
            defaultDwellingRoll[8] = 1;
            defaultDwellingRoll[9] = 2;
            dwellingCount = defaultDwellingRoll[Random(0, 99) / 10];
            castle->m_buildings |= IDX(TOWN_BUILDING_DWELLING_1);
            if (!gbHumanPlayer[castle->m_owner] && dwellingCount == 1 && Random(1, 10) < 4)
                dwellingCount++;
            if (--dwellingCount != 0)
                castle->m_buildings |= IDX(TOWN_BUILDING_DWELLING_2);
            dwellingCount--;
            castle->m_buildState = 0;
        }

        for (building = 25; building <= 30; building++) {
            if (castle->m_buildings & (1 << building)) {
                if (building == 30)
                    castle->m_buildings &= -553648129;
                else
                    castle->m_buildings &= -1 - (1 << (building - 5));
            }
        }
        for (building = 19; building <= 30; building++) {
            if (castle->m_buildings & (1 << building)) {
                castle->m_garrison[building - 19] =
                    gMonsterDatabase[gDwellingType[castle->m_type][building - 19]].growth;
            }
        }
        if (castle->m_buildings & 1) {
            for (slot = 1; slot <= castle->m_buildState; slot++) {
                castle->m_spellCounts[slot] = gSpellLimits[slot - 1];
                if (castle->m_type == 4 && (castle->m_buildings & BIT(BUILDING_SLOT_SPECIAL)))
                    castle->m_spellCounts[slot]++;
            }
        }
        if (extra->hasShrine)
            castle->m_buildings |= IDX(TOWN_BUILDING_CAPTAIN_QUARTERS);
        castle->m_mayNotUpgradeToCastle = extra->unknown28;
        strcpy(castle->m_name, extra->name);

        memset(usedSpells, 0, SPELL_COUNT);
        for (spellLevel = 0; spellLevel < TOWN_MAGE_GUILD_LEVEL_COUNT; spellLevel++) {
            spellsPerLevel[spellLevel] = 0;
            for (spellSlot = 0; spellSlot < TOWN_MAGE_GUILD_SPELLS_PER_LEVEL; spellSlot++)
                castle->m_spellSlots[spellLevel * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + spellSlot] =
                    IDX(SPELL_NONE);
        }

        if (castle->m_type == 5 && castle->m_owner != -1 && !gbHumanPlayer[castle->m_owner]) {
            if (Random(0, 100) < 50)
                spell = SPELL_DEATH_RIPPLE;
            else
                spell = SPELL_DEATH_WAVE;
            spellLevel = gsSpellInfo[IDX(spell)].level - 1;
            castle->m_spells[spellLevel][spellsPerLevel[spellLevel]] = static_cast<i8>(spell);
            spellsPerLevel[spellLevel]++;
        }

        roll = Random(0, 100);
        if (roll < 25)
            spell = SPELL_DISPEL;
        else if (roll < 50)
            spell = SPELL_MASS_DISPEL;
        else if (roll < 75)
            spell = SPELL_ANTI_MAGIC;
        else
            spell = SPELL_CURE;
        spellLevel = gsSpellInfo[IDX(spell)].level - 1;
        castle->m_spells[spellLevel][spellsPerLevel[spellLevel]] = static_cast<i8>(spell);
        spellsPerLevel[spellLevel]++;

        roll = Random(0, 100);
        if (roll < 20)
            spell = SPELL_MAGIC_ARROW;
        else if (roll < 40)
            spell = SPELL_LIGHTNING_BOLT;
        else if (roll < 60)
            spell = SPELL_FIREBALL;
        else if (roll < 80)
            spell = SPELL_COLD_RAY;
        else
            spell = SPELL_COLD_RING;
        spellLevel = gsSpellInfo[IDX(spell)].level - 1;
        castle->m_spells[spellLevel][spellsPerLevel[spellLevel]] = static_cast<i8>(spell);
        spellsPerLevel[spellLevel]++;

        for (spellLevel = 0; spellLevel < TOWN_MAGE_GUILD_LEVEL_COUNT; spellLevel++) {
            combatSpells = 0;
            for (spellSlot = 0; spellSlot < TOWN_MAGE_GUILD_SPELLS_PER_LEVEL; spellSlot++) {
                if (castle->m_spellSlots[spellLevel * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + spellSlot]
                    != IDX(SPELL_NONE)) {
                    usedSpells[castle->m_spellSlots
                                   [spellLevel * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + spellSlot]] = 1;
                } else {
                    attempts = 0;
                    do {
                        spell = Random(SPELL_FIREBALL, SPELL_SET_WATER_GUARDIAN);
                        while (gsSpellInfo[IDX(spell)].level - 1 != spellLevel)
                            spell = Random(SPELL_FIREBALL, SPELL_SET_WATER_GUARDIAN);
                        if (castle->m_owner != -1 && !gbHumanPlayer[castle->m_owner])
                            spellValue = (gsSpellInfo[IDX(spell)].attributes & 1 ? 4 : 1)
                                             * gsSpellInfo[IDX(spell)].aiValue
                                         + 50;
                        else
                            spellValue = 1500;
                        if (spell == SPELL_DIMENSION_DOOR)
                            spellValue = 1500;
                    } while ((combatSpells == 1 && (gsSpellInfo[IDX(spell)].attributes & 4))
                             || gsSpellInfo[IDX(spell)].raceChance[castle->m_type] < Random(0, 10)
                             || attempts++ > 500 || usedSpells[IDX(spell)]
                             || spellValue < Random(1, 1500));
                    if (gsSpellInfo[IDX(spell)].attributes & 4)
                        combatSpells++;
                    castle
                        ->m_spellSlots[spellLevel * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + spellSlot] =
                        static_cast<i8>(spell);
                    usedSpells[IDX(spell)] = 1;
                }
            }
        }
        H2_FREE(ppMapExtra[extraIndex], 6375);
        ppMapExtra[extraIndex] = 0;
    }
}

// @semantic
// The recovered mapHeroExtra offsets, 0x7c frame, CFG, and all 23 relocation semantics
// match. Retail's MAP_HEIGHT/MAP_WIDTH tests load each global first at +0x44/+0x61;
// this TU loads each local first, making the body 0x776 versus retail's 0x774. Reversed
// comparisons and |0 bounds did not steer them; the remaining $SG spelling is compiler-local.
VA(0x00482547, 0x774)
void game::ProcessOnMapHeroes(void) {
    DATA(0x004f7598) static i16 processOnMapHeroesSourceLineBase = 0x18ef;
    u32 extraIndex0;
    i32 pass19;
    i8 usedHeroes4[GAME_HERO_COUNT];
    i8 isJail6;
    hero* mapHero0;
    mapCell* cell5;
    mapHeroExtra* extra0;
    mapCell* townCell1;
    i32 heroId1;
    i32 armySlot0;
    i32 mapY15;
    i32 artifactSlot10;
    i32 mapX0;
    i32 townId4;
    i32 heroClass6;
    i32 owner1;
    town* occupiedTown4;

    memset(usedHeroes4, 0, GAME_HERO_COUNT);
    for (pass19 = 0; pass19 < 3; pass19++) {
        for (mapY15 = 0; mapY15 < MAP_HEIGHT; mapY15++) {
            for (mapX0 = 0; mapX0 < MAP_WIDTH; mapX0++) {
                cell5 = &WORLDMAP->Row(mapY15)[mapX0];
                if ((cell5->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_HERO
                    || cell5->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_JAIL)) {

                    if ((cell5->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_JAIL)
                        isJail6 = 1;
                    else
                        isJail6 = 0;
                    extraIndex0 = cell5->m_objectMetadata;
                    extra0 = reinterpret_cast<mapHeroExtra*>(ppMapExtra[extraIndex0]);

                    if (pass19 == 0) {
                        if (extra0->hasCustomHero && extra0->heroId < GAME_HERO_COUNT
                            && !usedHeroes4[extra0->heroId]) {
                            usedHeroes4[extra0->heroId] = 1;
                            extra0->hasAssignedHero = 1;
                        } else {
                            extra0->hasAssignedHero = 0;
                        }
                        if (isJail6) {
                            extra0->owner = -1;
                        } else {
                            extra0->owner = static_cast<i8>(cell5->m_objectIndex / 7);
                            owner1 = gcColorToPlayerPos[extra0->owner];
                            extra0->owner = static_cast<i8>(owner1);
                        }
                    }

                    if (pass19 == 1) {
                        if (isJail6) {
                            heroClass6 = extra0->heroClass;
                        } else {
                            heroClass6 = cell5->m_objectIndex % 7;
                            if (heroClass6 == 6) {
                                heroClass6 = m_setupPlayerRace
                                    [gcColorToSetupPos[gpGame->m_players[extra0->owner].m_color]];
                            }
                        }

                        if (extra0->hasAssignedHero) {
                            mapHero0 = GetHero(extra0->heroId);
                            mapHero0->m_cursorType = static_cast<u8>(heroClass6);
                        } else {
                            heroId1 = RandomScan(usedHeroes4, heroClass6 * 9, 9, 1000, 0);
                            if (heroId1 == -1) {
                                heroId1 = RandomScan(usedHeroes4, 0, GAME_HERO_COUNT, 10000, 0);
                                heroClass6 = heroId1 / 9;
                            }
                            usedHeroes4[heroId1] = 1;
                            mapHero0 = GetHero(heroId1);
                            mapHero0->m_cursorType = static_cast<u8>(heroClass6);
                            if (extra0->hasCustomHero && extra0->heroId >= GAME_HERO_COUNT)
                                mapHero0->m_portrait = extra0->heroId;
                            extra0->heroId = static_cast<i8>(heroId1);
                        }
                    }

                    if (pass19 == 2) {
                        mapHero0 = GetHero(extra0->heroId);
                        if (!isJail6 && extra0->hasPatrol) {
                            mapHero0->m_patrolX = static_cast<i8>(mapX0);
                            mapHero0->m_patrolY = static_cast<i8>(mapY15);
                            mapHero0->m_patrolRadius = extra0->patrolRadius;
                        }
                        if (extra0->hasCustomArmy) {
                            for (armySlot0 = 0; armySlot0 < 5; armySlot0++) {
                                mapHero0->m_army.m_troopCounts[armySlot0] =
                                    extra0->troopCounts[armySlot0];
                                if (static_cast<i16>(mapHero0->m_army.m_troopCounts[armySlot0]) > 0)
                                    mapHero0->m_army.m_troopTypes[armySlot0] =
                                        extra0->troopTypes[armySlot0];
                                else
                                    mapHero0->m_army.m_troopTypes[armySlot0] = -1;
                            }
                        }
                        for (artifactSlot10 = 0; artifactSlot10 < 3; artifactSlot10++) {
                            if (extra0->artifacts[artifactSlot10] >= 0)
                                GiveArtifact(mapHero0, extra0->artifacts[artifactSlot10], 1, -1);
                        }
                        if (extra0->hasCustomName)
                            strcpy(mapHero0->m_name, extra0->name);
                        mapHero0->m_experience = 0;
                        gpAdvManager->GiveExperience(mapHero0, extra0->experience, 1);
                        mapHero0->CheckLevel();
                        mapHero0->m_x = mapX0;
                        mapHero0->m_y = mapY15;

                        if (isJail6) {
                            mapHero0->m_owner = -1;
                            m_availableHeroes[extra0->heroId] = HERO_AVAILABILITY_JAILED;
                        } else {
                            mapHero0->m_owner = extra0->owner;
                            m_availableHeroes[extra0->heroId] = mapHero0->m_owner;
                            m_players[mapHero0->m_owner]
                                .m_heroIds[m_players[mapHero0->m_owner].m_heroCount] =
                                mapHero0->m_id;
                            m_players[mapHero0->m_owner].m_heroCount++;
                        }

                        if (!isJail6 && mapY15 > 0) {
                            townCell1 = &WORLDMAP->Row(mapY15 - 1)[mapX0];
                            if (townCell1->m_triggerType
                                == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)) {
                                mapHero0->m_patrolY--;
                                mapHero0->m_y--;
                                townId4 = GetTownId(mapX0, mapY15 - 1);
                                occupiedTown4 = GetTown(townId4);
                                occupiedTown4->m_occupyingHeroId = mapHero0->m_id;
                            }
                        }

                        if (isJail6) {
                            cell5->m_objectMetadata = extra0->heroId;
                        } else {
                            cell5->m_objectTileset = 0;
                            cell5->m_objectIndex = IDX(MAPCELL_SPRITE_NONE);
                            cell5->m_objectMetadata = 0;
                            cell5->m_triggerType = 0;
                        }

                        if (extra0->hasCustomSkills) {
                            mapHero0->m_secondarySkillCount = 0;
                            for (artifactSlot10 = 0; artifactSlot10 < 14; artifactSlot10++) {
                                mapHero0->m_secondarySkills[artifactSlot10] = 0;
                                mapHero0->m_secondarySkillOrder[artifactSlot10] = 0;
                            }
                            for (artifactSlot10 = 0; artifactSlot10 < 8; artifactSlot10++) {
                                if (extra0->skillTypes[artifactSlot10] != -1) {
                                    mapHero0->GiveSS(
                                        extra0->skillTypes[artifactSlot10],
                                        extra0->skillLevels[artifactSlot10]
                                    );
                                }
                            }
                        }
                        if (!isJail6) {
                            SetVisibility(
                                mapHero0->m_x,
                                mapHero0->m_y,
                                mapHero0->m_owner,
                                giVisRange[mapHero0->m_secondarySkills[3]]
                            );
                        }
                        H2_FREE(ppMapExtra[extraIndex0], 6604);
                        ppMapExtra[extraIndex0] = 0;
                    }
                }
            }
        }
    }
}

// @semantic
// Frame layout and all seven relocations are exact. The 0x8-byte size residual is one
// five-byte inlined hero-bounds continuation plus three bytes of equivalent commutative
// packed-record index arithmetic; every ownership, repair, and army check realigns.
VA(0x00482cbb, 0x55e)
void game::CheckHeroConsistency(void) {
    hero* mapHero3;
    mapCell* cell1;
    i32 x11;
    i32 y8;
    i32 player3;
    i32 slot1;
    i32 total26 = 0;
    i32 consistent13;
    town* occupiedTown9;

    for (player3 = 0; player3 < m_playerCount; player3++) {
        if (m_playerDead[player3] != 0)
            continue;
        total26 += m_players[player3].m_heroCount;
        for (slot1 = 0; slot1 < m_players[player3].m_heroCount; slot1++) {
            if (m_heroRecs[m_players[player3].m_heroIds[slot1]].m_owner != player3)
                consistent13 = 0;
        }
    }

    for (player3 = 0; player3 < m_playerCount; player3++) {
        if (m_playerDead[player3] == 0) {
            for (slot1 = 0; slot1 < 2; slot1++) {
                if ((m_availableHeroes[m_players[player3].m_availableHeroIds[slot1]] >= 0
                     && m_availableHeroes[m_players[player3].m_availableHeroIds[slot1]] <= 5)
                    || (total26 < 40
                        && m_availableHeroes[m_players[player3].m_availableHeroIds[slot1]] == -1)) {
                    m_players[player3].m_availableHeroIds[slot1] =
                        static_cast<i8>(GetNewHeroId(player3, -1, 0));
                    m_availableHeroes[m_players[player3].m_availableHeroIds[slot1]] = 64;
                }
            }
        }
    }

    for (x11 = 0; x11 < MAP_WIDTH; x11++) {
        for (y8 = 0; y8 < MAP_HEIGHT; y8++) {
            cell1 = gpAdvManager->GetCell(x11, y8);
            if (cell1->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERMAID)) {
                if (cell1->m_objectMetadata >= 0 && cell1->m_objectMetadata < 54) {
                    mapHero3 = &m_heroRecs[cell1->m_objectMetadata];
                    if (mapHero3->m_x != x11 || mapHero3->m_y != y8) {
                        cell1->m_triggerType = 0;
                        cell1->m_objectMetadata = 0;
                    }
                    if (mapHero3->m_owner < 0 || mapHero3->m_owner >= 6) {
                        if (mapHero3->m_locationType
                            == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)) {
                            occupiedTown9 = GetCastle(mapHero3->m_occupiedTown);
                            occupiedTown9->m_occupyingHeroId = -1;
                        }
                        if (mapHero3->m_x == x11 && mapHero3->m_y == y8) {
                            RestoreCell(
                                mapHero3->m_x,
                                mapHero3->m_y,
                                mapHero3->m_locationType,
                                mapHero3->m_occupiedTown,
                                0,
                                1
                            );
                        } else {
                            cell1->m_triggerType = 0;
                            cell1->m_objectMetadata = 0;
                        }
                    }
                } else {
                    cell1->m_triggerType = 0;
                }
            }
        }
    }

    for (player3 = 0; player3 < 54; player3++) {
        for (slot1 = 0; slot1 < 5; slot1++) {
            if (m_heroRecs[player3].m_army.m_troopTypes[slot1] == -1
                || m_heroRecs[player3].m_army.m_creatureCounts[slot1] < 0)
                m_heroRecs[player3].m_army.m_creatureCounts[slot1] = 0;
        }
    }
    for (player3 = 0; player3 < 72; player3++) {
        for (slot1 = 0; slot1 < 5; slot1++) {
            if (m_castleRecs[player3].m_army.m_troopTypes[slot1] == -1
                || m_castleRecs[player3].m_army.m_creatureCounts[slot1] < 0)
                m_castleRecs[player3].m_army.m_creatureCounts[slot1] = 0;
        }
    }
}

#define done done36
#define fileData fileData9
#define chunkSize chunkSize5
#define fileSize fileSize6
#define result result29
#define fileCrc fileCrc29
#define acknowledged acknowledged4
#define reply reply6
#define transmitCrc transmitCrc18
#define packetsInBatch packetsInBatch2
#define header header3
#define batch batch12
#define file file37
#define oldTrack oldTrack12
#define packetCount packetCount14
#define packet packet6
#define batchCount batchCount29
#define transmitData transmitData3
#define filename filename8
#define samplesReady samplesReady1
#define success success14

// @early-stop
// Complete 0x71e body, 0x208 frame/slots, CFG, semantics, and all 94 ordered
// relocation sites/targets agree. Five operand bytes differ:
// +0x524/+0x527/+0x529 reverse packet/batch-bound loads, and +0x640/+0x643
// reverse fileData/transmitData equality loads. Both source operand orders emit
// identically. Revisit after a relevant GAME predecessor/header change alters
// these TU-cumulative commutative choices.
VA(0x00483219, 0x71e)
i32 game::TransmitSaveGame(i32 remotePlayer, i32 player, i32 useCurrentSave) {
    i32 success;
    i32 samplesReady;
    char filename[456];
    u8* transmitData;
    i32 batchCount;
    i32 packet;
    i32 packetCount;
    i32 unused484;
    i32 oldTrack;
    i32 file;
    i32 unused3c12;
    i32 batch;
    i32* header;
    i32 packetsInBatch;
    u32 transmitCrc;
    char* reply;
    char* acknowledged;
    u32 fileCrc;
    i32 unused1c3;
    i32 result;
    i32 unused140;
    i32 fileSize;
    i32 chunkSize;
    u8* fileData;
    i32 done;

    gpAdvManager->TrimLoopingSounds(4);
    header = 0;
    reply = 0;
    transmitData = 0;
    fileData = 0;
    success = 0;
    result = 0;
    acknowledged = 0;
    oldTrack = -1;

    samplesReady = gpSoundManager->m_samplesReady;
    gpSoundManager->m_samplesReady = 1;
    oldTrack = static_cast<i8>(gpSoundManager->m_currentTrack);
    gpSoundManager->SwitchAmbientMusic(-1);
    gpSoundManager->m_samplesReady = samplesReady;

    LogStr(const_cast<char*>("Transmit Game Start"));
    if (gpAdvManager->m_active)
        BVResMsg(const_cast<char*>("Sending Data"), -1, 0);
    AiPrint(const_cast<char*>("Transmit Start - Compressing"));

    acknowledged = static_cast<char*>(H2_ALLOC(5000, 6777));
    memset(acknowledged, 0, 5000);
    SaveGame(gConfig.rmtSCName, 0, 0);
    if (!gbUseDiffCompression)
        useCurrentSave = 1;
    CreateDiffFile(
        gConfig.rmtSLName,
        gConfig.rmtSCName,
        gConfig.rmtSDName,
        remotePlayer,
        useCurrentSave
    );
    sprintf(filename, "%s%s", ".\\DATA\\", gConfig.rmtSDName);
    fileSize = FileSize(filename);
    LogInt(const_cast<char*>("PostDiffFileSize"), fileSize, -999, -999, -999, -999, -999, -999);

    header = static_cast<i32*>(H2_ALLOC(256, 6797));
    if (gbUseRegularCompression)
        transmitData = static_cast<u8*>(H2_ALLOC(fileSize + 2000, 6799));
    fileData = static_cast<u8*>(H2_ALLOC(fileSize + 2000, 6800));

    file = open(filename, _O_BINARY);
    if (file == -1)
        FileError(filename);
    if (file == -1) {
        goto transmitCleanup;
    }
    {
        read(file, fileData, fileSize);
        close(file);
        fileCrc = calc_crc_long(fileData, fileSize);
        if (gbUseRegularCompression)
            fileSize = EncodeData(
                reinterpret_cast<char*>(transmitData),
                reinterpret_cast<char*>(fileData),
                fileSize
            );
        else
            transmitData = fileData;

        AiPrint(const_cast<char*>("Transmit Start - Sending"));
        if (gbUseRegularCompression)
            transmitCrc = calc_crc_long(transmitData, fileSize);
        else
            transmitCrc = fileCrc;
        LogInt(const_cast<char*>("Send"), fileSize, transmitCrc, -999, -999, -999, -999, -999);

        header[0] = fileSize;
        header[1] = fileCrc;
        header[2] = transmitCrc;
        header[3] = player;
        result = TransmitAndWait(reinterpret_cast<char*>(header), remotePlayer, 16, 1, 2, &reply);
        if (!result)
            ShutDown(0);

        packetCount = (fileSize - 1) / 200 + 1;
        batchCount = (packetCount - 1) / 100 + 1;
        for (batch = 0; batch < batchCount; batch++) {
            if (batch + 1 == batchCount)
                packetsInBatch = packetCount - batch * 100;
            else
                packetsInBatch = 100;

            done = 0;
            while (!done) {
                for (packet = batch * 100; packet < batch * 100 + packetsInBatch; packet++) {
                    PollSound();
                    CheckDoMain(0, 1);
                    if (!acknowledged[packet]) {
                        if (packet + 1 == packetCount)
                            chunkSize = fileSize - packet * 200;
                        else
                            chunkSize = 200;
                        *reinterpret_cast<i16*>(header) = static_cast<i16>(packet);
                        memcpy(
                            reinterpret_cast<char*>(header) + 2,
                            transmitData + packet * 200,
                            chunkSize
                        );
                        result = TransmitRemoteData(
                            reinterpret_cast<char*>(header),
                            remotePlayer,
                            chunkSize + 2,
                            3,
                            0,
                            1,
                            -1
                        );
                        if (!result)
                            ShutDown(0);
                    }
                }
                LogStr(const_cast<char*>("PreWait"));
                *reinterpret_cast<i16*>(header) = static_cast<i16>(batch * 100);
                result =
                    TransmitAndWait(reinterpret_cast<char*>(header), remotePlayer, 2, 4, 5, &reply);
                LogStr(const_cast<char*>("PostWait"));
                if (!result)
                    ShutDown(0);
                for (packet = 0; packetsInBatch > packet; packet++) {
                    if (reinterpret_cast<RemoteMessage*>(reply)->payload[packet] > 0)
                        acknowledged[batch * 100 + packet] = 1;
                }
                done = 1;
                for (packet = batch * 100; packet < batch * 100 + packetsInBatch; packet++) {
                    if (!acknowledged[packet])
                        done = 0;
                }
            }
        }
        result = TransmitRemoteData(0, remotePlayer, 0, 6, 1, 1, -1);
        if (!result)
            ShutDown(0);
        success = 1;
    }

transmitCleanup:
    if (header)
        H2_FREE(header, 6933);
    if (transmitData)
        H2_FREE(transmitData, 6934);
    if (fileData && fileData != transmitData)
        H2_FREE(fileData, 6935);
    if (acknowledged)
        H2_FREE(acknowledged, 6936);

    AiPrint(const_cast<char*>("Transmit End"));
    if (gpAdvManager->m_active) {
        giBottomViewOverride = 0;
        gpAdvManager->UpdBottomView(1, 1, 1);
    }
    if (oldTrack != -1) {
        samplesReady = gpSoundManager->m_samplesReady;
        gpSoundManager->m_samplesReady = 1;
        gpSoundManager->SwitchAmbientMusic(oldTrack);
        gpSoundManager->m_samplesReady = samplesReady;
    }
    return success;
}

#undef done
#undef fileData
#undef chunkSize
#undef fileSize
#undef result
#undef fileCrc
#undef acknowledged
#undef reply
#undef transmitCrc
#undef packetsInBatch
#undef header
#undef batch
#undef file
#undef oldTrack
#undef packetCount
#undef packet
#undef batchCount
#undef transmitData
#undef filename
#undef samplesReady
#undef success

#define decodedData decodedData14
#define packetStart packetStart6
#define received received29
#define lastPacketTime lastPacketTime5
#define result result9
#define packet packet15
#define ackBuffer ackBuffer29
#define receivedCrc receivedCrc7
#define finished finished26
#define computedCrc computedCrc1
#define file file6
#define oldTrack oldTrack36
#define index index1
#define incomingData incomingData36
#define filename filename7
#define samplesReady samplesReady0
#define success success15

// @semantic: Complete receive/decompress/write/cleanup semantics, 0x1e4 frame,
// CFG, and all 102 ordered relocation sites/effective targets agree. The only
// unmasked bytes are +0x5bc/+0x5bf: decodedData/incomingData equality loads use
// opposite stack operands. Reversing the source equality was byte-neutral; ten
// bounded TU-state probes found only sibling-rejected disposable closures.
// Revisit after a material GAME predecessor/header or comparison-tool change.
VA(0x00483937, 0x68d)
i32 game::ReceiveSaveGame(
    i32 dataSize,
    i32 expectedCrc,
    i32 expectedTransmitCrc,
    i32 remotePlayer
) {
    i32 unused20819;
    i32 success;
    i32 samplesReady;
    char filename[452];
    u8* incomingData;
    i32 index;
    i32 oldTrack;
    i32 file;
    i32 computedCrc;
    i32 finished;
    i32 receivedCrc;
    u8* ackBuffer;
    RemoteMessage* packet;
    i32 result;
    i32l lastPacketTime;
    char* received;
    i32 packetStart;
    u8* decodedData;

    LogInt(const_cast<char*>("FW1"), remotePlayer, -999, -999, -999, -999, -999, -999);
    LogStr(const_cast<char*>("RSG1"));
    AiPrint(const_cast<char*>("Receive Start - Getting Data"));
    gpAdvManager->TrimLoopingSounds(4);

    ackBuffer = 0;
    incomingData = 0;
    decodedData = 0;
    packet = 0;
    file = 0;
    finished = 0;
    unused20819 = 0;
    received = 0;
    success = 0;
    oldTrack = -1;

    gpAdvManager->UnwindMapChangeQueue(999, 0);
    if (gpAdvManager->m_active)
        BVResMsg(const_cast<char*>("Receiving Data"), -1, 0);

    samplesReady = gpSoundManager->m_samplesReady;
    oldTrack = static_cast<i8>(gpSoundManager->m_currentTrack);
    gpSoundManager->m_samplesReady = 1;
    gpSoundManager->SwitchAmbientMusic(-1);
    gpSoundManager->m_samplesReady = samplesReady;

    LogStr(const_cast<char*>("Begin Transmit Init Confirm"));
    result = TransmitRemoteData(0, remotePlayer, 0, 2, 1, 1, -1);
    LogStr(const_cast<char*>("End Transmit Init Confirm"));
    if (!result)
        ShutDown(0);

    received = static_cast<char*>(H2_ALLOC(5000, 7008));
    memset(received, 0, 5000);
    if (gbUseRegularCompression)
        decodedData = static_cast<u8*>(H2_ALLOC(700000, 7012));
    ackBuffer = static_cast<u8*>(H2_ALLOC(256, 7014));
    incomingData = static_cast<u8*>(H2_ALLOC(dataSize + 2000, 7015));

    lastPacketTime = KBTickCount();
    LogInt(const_cast<char*>("FW2"), remotePlayer, -999, -999, -999, -999, -999, -999);
    while (!finished) {
        PollSound();
        CheckDoMain(0, 1);
        if (KBTickCount() > lastPacketTime + 90000) {
            NormalDialog(
                const_cast<char*>("Error receiving data.  Keep trying?"),
                2,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE)
                lastPacketTime = KBTickCount();
            else
                ShutDown(0);
        }

        packet = reinterpret_cast<RemoteMessage*>(GetRemoteData(1));
        if (packet && (packet->type == 2 || packet->type == 3)) {
            lastPacketTime = KBTickCount();
            switch (packet->command) {
                case 3:
                    packetStart = *reinterpret_cast<i16*>(packet->payload);
                    received[packetStart] = 1;
                    memcpy(
                        incomingData + packetStart * 200,
                        packet->payload + 2,
                        packet->payloadSize - 2
                    );
                    break;
                case 4:
                    packetStart = *reinterpret_cast<i16*>(packet->payload);
                    for (index = packetStart; index < packetStart + 100; index++)
                        *(ackBuffer + index - packetStart) = received[index];
                    LogInt(
                        const_cast<char*>("FW3"),
                        remotePlayer,
                        -999,
                        -999,
                        -999,
                        -999,
                        -999,
                        -999
                    );
                    result = TransmitRemoteData(
                        reinterpret_cast<char*>(ackBuffer),
                        remotePlayer,
                        200,
                        5,
                        1,
                        1,
                        -1
                    );
                    if (!result)
                        ShutDown(0);
                    break;
                case 6:
                    finished = 1;
                    break;
            }
        }
    }

    AiPrint(const_cast<char*>("Receive Start - Decompressing Data"));
    receivedCrc = calc_crc_long(incomingData, dataSize);
    LogInt(
        const_cast<char*>("Receive"),
        dataSize,
        receivedCrc,
        expectedTransmitCrc,
        -999,
        -999,
        -999,
        -999
    );
    if (gbUseRegularCompression) {
        dataSize = DecodeData(
            reinterpret_cast<char*>(decodedData),
            reinterpret_cast<char*>(incomingData),
            dataSize
        );
        computedCrc = calc_crc_long(decodedData, dataSize);
    } else {
        decodedData = incomingData;
        computedCrc = receivedCrc;
    }
    LogInt(
        const_cast<char*>("Receive"),
        dataSize,
        computedCrc,
        expectedCrc,
        -999,
        -999,
        -999,
        -999
    );

    sprintf(filename, "%s%s", ".\\DATA\\", gConfig.rmtRDName);
    file = open(filename, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (file == -1)
        FileError(filename);
    write(file, decodedData, dataSize);
    close(file);
    success = 1;

    if (received)
        H2_FREE(received, 7118);
    if (ackBuffer)
        H2_FREE(ackBuffer, 7119);
    if (incomingData)
        H2_FREE(incomingData, 7120);
    if (decodedData && incomingData != decodedData)
        H2_FREE(decodedData, 7121);

    CreateJoinFile(gConfig.rmtRLName, gConfig.rmtRDName, gConfig.rmtRCName);
    AiPrint(const_cast<char*>("Receive End"));
    if (gpAdvManager->m_active) {
        giBottomViewOverride = 0;
        gpAdvManager->UpdBottomView(1, 1, 1);
    }
    if (oldTrack != -1) {
        samplesReady = gpSoundManager->m_samplesReady;
        gpSoundManager->m_samplesReady = 1;
        gpSoundManager->SwitchAmbientMusic(oldTrack);
        gpSoundManager->m_samplesReady = samplesReady;
    }
    return success;
}

#undef decodedData
#undef packetStart
#undef received
#undef lastPacketTime
#undef result
#undef packet
#undef ackBuffer
#undef receivedCrc
#undef finished
#undef computedCrc
#undef file
#undef oldTrack
#undef index
#undef incomingData
#undef filename
#undef samplesReady
#undef success

VA(0x00483fc4, 0x455)
void game::DoNewTurn(void) {
    char musicFile18[16];
    char lowerName19[52];
    i32 musicTrack2;

    CheckForTimeEvent();
    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        CheckEndGame(0, 0);
        return;
    }
    giBottomViewOverrideEndTime = KBTickCount() + 3000;
    giBottomViewOverride = 1;
    gpAdvManager->UpdBottomView(1, 1, 1);
    gpAdvManager->SetInitialMapOrigin();
    gpAdvManager->CompleteDraw(0);
    gpAdvManager->UpdateScreen(0, 0);
    CheckEndGame(0, 0);

    if (gpCurPlayer->m_daysLeft >= 0) {
        if (gpCurPlayer->m_daysLeft == 1) {
            sprintf(gText, cNewTurn[1], cPlayerNames[giCurPlayer]);
        } else {
            sprintf(gText, cNewTurn[0], cPlayerNames[giCurPlayer], gpCurPlayer->m_daysLeft);
        }
        NormalDialog(
            gText,
            1,
            -1,
            -1,
            9,
            gpGame->GetPlayerColor(static_cast<i8>(giCurPlayer)),
            -1,
            0,
            -1,
            0
        );
    }

    if (gpCurPlayer->m_heroCount > 0) {
        gpAdvManager->SetHeroContext(gpCurPlayer->NextHero(0), 0);
    } else if (gpCurPlayer->m_townCount > 0) {
        gpAdvManager->SetTownContext(gpCurPlayer->m_townIds[0]);
    }
    gpAdvManager->CheckDimNextHeroBut();

    if (m_day == 1 && (m_month != 1 || m_week != 1 || m_day != 1)) {
        if (gbThisNetHumanPlayer[giCurPlayer])
            gpSoundManager->m_samplesReady = 1;
        if (giWeekType != -1) {
            musicTrack2 = -1;
            if (m_week == 1) {
                musicTrack2 = 21;
                strcpy(musicFile18, "newmonth.82m");
                if (giMonthType == 0) {
                    sprintf(gText, cNewTurn[2], gMonthNames[giMonthTypeExtra]);
                } else if (giMonthType == 1) {
                    strcpy(lowerName19, gArmyNames[giMonthTypeExtra]);
                    lowerName19[0] -= 'a' - 'A';
                    sprintf(gText, cNewTurn[3], gArmyNames[giMonthTypeExtra], lowerName19);
                } else {
                    sprintf(gText, cNewTurn[4]);
                }
            } else {
                musicTrack2 = 20;
                strcpy(musicFile18, "newweek.82m");
                if (giWeekType == 0) {
                    sprintf(gText, cNewTurn[5], gWeekNames[giWeekTypeExtra]);
                } else {
                    strcpy(lowerName19, gArmyNames[giWeekTypeExtra]);
                    lowerName19[0] -= 'a' - 'A';
                    sprintf(gText, cNewTurn[6], gArmyNames[giWeekTypeExtra], lowerName19);
                }
            }
            gpSoundManager->PlayAmbientMusic(musicTrack2, 0, -1);
            gpMouseManager->SetPointer(0);
            NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            gpSoundManager->SwitchAmbientMusic(
                giTerrainToMusicTrack[gpAdvManager->m_currentTerrain]
            );
        }
    }
    gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[gpAdvManager->m_currentTerrain]);
    gpAdvManager
        ->SetEnvironmentOrigin(gpAdvManager->m_mapOriginX + 7, gpAdvManager->m_mapOriginY + 7, 1);
}

VA(0x00484419, 0x58)
i32 game::GetBoatsBuilt(void) {
    i32 count = 0;
    i32 i;
    for (i = 0; i < GAME_BOAT_COUNT; i++) {
        if (m_boatSlots[i] != -1)
            count++;
    }
    return count;
}

// @semantic
// Complete logic and frame; the sole relocation agrees. The canonical player layout
// leaves an operand-order residual: retail forms i + color * sizeof(playerData), while
// this TU forms color * sizeof(playerData) + i. Keep the retained maximum.
VA(0x00484471, 0x9c)
i32 game::GetNumThievesGuilds(i32 color) {
    i32 num = 0;
    i32 i;
    for (i = 0; i < m_players[color].m_townCount; i++) {
        if (gpGame->m_castleRecs[m_players[color].m_townIds[i]].m_buildings & IDX(TOWN_BUILDING_TAVERN))
            num++;
    }
    return num;
}

VA(0x0048450d, 0x113)
i32 game::CalcDifficultyRating(void) {
    i32 notused;
    i32 rating = 0;
    if (m_difficulty == 0)
        rating += 50;
    else if (m_difficulty == 1)
        rating += 80;
    else if (m_difficulty == 2)
        rating += 100;
    else if (m_difficulty == 3)
        rating += 120;
    else if (m_difficulty == 4)
        rating += 140;
    if (m_mapHeader.difficulty == 0)
        ;
    else if (m_mapHeader.difficulty == 1)
        rating += 20;
    else if (m_mapHeader.difficulty == 2)
        rating += 40;
    else if (m_mapHeader.difficulty == 3)
        rating += 80;
    return rating;
}

// @early-stop
// The frame and every non-jump instruction/operand match. Retail is exactly 45 bytes
// longer: nine five-byte local continuation/trampoline jumps; no jump table is present.
VA(0x00484620, 0x1ea)
i32 CalcBaseScore(i32 days) {
    i32 score = GAME_SCORE_BASE;

    if (gpGame->m_mapHeader.width == GAME_SCORE_MAP_EXTRA_LARGE)
        days = static_cast<i32>(days * GAME_SCORE_EXTRA_LARGE_DAY_SCALE);
    else if (gpGame->m_mapHeader.width == GAME_SCORE_MAP_LARGE)
        days = static_cast<i32>(days * GAME_SCORE_LARGE_DAY_SCALE);
    else if (gpGame->m_mapHeader.width == GAME_SCORE_MAP_MEDIUM) {
    } else if (gpGame->m_mapHeader.width == GAME_SCORE_MAP_SMALL)
        days = static_cast<i32>(days * GAME_SCORE_SMALL_DAY_SCALE);

    if (days <= GAME_SCORE_FIRST_TIER) {
        score -= -(-days);
    } else {
        score -= GAME_SCORE_FIRST_TIER;
        if (days <= GAME_SCORE_SECOND_TIER) {
            score = static_cast<i32>(score - (days - GAME_SCORE_FIRST_TIER) * 0.5);
        } else {
            score = static_cast<i32>(score - 30.0);
            if (days <= GAME_SCORE_THIRD_TIER) {
                score = static_cast<i32>(score - (days - GAME_SCORE_SECOND_TIER) * 0.25);
            } else {
                score = static_cast<i32>(score - 60.0);
                score = static_cast<i32>(score - (days - GAME_SCORE_THIRD_TIER) * 0.125);
            }
        }
    }

    if (score < GAME_SCORE_MINIMUM)
        score = GAME_SCORE_MINIMUM;
    return score;
}

// @early-stop
// Complete semantics, 0x8 frame/slots, CFG, and all four ordered relocation
// sites/targets agree. The only unmasked byte is +0x80: base's empty-town arm
// jumps directly to the epilogue at +0xa9, while retail routes through its
// trailing +0xa9 jump at +0xae. Revisit after a relevant GAME predecessor/header
// change alters this local trampoline placement.
VA(0x0048480a, 0xb5)
void game::RestoreCell(i32 x, i32 y, i32 obj, i32 barrier, mapCell* passedCell, i32 p6) {
    mapCell* cell;
    if (passedCell)
        cell = passedCell;
    else
        cell = gpAdvManager->GetCell(x, y);
    if (y > 0 && obj == MAP_TRIGGER_TOWN
        && gpAdvManager->GetCell(x, y - 1)->m_triggerType != IDX(MAP_TRIGGER_TOWN_BASE)) {
        cell->m_triggerType = 0;
        cell->m_objectMetadata = 0;
    } else {
        cell->m_triggerType = static_cast<u8>(obj);
        cell->m_objectMetadata = barrier;
    }
}

// @semantic
// Frame, reinit, realloc, and all 23 relocations agree. The first divergence is the
// height equality's commutative load order; retail also retains two redundant jumps
// for the empty then-branch: an end-of-function trampoline and a dead `jmp realloc`
// while this build collapses to one direct jump. Both equality operand orders and named
// source-line offsets were tried; revisit for inline/jump placement after 95%.
VA(0x004848bf, 0xe3)
void game::SetMapSize(i32 w, i32 h) {
    DATA(0x004f7a0c) static i16 setMapSizeSourceLineBase = 0x1d0d;
    if (h == MAP_HEIGHT && w == MAP_WIDTH && bMapInitialized) {
    } else {
        bMapInitialized = 1;
        MAP_WIDTH = w;
        MAP_HEIGHT = h;
        gpSearchArray->Init();
    }
    if (mapExtra)
        H2_FREE(mapExtra, 7449);
    mapExtra = static_cast<u8*>(H2_ALLOC(MAP_WIDTH * MAP_HEIGHT, 7450));
    memset(mapExtra, 0, MAP_WIDTH * MAP_HEIGHT);
}

// @semantic
// Logic + frame slots byte-exact; residual is the operand-eval order of the two
// `flags |= <extracted len bits>` ORs: retail loads `flags` into al first (then keeps
// len in ecx and pulls the shifted byte via ch), my build evaluates the value first and
// ORs flags from memory. Same TU-cumulative /Od eval-order parity as MakeAllWaterVisible;
// not source-steerable. Aligns when GAME is fuller.
VA(0x004849a2, 0x100)
void WriteDiffHeaderInfo(u8 cmd, i32 len, u8* buf, i32* pos) {
    u8 flags = 0;
    flags = (cmd << IDX(DIFF_COMMAND_SHIFT)) | flags;
    if (len > DIFF_LEN_WORD_MAX) {
        flags |= IDX(DIFF_LEN_WORD_FLAG);
        flags |= (len & DIFF_LEN_HIGH_MASK) >> 16;
        u16 word = static_cast<u16>(len & DIFF_LEN_LOW_MASK);
        buf[*pos] = flags;
        *reinterpret_cast<u16*>(buf + *pos + 1) = word;
        *pos += 3;
    } else if (len > DIFF_LEN_BYTE_MAX) {
        flags |= IDX(DIFF_LEN_BYTE_FLAG);
        flags |= (len >> 8) & DIFF_LEN_SHORT_MASK;
        u8 lo = static_cast<u8>(len);
        buf[*pos] = flags;
        buf[*pos + 1] = lo;
        *pos += 2;
    } else {
        flags |= static_cast<u8>(len);
        buf[*pos] = flags;
        (*pos)++;
    }
}

VA(0x00484aa2, 0xab)
i32 GetSkipCopyLen(u8* buf, i32* pos) {
    u8 b = buf[*pos];
    i32 len;
    if (b & IDX(DIFF_LEN_WORD_FLAG)) {
        len = b & IDX(DIFF_LEN_WORD_HIGH_MASK);
        len <<= 16;
        len |= *reinterpret_cast<u16*>(buf + *pos + 1);
        *pos += 3;
    } else if (b & IDX(DIFF_LEN_BYTE_FLAG)) {
        len = b & IDX(DIFF_LEN_SHORT_MASK);
        len <<= 8;
        len |= buf[*pos + 1];
        *pos += 2;
    } else {
        len = b & IDX(DIFF_LEN_SHORT_MASK);
        (*pos)++;
    }
    return len;
}

// @early-stop
// The 0x44-byte frame and all 83 relocations are exact. Retail retains one additional
// five-byte local jump at +0x5ae (0x5ba versus 0x5b5); explicit return and continue
// spellings are folded by this /Od TU, while all allocation, CRC, diff, write, and cleanup
// paths realign around the delinked local-label placement.
VA(0x00484b4d, 0x5ba)
void CreateDiffFile(
    char* oldName,
    char* joinName,
    char* diffName,
    i32 remotePlayer,
    i32 forceWhole
) {
    u8* diffData6;
    // Retail's debug frame contains two otherwise unused words before joinData29.
    i32 unusedFirst0;
    i32 unusedSecond4;
    u8* joinData29;
    i32 joinSize36;
    i32 joinFile1;
    i32 copyLength28;
    i32l startTime11;
    i32 diffSize29;
    u8* oldData13;
    i32 oldSize37;
    i32 compareOffset4;
    i32 position1;
    i32 sendWhole4;
    i32 oldFile17;

    startTime11 = KBTickCount();
    oldData13 = 0;
    joinData29 = 0;
    diffData6 = 0;
    oldSize37 = 0;
    joinSize36 = 0;
    diffSize29 = 0;
    sendWhole4 = 0;

    if (forceWhole || (iLastDiffSendTo != -1 && remotePlayer != iLastDiffSendTo))
        sendWhole4 = 1;
    iLastDiffSendTo = remotePlayer;

    sprintf(gText, "%s%s", ".\\DATA\\", joinName);
    joinSize36 = FileSize(gText);
    joinData29 = static_cast<u8*>(H2_ALLOC(joinSize36, 7550));
    sprintf(gText, "%s%s", ".\\DATA\\", joinName);
    joinFile1 = open(gText, _O_BINARY);
    if (joinFile1 == -1)
        FileError(gText);
    read(joinFile1, joinData29, joinSize36);
    close(joinFile1);
    LogInt(
        const_cast<char*>("Orig Join CRC"),
        calc_crc_long(joinData29, joinSize36),
        joinSize36,
        -999,
        -999,
        -999,
        -999,
        -999
    );

    if (!forceWhole) {
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        oldSize37 = FileSize(gText);
        oldData13 = static_cast<u8*>(H2_ALLOC(oldSize37, 7571));
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        oldFile17 = open(gText, _O_BINARY);
        if (oldFile17 == -1)
            FileError(gText);
        read(oldFile17, oldData13, oldSize37);
        close(oldFile17);
    }

    diffData6 =
        static_cast<u8*>(H2_ALLOC((oldSize37 > joinSize36 ? oldSize37 : joinSize36) + 5000, 7581));
    if (sendWhole4) {
        diffData6[0] = 0;
        diffData6[1] = 0;
        memcpy(diffData6 + 2, joinData29, joinSize36);
        diffSize29 = joinSize36 + 2;
    } else {
        diffData6[0] = 1;
        diffData6[1] = 0;
        diffSize29 = 2;
        position1 = 0;
        copyLength28 = 0;
        compareOffset4 = copyLength28;
        while (1) {
            if (position1 + copyLength28 >= oldSize37 || position1 + copyLength28 >= joinSize36) {
                copyLength28 = oldSize37 - position1;
                WriteDiffHeaderInfo(1, copyLength28, diffData6, &diffSize29);
                memcpy(diffData6 + diffSize29, joinData29 + position1, copyLength28);
                diffSize29 += copyLength28;
                position1 += copyLength28;
                copyLength28 = 0;
                break;
            }
            if (oldData13[position1 + copyLength28] == joinData29[position1 + copyLength28]) {
                compareOffset4 = 1;
                while (position1 + compareOffset4 + copyLength28 < oldSize37
                       && position1 + compareOffset4 + copyLength28 < joinSize36
                       && oldData13[position1 + compareOffset4 + copyLength28]
                              == joinData29[position1 + compareOffset4 + copyLength28])
                    compareOffset4++;
                if (compareOffset4 <= 3) {
                    copyLength28 += compareOffset4;
                    compareOffset4 = 0;
                    continue;
                } else {
                    if (copyLength28 != 0) {
                        WriteDiffHeaderInfo(1, copyLength28, diffData6, &diffSize29);
                        memcpy(diffData6 + diffSize29, joinData29 + position1, copyLength28);
                        diffSize29 += copyLength28;
                        position1 += copyLength28;
                        copyLength28 = 0;
                    }
                    WriteDiffHeaderInfo(0, compareOffset4, diffData6, &diffSize29);
                    position1 += compareOffset4;
                    compareOffset4 = 0;
                }
            } else {
                while (position1 + copyLength28 < oldSize37 && position1 + copyLength28 < joinSize36
                       && oldData13[position1 + copyLength28]
                              != joinData29[position1 + copyLength28])
                    copyLength28++;
            }
        }
    }

    sprintf(gText, "%s%s", ".\\DATA\\", diffName);
    joinFile1 = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (joinFile1 == -1)
        FileError(gText);
    write(joinFile1, diffData6, diffSize29);
    close(joinFile1);

    sprintf(gText, "%s%s", ".\\DATA\\", oldName);
    joinFile1 = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (joinFile1 == -1)
        FileError(gText);
    write(joinFile1, joinData29, joinSize36);
    close(joinFile1);

    if (oldData13 != 0)
        H2_FREE(oldData13, 7687);
    if (joinData29 != 0)
        H2_FREE(joinData29, 7689);
    if (diffData6 != 0)
        H2_FREE(diffData6, 7691);
    return;
}

// @semantic: Complete join-file semantics, frame/slots, CFG, and all 77 ordered
// relocation sites/effective targets agree. The only unmasked bytes are
// +0x1ea/+0x1ed/+0x1ef: the diffSize/position loop comparison uses reversed
// operands and branch polarity. Reversed relational and OD_STEER(position) forms were
// neutral; ten bounded TU-state probes did not produce an admissible closure.
// Revisit after a material GAME predecessor/header or comparison-tool change.
VA(0x00485107, 0x3ce)
void CreateJoinFile(char* oldName, char* diffName, char* joinName) {
    DATA(0x004f7bc4) static i16 createJoinFileSourceLineBase = 0x1e0f;
    u8* oldData13 = 0;
    u8* diffData5 = 0;
    u8* joinData9 = 0;
    i32 joinSize37 = 0;
    i32 diffSize1;
    i32 copyLength9;
    i32 diffFile2;
    i32 oldSize10;
    u8 copyFlag16;
    i32 position1;
    i32 joinFile0;

    sprintf(gText, "%s%s", ".\\DATA\\", diffName);
    diffSize1 = FileSize(gText);
    diffData5 = static_cast<u8*>(H2_ALLOC(diffSize1, 7708));
    sprintf(gText, "%s%s", ".\\DATA\\", diffName);
    diffFile2 = open(gText, _O_BINARY);
    if (diffFile2 == -1)
        FileError(gText);
    read(diffFile2, diffData5, diffSize1);
    close(diffFile2);

    joinData9 = static_cast<u8*>(H2_ALLOC(GAME_JOIN_BUFFER_SIZE, 7717));
    if (diffData5[0] == 0) {
        memcpy(joinData9, diffData5 + GAME_JOIN_HEADER_SIZE, diffSize1 - GAME_JOIN_HEADER_SIZE);
        joinSize37 = diffSize1 - GAME_JOIN_HEADER_SIZE;
    } else {
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        oldSize10 = FileSize(gText);
        oldData13 = static_cast<u8*>(H2_ALLOC(oldSize10, 7728));
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        diffFile2 = open(gText, _O_BINARY);
        if (diffFile2 == -1)
            FileError(gText);
        read(diffFile2, oldData13, oldSize10);
        close(diffFile2);
        memcpy(joinData9, oldData13, oldSize10);

        position1 = GAME_JOIN_HEADER_SIZE;
        while (diffSize1 > position1) {
            copyFlag16 = diffData5[position1] >> 7;
            copyLength9 = GetSkipCopyLen(diffData5, &position1);
            if (copyFlag16) {
                memcpy(joinData9 + joinSize37, diffData5 + position1, copyLength9);
                joinSize37 += copyLength9;
                position1 += copyLength9;
            } else {
                joinSize37 += copyLength9;
            }
        }
    }

    sprintf(gText, "%s%s", ".\\DATA\\", joinName);
    joinFile0 = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (joinFile0 == -1)
        FileError(gText);
    write(joinFile0, joinData9, joinSize37);
    close(joinFile0);
    LogInt(
        const_cast<char*>("New Join CRC"),
        calc_crc_long(joinData9, joinSize37),
        joinSize37,
        GAME_JOIN_LOG_UNUSED,
        GAME_JOIN_LOG_UNUSED,
        GAME_JOIN_LOG_UNUSED,
        GAME_JOIN_LOG_UNUSED,
        GAME_JOIN_LOG_UNUSED
    );

    sprintf(gText, "%s%s", ".\\DATA\\", oldName);
    joinFile0 = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (joinFile0 == -1)
        FileError(gText);
    write(joinFile0, joinData9, joinSize37);
    close(joinFile0);

    if (oldData13)
        H2_FREE(oldData13, 7778);
    if (diffData5)
        H2_FREE(diffData5, 7780);
    if (joinData9)
        H2_FREE(joinData9, 7782);
}

VA(0x004854d5, 0x5d)
i32 game::HeroIDToHeroPos(playerData* pd, i32 heroId) {
    i32 i;
    for (i = 0; i < pd->m_heroCount; i++) {
        if (pd->m_heroIds[i] == heroId)
            return i;
    }
    return -1;
}

VA(0x00485532, 0x5d)
i32 game::TownIDToTownPos(playerData* pd, i32 townId) {
    i32 i;
    for (i = 0; i < pd->m_townCount; i++) {
        if (pd->m_townIds[i] == townId)
            return i;
    }
    return -1;
}

VA(0x0048558f, 0x79f)
void game::SetupNewRumour(void) {
    char rumourBuffer6[100];
    i32l categoryStats7[6];
    i8 categoryOrder2[8];
    rumourEventExtra* event4;
    i32 eventIndex11;
    i32 attempts13;
    i32 category10;
    i32 roll2;
    i32 direction9;
    if (m_rumourEventCount != 0 && Random(0, 9) < static_cast<i32>(m_rumourEventCount)) {
        attempts13 = 0;
        while (attempts13++ < 200) {
            if (m_rumourEventCount > 1)
                eventIndex11 = Random(0, m_rumourEventCount - 1);
            else
                eventIndex11 = 0;
            event4 =
                reinterpret_cast<rumourEventExtra*>(ppMapExtra[m_rumourEventIndices[eventIndex11]]);
            if (strlen(event4->text) > 2 && event4->text[0] != '@') {
                strcpy(m_rumour, event4->text);
                event4->text[0] = '@';
                return;
            }
        }
    }

    if (Random(0, 100) < 30) {
        strcpy(m_rumour, cRandomTavernText[(giCurTurn / 7) % 8]);
    } else {
        roll2 = Random(0, 100);
        if (roll2 < 80 && giCurTurn > 1) {
            attempts13 = 0;
            while (attempts13++ < 200) {
                category10 = Random(6, 9);
                GetCategoryStats(category10, categoryStats7, categoryOrder2);
                SortStats(categoryStats7, categoryOrder2);
                if (categoryStats7[1] != categoryStats7[0]) {
                    if (category10 == 6)
                        sprintf(
                            m_rumour,
                            "%s has found the most obelisks.",
                            cPlayerNames[categoryOrder2[0]]
                        );
                    else if (category10 == 7)
                        sprintf(
                            m_rumour,
                            "%s has found the most artifacts.",
                            cPlayerNames[categoryOrder2[0]]
                        );
                    else if (category10 == 8)
                        sprintf(
                            m_rumour,
                            "%s has the most powerful forces.",
                            cPlayerNames[categoryOrder2[0]]
                        );
                    else
                        sprintf(
                            m_rumour,
                            "%s earns the most gold.",
                            cPlayerNames[categoryOrder2[0]]
                        );
                    return;
                }
            }
            goto ultimateRumour;
        } else {
        ultimateRumour:
            category10 = Random(0, 100);
            if (category10 < 33) {
                // Retail uses the X coordinate for both threshold axes.
                if (!(m_mapHeader.width * 0.33 <= m_ultimateArtifactX
                      || m_mapHeader.height * 0.33 <= m_ultimateArtifactX)) {
                    direction9 = 7;
                } else if (!(m_mapHeader.width * 0.33 <= m_ultimateArtifactX
                             || m_ultimateArtifactX <= m_mapHeader.height * 0.66)) {
                    direction9 = 5;
                } else if (!(m_mapHeader.width * 0.33 <= m_ultimateArtifactX)) {
                    direction9 = 6;
                } else if (!(m_ultimateArtifactX <= m_mapHeader.width * 0.66
                             || m_mapHeader.height * 0.33 <= m_ultimateArtifactX)) {
                    direction9 = 1;
                } else if (!(m_ultimateArtifactX <= m_mapHeader.width * 0.66
                             || m_ultimateArtifactX <= m_mapHeader.height * 0.66)) {
                    direction9 = 3;
                } else if (!(m_ultimateArtifactX <= m_mapHeader.width * 0.66)) {
                    direction9 = 2;
                } else if (!(m_mapHeader.height * 0.33 <= m_ultimateArtifactX)) {
                    direction9 = 0;
                } else if (!(m_ultimateArtifactX <= m_mapHeader.height * 0.66)) {
                    direction9 = 4;
                } else {
                    direction9 = 8;
                }
                sprintf(
                    m_rumour,
                    "The ultimate artifact may be found %s.",
                    cDirections[direction9]
                );
            } else if (category10 < 66) {
                sprintf(
                    m_rumour,
                    "The ultimate artifact may be found in the %s regions of the world.",
                    cRumourTerrainDescriptions
                        [giGroundToTerrain[gpAdvManager
                                               ->GetCell(m_ultimateArtifactX, m_ultimateArtifactY)
                                               ->m_terrainImageIndex]]
                );
            } else {
                sprintf(
                    m_rumour,
                    "The ultimate artifact is really the %s.",
                    gArtifactNames[m_ultimateArtifactId]
                );
            }
        }
    }
}

VA(0x00485d2e, 0xd9)
EventExtra* GetMapEvent(i32 x, i32 y) {
    i32 i;
    for (i = 0; i < gpGame->m_mapEventCount; i++) {
        EventExtra* ev = reinterpret_cast<EventExtra*>(ppMapExtra[gpGame->m_mapEventIndices[i]]);
        if (ev->x == x && ev->y == y && ev->active != 0
            && ev->players[PlayerEventByte(giCurPlayer)] != 0)
            return ev;
    }
    return 0;
}

// @semantic
// Frame/CFG/logic match. Only +0xc..+0x37 (calendar term register order) and
// +0x19d..+0x286 (four equivalent player-resource address orders) are TU-cumulative.
VA(0x00485e07, 0x34c)
void game::CheckForTimeEvent(void) {
    i32 dayNumber6;
    i32 eventIndex11;
    i32 resourceIndex27;
    timeEventExtra* event15;
    i32 primaryType14;
    i32 secondaryType18;
    i32 primaryAmount12;
    i32 secondaryAmount9;
    i32 resourceAmount7;

    dayNumber6 =
        (m_week - 1) * TIME_EVENT_DAYS_PER_WEEK + (m_month - 1) * TIME_EVENT_DAYS_PER_MONTH + m_day;
    for (eventIndex11 = 0; eventIndex11 < m_timeEventCount; eventIndex11++) {
        event15 = static_cast<timeEventExtra*>(ppMapExtra[m_timeEventIndices[eventIndex11]]);
        if (((gbHumanPlayer[giCurPlayer] && event15->appliesToHuman)
             || (!gbHumanPlayer[giCurPlayer] && event15->appliesToComputer))
            && event15->players[GetPlayerColor(static_cast<i8>(giCurPlayer))]
            && (event15->firstDay == dayNumber6
                || (event15->repeatInterval != 0 && event15->firstDay < dayNumber6
                    && (dayNumber6 - event15->firstDay) % event15->repeatInterval == 0))) {
            primaryType14 = -1;
            primaryAmount12 = 0;
            secondaryType18 = -1;
            secondaryAmount9 = 0;
            for (resourceIndex27 = 0; resourceIndex27 < TIME_EVENT_RESOURCE_COUNT;
                 resourceIndex27++) {
                resourceAmount7 = event15->resources[resourceIndex27];
                if (gpGame->m_players[giCurPlayer].m_resources[resourceIndex27]
                    < -resourceAmount7) {
                    resourceAmount7 = -gpGame->m_players[giCurPlayer].m_resources[resourceIndex27];
                }
                gpGame->m_players[giCurPlayer].m_resources[resourceIndex27] +=
                    event15->resources[resourceIndex27];
                if (gpGame->m_players[giCurPlayer].m_resources[resourceIndex27] < 0)
                    gpGame->m_players[giCurPlayer].m_resources[resourceIndex27] = 0;
                if (resourceAmount7 != 0) {
                    if (primaryType14 != -1) {
                        secondaryType18 = primaryType14;
                        secondaryAmount9 = primaryAmount12;
                    }
                    primaryType14 = resourceIndex27;
                    primaryAmount12 = resourceAmount7;
                }
            }
            if (primaryType14 >= 0 && primaryType14 <= TIME_EVENT_RESOURCE_COUNT - 1
                && primaryAmount12 < 0) {
                primaryAmount12 -= TIME_EVENT_RESOURCE_PENALTY;
            }
            if (secondaryType18 >= 0 && secondaryType18 <= TIME_EVENT_RESOURCE_COUNT - 1
                && secondaryAmount9 < 0) {
                secondaryAmount9 -= TIME_EVENT_RESOURCE_PENALTY;
            }
            if (gbThisNetHumanPlayer[giCurPlayer]) {
                NormalDialog(
                    event15->message,
                    1,
                    -1,
                    -1,
                    primaryType14,
                    primaryAmount12,
                    secondaryType18,
                    secondaryAmount9,
                    -1,
                    0
                );
            }
        }
    }
}

// @semantic
// The 0x143 frame/loops and all eight ordered relocations match; 17 raw bytes at
// +0xc5..+0xe8 are only the equivalent packed expression heroIndex + player * 283.
// Direct, commuted, accessor, and AST variants did not steer it.
VA(0x00486153, 0x143)
void CheckValidAvailableHeroes(void) {
    i32 candidatePlayer0;
    i32 heroIndex5;
    i32 availableSlot13;
    i32 heroPlayer26;

    for (heroPlayer26 = 0; heroPlayer26 < gpGame->m_playerCount; heroPlayer26++) {
        for (heroIndex5 = 0; heroIndex5 < gpGame->m_players[heroPlayer26].m_heroCount;
             heroIndex5++) {
            for (candidatePlayer0 = 0; candidatePlayer0 < gpGame->m_playerCount;
                 candidatePlayer0++) {
                for (availableSlot13 = 0; availableSlot13 < AVAILABLE_HERO_SLOTS;
                     availableSlot13++) {
                    if (gpGame->m_players[candidatePlayer0].m_availableHeroIds[availableSlot13]
                        == gpGame->m_players[heroPlayer26].m_heroIds[heroIndex5]) {
                        gpGame->m_players[candidatePlayer0].m_availableHeroIds[availableSlot13] =
                            static_cast<i8>(gpGame->GetNewHeroId(heroPlayer26, -1, 0));
                    }
                }
            }
        }
    }
}

VA(0x00486296, 0xab)
i32 CalcFileCRC(char* filename) {
    DATA(0x004f7e3c) static i16 calcFileCrcSourceLineBase = 0x1f5e;
    i32l size = FileSize(filename);
    char* block = static_cast<char*>(H2_ALLOC(size, 8033));
    i32 hand = open(filename, _O_BINARY);
    if (hand == -1)
        FileError(filename);
    read(hand, block, size);
    i32 crc = calc_crc_long(reinterpret_cast<u8*>(block), size);
    close(hand);
    H2_FREE(block, 8044);
    return crc;
}

// @semantic
// All bytes except +0x8d..+0x99 match; /Od reverses the equivalent index/size
// loop-test load order and branch polarity. Relational and AST variants did not steer it.
VA(0x00486341, 0x153)
void CompressTest2(void) {
    i32 dataSize2;
    i32l encodedSize14;
    i32l decodedSize17;
    char* sourceData6;
    i32 sourceCrc0;
    char* decodedData6;
    i32 index7;
    i32 decodedCrc5;
    i32 sourceCrcCheck7;
    char* encodedData6;

    dataSize2 = Random(COMPRESSION_TEST_RANDOM_SIZE_MIN, COMPRESSION_TEST_RANDOM_SIZE_MAX);
    sourceData6 =
        static_cast<char*>(H2_ALLOC(dataSize2 + COMPRESSION_TEST_RANDOM_BUFFER_EXTRA, 8057));
    encodedData6 =
        static_cast<char*>(H2_ALLOC(dataSize2 + COMPRESSION_TEST_RANDOM_BUFFER_EXTRA, 8058));
    decodedData6 =
        static_cast<char*>(H2_ALLOC(dataSize2 + COMPRESSION_TEST_RANDOM_BUFFER_EXTRA, 8059));
    for (index7 = 0; index7 < dataSize2; index7++)
        sourceData6[index7] = static_cast<char>(Random(0, 255));
    sourceCrc0 = calc_crc_long(reinterpret_cast<u8*>(sourceData6), dataSize2);
    encodedSize14 = EncodeData(encodedData6, sourceData6, dataSize2);
    decodedSize17 = DecodeData(decodedData6, encodedData6, encodedSize14);
    decodedCrc5 = calc_crc_long(reinterpret_cast<u8*>(decodedData6), dataSize2);
    sourceCrcCheck7 = calc_crc_long(reinterpret_cast<u8*>(sourceData6), dataSize2);
    H2_FREE(sourceData6, 8076);
    H2_FREE(encodedData6, 8077);
    H2_FREE(decodedData6, 8078);
}

VA(0x00486494, 0x1be)
void CompressTest(void) {
    i32l fileSize7;
    i32l encodedSize14;
    i32l decodedSize17;
    char* sourceData6;
    i32 sourceCrc0;
    char* decodedData6;
    i32 fileHandle4;
    i32 decodedCrc5;
    i32 sourceCrcCheck7;
    char* encodedData6;
    char filename3[COMPRESSION_TEST_FILENAME_SIZE];

    LogStr(const_cast<char*>("C1"));
    strcpy(filename3, "c:\\TEMP\\Z.DIF");
    fileSize7 = FileSize(filename3);
    sourceData6 =
        static_cast<char*>(H2_ALLOC(fileSize7 + COMPRESSION_TEST_FILE_BUFFER_EXTRA, 8094));
    encodedData6 =
        static_cast<char*>(H2_ALLOC(fileSize7 + COMPRESSION_TEST_FILE_BUFFER_EXTRA, 8095));
    decodedData6 =
        static_cast<char*>(H2_ALLOC(fileSize7 + COMPRESSION_TEST_FILE_BUFFER_EXTRA, 8096));
    LogStr(const_cast<char*>("C2"));
    fileHandle4 = open(filename3, _O_BINARY);
    if (fileHandle4 == -1)
        FileError(filename3);
    read(fileHandle4, sourceData6, fileSize7);
    LogStr(const_cast<char*>("C3"));
    sourceCrc0 = calc_crc_long(reinterpret_cast<u8*>(sourceData6), fileSize7);
    LogStr(const_cast<char*>("C4"));
    close(fileHandle4);
    LogStr(const_cast<char*>("C5"));
    encodedSize14 = EncodeData(encodedData6, sourceData6, fileSize7);
    LogStr(const_cast<char*>("C6"));
    decodedSize17 = DecodeData(decodedData6, encodedData6, encodedSize14);
    LogStr(const_cast<char*>("C7"));
    decodedCrc5 = calc_crc_long(reinterpret_cast<u8*>(decodedData6), fileSize7);
    sourceCrcCheck7 = calc_crc_long(reinterpret_cast<u8*>(sourceData6), fileSize7);
    H2_FREE(sourceData6, 8121);
    H2_FREE(encodedData6, 8122);
    H2_FREE(decodedData6, 8123);
    LogStr(const_cast<char*>("C8"));
}

VA(0x00486652, 0x53)
void CompressTest3(void) {
    char buf[40];
    i32 i;
    for (i = 0; i < COMPRESS_TEST_ITERATIONS; i++) {
        sprintf(buf, "Test # %d", i);
        AiPrint(buf);
        CompressTest2();
    }
}

// @semantic: Complete shrine-count semantics, frame/slots, CFG, and all four
// relocation identities agree. Base retains one extra local continuation jump
// before the building-mask load; later relocation sites shift accordingly and
// then realign. Ten bounded TU-state probes were neutral. Revisit after a
// material GAME predecessor/header or comparison-tool change.
VA(0x004866a5, 0x119)
i32 game::CountShrines(i32 player) {
    if (xIsExpansionMap == 0)
        return 0;
    i32 count = 0;
    mapCell* cell;
    {
        i32 col;
        i32 row;
        town* castle;
        for (row = 0; row < MAP_HEIGHT; row++) {
            for (col = 0; col < MAP_WIDTH; col++) {
                cell = WORLDMAP->Row(row) + col;
                if (cell->m_triggerType == IDX(MAP_TRIGGER_TOWN)) {
                    castle = GetCastle(cell->m_objectMetadata);
                    if (castle->m_owner == player && (castle->m_buildings & IDX(TOWN_BUILDING_TAVERN))
                        && castle->m_type == IDX(FACTION_NECROMANCER))
                        count++;
                }
            }
        }
    }
    return count;
}

// ---- remaining globals (retail RVA order) ----
DATA(0x004f7550) i8 giMonType[] = {
    IDX(CREATURE_PEASANT),
    IDX(CREATURE_TROLL),
    IDX(CREATURE_DWARF),
    IDX(CREATURE_ROC),
    IDX(CREATURE_OGRE),
    IDX(CREATURE_DRUID),
    IDX(CREATURE_VAMPIRE),
    IDX(CREATURE_WOLF),
    IDX(CREATURE_CENTAUR),
    IDX(CREATURE_GARGOYLE),
    IDX(CREATURE_UNICORN),
    IDX(CREATURE_LICH)
};
DATA(0x004f7a08) char bMapInitialized = 0;
// @data-layout-note Retail's loader-zero GAME contribution is
// 0x1280e8..0x1284b4 (0x3cc); candidate .bss is 0x3b4. All 23 public
// definitions have recovered types, logical extents, and S_PUB32 RVAs. Retail
// lays them out in RVA order, while VC4.2 emits the candidate in identifier-hash
// order; the same objects occupy 0x18 fewer bytes only because their natural
// alignment holes differ. In particular, retail ViewArmy references prove
// sViewArmyMonFrameInfo +0x65 and +0x175, replacing the former fake initialized
// byte arrays. Reordering definitions, first extern declarations, and owner
// headers leaves the candidate BSS order unchanged. No retail reference proves
// missing storage in the residual holes. Revisit only with evidence for the
// original common/section topology; do not introduce padding variables,
// compatibility aliases, or section pragmas to force the retail order.
DATA(0x005280e8) i32 iViewArmyNumTroops;
DATA(0x005280ec) i8* gbNGHeroType;
DATA(0x005280f8) SMonFrameInfo sViewArmyMonFrameInfo;
DATA(0x00528430) i16 giUABaseX;
DATA(0x00528434) i16 giUABaseY;
DATA(0x00528438) i32 giEndSequence;
DATA(0x0052843c) b32 gbDismissArmy;
DATA(0x00528440) i8* gbNGHuman;
DATA(0x00528448) i32 iViewArmyFrame;
DATA(0x0052844c) b32 gbAllowUpgrade;
DATA(0x00528450) i32 iViewArmyType;
DATA(0x00528454) class hero* viewSpellsHero;
DATA(0x00528458) b32 gbUpgradeArmy;
DATA(0x00528460) i16 RandMineQty[8];
DATA(0x00528470) char gcCurMapName[16];
DATA(0x00528480) i8* gbNGDifficulty;
DATA(0x00528488) CreatureType iViewArmyUpgradeToType;
DATA(0x0052848c) i32 viewArmyBaseX;
DATA(0x00528490) i32 viewArmyBaseY;
DATA(0x00528498) i8* gbNGColor;
DATA(0x005284a0) i16 giUARadius;
DATA(0x005284a8) i8* gbNGPlayerPos;
DATA(0x005284b0) i32 viewArmyFacingWIPXMod;

#undef RETAIL_FILE
