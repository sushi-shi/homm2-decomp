// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\GAME.OBJ   from: (directly linked into exe)
// functions: 99   data: 26
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/philAI.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/GAME.h>
#include <_types.h>
#include <_globals_model.h>
#include <BASE/Icon2b.h>
#include <BASE/Misc.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/FINDPATH.h>
#include <SOURCE/HERO.h>
#include <SOURCE/KB.h>
#include <SOURCE/REMOTE.h>
#include <io.h>
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


// configStruct/SCreatureInfo shared in _types.h; EventExtra/SThievesData in SOURCE/GAME.h.

// GAME's BaseAlloc/BaseFree pass __FILE__ + a source line number. The retail
// encodes the base line as a 2-byte string read via movswl, then adds a per-call
// delta; reproduce byte-exactly. __FILE__ is the original build path (reloc-masked).
#define GFILE const_cast<char *>("I:\\Projects\\Heroes\\Prog\\SOURCE\\GAME.CPP")
#define GSAVELINE (*reinterpret_cast<const short *>("\x94\x02"))
#define GLOADLINE (*reinterpret_cast<const short *>("\x4f\x04"))
#define GMAPLINE (*reinterpret_cast<const short *>("\n"))
#define GTRANSMITLINE (*reinterpret_cast<const short *>("N\""))
#define GRECEIVELINE (*reinterpret_cast<const short *>("-["))
#define GDIFFLINE (*reinterpret_cast<const short *>("f\x1d"))
#define GCOMPRESSTEST2LINE (*reinterpret_cast<const short *>("r\x1f"))
#define GCOMPRESSTESTLINE (*reinterpret_cast<const short *>("\x95\x1f"))
#define VIEW_ARMY_FRAMES \
    "\x37\x3a\x37\x62\x37\x8a\x37\xb9\x37\xc0\x37\xc6\x37\x0d\x38\x11\x38\x15\x38\x19" \
    "\x38\x1d\x38\x21\x38\x25\x38\x29\x38\x2d\x38\x31\x38\x35\x38\x39\x38\x3d\x38\x41" \
    "\x38\x45\x38\x49\x38\x4d\x38\x51\x38\x48\x39\xa1\x39\xad\x39\xb9\x39\x0d\x3a\x5d\x3a" \
    "\x8d\x3a\xb1\x3a\x8d\x3b\xf7\x3b\x80\x3c\x93\x3c\x0a\x3d\x75\x3d\x7a\x3d\xc1\x3d\xc7" \
    "\x3d\x1f\x3e\x25\x3e\x36\x3e\x82\x3e\xef\x3e\x22\x3f\xdd\x3f\xe2\x3f"
#define VIEW_ARMY_FRAME_OFFSETS \
    "\x3b\x8a\x3b\x9c\x3b\xae\x3b\xd2\x3b\xe7\x3b\xf9\x3b\x1d\x3c\x32\x3c\x44\x3c\x5b\x3c" \
    "\x7d\x3c\x8f\x3c\xa6\x3c\xd5\x3c\xe7\x3c\x0b\x3d\x20\x3d\x35\x3d\x4a\x3d\x5e\x3d\x70" \
    "\x3d\x94\x3d\xb8\x3d\xcd\x3d\xe7\x3d\x18\x3e\x2d\x3e\x1a\x3f"

// fullMap is embedded in `game` at this+0xb3e; the retail folds the +0xb3e into the
// member offsets and inlines Row/Extra (/Ob1), so access it inline via the cast.
#define WORLDMAP (reinterpret_cast<fullMap *>(reinterpret_cast<char *>(this) + 0xb3e))

// Inline accessors that reference gpGame directly (the retail emits `add [gpGame]`
// + a per-call `jmp $+0`), so they are free inline helpers, not game methods.
inline town *GetCastle(int idx) { return reinterpret_cast<town *>(reinterpret_cast<char *>(gpGame) + idx * 100 + 0xb53); }
inline signed char PlayerEventByte(signed char color) { return reinterpret_cast<signed char *>(gpGame)[color * 283 + 0x49c]; }

VA(0x004708b0, 0x23d)
void playerData::Write(int file)
{
    char unused[52];

    _write(file, &m_color, 1);
    _write(file, &m_heroCount, 1);
    _write(file, &m_currentHero, 1);
    _write(file, &m_heroLocatorPage, 1);
    _write(file, m_heroIds, 8);
    _write(file, m_availableHeroIds, 2);
    memset(unused, 0, 48);
    _write(file, unused, 42);
    _write(file, &gpGame->m_cheated, 1);
    _write(file, &m_cheatValue, 1);
    _write(file, &m_unknown0f, 4);
    _write(file, &m_unknown0e, 1);
    _write(file, &m_unknownab, 1);
    _write(file, &m_canDig, 1);
    _write(file, &m_unknown41, 1);
    _write(file, &m_unknown42, 1);
    _write(file, &m_daysLeft, 1);
    _write(file, &m_townCount, 1);
    _write(file, &m_currentTown, 1);
    _write(file, &m_townLocatorPage, 1);
    _write(file, m_townIds, 72);
    _write(file, m_resources, 28);
    _write(file, m_secondaryResources, 28);
    _write(file, &m_barrierTents, 1);
    _write(file, &m_barrierTents, 1);
    _write(file, m_unknownad, 6);
}

VA(0x00470aed, 0x22d)
void playerData::Read(int file)
{
    char unused[52];

    _read(file, &m_color, 1);
    _read(file, &m_heroCount, 1);
    _read(file, &m_currentHero, 1);
    _read(file, &m_heroLocatorPage, 1);
    _read(file, m_heroIds, 8);
    _read(file, m_availableHeroIds, 2);
    _read(file, unused, 42);
    _read(file, &gpGame->m_cheated, 1);
    _read(file, &m_cheatValue, 1);
    _read(file, &m_unknown0f, 4);
    _read(file, &m_unknown0e, 1);
    _read(file, &m_unknownab, 1);
    _read(file, &m_canDig, 1);
    _read(file, &m_unknown41, 1);
    _read(file, &m_unknown42, 1);
    _read(file, &m_daysLeft, 1);
    _read(file, &m_townCount, 1);
    _read(file, &m_currentTown, 1);
    _read(file, &m_townLocatorPage, 1);
    _read(file, m_townIds, 72);
    _read(file, m_resources, 28);
    _read(file, m_secondaryResources, 28);
    _read(file, &m_barrierTents, 1);
    _read(file, &m_barrierTents, 1);
    _read(file, m_unknownad, 6);
}

VA(0x00470d1a, 0x12d)
int playerData::NextHero(int)
{
    int current = -1;
    int i;

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
int playerData::HasMobileHero(void)
{
    int i;
    for (i = 0; i < reinterpret_cast<signed char *>(this)[1]; i++) {
        if (gpGame->IsMobile(reinterpret_cast<signed char *>(this)[i + 4]))
            return 1;
    }
    return 0;
}

VA(0x00470eac, 0x64)
int GetNumObelisks(int color)
{
    int count = 0;
    int i;
    for (i = 0; i < 0x30; i++) {
        if (reinterpret_cast<signed char *>(gpGame)[i + 0x634d] & (1 << color))
            count++;
    }
    return count;
}

VA(0x00470f10, 0xca)
int playerData::BuildingsOwned(int a, int b, int c)
{
    int count = 0;
    int i;
    for (i = 0; i < reinterpret_cast<signed char *>(this)[0x44]; i++) {
        signed char *town = reinterpret_cast<signed char *>(reinterpret_cast<char *>(gpGame) + reinterpret_cast<signed char *>(this)[i + 0x47] * 100 + 0xb53);
        if (b < 0x13 || town[3] == a) {
            if (b == 0) {
                if (*reinterpret_cast<int *>(town + 0x18) & 1) {
                    if (town[0x1c] == c)
                        count++;
                }
            } else {
                if (*reinterpret_cast<int *>(town + 0x18) & (1 << b))
                    count++;
            }
        }
    }
    return count;
}

VA(0x00470fda, 0x97)
int playerData::NumOfGivenArtifact(int artifact)
{
    int count = 0;
    int i;
    for (i = 0; i < reinterpret_cast<signed char *>(this)[1]; i++) {
        int j;
        for (j = 0; j < 0xe; j++) {
            if (reinterpret_cast<signed char *>(gpGame)[reinterpret_cast<signed char *>(this)[i + 4] * 250 + j + 0x2899] == artifact)
                count++;
        }
    }
    return count;
}

VA(0x00471071, 0x82)
int game::MineTypesOwned(int col, int row)
{
    int num = 0;
    int i;
    for (i = 0; i < 0x90; i++) {
        if (reinterpret_cast<signed char *>(this)[i * 7 + 0x5cb7] == col &&
            reinterpret_cast<signed char *>(this)[i * 7 + 0x5cb8] == row)
            num++;
    }
    return num;
}

// @early-stop
// reloc-masked: identical 0x40d-byte instruction stream; 36/36 relocation targets agree, residual is delinked local-label identity
VA(0x004710f3, 0x40d)
void ComputeUALoc(int player)
{
    int result = gpGame->SetupPuzzlePieces(player, 1);
    if (result < 8 || gpGame->m_ultimateArtifactId == -1) {
        gpGame->m_players[player].unknown40 = 0;
        gpGame->m_players[player].unknown41 = -1;
        gpGame->m_players[player].unknown42 = -1;
        return;
    }

    int probability = (result - 8) * 4;
    if (probability > 100)
        probability = 100;
    if (probability < 1)
        probability = 1;
    gpGame->m_players[player].unknown40 = static_cast<signed char>(probability);

    if (Random(1, 100) <= gpGame->m_players[player].unknown40) {
        gpGame->m_players[player].unknown41 = gpGame->m_ultimateArtifactX;
        gpGame->m_players[player].unknown42 = gpGame->m_ultimateArtifactY;
        return;
    }

    int x = -1;
    int y = -1;
    int direction = 0;
    int tries = 0;
    while (!(x >= 0 && (&x)[0] < MAP_WIDTH && y >= 0 && (&y)[0] < MAP_HEIGHT &&
             gpGame->m_worldMap.Row(y)[x].triggerType == 0 &&
             gpGame->m_worldMap.Row(y)[x].objIndex == 0xff &&
             gpGame->m_worldMap.Row(y)[x].ovlIndex == 0xff &&
             giGroundToTerrain[gpGame->m_worldMap.Row(y)[x].tile] != 0)) {
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
    gpGame->m_players[player].unknown41 = static_cast<signed char>(x);
    gpGame->m_players[player].unknown42 = static_cast<signed char>(y);
}

VA(0x00471500, 0x2ac)
int game::SetupPuzzlePieces(int player, int justCount)
{
    int pieceCountTotal = GetNumObelisks(player);
    int unvisitedObelisks = 48 - m_obeliskCount;
    float ratio = static_cast<float>(GetNumObelisks(player)) /
                  m_obeliskCount;
    float interpolation = (ratio * ratio + ratio) / 2.0f;
    pieceCountTotal = static_cast<int>(pieceCountTotal +
                                      unvisitedObelisks * interpolation);

    if (GetNumObelisks(player) == m_obeliskCount)
        pieceCountTotal = 48;
    pieceCountTotal += m_players[player].unknown13;
    if (pieceCountTotal > 48)
        pieceCountTotal = 48;
    if (justCount)
        return pieceCountTotal;

    memset(puzzlePiecesRemoved, 0, 6);
    SRand(m_players[player].color + m_players[player].evilInterface * 3);
    int tries;
    int fallbackNum;
    int pieceValue;
    int i;
    for (i = 0; (&i)[0] < pieceCountTotal; i++) {
        for (pieceValue = 0; pieceValue < 48;
             pieceValue += SRandom(1, 5)) {
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
int game::IsMobile(int heroId)
{
    if (heroId == -1)
        return 0;
    char *hp = reinterpret_cast<char *>(this) + heroId * 250 + 0x27c4;
    mapCell *cp = gpAdvManager->GetCell(*reinterpret_cast<int *>(hp + 0x19), *reinterpret_cast<int *>(hp + 0x1d));
    return CalcTerrainCost(giGroundToTerrain[cp->tile], 1, *reinterpret_cast<int *>(hp + 0x35),
                           reinterpret_cast<signed char *>(hp)[0x74], cp->objFlag1, 0) <= *reinterpret_cast<int *>(hp + 0x35);
}

VA(0x00471861, 0x1e)
fullMap *game::GetWorldMapData(void)
{
    return reinterpret_cast<fullMap *>(reinterpret_cast<char *>(this) + 0xb3e);
}

VA(0x0047187f, 0x11e)
int game::CreateBoat(int x, int y, int notify)
{
    int boatIdx = Scan(m_boatSlots, 0, GAME_BOAT_COUNT);
    if (boatIdx != -1) {
        if (notify == 0)
            SendMapChange(4, 0, x, y, -999, 0, 0);
        m_boatSlots[boatIdx] = static_cast<signed char>(boatIdx);
        boatRecord *boat = &m_boats[boatIdx];
        boat->id = static_cast<signed char>(boatIdx);
        boat->x = static_cast<signed char>(x);
        boat->y = static_cast<signed char>(y);
        boat->direction = 2;
        boat->owner = static_cast<signed char>(giCurPlayer);
        mapCell *cell = WORLDMAP->Row(y) + x;
        boat->savedTriggerType = cell->triggerType;
        boat->savedEventData = static_cast<unsigned char>(cell->w4hi);
        cell->triggerType = 0xab;
        cell->w4hi = boatIdx;
    }
    return boatIdx;
}

VA(0x0047199d, 0x5a)
int game::Scan(signed char *array, int start, int length)
{
    int i;
    for (i = start; i < length + start; i++) {
        if (array[i] == -1)
            return i;
    }
    return -1;
}

VA(0x004719f7, 0x76)
int game::RandomScan(signed char *array, int start, int range, int unused, signed char target)
{
    int idx = target;
    int i;
    for (i = 0; i < 0x2710; i++) {
        idx = start + Random(0, range - 1);
        if (array[idx] == target)
            return idx;
    }
    return -1;
}

VA(0x00471a6d, 0x213)
int game::GetNewHeroId(int, int heroClass, int requireExperienced)
{
    int result = -1;
    int previousHero;
    int heroId = -1;
    int attempts = 0;
    int oldHeroId;
    while (attempts < 2000) {
        attempts++;
        heroId = Random(0, 53);
        if (m_availableHeroes[heroId] != -1 &&
            m_availableHeroes[heroId] != 64)
            continue;
        if (m_availableHeroes[heroId] == 64 && attempts < 1500)
            continue;
        if (heroClass >= 0 && heroClass <= 5 && attempts < 100 &&
            m_heroRecs[heroId].m_cursorType != heroClass)
            continue;
        if (requireExperienced && attempts < 40 &&
            m_heroRecs[heroId].m_experience < 1000 &&
            (m_heroRecs[heroId].m_artifacts[0] == -1 ||
             m_heroRecs[heroId].m_artifacts[0] == 81) &&
            (m_heroRecs[heroId].m_artifacts[1] == -1 ||
             m_heroRecs[heroId].m_artifacts[1] == 81))
            continue;
        if (gbInCampaign && attempts < 500 &&
            m_heroRecs[heroId].m_portrait >= 54 &&
            m_heroRecs[heroId].m_portrait <= 59)
            continue;
        break;
    }
    return heroId;
}

VA(0x00471c80, 0x85)
int game::GetTownId(int col, int row)
{
    int i;
    for (i = 0; i < 0x48; i++) {
        if (reinterpret_cast<unsigned char *>(this)[i * 100 + 0xb57] == col &&
            reinterpret_cast<unsigned char *>(this)[i * 100 + 0xb58] == row)
            return i;
    }
    return -1;
}

VA(0x00471d05, 0x84)
int game::GetMineId(int col, int row)
{
    int i;
    for (i = 0; i < 0x90; i++) {
        if (reinterpret_cast<unsigned char *>(this)[i * 7 + 0x5cbb] == col &&
            reinterpret_cast<unsigned char *>(this)[i * 7 + 0x5cbc] == row)
            return i;
    }
    return -1;
}

VA(0x00471d89, 0x12e)
void GenerateStandardFileName(char *source, char *destination)
{
    char *extension = FindLastToken(source, '.');
    if (extension == 0) {
        strcpy(destination, source);
        return;
    }

    *extension = 0;
    int indexOut = 0;
    int sourceLength = strlen(source);
    int i;
    char c;
    for (i = 0; i < (&sourceLength)[0]; i++) {
        c = source[i];
        if (c >= 'a' && c <= 'z')
            c -= 'a' - 'A';
        if ((c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '_') {
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
int game::SaveGame(char *filename, int generateName, signed char expansionFormat)
{
    void *emptyPayload = BaseAlloc(GAME_SAVE_BUFFER_SIZE, GFILE, GSAVELINE + 10);
    memset(emptyPayload, 0, GAME_SAVE_BUFFER_SIZE);
    if (!xIsExpansionMap)
        expansionFormat = 1;
    gpAdvManager->DemobilizeCurrHero();

    char savePathValue[452];
    char generatedNameStorage[452];
    int humanPlayersVar;
    int indexFile;
    int unusedTemp;
    int oldFlag;
    int saveValue;
    int filePadding;
    int compatibilityReserved;
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

    if (_strnicmp(generatedNameStorage, "RMT", 3) == 0) {
        sprintf(savePathValue, "%s%s", ".\\DATA\\", generatedNameStorage);
    } else {
        sprintf(savePathValue, "%s%s", gcGamePath, generatedNameStorage);
        if (_strnicmp(generatedNameStorage, "AUTOSAVE", 8) != 0 &&
            _strnicmp(generatedNameStorage, "PLYREXIT", 8) != 0)
            strcpy(gpGame->m_saveName, filename);
    }

    int fileInfo = _open(savePathValue, 0x8301, 0x80);
    if (fileInfo == -1)
        FileError(savePathValue);

    int legacyMarkerTemp = -1;
    if (!expansionFormat)
        _write(fileInfo, &legacyMarkerTemp, 4);
    _write(fileInfo, &m_worldMap.width, 4);
    _write(fileInfo, &m_worldMap.height, 4);
    _write(fileInfo, &m_mapHeader, sizeof(m_mapHeader));
    _write(fileInfo, m_setupPlayerColor, 0x41);
    _write(fileInfo, &gbIAmGreatest, 1);
    _write(fileInfo, this, 2);
    _write(fileInfo, &giMonthType, 1);
    _write(fileInfo, &giMonthTypeExtra, 1);
    _write(fileInfo, &giWeekType, 1);
    _write(fileInfo, &giWeekTypeExtra, 1);
    _write(fileInfo, cPlayerNames, 126);

    char legacyData[100];
    memset(legacyData, 0, 40);
    _write(fileInfo, legacyData, 36);
    if (xIsPlayingExpansionCampaign) {
        int campaignTypeInfo = 2;
        _write(fileInfo, &campaignTypeInfo, 4);
        _write(fileInfo, &xCampaign, 0x4f);
    } else {
        _write(fileInfo, &gbInCampaign, 4);
        if (gbInCampaign)
            _write(fileInfo, reinterpret_cast<unsigned char *>(this) + 2, 0x147);
    }
    if (!expansionFormat)
        _write(fileInfo, &xIsExpansionMap, 1);

    gpAdvManager->PurgeMapChangeQueue();
    _write(fileInfo, &giMapChangeCtr, 4);
    GenerateStandardFileName(m_saveName, legacyData);
    _write(fileInfo, legacyData, 14);
    _write(fileInfo, &m_playerCount, 1);
    char currentPlayerInfo[4];
    currentPlayerInfo[0] = static_cast<char>(giCurPlayer);
    _write(fileInfo, currentPlayerInfo, 1);
    _write(fileInfo, &m_deadPlayerCount, 1);
    _write(fileInfo, m_playerDead, 6);

    char humanFlagsLocal[8];
    for (indexFile = 0; indexFile < GAME_PLAYER_COUNT; indexFile++) {
        humanFlagsLocal[indexFile] = static_cast<char>(gbHumanPlayer[indexFile]);
        if (m_playerDead[indexFile] != 0)
            humanFlagsLocal[indexFile] = 0;
    }
    _write(fileInfo, humanFlagsLocal, 6);
    _write(fileInfo, &m_day, 2);
    _write(fileInfo, &m_week, 2);
    _write(fileInfo, &m_month, 2);
    for (indexFile = 0; indexFile < GAME_PLAYER_COUNT; indexFile++)
        reinterpret_cast<playerData *>(&m_players[indexFile])->Write(fileInfo);

    _write(fileInfo, &m_obeliskCount, 1);
    for (indexFile = 0; indexFile < GAME_HERO_COUNT; indexFile++)
        m_heroRecs[indexFile].Write(fileInfo, !expansionFormat);
    _write(fileInfo, m_availableHeroes, GAME_HERO_COUNT);
    _write(fileInfo, m_castleRecs, 0x1c20);
    _write(fileInfo, reinterpret_cast<unsigned char *>(this) + 0x2773, 0x48);
    _write(fileInfo, reinterpret_cast<unsigned char *>(this) + 0x27bb, 9);
    _write(fileInfo, reinterpret_cast<unsigned char *>(this) + 0x5cb6, 0x3f0);
    _write(fileInfo, m_mineOwners, 0x90);
    if (!expansionFormat)
        _write(fileInfo, m_randomArtifacts, 0x67);
    else
        _write(fileInfo, m_randomArtifacts, 0x52);
    _write(fileInfo, m_boats, 0x180);
    _write(fileInfo, m_boatSlots, sizeof(m_boatSlots));
    _write(fileInfo, reinterpret_cast<unsigned char *>(this) + 0x634d, 0x30);
    _write(fileInfo, &m_ultimateArtifactX, 1);
    _write(fileInfo, &m_ultimateArtifactY, 1);
    _write(fileInfo, &m_ultimateArtifactId, 1);
    _write(fileInfo, reinterpret_cast<unsigned char *>(this) + 0x63aa, 0x12d);
    _write(fileInfo, reinterpret_cast<unsigned char *>(this) + 0x637d, 0x18);
    _write(fileInfo, reinterpret_cast<unsigned char *>(this) + 0x64d7, 4);
    _write(fileInfo, reinterpret_cast<unsigned char *>(this) + 0x64d9,
           *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x64d7) * 2);
    _write(fileInfo, reinterpret_cast<unsigned char *>(this) + 0x6515, 4);
    _write(fileInfo, reinterpret_cast<unsigned char *>(this) + 0x6517,
           *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x6515) * 2);
    _write(fileInfo, reinterpret_cast<unsigned char *>(this) + 0x657b, 4);
    _write(fileInfo, reinterpret_cast<unsigned char *>(this) + 0x657d,
           *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x657b) * 2);

    int markerBuffer[3];
    markerBuffer[0] = GAME_FILE_MARKER;
    int unusedMarkerInfo = GAME_UNUSED_FILE_MARKER;
    _write(fileInfo, markerBuffer, 4);
    _write(fileInfo, &iMaxMapExtra, 4);
    _write(fileInfo, markerBuffer, 4);
    for (indexFile = 1; indexFile < iMaxMapExtra; indexFile++) {
        _write(fileInfo, markerBuffer, 4);
        _write(fileInfo, pwSizeOfMapExtra + indexFile, 2);
        if (ppMapExtra[indexFile] != 0)
            _write(fileInfo, ppMapExtra[indexFile], pwSizeOfMapExtra[indexFile]);
        else
            _write(fileInfo, emptyPayload, pwSizeOfMapExtra[indexFile]);
    }
    _write(fileInfo, markerBuffer, 4);
    _write(fileInfo, mapExtra, MAP_WIDTH * MAP_HEIGHT);
    _write(fileInfo, markerBuffer, 4);
    m_worldMap.Write(fileInfo);
    _write(fileInfo, markerBuffer, 4);
    _close(fileInfo);
    BaseFree(emptyPayload, GFILE, GSAVELINE + 0xed);
    return 1;
}

// @early-stop: complete semantics and layout. The typed four-byte primary-stat
// array requires the adjacent byte at hero+0x43 to be initialized separately;
// retail's aliasing five-iteration loop accounts for the remaining code shape.
VA(0x00472a7b, 0xb44)
void game::SetupOrigData(void)
{
    ClearMapExtra();
    gbIAmGreatest = 0;
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

    int i;
    int j;
    for (i = 0; i < GAME_PLAYER_COUNT; i++) {
        strcpy(m_defaultPlayerNames + i * 4, "Lord");
        if (i < (&giNumHumanPlayers)[0]) {
            if (i == 0 || iMPBaseType == 2)
                gbThisNetHumanPlayer[i] = 1;
            else
                gbThisNetHumanPlayer[i] = 0;
            gbHumanPlayer[i] = 1;
        } else {
            gbThisNetHumanPlayer[i] = 0;
            gbHumanPlayer[i] = 0;
        }
        memset(&m_players[i], 0, sizeof(m_players[i]));
        m_players[i].color = static_cast<signed char>(i);
        m_players[i].heroCount = 0;
        m_players[i].townCount = 0;
        m_players[i].daysLeft = -1;
        m_players[i].unknown13 = 0;
        memset(m_players[i].availableHeroes, -1, 2);
        memset(m_players[i].heroes, -1, 8);
        memset(m_players[i].towns, -1, GAME_TOWN_COUNT);
    }

    m_obeliskCount = 0;
    gpAdvManager->m_heroContextLocked = 0;
    memset(m_availableHeroes, -1, GAME_HERO_COUNT);
    for (i = 0; i < GAME_HERO_COUNT; i++) {
        memset(&m_heroRecs[i], 0, sizeof(m_heroRecs[i]));
        memset(reinterpret_cast<unsigned char *>(&m_heroRecs[i]) + 0x94, 0, 0x41);
        memset(m_heroRecs[i].m_artifacts, -1, 14);
        m_heroRecs[i].m_patrolY = -1;
        m_heroRecs[i].m_patrolX = m_heroRecs[i].m_patrolY;
        m_heroRecs[i].m_id = static_cast<signed char>(i);
        m_heroRecs[i].m_portrait = static_cast<unsigned char>(i);
        m_heroRecs[i].m_owner = -1;
        m_heroRecs[i].m_direction = 2;
        strcpy(m_heroRecs[i].m_name, gHeroDefaultNames[i]);
        m_heroRecs[i].m_cursorType = static_cast<unsigned char>(i / 9);
        for (j = 0; j < HERO_PRIMARY_STAT_COUNT; j++)
            m_heroRecs[i].m_primaryStats[j] =
                gStartingHeroStats[m_heroRecs[i].m_cursorType][j];
        m_heroRecs[i].m_unknown43 =
            gStartingHeroStats[m_heroRecs[i].m_cursorType]
                              [HERO_PRIMARY_STAT_COUNT];
        for (j = 0; j < 5; j++)
            reinterpret_cast<signed char *>(&m_heroRecs[i].m_army)[j] = -1;
        m_heroRecs[i].m_destinationY = -1;
        m_heroRecs[i].m_destinationX = m_heroRecs[i].m_destinationY;
        m_heroRecs[i].m_level = 1;
        m_heroRecs[i].m_spellPoints = m_heroRecs[i].Stats(3) * 10;
        m_heroRecs[i].m_secondarySkillCount = 0;
        for (j = 0; j < 14; j++) {
            m_heroRecs[i].m_secondarySkills[j] = 0;
            m_heroRecs[i].m_secondarySkillOrder[j] = 0;
        }
        if (m_heroRecs[i].m_cursorType == 0) {
            m_heroRecs[i].GiveSS(HERO_SKILL_LEADERSHIP, 1);
            m_heroRecs[i].GiveSS(HERO_SKILL_BALLISTICS, 1);
        }
        if (m_heroRecs[i].m_cursorType == 2) {
            m_heroRecs[i].m_artifacts[0] = 81;
            m_heroRecs[i].GiveSS(HERO_SKILL_MYSTICISM, 2);
            m_heroRecs[i].GiveSS(HERO_SKILL_WISDOM, 1);
        }
        if (m_heroRecs[i].m_cursorType == 1)
            m_heroRecs[i].GiveSS(HERO_SKILL_PATHFINDING, 2);
        if (m_heroRecs[i].m_cursorType == 3) {
            m_heroRecs[i].m_artifacts[0] = 81;
            m_heroRecs[i].GiveSS(HERO_SKILL_ARCHERY, 2);
            m_heroRecs[i].GiveSS(HERO_SKILL_WISDOM, 1);
        }
        if (m_heroRecs[i].m_cursorType == 4) {
            m_heroRecs[i].m_artifacts[0] = 81;
            m_heroRecs[i].GiveSS(HERO_SKILL_WISDOM, 2);
        }
        if (m_heroRecs[i].m_cursorType == 5) {
            m_heroRecs[i].m_artifacts[0] = 81;
            m_heroRecs[i].GiveSS(HERO_SKILL_WISDOM, 1);
            m_heroRecs[i].GiveSS(HERO_SKILL_NECROMANCY, 1);
        }
    }

    memset(reinterpret_cast<unsigned char *>(this) + 0x2773, -1, GAME_TOWN_COUNT);
    for (i = 0; i < GAME_TOWN_COUNT; i++) {
        memset(&m_castleRecs[i], 0, 100);
        reinterpret_cast<unsigned char *>(&m_castleRecs[i])[0x36] = 0;
        reinterpret_cast<unsigned char *>(&m_castleRecs[i])[0] = static_cast<unsigned char>(i);
        reinterpret_cast<unsigned char *>(&m_castleRecs[i])[1] = 0xff;
        reinterpret_cast<unsigned char *>(&m_castleRecs[i])[3] = static_cast<unsigned char>(i / 9);
        reinterpret_cast<unsigned char *>(&m_castleRecs[i])[0x17] = 0xff;
        for (j = 0; j < 5; j++)
            reinterpret_cast<unsigned char *>(this)[(&j)[0] + i * 100 + 0xb5b] = 0xff;
    }
    for (i = 0; i < GAME_MINE_COUNT; i++)
        memset(&m_mines[i], -1, sizeof(m_mines[i]));
    memset(m_mineOwners, -1, GAME_MINE_COUNT);
    for (i = 0; i < GAME_BOAT_COUNT; i++) {
        memset(&m_boats[i], 0, sizeof(m_boats[i]));
        m_boats[i].id = static_cast<signed char>(i);
        m_boats[i].heroId = -1;
    }
    memset(reinterpret_cast<unsigned char *>(this) + 0x27bb, 0, 9);
    memset(m_boatSlots, -1, sizeof(m_boatSlots));
    m_ultimateArtifactY = -1;
    m_ultimateArtifactX = m_ultimateArtifactY;
    memset(m_obeliskVisitors, 0, 48);
    strcpy(gpGame->m_saveName, "NEWGAME");
    giCurPlayer = 0;
    gpCurPlayer = reinterpret_cast<playerData *>(&gpGame->m_players[giCurPlayer]);
    giCurPlayerBit = static_cast<unsigned char>(1 << giCurPlayer);
    giCurWatchPlayer = giCurPlayer;
    while (!gbThisNetHumanPlayer[giCurWatchPlayer])
        giCurWatchPlayer = (giCurWatchPlayer + 1) % m_playerCount;
    giCurWatchPlayerBit = static_cast<unsigned char>(1 << giCurWatchPlayer);
    gpAdvManager->CheckSetEvilInterface(0, -1);
    bShowIt = gbThisNetHumanPlayer[giCurPlayer];
}

// @early-stop
// reloc-masked: identical 0xc27-byte code/frame; 164/164 targets agree, only compiler literal/constant symbol identities differ
VA(0x004735bf, 0xc27)
void game::LoadGame(char *filename, int loadFromFile, int)
{
    LogStr("LG1");
    if (loadFromFile) {
        SetupOrigData();
        return;
    }
    LogStr("LG2");
        int humansLoaded = 0;
        gbGameOver = 0;
        reinterpret_cast<unsigned char *>(this)[0x660e] = 1;

        char path[452];
        if (!loadFromFile && _strnicmp(filename, "RMT", 3) == 0)
            sprintf(path, "%s%s", ".\\DATA\\", filename);
        else
            sprintf(path, "%s%s", gcGamePath, filename);

        int file = _open(path, 0x8000);
        if (file == -1)
            FileError(path);
        ClearMapExtra();

        signed char expansionMarker = 0;
        int width;
        int height[11];
        _read(file, &width, 4);
        if (width == -1) {
            expansionMarker = 1;
            _read(file, &width, 4);
        }
        _read(file, height, 4);
        SetMapSize(width, height[0]);
        _read(file, &m_mapHeader, sizeof(m_mapHeader));
        _read(file, m_setupPlayerColor, 0x41);
        _read(file, &gbIAmGreatest, 1);
        _read(file, this, 2);
        _read(file, &giMonthType, 1);
        _read(file, &giMonthTypeExtra, 1);
        _read(file, &giWeekType, 1);
        _read(file, &giWeekTypeExtra, 1);
        _read(file, cPlayerNames, 126);

        char oldData[40];
        _read(file, oldData, 36);
        _read(file, &gbInCampaign, 4);
        if (gbInCampaign == 1) {
            _read(file, reinterpret_cast<unsigned char *>(this) + 2, 0x147);
        } else if (gbInCampaign == 2) {
            xIsPlayingExpansionCampaign = 1;
            gbInCampaign = 0;
            _read(file, &xCampaign, 0x4f);
        }
        if (expansionMarker)
            _read(file, &xIsExpansionMap, 1);

        gpAdvManager->PurgeMapChangeQueue();
        _read(file, &giMapChangeCtr, 4);
        _read(file, oldData, 14);
        if (_strnicmp(filename, "RMT", 3) != 0)
            sprintf(gpGame->m_saveName, filename);
        _read(file, &m_playerCount, 1);

        char currentPlayer[8];
        _read(file, currentPlayer, 1);
        giCurPlayer = currentPlayer[0];
        _read(file, &m_deadPlayerCount, 1);
        _read(file, m_playerDead, 6);

        char humanFlags[8];
        _read(file, humanFlags, 6);
        int i;
        for (i = 0; i < GAME_PLAYER_COUNT; i++) {
            if (humanFlags[i] && humansLoaded < (&giNumHumanPlayers)[0]) {
                humansLoaded++;
                gbHumanPlayer[i] = 1;
            } else {
                gbHumanPlayer[i] = 0;
            }
        }
        for (i = 0; i < GAME_PLAYER_COUNT; i++) {
            if (gbHumanPlayer[i]) {
                if (!gbRemoteOn || (&i)[0] == giThisGamePos)
                    gbThisNetHumanPlayer[i] = 1;
                else
                    gbThisNetHumanPlayer[i] = 0;
            } else {
                gbThisNetHumanPlayer[i] = 0;
            }
        }

        _read(file, &m_day, 2);
        _read(file, &m_week, 2);
        _read(file, &m_month, 2);
        giCurTurn = (m_week - 1) * 7 + (m_month - 1) * 28 + m_day;
        for (i = 0; i < GAME_PLAYER_COUNT; i++)
            reinterpret_cast<playerData *>(&m_players[i])->Read(file);

        _read(file, &m_obeliskCount, 1);
        for (i = 0; i < GAME_HERO_COUNT; i++)
            m_heroRecs[i].Read(file, expansionMarker);
        _read(file, m_availableHeroes, GAME_HERO_COUNT);
        _read(file, m_castleRecs, 0x1c20);
        _read(file, reinterpret_cast<unsigned char *>(this) + 0x2773, 0x48);
        _read(file, reinterpret_cast<unsigned char *>(this) + 0x27bb, 9);
        _read(file, m_mines, 0x3f0);
        _read(file, m_mineOwners, 0x90);
        if (expansionMarker)
            _read(file, m_randomArtifacts, 0x67);
        else
            _read(file, m_randomArtifacts, 0x52);
        _read(file, m_boats, 0x180);
        _read(file, m_boatSlots, sizeof(m_boatSlots));
        _read(file, m_obeliskVisitors, 0x30);
        _read(file, &m_ultimateArtifactX, 1);
        _read(file, &m_ultimateArtifactY, 1);
        _read(file, &m_ultimateArtifactId, 1);
        _read(file, reinterpret_cast<unsigned char *>(this) + 0x63aa, 0x12d);
        _read(file, m_defaultPlayerNames, 0x18);
        _read(file, reinterpret_cast<unsigned char *>(this) + 0x64d7, 4);
        _read(file, reinterpret_cast<unsigned char *>(this) + 0x64d9,
              *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x64d7) * 2);
        _read(file, reinterpret_cast<unsigned char *>(this) + 0x6515, 4);
        _read(file, reinterpret_cast<unsigned char *>(this) + 0x6517,
              *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x6515) * 2);
        _read(file, reinterpret_cast<unsigned char *>(this) + 0x657b, 4);
        _read(file, reinterpret_cast<unsigned char *>(this) + 0x657d,
              *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x657b) * 2);

        char marker[8];
        _read(file, marker, 4);
        _read(file, &iMaxMapExtra, 4);
        _read(file, marker, 4);
        ppMapExtra = reinterpret_cast<void **>(
            BaseAlloc(iMaxMapExtra * 4, GFILE, GLOADLINE + 0xcb));
        pwSizeOfMapExtra = reinterpret_cast<short *>(
            BaseAlloc(iMaxMapExtra * 2, GFILE, GLOADLINE + 0xcc));
        memset(ppMapExtra, 0, iMaxMapExtra * 4);
        memset(pwSizeOfMapExtra, 0, iMaxMapExtra * 2);
        for (i = 1; (&i)[0] < iMaxMapExtra; i++) {
            _read(file, marker, 4);
            _read(file, pwSizeOfMapExtra + i, 2);
            ppMapExtra[i] = BaseAlloc(pwSizeOfMapExtra[i], GFILE, GLOADLINE + 0xd5);
            _read(file, ppMapExtra[i], pwSizeOfMapExtra[i]);
        }
        _read(file, marker, 4);
        _read(file, mapExtra, MAP_WIDTH * MAP_HEIGHT);
        _read(file, marker, 4);
        m_worldMap.Read(file, 0);
        _read(file, marker, 4);
        _close(file);

        gpAdvManager->m_heroContextLocked = 0;
        gpCurPlayer = reinterpret_cast<playerData *>(&gpGame->m_players[giCurPlayer]);
        giCurPlayerBit = static_cast<unsigned char>(1 << giCurPlayer);
        giCurWatchPlayer = giCurPlayer;
        while (!gbThisNetHumanPlayer[giCurWatchPlayer])
            giCurWatchPlayer = (giCurWatchPlayer + 1) % m_playerCount;
        giCurWatchPlayerBit = static_cast<unsigned char>(1 << giCurWatchPlayer);
        bShowIt = gbThisNetHumanPlayer[giCurPlayer];
        SetupAdjacentMons();
        LogStr("LG3");
        gpAdvManager->CheckSetEvilInterface(0, -1);
}

// @early-stop
// reloc-masked: all 0x3ee code bytes identical; 42/42 relocations, residual is delinked switch local-label identity
VA(0x004741e6, 0x3ee)
void game::GiveTroopsToNeutralTown(int townId)
{
    int kn;
    int jb;
    int idx;
    int random;
    int cnt;
    int divisor;

    if ((m_castleRecs[townId].m_x > 0 || m_castleRecs[townId].m_y > 0) &&
        m_castleRecs[townId].m_owner < 0) {
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
        case 10: idx = 0; break;
        case 20: idx = 1; break;
        case 30: idx = 3; break;
        case 40: idx = 5; break;
        case 50: idx = 7; break;
        case 11: idx = 11; break;
        case 21: idx = 12; break;
        case 31: idx = 14; break;
        case 41: idx = 15; break;
        case 51: idx = 17; break;
        case 12: idx = 20; break;
        case 22: idx = 21; break;
        case 32: idx = 23; break;
        case 42: idx = 25; break;
        case 52: idx = 27; break;
        case 13: idx = 29; break;
        case 23: idx = 30; break;
        case 33: idx = 31; break;
        case 43: idx = 32; break;
        case 53: idx = 34; break;
        case 14: idx = 38; break;
        case 24: idx = 39; break;
        case 34: idx = 40; break;
        case 44: idx = 42; break;
        case 54: idx = 43; break;
        case 15: idx = 47; break;
        case 25: idx = 48; break;
        case 35: idx = 50; break;
        case 45: idx = 52; break;
        case 55: idx = 54; break;
        }
        GiveArmy(&m_castleRecs[townId].m_army, idx, cnt, -1);
    }
}

VA(0x004745d4, 0xa4)
void game::GiveTroopsToNeutralTowns(void)
{
    int i;
    for (i = 0; i < 0x48; i++) {
        GiveTroopsToNeutralTown(i);
        if (*(int *)((char *)this + i * 100 + 0xb6b) & 0x40) {
            if (Random(0, 100) < 0x50)
                GiveTroopsToNeutralTown(i);
        } else {
            if (Random(0, 100) < 0x28)
                GiveTroopsToNeutralTown(i);
        }
    }
}

// @early-stop
// reloc-masked: identical 0x1dd0-byte instruction stream; 126/126 relocations, residual is compiler literal/global symbol identity
VA(0x00474678, 0x1dd0)
void game::NewMap(char *filename)
{
    char *extension0;
    int randomColor2;
    int nextHuman6;
    int player2;
    int townIndex9;
    int heroIndex1;
    int pass27;
    int selectedTown14;
    int ultimateDistance5;
    int ultimateTries4;
    int campaignHero15;
    int heroClass5;
    int heroX6;
    int heroY16;
    signed char setupClass12;
    int specialPortrait6;
    char *specialName3;
    int specialClass6;
    int resource13;

    extension0 = FindLastToken(gMapName, '.');
    if (extension0 != 0 && StrEqNoCase(extension0 + 1, "MX2"))
        xIsExpansionMap = 1;
    if (xIsExpansionMap)
        gMapTypeFlags |= 4;
    else
        gMapTypeFlags &= ~4;

    gbInNewGameSetup = 1;
    giCurPlayer = 0;
    gpCurPlayer = reinterpret_cast<playerData *>(&gpGame->m_players[giCurPlayer]);
    giCurPlayerBit = static_cast<unsigned char>(1 << giCurPlayer);
    giCurWatchPlayerBit = giCurPlayerBit;
    giCurWatchPlayer = giCurPlayer;
    randomColor2 = Random(0, 5);
    nextHuman6 = giNumHumanPlayers;

    for (player2 = 0; player2 < GAME_PLAYER_COUNT; player2++) {
        if (player2 >= static_cast<unsigned char>(gpGame->m_mapHeader.playerCount)) {
            gbSetupGamePosToRealGamePos[player2] = -1;
        } else {
            if (m_setupPlayerNetworkId[player2] == 10)
                gbSetupGamePosToRealGamePos[player2] = static_cast<signed char>(nextHuman6++);
            else
                gbSetupGamePosToRealGamePos[player2] =
                    m_setupPlayerNetworkId[player2];
        }
    }
    for (player2 = 0; player2 < GAME_PLAYER_COUNT; player2++) {
        m_players[player2].color = -1;
        gcColorToPlayerPos[player2] = -1;
        gcColorToSetupPos[player2] = -1;
        if (gpGame->m_setupPlayerRace[player2] == 7)
            gpGame->m_setupPlayerRace[player2] = static_cast<signed char>(randomColor2);
        randomColor2 = (randomColor2 + 1) % GAME_PLAYER_COUNT;
    }
    for (player2 = 0; player2 < m_playerCount; player2++)
        gcColorToSetupPos[m_setupPlayerColor[player2]] =
            static_cast<signed char>(player2);
    for (player2 = 0; player2 < m_playerCount; player2++)
        m_players[gbSetupGamePosToRealGamePos[player2]].color =
            m_setupPlayerColor[player2];
    for (player2 = 0; player2 < m_playerCount; player2++)
        gcColorToPlayerPos[m_players[player2].color] = static_cast<signed char>(player2);
    for (player2 = 0; player2 < m_playerCount; player2++) {
        m_players[player2].townCount = 0;
        m_players[player2].townWindowTop = 0;
        m_players[player2].currentTown = -1;
        m_players[player2].heroCount = 0;
        m_players[player2].heroWindowTop = 0;
        m_players[player2].currentHero = -1;
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
        m_players[player2].unknownac = 0;
    RandomizeEvents();
    ProcessOnMapHeroes();
    m_deadPlayerCount = 0;
    for (player2 = m_playerCount; player2 < GAME_PLAYER_COUNT; player2++)
        m_playerDead[player2] = 1;

    if (m_mapHeader.victoryCondition == 4 ||
        m_mapHeader.victoryCondition == 2) {
        m_mapHeader.computerAlsoWins = 1;
        m_mapHeader.allowNormalVictory = 0;
    }
    if (m_mapHeader.victoryCondition == 4) {
        townIndex9 = 0;
        for (player2 = 0; player2 < GAME_PLAYER_COUNT; player2++) {
            if (m_mapHeader.playerEnabled[player2] != 0)
                townIndex9++;
            if (m_mapHeader.victoryConditionValue + 1 == townIndex9) {
                m_mapHeader.victorySideThreshold = static_cast<unsigned short>(player2);
                player2 = 99;
            }
        }
    }
    if (m_mapHeader.victoryCondition == 3)
        m_mapHeader.computerAlsoWins = 1;

    for (player2 = 0; player2 < m_playerCount; player2++) {
        m_players[player2].unknown40 = 0;
        m_players[player2].unknown41 = -1;
        m_players[player2].unknown42 = -1;
        heroIndex1 = 0;
        selectedTown14 = -1;
        if (m_mapHeader.unknown25 == 0 &&
            m_players[player2].townCount > 0) {
            for (pass27 = 0; pass27 < 2; pass27++) {
                for (townIndex9 = 0; townIndex9 < m_players[player2].townCount; townIndex9++) {
                    if (selectedTown14 == -1 &&
                        m_castleRecs[(m_players + player2)->towns[townIndex9]].m_occupyingHeroId == -1 &&
                        ((m_castleRecs[(m_players + player2)->towns[townIndex9]].m_buildings & 0x40) != 0 ||
                         pass27 == 1))
                        selectedTown14 = townIndex9;
                }
            }
        }
        if (selectedTown14 != -1) {
            m_players[player2].heroes[m_players[player2].heroCount] =
                static_cast<signed char>(GetNewHeroId(
                    player2, m_castleRecs[m_players[player2].towns[selectedTown14]].m_type, 0));
            m_availableHeroes[m_players[player2].heroes[m_players[player2].heroCount]] =
                static_cast<signed char>(player2);
            m_heroRecs[m_players[player2].heroes[m_players[player2].heroCount]].m_owner =
                static_cast<signed char>(player2);
            m_heroRecs[m_players[player2].heroes[m_players[player2].heroCount]].m_x =
                m_castleRecs[m_players[player2].towns[selectedTown14]].m_x;
            m_heroRecs[m_players[player2].heroes[m_players[player2].heroCount]].m_y =
                m_castleRecs[m_players[player2].towns[selectedTown14]].m_y;
            m_castleRecs[m_players[player2].towns[selectedTown14]].m_occupyingHeroId =
                m_players[player2].heroes[m_players[player2].heroCount];
            SetVisibility(m_heroRecs[m_players[player2].heroes[m_players[player2].heroCount]].m_x,
                          m_heroRecs[m_players[player2].heroes[m_players[player2].heroCount]].m_y,
                          player2,
                          giVisRange[static_cast<signed char>(
                              m_heroRecs[m_players[player2].heroes[0]].m_cursorType)]);
            m_players[player2].heroCount++;
        }
    }

    for (player2 = 0; player2 < m_playerCount; player2++) {
        if (player2 == 0 && gbInCampaign &&
            (reinterpret_cast<unsigned char *>(this)[0x7f] != 0 ||
             reinterpret_cast<unsigned char *>(this)[0x84] != 0)) {
            if (reinterpret_cast<unsigned char *>(this)[0x7f] != 0)
                specialPortrait6 = 2;
            else
                specialPortrait6 = 5;
            for (campaignHero15 = 0; campaignHero15 < GAME_HERO_COUNT; campaignHero15++) {
                if (m_heroRecs[campaignHero15].m_cursorType == specialPortrait6 &&
                    m_availableHeroes[campaignHero15] == -1)
                    break;
            }
            if (campaignHero15 < GAME_HERO_COUNT) {
                if (reinterpret_cast<unsigned char *>(this)[0x7f] != 0) {
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
                m_players[player2].availableHeroes[0] = static_cast<char>(campaignHero15);
                m_availableHeroes[m_players[player2].availableHeroes[0]] = 64;
                heroClass5 = m_heroRecs[campaignHero15].m_cursorType;
                goto secondHero;
            }
        }
        {
            if (xIsPlayingExpansionCampaign && player2 == 0) {
                specialClass6 = 0xffffffff;
                if (xCampaign.HasAward(6)) {
                    specialClass6 = 4;
                    specialName3 = xCampaign.JosephName();
                    specialPortrait6 = 64;
                } else if (xCampaign.HasAward(7)) {
                    specialClass6 = 1;
                    specialName3 = xCampaign.IvanName();
                    specialPortrait6 = 63;
                }
                if (specialClass6 != 0xffffffff) {
                    for (campaignHero15 = 0; campaignHero15 < GAME_HERO_COUNT; campaignHero15++) {
                        if (m_heroRecs[campaignHero15].m_cursorType == specialClass6 &&
                            m_availableHeroes[campaignHero15] == -1)
                            break;
                    }
                    if (campaignHero15 < GAME_HERO_COUNT) {
                        m_heroRecs[campaignHero15].m_experience = 5000;
                        m_heroRecs[campaignHero15].CheckLevel();
                        strcpy(m_heroRecs[campaignHero15].m_name, specialName3);
                        m_heroRecs[campaignHero15].m_portrait = specialPortrait6;
                        m_players[player2].availableHeroes[0] = static_cast<char>(campaignHero15);
                        m_availableHeroes[m_players[player2].availableHeroes[0]] = 64;
                        heroClass5 = m_heroRecs[campaignHero15].m_cursorType;
                        goto secondHero;
                    }
                }
            }
            heroClass5 = Random(0, 5);
            if (m_setupPlayerRace[gcColorToSetupPos[m_players[player2].color]] < 6)
                heroClass5 = m_setupPlayerRace[gcColorToSetupPos[m_players[player2].color]];
            m_players[player2].availableHeroes[0] =
                static_cast<char>(GetNewHeroId(player2, heroClass5, 0));
            m_availableHeroes[m_players[player2].availableHeroes[0]] = 64;
        }
secondHero:
        heroClass5 = (Random(1, 5) + heroClass5) % 6;
        m_players[player2].availableHeroes[1] =
            static_cast<char>(GetNewHeroId(player2, heroClass5, 0));
        m_availableHeroes[m_players[player2].availableHeroes[1]] = 64;
    }

    for (player2 = 0; player2 < m_playerCount; player2++) {
        for (campaignHero15 = 0; campaignHero15 < m_players[player2].heroCount; campaignHero15++) {
            heroX6 = m_heroRecs[m_players[player2].heroes[campaignHero15]].m_x;
            heroY16 = m_heroRecs[m_players[player2].heroes[campaignHero15]].m_y;
            m_heroRecs[m_players[player2].heroes[campaignHero15]].m_locationType =
                m_worldMap.GetCell(heroX6, heroY16)->triggerType;
            m_heroRecs[m_players[player2].heroes[campaignHero15]].m_occupiedTown =
                m_worldMap.GetCell(heroX6, heroY16)->w4hi;
            m_worldMap.GetCell(heroX6, heroY16)->triggerType = 0xaa;
            m_worldMap.GetCell(heroX6, heroY16)->w4hi =
                m_players[player2].heroes[campaignHero15];
        }
        if (m_players[player2].heroCount > 0)
            m_players[player2].currentHero = m_players[player2].heroes[0];
        else if (m_players[player2].townCount > 0)
            m_players[player2].currentTown = m_players[player2].towns[0];
    }

    player2 = -1;
    townIndex9 = -1;
    ultimateTries4 = 0;
    ultimateDistance5 = Random(1, 20) + Random(1, 20) + Random(1, 30);
    while (player2 < 9 || townIndex9 < 9 || player2 > MAP_WIDTH - 10 ||
           townIndex9 > MAP_HEIGHT - 10 ||
           m_worldMap.GetCell(player2, townIndex9)->objIndex != 0xff ||
           m_worldMap.GetCell(player2, townIndex9)->ovlIndex != 0xff ||
           giGroundToTerrain[m_worldMap.GetCell(player2, townIndex9)->tile] == 0 ||
           (giNumHumanPlayers == 1 && ultimateTries4 < 200 &&
            ultimateDistance5 >=
                abs(player2 - m_heroRecs[m_players[0].heroes[0]].m_x) +
                abs(townIndex9 - m_heroRecs[m_players[0].heroes[0]].m_y))) {
        if (ultimateTries4 < 400 && giUABaseX > 0) {
            player2 = giUABaseX +
                (giUARadius != 0 ? Random(-giUARadius, giUARadius) : 0);
            townIndex9 = giUABaseY +
                (giUARadius != 0 ? Random(-giUARadius, giUARadius) : 0);
        } else {
            player2 = Random(9, MAP_WIDTH - 10);
            townIndex9 = Random(9, MAP_HEIGHT - 10);
        }
        ultimateDistance5 = Random(1, 20) + Random(1, 20) + Random(1, 30);
        ultimateTries4++;
    }
                m_ultimateArtifactX = static_cast<signed char>(player2);
                m_ultimateArtifactY = static_cast<signed char>(townIndex9);
                m_ultimateArtifactId = static_cast<signed char>(Random(0, 7));
                if (gbInCampaign &&
                    ((m_campaignType == 0 && static_cast<signed char>(m_campaignScenario) + 1 == 8) ||
                     (m_campaignType == 1 && static_cast<signed char>(m_campaignScenario) + 1 == 9)))
                    m_ultimateArtifactId = 6;
                for (player2 = 0; player2 < m_playerCount; player2++) {
                    if (gbHumanPlayer[player2]) {
                        m_players[player2].unknown0f = 3;
                        memcpy(m_players[player2].resources,
                               gInitResourcesHuman[m_difficulty],
                               28);
                        if (m_playerHandicap[player2] != 0) {
                            for (townIndex9 = 0; townIndex9 < 7; townIndex9++) {
                                double resourceScale;
                                if (m_playerHandicap[player2] == 1)
                                    resourceScale = 0.85;
                                else
                                    resourceScale = 0.7;
                                (m_players + player2)->resources[townIndex9] =
                                    static_cast<int>((m_players + player2)->resources[townIndex9] * resourceScale);
                            }
                        }
                    } else {
                        m_players[player2].unknown0f = Random(0, 2);
                        memcpy(m_players[player2].resources,
                               gInitResourcesComputer[m_difficulty],
                               28);
                    }
                }
                SetupAdjacentMons();
                if (m_mapHeader.lossCondition == 2) {
                    ultimateDistance5 = m_mapHeader.lossConditionValue;
                    ultimateTries4 = m_mapHeader.lossTownY;
                    m_mapHeader.lossConditionValue = 0;
                    if (m_worldMap.GetCell(ultimateDistance5, ultimateTries4)->triggerType == 0xaa)
                        m_mapHeader.lossConditionValue =
                            m_worldMap.GetCell(ultimateDistance5, ultimateTries4)->w4hi;
                    else {
                        if (m_worldMap.GetCell(ultimateDistance5, ultimateTries4 - 1)->triggerType == 0xaa)
                            m_mapHeader.lossConditionValue =
                                m_worldMap.GetCell(ultimateDistance5, ultimateTries4 - 1)->w4hi;
                        else
                            m_mapHeader.lossCondition = 0;
                    }
                }
                if (m_mapHeader.victoryCondition == 2) {
                    ultimateDistance5 = m_mapHeader.victoryConditionValue;
                    ultimateTries4 = m_mapHeader.victoryTownY;
                    m_mapHeader.victoryConditionValue = 0;
                    if (m_worldMap.GetCell(ultimateDistance5, ultimateTries4)->triggerType == 0xaa)
                        m_mapHeader.victoryConditionValue =
                            m_worldMap.GetCell(ultimateDistance5, ultimateTries4)->w4hi;
                    else {
                        if (m_worldMap.GetCell(ultimateDistance5, ultimateTries4 - 1)->triggerType == 0xaa)
                            m_mapHeader.victoryConditionValue =
                                m_worldMap.GetCell(ultimateDistance5, ultimateTries4 - 1)->w4hi;
                        else
                            m_mapHeader.victoryCondition = 0;
                    }
                }
                for (player2 = 0; player2 < m_playerCount; player2++) {
                    heroClass5 = 0;
                    if (m_setupPlayerRace[gcColorToSetupPos[m_players[player2].color]] >= 0 &&
                        m_setupPlayerRace[gcColorToSetupPos[m_players[player2].color]] < 6) {
                        heroClass5 = m_setupPlayerRace[gcColorToSetupPos[m_players[player2].color]];
                    } else {
                        if (!!m_players[player2].townCount) {
                            heroClass5 = gpGame->m_castleRecs[m_players[player2].Town(0)].m_type;
                        } else if (!!m_players[player2].heroCount) {
                            heroClass5 = gpGame->m_heroRecs[m_players[player2].Hero(0)].m_cursorType;
                        }
                    }
                    m_players[player2].evilInterface =
                        heroClass5 == 1 || heroClass5 == 3 || heroClass5 == 5;
                    if (gbInCampaign && player2 == 0)
                        m_players[player2].evilInterface = m_campaignType == 1;
                    for (townIndex9 = 0;
                         townIndex9 < gpGame->m_players[player2].townCount;
                         townIndex9++)
                        GetCastle(gpGame->m_players[player2].towns[townIndex9])->GiveSpells(0);
                    gpGame->m_players[player2].unknown0e =
                        gpGame->m_players[player2].heroCount;
                }
                gpPhilAI->GetGameAIVars();
                gbInNewGameSetup = 0;
                SetupNewRumour();
                gpAdvManager->CheckSetEvilInterface(0, -1);
                return;
}

inline townSlot *GetCastleSlot(game *instance, int index)
{
    return reinterpret_cast<townSlot *>(&instance->m_castleRecs[index]);
}

// @early-stop
// soft/TU-cumulative: frame 0x1d8 exact; base 0x25fe vs retail 0x2601. Residual is three MAP_WIDTH/xPos2 commutative compare encodings (+0x9d, +0x1db7, +0x1ee4; one byte each) and equal-length packed-w4hi RHS/cell-word evaluation order at +0x2e1; val|0, setter, union, cast, and bitfield-type spellings do not steer MSVC 4.2.
VA(0x00476448, 0x2601)
void game::RandomizeEvents(void)
{
    int shrineId8 = 1;
    int bottleId11 = 1;
    int jailId28 = 1;
    int sphinxId26 = 1;
    int tentId10 = 1;
    int hutId11 = 1;
    int eyeId13 = 1;
    int row18;
    int signId4 = 1;
    unsigned int extraIndex3;
    int yPos19;
    int xPos2;
    int j9;
    int value26;
    int randomValue7;
    int mineId2;
    int column1;
    int upperCount;
    int lowerCount16;
    int upperTilesets29[5];
    int upperIndexes1[5];
    int lowerTilesets4[5];
    int lowerIndexes7[5];
    int artifactChoices17[10];
    void *mapEvent1;
    mapCell *townEntrance;
    mapCell *cell2;
    mapCellExtra *extra15;
    townSlot *townRec4;
    unsigned char *eventData16;
    int valid27;

    *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x657b) = 0;
    memset(reinterpret_cast<unsigned char *>(this) + 0x657d, 0, 100);

    for (yPos19 = 0; yPos19 < MAP_HEIGHT; yPos19++) {
        for (xPos2 = 0; (xPos2 | 0) < MAP_WIDTH; xPos2++) {
            cell2 = m_worldMap.Row(yPos19) + xPos2;
            switch (cell2->triggerType) {
            case 0xd5:
                cell2->w4hi = 12;
                while (cell2->w4hi == 12 || cell2->w4hi == 6)
                    cell2->w4hi = Random(0, 13);
                break;
            case 0xab:
                cell2->objTileset = 0; cell2->objIndex = 0xff; cell2->w4hi = 0;
                cell2->triggerType = 0; CreateBoat(xPos2, yPos19, 1); break;
            case 0xcf:
                eventData16 = reinterpret_cast<unsigned char *>(ppMapExtra[cell2->w4hi]);
                if (strlen(reinterpret_cast<char *>(eventData16 + 0x88)) > 1 &&
                    eventData16[0x1f] >= 1)
                    eventData16[0] = 1;
                else
                    eventData16[0] = 0;
                break;
            case 0x93:
                *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x657d +
                    *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x657b) * 2) =
                    cell2->w4hi;
                mapEvent1 = ppMapExtra[cell2->w4hi];
                *reinterpret_cast<short *>(reinterpret_cast<unsigned char *>(mapEvent1) + 0x26) = static_cast<short>(xPos2);
                *reinterpret_cast<short *>(reinterpret_cast<unsigned char *>(mapEvent1) + 0x28) = static_cast<short>(yPos19);
                reinterpret_cast<unsigned char *>(mapEvent1)[0x25] = 1;
                cell2->w4hi = 0;
                cell2->triggerType = 0;
                cell2->objIndex = 0xff;
                cell2->objTileset = 0;
                (*reinterpret_cast<short *>(reinterpret_cast<unsigned char *>(this) + 0x657b))++;
                break;
            case 0x8a: cell2->w4hi = bottleId11++; break;
            case 0xbe: cell2->w4hi = jailId28++; break;
            case 0xc5:
                cell2->w4hi = sphinxId26;
                sphinxId26++;
                break;
            case 0xc9: cell2->w4hi = tentId10++; break;
            case 0xc2:
                if (xPos2 <= 0 || m_worldMap.GetCell(xPos2 - 1, yPos19)->triggerType != 0xc2)
                    cell2->w4hi = hutId11++;
                else
                    cell2->w4hi = m_worldMap.GetCell(xPos2 - 1, yPos19)->w4hi;
                break;
            case 0xd6: cell2->w4hi = signId4++; break;
            case 0x27:
                cell2->triggerType |= 0x80;
                break;
            case 0x99: cell2->w4hi = shrineId8++; break;
            case 0xda: cell2->w4hi = Random(0, 3); break;
            case 0x84:
                if (!HasObjectTilesetIndex(xPos2, yPos19, 0x37, 0x54)) {
                    cell2->triggerType &= 0x7f;
                } else if (Random(0, 9) > 2) {
                    cell2->w4hi = 1;
                } else {
                    cell2->w4hi = GetRandomArtifactId(14, 1) + 2;
                }
                break;
            case 0xd0:
                randomValue7 = Random(0, 100);
                if (randomValue7 < 40) cell2->w4hi = 0;
                else if (randomValue7 < 50)
                    cell2->w4hi = GetRandomArtifactId(12, 1) | 0x80;
                else
                    cell2->w4hi = Random(0, 5) + (Random(2, 5) << 4) + 1;
                break;
            case 0xd8:
                cell2->w4hi = Random(0, 5) + (Random(1, 4) << 4) + 1; break;
            case 0x85:
                switch (Random(0, 99) % 10) {
                case 0: case 1: case 2: cell2->w4hi = 2; break;
                case 3: cell2->w4hi = 3; break;
                case 4: case 5: case 6: cell2->w4hi = 4; break;
                case 7: case 8: case 9: cell2->w4hi = 5; break;
                }
                break;
            case 0x86:
                if (giGroundToTerrain[cell2->tile] == 0) {
                    cell2->triggerType = 0xa1;
                    randomValue7 = Random(0, 100);
                    if (randomValue7 < 20)
                        cell2->w4hi = 0;
                    else if (randomValue7 < 90)
                        cell2->w4hi = 1;
                    else
                        cell2->w4hi = GetRandomArtifactId(8, 1) | 0x100;
                } else {
                    randomValue7 = Random(0, 100);
                    if (randomValue7 < 32)
                        cell2->w4hi = 2;
                    else if (randomValue7 < 64)
                        cell2->w4hi = 3;
                    else if (randomValue7 < 95)
                        cell2->w4hi = 4;
                    else
                        cell2->w4hi = GetRandomArtifactId(8, 1) | 0x100;
                }
                break;
            case 0x88:
                cell2->w4hi = Random(4, 6) << 4;
                cell2->w4hi |= Random(0, 5);
                break;
            case 0x8b: cell2->w4hi = Random(0, 2) + 2; break;
            case 0xdc:
                randomValue7 = Random(0, 100);
                if (randomValue7 < 60)
                    cell2->w4hi = GetRandomArtifactId(8, 1);
                else if (randomValue7 < 80)
                    cell2->w4hi = GetRandomArtifactId(4, 1);
                else
                    cell2->w4hi = GetRandomArtifactId(2, 1);
                break;
            case 0x8c: case 0xa0: case 0xdb:
                switch (Random(0, 99) % 10) {
                case 0: case 1: case 2: cell2->w4hi = 2; break;
                case 3: case 4: case 5: cell2->w4hi = 3; break;
                case 6: case 7: case 8: cell2->w4hi = 4; break;
                case 9: cell2->w4hi = 5; break;
                }
                break;
            case 0x8d: cell2->w4hi = Random(10, 25); break;
            case 0x8e: cell2->w4hi = Random(15, 40); break;
            case 0x8f: cell2->w4hi = Random(0, 20) + 1; break;
            case 0x90: cell2->w4hi = Random(0, 40) + 1; break;
            case 0x91: cell2->w4hi = Random(20, 50); break;
            case 0x96: cell2->w4hi = 1; break;
            case 0xd2: cell2->w4hi = 1; break;
            case 0xdf:
                cell2->w4hi = Random(0, 1) == 0 ? 6 : 7; break;
            case 0xc4:
                cell2->w4hi = (Random(1, 3) << 6) | eyeId13++; break;
            case 0x98:
                if (cell2->w4hi == 0) {
                    cell2->w4hi = GetRandomNumTroops(cell2->objIndex);
                    if (cell2->objIndex != 59 && cell2->objIndex != 62 && cell2->objIndex != 63 &&
                        cell2->objIndex != 64 && cell2->objIndex != 65 && Random(0, 100) < 20)
                        cell2->w4hi |= 0x1000;
                }
                break;
            case 0x9b:
                cell2->w4hi = cell2->objIndex >> 1;
                switch (cell2->w4hi) {
                case 0:
                case 2:
                    cell2->w4hi = Random(5, 10);
                    break;
                case 6:
                    cell2->w4hi = Random(5, 10);
                    break;
                default:
                    cell2->w4hi = Random(3, 6);
                    break;
                }
                break;
            case 0x9f:
                cell2->w4hi = Random(0, 64) + 1;
                while (reinterpret_cast<unsigned char *>(gMonsterDatabase)[
                    (cell2->w4hi - 1) * 22 + 0xfd1] != 1) {
                    cell2->w4hi = Random(0, 64) + 1;
                }
                break;
            case 0xca:
                cell2->w4hi = Random(0, 64) + 1;
                while (reinterpret_cast<unsigned char *>(gMonsterDatabase)[
                    (cell2->w4hi - 1) * 22 + 0xfd1] != 2) {
                    cell2->w4hi = Random(0, 64) + 1;
                }
                break;
            case 0xcb:
                cell2->w4hi = Random(0, 64) + 1;
                while (reinterpret_cast<unsigned char *>(gMonsterDatabase)[
                    (cell2->w4hi - 1) * 22 + 0xfd1] != 3) {
                    cell2->w4hi = Random(0, 64) + 1;
                }
                break;
            case 0xcc:
                cell2->w4hi = Random(0, 64) + 1;
                while (reinterpret_cast<unsigned char *>(gMonsterDatabase)[
                    (cell2->w4hi - 1) * 22 + 0xfd1] != 5) {
                    cell2->w4hi = Random(0, 64) + 1;
                }
                break;
            case 0xbb: cell2->w4hi = Random(15, 25); break;
            case 0xc1: cell2->w4hi = Random(10, 20); break;
            case 0xba: cell2->w4hi = Random(7, 10); break;
            case 0xbd: cell2->w4hi = Random(3, 5); break;
            case 0xbc: cell2->w4hi = Random(20, 40); break;
            case 0xc8: cell2->w4hi = Random(20, 40); break;
            case 0xd3: cell2->w4hi = Random(4, 6) | 0x100; break;
            case 0xcd: cell2->w4hi = Random(4, 6) | 0x100; break;
            case 0x94: cell2->w4hi = 0x102; break;
            case 0xd7: cell2->w4hi = Random(10, 20); break;
            case 0xce: cell2->w4hi = Random(10, 25); break;
            case 0xa2: cell2->w4hi = Random(10, 20); break;
            case 0xa5:
                if (!HasObjectTilesetIndex(xPos2, yPos19, 0x29, 0x81))
                    cell2->triggerType &= 0x7f;
                else
                    cell2->w4hi = Random(30, 50);
                break;
            case 0xa9:
                randomValue7 = Random(0, 99);
                value26 = cell2->objIndex >> 1;
                if (value26 != 0x56) {
                    if (randomValue7 < 60) {
                        if (randomValue7 % 10 == 1) cell2->w4hi = 4;
                        else if (randomValue7 % 10 == 2) cell2->w4hi = 5;
                        else cell2->w4hi = 1;
                    } else if (randomValue7 < 80) {
                        if (gArtifactLevel[value26] == 8) cell2->w4hi = 3;
                        else if (gArtifactLevel[value26] == 4)
                            cell2->w4hi = (Random(0, 5) << 4) | 6;
                        else if (gArtifactLevel[value26] == 2)
                            cell2->w4hi = (Random(0, 5) << 4) | 7;
                    } else {
                        artifactChoices17[6] = 9; artifactChoices17[7] = 10;
                        artifactChoices17[8] = 19; artifactChoices17[9] = 60;
                        artifactChoices17[0] = 35; artifactChoices17[1] = 36;
                        artifactChoices17[2] = 37; artifactChoices17[3] = 56;
                        artifactChoices17[4] = 45; artifactChoices17[5] = 46;
                        cell2->w4hi = 1;
                        if (gArtifactLevel[value26] == 8)
                            cell2->w4hi |= 0x39;
                        else if (gArtifactLevel[value26] == 4)
                            cell2->w4hi |= artifactChoices17[Random(0, 3) + 6];
                        else
                            cell2->w4hi |= artifactChoices17[Random(0, 5)];
                    }
                }
                break;
            case 0xa3:
                mineId2 = GetTownId(xPos2, yPos19);
                for (row18 = yPos19 - 2; row18 <= yPos19 + 1; row18++) {
                    for (column1 = xPos2 - 2; column1 <= xPos2 + 2; column1++) {
                        if (m_worldMap.GetCell(column1, row18)->w4hi != 0)
                            continue;
                        m_worldMap.GetCell(column1, row18)->w4hi = mineId2;
                    }
                }
                townRec4 = GetCastleSlot(this, mineId2);
                townRec4->unknown7 = -1;
                townRec4->unknown6 = townRec4->unknown7;
                if (yPos19 <= MAP_HEIGHT - 3) {
                    townEntrance = gpAdvManager->GetCell(xPos2 - 1, yPos19 + 2);
                    if (giGroundToTerrain[townEntrance->tile] == 0) {
                        townRec4->unknown6 = static_cast<signed char>(xPos2 - 1);
                        townRec4->unknown7 = static_cast<signed char>(yPos19 + 2);
                    } else {
                        townEntrance = gpAdvManager->GetCell(xPos2 + 1, yPos19 + 2);
                        if (giGroundToTerrain[townEntrance->tile] == 0) {
                            townRec4->unknown6 = static_cast<signed char>(xPos2 + 1);
                            townRec4->unknown7 = static_cast<signed char>(yPos19 + 2);
                        }
                    }
                }
                break;
            case 0x95: {
                m_worldMap.GetCell(xPos2, yPos19)->w4hi = GetMineId(xPos2, yPos19);
                break;
            }
            case 0xc0:
                mineId2 = GetMineId(xPos2, yPos19);
                m_mines[mineId2].guardianType = 59;
                m_mines[mineId2].guardianCount = static_cast<unsigned char>(Random(30, 60));
            case 0x81: case 0x97: case 0x9d:
                mineId2 = GetMineId(xPos2, yPos19);
                for (row18 = yPos19 - 1; row18 <= yPos19; row18++) {
                    for (column1 = xPos2 - 2; column1 <= xPos2 + 1; column1++) {
                        if (column1 == xPos2 - 2 && cell2->triggerType != 0x81)
                            continue;
                        if (m_worldMap.GetCell(column1, row18)->w4hi == 0 ||
                            ((m_worldMap.GetCell(column1, row18)->triggerType & 0x7f) ==
                             (cell2->triggerType & 0x7f)))
                            m_worldMap.GetCell(column1, row18)->w4hi = mineId2;
                    }
                }
                break;
            case 0xa8: cell2->w4hi = Random(1, 5); break;
            case 0xf7: RandomizeBarrier(cell2); break;
            case 0xf8: RandomizePassword(cell2); break;
            case 0xfa: WeeklyGenericSite(cell2); break;
            case 0xf9: WeeklyRecruitSite(cell2); break;
            }
        }
    }


    for (yPos19 = 0; yPos19 < MAP_HEIGHT; yPos19++) {
        for (xPos2 = 0; (xPos2 | 0) < MAP_WIDTH; xPos2++) {
            cell2 = m_worldMap.Row(yPos19) + xPos2;
            if (cell2->objIndex != 0xff && cell2->w4b) {
                valid27 = 1;
                extraIndex3 = cell2->extra;
                while (extraIndex3 != 0) {
                    extra15 = m_worldMap.Extra(extraIndex3);
                    if (extra15->objIndex != 0xff && !extra15->f4b)
                        valid27 = 0;
                    extraIndex3 = extra15->index;
                }
                if (valid27)
                    cell2->field8 |= 0x80;
            }
        }
    }

    for (yPos19 = 0; yPos19 < MAP_HEIGHT; yPos19++) {
        for (xPos2 = 0; (xPos2 | 0) < MAP_WIDTH; xPos2++) {
            cell2 = m_worldMap.Row(yPos19) + xPos2;
            if ((cell2->triggerType & 0x7f) == 0x67 && cell2->objTileset == 0x3e)
                cell2->field8 |= 8;
            if (cell2->objIndex != 0xff && !(cell2->triggerType & 0x80) &&
                !(cell2->field8 & 0x80) && cell2->ovlIndex != 0xff)
                cell2->field8 |= 8;
            upperCount = 0;
            lowerCount16 = 0;
            if (!(cell2->field8 & 8) && yPos19 < MAP_HEIGHT - 1 &&
                cell2->objIndex != 0xff && !(cell2->triggerType & 0x80) &&
                !(cell2->field8 & 0x80)) {
                mapCell *below0;
                if (m_worldMap.GetCell(xPos2, yPos19 + 1)->objIndex != 0xff &&
                    !(m_worldMap.GetCell(xPos2, yPos19 + 1)->triggerType & 0x80) &&
                    !(m_worldMap.GetCell(xPos2, yPos19 + 1)->field8 & 0x80)) {
                    if (!cell2->w4b) {
                        upperTilesets29[upperCount] = cell2->objTileset;
                        upperIndexes1[upperCount] = cell2->objIndex;
                        upperCount++;
                    }
                    if (cell2->extra != 0)
                        extra15 = m_worldMap.Extra(cell2->extra);
                    else
                        extra15 = 0;
                    while (upperCount < 5 && extra15 != 0) {
                        if (extra15->objIndex != 0xff && !extra15->f4b) {
                            upperTilesets29[upperCount] = extra15->objTileset;
                            upperIndexes1[upperCount] = extra15->objIndex;
                            upperCount++;
                        }
                        if (extra15->index != 0)
                            extra15 = m_worldMap.Extra(extra15->index);
                        else
                            extra15 = 0;
                    }
                    below0 = m_worldMap.GetCell(xPos2, yPos19 + 1);
                    if (!below0->w4b) {
                        lowerTilesets4[lowerCount16] = below0->objTileset;
                        lowerIndexes7[lowerCount16] = below0->objIndex;
                        lowerCount16++;
                    }
                    if (below0->extra != 0)
                        extra15 = m_worldMap.Extra(below0->extra);
                    else
                        extra15 = 0;
                    while (lowerCount16 < 5 && extra15 != 0) {
                        if (extra15->objIndex != 0xff && !extra15->f4b) {
                            lowerTilesets4[lowerCount16] = extra15->objTileset;
                            lowerIndexes7[lowerCount16] = extra15->objIndex;
                            lowerCount16++;
                        }
                        if (extra15->index != 0)
                            extra15 = m_worldMap.Extra(extra15->index);
                        else
                            extra15 = 0;
                    }
                    for (randomValue7 = 0; randomValue7 < upperCount; randomValue7++) {
                        for (j9 = 0; lowerCount16 > j9; j9++) {
                            if (lowerTilesets4[j9] == upperTilesets29[randomValue7] ||
                                (upperTilesets29[randomValue7] >= 35 && upperTilesets29[randomValue7] <= 38 &&
                                 lowerTilesets4[j9] >= 35 && lowerTilesets4[j9] <= 38))
                                cell2->field8 |= 8;
                        }
                    }
                }
            }
            if (yPos19 < MAP_HEIGHT - 1) {
                if (m_worldMap.GetCell(xPos2, yPos19 + 1)->triggerType == 0xa3 ||
                    m_worldMap.GetCell(xPos2, yPos19 + 1)->triggerType == 0xb0 ||
                    m_worldMap.GetCell(xPos2, yPos19 + 1)->triggerType == 0xb1)
                    cell2->field8 |= 8;
            }
            if (cell2->objIndex != 0xff && !(cell2->triggerType & 0x80) &&
                !(cell2->field8 & 0x80) &&
                (yPos19 == MAP_HEIGHT - 1 || (m_worldMap.Row(yPos19 + 1)[xPos2].field8 & 4)))
                cell2->field8 |= 8;
        }
    }

}

VA(0x00478a49, 0xa1)
void game::InitializePasswords(void)
{
    char flag;
    int i;
    int j;
    for (i = 0; i < 8; i++) {
        flag = 0;
        while (flag == 0) {
            xPasswordStringsIndex[i] = Random(0, 0xd2);
            flag = 1;
            for (j = 0; j < i; j++) {
                if (xPasswordStringsIndex[j] == xPasswordStringsIndex[i])
                    flag = 0;
            }
        }
    }
}

VA(0x00478aea, 0x64)
void game::RandomizeBarrier(mapCell *cell)
{
    int idx = cell->w4hi;
    idx &= 7;
    int pass = xPasswordStringsIndex[idx];
    int color = (pass << 3) | idx;
    cell->w4hi = color | 0;
}

VA(0x00478b4e, 0x24)
void game::RandomizePassword(mapCell *cell)
{
    RandomizeBarrier(cell);
}

// @early-stop
// reloc-masked: identical 0x478-byte code/frame; all residuals are compiler literal/source-line constant symbol identities
VA(0x00478b72, 0x478)
int game::LoadMap(char *filename)
{
    char column[4];
    int i;
    int file;
    char row[4];
    char type[8];
    char trailer[8];

    sprintf(gText, "%s%s", gcMapPath, filename);
    file = _open(gText, 0x8000);
    if (file == -1)
        FileError(gText);
    _read(file, &m_mapHeader, sizeof(m_mapHeader));
    m_worldMap.Read(file, 1);
    SetMapSize(m_worldMap.width, m_worldMap.height);

    for (i = 0; i < GAME_TOWN_COUNT; i++) {
        _read(file, column, 1);
        _read(file, row, 1);
        _read(file, type, 1);
        if (static_cast<unsigned char>(column[0]) != 0xff) {
            m_castleRecs[i].m_formation = TOWN_FORMATION_GROUPED;
            m_castleRecs[i].m_x = static_cast<unsigned char>(column[0]);
            m_castleRecs[i].m_y = static_cast<unsigned char>(row[0]);
            m_castleRecs[i].m_type = static_cast<signed char>(type[0] & 0x7f);
            if (type[0] < 0)
                m_castleRecs[i].m_buildings |= 0x40;
            else
                m_castleRecs[i].m_buildings |= 0x20;
        }
    }

    for (i = 0; i < GAME_MINE_COUNT; i++) {
        if (m_mapHeader.magic == MAP_HEADER_MAGIC_BASE_GAME && i >= GAME_TOWN_COUNT) {
            column[0] = -1;
            row[0] = -1;
            type[0] = -1;
        } else {
            _read(file, column, 1);
            _read(file, row, 1);
            _read(file, type, 1);
        }
        if (static_cast<unsigned char>(column[0]) != 0xff) {
            m_mines[i].guardianType = -1;
            m_mines[i].x = static_cast<unsigned char>(column[0]);
            m_mines[i].y = static_cast<unsigned char>(row[0]);
            m_mines[i].resourceType = type[0];
        }
    }

    m_mapHeader.magic = MAP_HEADER_MAGIC_EXPANSION_GAME;
    _read(file, &m_obeliskCount, 1);
    _read(file, reinterpret_cast<unsigned char *>(this) + 0x64d9,
          m_mapHeader.rumourCount * 2);
    *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x64d7) =
        m_mapHeader.rumourCount;
    _read(file, reinterpret_cast<unsigned char *>(this) + 0x6517,
          m_mapHeader.timeEventCount * 2);
    *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x6515) =
        m_mapHeader.timeEventCount;
    _read(file, &iMaxMapExtra, 4);
    ppMapExtra = reinterpret_cast<void **>(BaseAlloc(iMaxMapExtra * 4, GFILE, GMAPLINE + 0x59));
    pwSizeOfMapExtra = reinterpret_cast<short *>(BaseAlloc(iMaxMapExtra * 2, GFILE, GMAPLINE + 0x5a));
    memset(ppMapExtra, 0, iMaxMapExtra * 4);
    memset(pwSizeOfMapExtra, 0, iMaxMapExtra * 2);
    for (i = 1; (&i)[0] < iMaxMapExtra; i++) {
        _read(file, pwSizeOfMapExtra + i, 2);
        ppMapExtra[i] = BaseAlloc(pwSizeOfMapExtra[i], GFILE, GMAPLINE + 0x62);
        _read(file, ppMapExtra[i], pwSizeOfMapExtra[i]);
    }
    _read(file, trailer, 2);
    _close(file);
    return 0;
}

// @early-stop
// reloc-masked: only two retail /Ob1 GetCell continuation jmps differ (5 bytes each)
VA(0x00478fea, 0x3aa)
void game::ClaimTown(int townId, int player, int suppressVisibility)
{
    int i;
    townSlot *townRec;
    mapCell *cell;

    if (!gbInNewGameSetup)
        SendMapChange(7, static_cast<signed char>(townId), 0, 0, player, 0, 0);
    townRec = reinterpret_cast<townSlot *>(&m_castleRecs[townId]);
    if (townRec->owner == player)
        return;
    townRec->unknown38 = 0;
    if (m_castleOwners[townId] != -1)
        GetCastle(townId)->Deallocate();
    for (i = 0; i < 5; i++) {
        townRec->army[i] = -1;
        *reinterpret_cast<short *>(townRec->army + 5 + i * 2) = 0;
    }
    m_castleRecs[townId].m_unknown55 = m_castleRecs[townId].m_owner == -1 ? 2 : 0;
    m_castleRecs[townId].m_owner = static_cast<signed char>(player);
    m_castleOwners[townId] = static_cast<signed char>(player);
    m_players[player].towns[m_players[player].townCount] = static_cast<signed char>(townId);
    m_players[player].townCount++;

    cell = m_worldMap.GetCell(m_castleRecs[townId].m_x - 1, m_castleRecs[townId].m_y);
    m_worldMap.ChangeTilesetIndex(cell, m_castleRecs[townId].m_x - 1,
                                 m_castleRecs[townId].m_y, 14,
                                 m_players[static_cast<signed char>(player)].color * 2, 1, -1);
    cell = m_worldMap.GetCell(m_castleRecs[townId].m_x + 1, m_castleRecs[townId].m_y);
    m_worldMap.ChangeTilesetIndex(cell, m_castleRecs[townId].m_x + 1,
                                 m_castleRecs[townId].m_y, 14,
                                 m_players[static_cast<signed char>(player)].color * 2 + 1, 1, -1);
    if (suppressVisibility != 0)
        return;
    SetVisibility(m_castleRecs[townId].m_x, m_castleRecs[townId].m_y,
                  player, giVisRangeTown);
    CheckEndGame(0, 0);
}

// @early-stop
// reloc-masked: residual is the /Ob1 Row continuation plus delinked switch local-label identity
VA(0x00479394, 0x4c2)
void game::ClaimMine(int mineId, int player)
{
    mapCell *acc;
    int flag;
    unsigned int x;
    unsigned int y;

    SendMapChange(8, static_cast<signed char>(mineId), 0, 0, player, 0, 0);
    m_mines[mineId].owner = static_cast<signed char>(player);
    m_mineOwners[mineId] = static_cast<signed char>(player);
    switch (m_mines[mineId].resourceType) {
    case 101: flag = 35; break;
    case 100: flag = 42; break;
    case 0: flag = 28; break;
    case 1: flag = 21; break;
    default: flag = 14; break;
    }
    switch (m_mines[mineId].resourceType) {
    case 1: x = m_mines[mineId].x; y = m_mines[mineId].y - 1; break;
    case 0: x = m_mines[mineId].x + 1; y = m_mines[mineId].y - 1; break;
    case 101: x = m_mines[mineId].x - 1; y = m_mines[mineId].y - 3; break;
    case 100: x = m_mines[mineId].x; y = m_mines[mineId].y; break;
    default: x = m_mines[mineId].x; y = m_mines[mineId].y; break;
    }
    acc = m_worldMap.Row(y) + x;
    if (player == -1) {
        m_worldMap.ChangeTilesetIndex(acc, x, y, 14, 255, 1, -1);
    } else {
        m_worldMap.ChangeTilesetIndex(acc, x, y, 14,
                                     m_players[static_cast<signed char>(player)].color + flag,
                                     1, -1);
        if (m_mines[mineId].resourceType == 1) {
            ConvertFlagToLateOverlay(x, y);
        } else if (y > 0 &&
                   (m_mines[mineId].resourceType == 2 || m_mines[mineId].resourceType == 6 ||
                    m_mines[mineId].resourceType == 5 || m_mines[mineId].resourceType == 3 ||
                    m_mines[mineId].resourceType == 4) &&
                   HasLateOverlay(x, y - 1)) {
            ConvertFlagToLateOverlay(x, y);
        }
    }
}

// @early-stop
// Relocation-masked comparison is identical for the full 0x1e2-byte span;
// both objects contain the same 16 relocation sites and objdiff reports 100%.
VA(0x00479856, 0x1e2)
int game::ViewSpells(hero *spellHero, int spellType,
                     int (*callback)(tag_message &), int readOnly)
{
    tag_message message;

    viewSpellsHero = spellHero;
    m_viewSpell = -1;
    if (spellHero->GetNumSpells(spellType) == 0) {
        NormalDialog(const_cast<char *>("No spells to cast."), 1,
                     -1, -1, -1, 0, -1, 0, -1, 0);
    } else {
        m_viewSpellsCallback = callback;
        m_viewSpellsReadOnly = static_cast<signed char>(readOnly);
        m_viewSpellsHero = spellHero;
        if (spellType == 2)
            m_viewSpellsType = 1;
        else
            m_viewSpellsType = spellType;
        m_viewSpellsTop[0] = 0;
        m_viewSpellsCount[0] = spellHero->GetNumSpells(0);
        m_viewSpellsTop[1] = 0;
        m_viewSpellsCount[1] = spellHero->GetNumSpells(1);
        m_viewSpellsWindow = new heroWindow(
            0x56, 0x57, const_cast<char *>("spellwin.bin"));
        if (m_viewSpellsWindow == 0)
            MemError();
        if (spellType != 2) {
            message.type = 0x200;
            message.field4 = 6;
            if (spellType == 0)
                message.field8 = 4;
            else
                message.field8 = 5;
            message.field18 = 6;
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
void game::UpdateSpellWidgets(void)
{
    tag_message message9;
    int spellSlot6;
    int spellPoints0;
    int spell2;
    int lineLength0;

    message9.type = 0x200;
    spellPoints0 = m_viewSpellsHero->m_spellPoints;
    if (spellPoints0 > 999)
        spellPoints0 = 999;
    message9.field18 = 6;
    if (spellPoints0 > 99)
        message9.field4 = 5;
    else
        message9.field4 = 6;
    message9.field8 = 7;
    m_viewSpellsWindow->BroadcastMessage(message9);
    if (spellPoints0 > 9)
        message9.field4 = 5;
    else
        message9.field4 = 6;
    message9.field8 = 8;
    m_viewSpellsWindow->BroadcastMessage(message9);

    sprintf(gText, "%d", (spellPoints0 / 100) % 10);
    message9.text = gText;
    message9.field4 = 3;
    message9.field8 = 7;
    m_viewSpellsWindow->BroadcastMessage(message9);
    sprintf(gText, "%d", (spellPoints0 / 10) % 10);
    message9.text = gText;
    message9.field4 = 3;
    message9.field8 = 8;
    m_viewSpellsWindow->BroadcastMessage(message9);
    sprintf(gText, "%d", spellPoints0 % 10);
    message9.text = gText;
    message9.field4 = 3;
    message9.field8 = 9;
    m_viewSpellsWindow->BroadcastMessage(message9);

    for (spellSlot6 = 0; spellSlot6 < 12; spellSlot6++) {
        if (m_viewSpellsTop[m_viewSpellsType] + spellSlot6 >=
            m_viewSpellsCount[m_viewSpellsType]) {
            message9.field4 = 6;
            message9.field8 = spellSlot6 + 100;
            message9.field18 = 6;
            m_viewSpellsWindow->BroadcastMessage(message9);
            message9.field8 = spellSlot6 + 30;
            m_viewSpellsWindow->BroadcastMessage(message9);
        } else {
            spell2 = m_viewSpellsHero->GetNthSpell(
                m_viewSpellsType,
                m_viewSpellsTop[m_viewSpellsType] + spellSlot6 + 1);
            message9.field4 = 8;
            message9.field8 = spellSlot6 + 30;
            if (GetManaCost(spell2, m_viewSpellsHero) >
                m_viewSpellsHero->m_spellPoints)
                message9.field18 = 3;
            else
                message9.field4 = 1;
            m_viewSpellsWindow->BroadcastMessage(message9);
            message9.field4 = 5;
            message9.field18 = 6;
            m_viewSpellsWindow->BroadcastMessage(message9);
            message9.field8 = spellSlot6 + 100;
            m_viewSpellsWindow->BroadcastMessage(message9);
            if (m_viewSpellsReadOnly) {
                message9.field4 = 5;
                message9.field18 = 2;
                m_viewSpellsWindow->BroadcastMessage(message9);
            }
            message9.field4 = 4;
            message9.field18 = gsSpellInfo[spell2].iconIndex;
            m_viewSpellsWindow->BroadcastMessage(message9);
            lineLength0 = smallFont->LineLength(gSpellNames[spell2], 78);
            if (lineLength0 == 1) {
                sprintf(gText, "%s\n[%d]", gSpellNames[spell2],
                        GetManaCost(spell2, m_viewSpellsHero));
            } else {
                sprintf(gText, "%s [%d]", gSpellNames[spell2],
                        GetManaCost(spell2, m_viewSpellsHero));
            }
            message9.field4 = 3;
            message9.field8 = spellSlot6 + 30;
            message9.text = gText;
            m_viewSpellsWindow->BroadcastMessage(message9);
        }
    }
}

// @early-stop
// Exact 0x692-byte span and 101 relocation sites. The live residual is the
// commutative equality at +0x14: retail loads the window-manager field before
// msg.field8, while this TU loads msg.field8 first. Both source operand orders
// emit the same sequence; this is an /Od TU-cumulative evaluation-order choice.
VA(0x00479e3b, 0x692)
int ViewSpellsHandler(tag_message &msg)
{
    int spell;

    if (msg.type == 4) {
        gpWindowManager->ConvertToHover(msg);
        if (msg.field8 == gpWindowManager->field_0x5e) {
            return 1;
        } else {
            return gpGame->m_viewSpellsCallback(msg);
        }
    }
    if (msg.type == 0x200) {
      switch (msg.field4) {
    case 13:
        if (msg.field4 == 14 || (msg.fieldC & 0x200) != 0)
            break;
        {
            switch (msg.field8) {
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
                if (gpGame->m_viewSpellsTop[gpGame->m_viewSpellsType] + 12 <
                    gpGame->m_viewSpellsCount[gpGame->m_viewSpellsType])
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
            case 0x7800:
                msg.field8 = 10;
                break;
            }
        }
        break;
    case 12:
    case 14:
        if (msg.field4 == 14 || (msg.fieldC & 0x200) != 0) {
            switch (msg.field8) {
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
                    gpGame->m_viewSpellsTop[gpGame->m_viewSpellsType] +
                        (msg.field8 - 100) + 1);
                NormalDialog(gSpellDesc[spell], 4, -1, -1, 8, spell, -1, 0, -1, 0);
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
          switch (msg.field8) {
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
                gpGame->m_viewSpellsTop[gpGame->m_viewSpellsType] +
                    (msg.field8 - 100) + 1);
            if (gpGame->m_viewSpellsReadOnly) {
                NormalDialog(gSpellDesc[spell], 1, -1, -1, 8, spell, -1, 0, -1, 0);
                return 1;
            }
            if (GetManaCost(spell, viewSpellsHero) > viewSpellsHero->m_spellPoints) {
                sprintf(gText,
                        "That spell costs %d mana.  You only have %d mana, so you can't cast the spell.",
                        GetManaCost(spell, viewSpellsHero), viewSpellsHero->m_spellPoints);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                return 0;
            }
            gpGame->m_viewSpell = spell;
            msg.field4 = 10;
            return 2;
          }
        }
        break;
    default:
        break;
      }

      if (msg.field8 == 10) {
          msg.field4 = msg.field8;
          return 2;
      }
    }
    return 1;
}

// @early-stop
// Manual relocation audit finds 35 sites in both objects. This body is 0x17d versus
// retail's 0x17c solely because the commutative hover equality loads msg first into eax
// (15 bytes) instead of the window manager first into eax (14 bytes); all later blocks
// and the jump table realign. Both operand orders and exhaustive AST variants emit alike.
VA(0x0047a4cd, 0x17c)
int ViewSpecialHandler(tag_message &msg)
{
    if (msg.type == 4) {
        if (gpWindowManager->field_0x5e == msg.field8)
            return 1;
        gpWindowManager->field_0x5e = msg.field8;
        switch (msg.field8) {
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
        case 0x7800:
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

// @early-stop
// reloc-masked: identical 0xc86-byte code/frame; the dwelling-table relocation resolves to
// retail's folded 0x4fca35 symbol (gDwellingType at 0x4fca48 plus the -0x13 addend).
VA(0x0047a649, 0xc86)
void game::ViewArmy(int x, int y, int monsterType, int numTroops, town *castle,
                    int disableUpgrade, int facing, int quickView, hero *theHero,
                    class army *theArmy, armyGroup *theGroup, int groupIndex)
{
    short baseX7 = 86;
    short quickBaseY3 = 164;
    short blankWidget3 = 1;
    short numWidget15 = 2;
    short titleMessage15 = 3;
    short detailMessage2 = 4;
    short frame18 = 5;
    int loopIndex0;
    tag_message message6;
    message6.type = 0x200;

    iViewArmyFrame = 0;
    iViewArmyType = monsterType;
    iViewArmyNumTroops = numTroops;
    gbAllowUpgrade = 0;

    if (castle && (gpAdvManager->m_active == 1 || gpTownManager->m_active == 1)) {
        for (loopIndex0 = 20; loopIndex0 <= 24; loopIndex0++) {
            if (gDwellingType[static_cast<signed char>(castle->m_type)][loopIndex0 - 19] == monsterType &&
                (castle->m_buildings & (1 << (loopIndex0 + 5)))) {
                gbAllowUpgrade = 1;
                iViewArmyUpgradeToType = monsterType + 1;
            }
        }
        if ((monsterType == 35 || monsterType == 36) &&
            (castle->m_buildings & 0x40000000)) {
            gbAllowUpgrade = 1;
            iViewArmyUpgradeToType = 37;
        }
    }

    tag_monsterInfo *monster8 = &gMonsterDatabase[monsterType];
    char *armyMonster11;
    if (theArmy)
        armyMonster11 = reinterpret_cast<char *>(theArmy) + 0xba;
    else
        armyMonster11 = reinterpret_cast<char *>(&gMonsterDatabase[monsterType]);

    x = 19;
    y = 75;
    m_viewArmyWindow = new heroWindow(x, y, const_cast<char *>("armywin.bin"));
    if (!m_viewArmyWindow)
        MemError();

    viewArmyFacingWIPXMod = facing == 1 ? -1 : 1;
    gpResourceManager->PointToFile(
        gpResourceManager->MakeId(cArmyFrameFileNames[monsterType], 1));
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(&sViewArmyMonFrameInfo), 0x335);
    ModifyFrameInfo(&sViewArmyMonFrameInfo, monsterType);
    BuildTempWalkSeq(&sViewArmyMonFrameInfo, 0, 1);

    viewArmyBaseX = 167;
    char filename4[16];
    if (gbLowMemory)
        sprintf(filename4, "monh%04d.icn", monsterType);
    else
        strcpy(filename4, cMonFilename[monsterType]);

    icon *monsterIcon5 = gpResourceManager->GetIcon(filename4);
    int iconFrame15 = static_cast<signed char>(VIEW_ARMY_FRAMES[0]);
    viewArmyBaseX += (GetIconEntry(monsterIcon5, iconFrame15)->w / 2) * viewArmyFacingWIPXMod;
    viewArmyBaseX += GetIconEntry(monsterIcon5, iconFrame15)->x * viewArmyFacingWIPXMod +
                     static_cast<signed char>(VIEW_ARMY_FRAME_OFFSETS[0]) * viewArmyFacingWIPXMod;
    viewArmyBaseY = 138;
    viewArmyBaseY += GetIconEntry(monsterIcon5, iconFrame15)->h / 2;
    if (gbLowMemory) {
        viewArmyBaseX = 126;
        viewArmyBaseY = 93;
    }

    iconWidget *monsterWidget7 = new iconWidget(
        static_cast<short>(viewArmyBaseX), static_cast<short>(viewArmyBaseY), 86, 149,
        filename4, gbLowMemory ? 0 : static_cast<signed char>(VIEW_ARMY_FRAMES[0]),
        facing == 0, 5, 16, 1);
    if (!monsterWidget7)
        MemError();
    m_viewArmyWindow->AddWidget(monsterWidget7, -1);
    gpResourceManager->Dispose(monsterIcon5);

    char armyName8[32];
    strcpy(armyName8, gArmyNames[monsterType]);
    armyName8[0] -= 32;
    message6.field4 = 3;
    message6.field8 = 3;
    message6.text = armyName8;
    m_viewArmyWindow->BroadcastMessage(message6);

    char *details9 = static_cast<char *>(BaseAlloc(0x226, GFILE, GSAVELINE + 0x93));
    int morale2 = theGroup ? theGroup->GetMorale(theHero, castle, 0) : 0;
    if (monster8->flags.all & MONSTER_FLAGS_NO_MORALE)
        morale2 = 0;

    sprintf(details9, "");
    int modifier15 = 0;
    sprintf(gText, "%s%d", cArmyDetail[0], static_cast<int>(monster8->attack));
    strcat(details9, gText);
    if (theHero)
        modifier15 += theHero->Stats(0);
    if (theArmy)
        modifier15 = theArmy->m_monster.attack - monster8->attack;
    if (modifier15) {
        sprintf(gText, " (%d)", monster8->attack + modifier15);
        strcat(details9, gText);
    }

    modifier15 = 0;
    sprintf(gText, "\n%s%d", cArmyDetail[1], static_cast<int>(monster8->defense));
    strcat(details9, gText);
    if (theHero)
        modifier15 += theHero->Stats(1);
    if (theArmy)
        modifier15 = theArmy->m_monster.defense - monster8->defense;
    if (modifier15) {
        sprintf(gText, " (%d)", monster8->defense + modifier15);
        strcat(details9, gText);
    }

    if (monster8->flags.all & MONSTER_FLAGS_SHOOTER) {
        int shots8 = static_cast<signed char>(armyMonster11[0x10]);
        if (shots8 > 0) {
            if (gpCombatManager->m_active == 1)
                sprintf(gText, "\n%s%d", cArmyDetail[2], shots8);
            else
                sprintf(gText, "\n%s%d", cArmyDetail[8], shots8);
            strcat(details9, gText);
        }
    }

    sprintf(gText, "\n%s%d", cArmyDetail[3], static_cast<int>(monster8->damageMin));
    strcat(details9, gText);
    if (monster8->damageMin != monster8->damageMax) {
        sprintf(gText, "-%d", static_cast<int>(monster8->damageMax));
        strcat(details9, gText);
    }
    sprintf(gText, "\n%s%d", cArmyDetail[4], static_cast<unsigned int>(monster8->hitPoints));
    strcat(details9, gText);
    if (gpCombatManager->m_active == 1) {
        sprintf(gText, "\n%s%d", "Hit Points Left: ",
                static_cast<unsigned int>(monster8->hitPoints) - theArmy->m_hitPointsLost);
        strcat(details9, gText);
    }
    sprintf(gText, "\n%s%s", cArmyDetail[5], speedText[static_cast<signed char>(armyMonster11[0xb])]);
    strcat(details9, gText);
    sprintf(gText, "\n%s%s", cArmyDetail[6], gMoraleText[morale2 + 3]);
    strcat(details9, gText);
    int luck1 = GetLuck(theHero, theArmy, castle);
    sprintf(gText, "\n%s%s", cArmyDetail[7], gLuckText[luck1 + 3]);
    strcat(details9, gText);

    message6.field8 = 4;
    message6.text = details9;
    m_viewArmyWindow->BroadcastMessage(message6);
    if (!gbAllowUpgrade) {
        message6.field4 = 6;
        message6.text = reinterpret_cast<char *>(6);
        message6.field8 = 500;
        m_viewArmyWindow->BroadcastMessage(message6);
    }
    if (disableUpgrade) {
        message6.field4 = 6;
        message6.text = reinterpret_cast<char *>(6);
        message6.field8 = 0x7803;
        m_viewArmyWindow->BroadcastMessage(message6);
    }
    if (quickView) {
        message6.field4 = 6;
        message6.text = reinterpret_cast<char *>(6);
        message6.field8 = 0x7800;
        m_viewArmyWindow->BroadcastMessage(message6);
    }
    if (numTroops < 1) {
        message6.field4 = 6;
        message6.text = reinterpret_cast<char *>(6);
        message6.field8 = 1;
        m_viewArmyWindow->BroadcastMessage(message6);
        message6.field8 = 2;
        m_viewArmyWindow->BroadcastMessage(message6);
    } else {
        char countText[12];
        sprintf(countText, "%d", numTroops);
        message6.field4 = 3;
        message6.field8 = 2;
        message6.text = countText;
        m_viewArmyWindow->BroadcastMessage(message6);
    }

    if (theArmy) {
        int spellY3 = 169;
        int spellCenterX8 = 420;
        if (quickView)
            spellY3 += 12;
        int spacing0 = 44 - theArmy->m_spellCount;
        int spellX3 = 10 - theArmy->m_spellCount + spellCenterX8 -
                      (theArmy->m_spellCount * spacing0) / 2;
        int spellIndex9 = -1;
        for (loopIndex0 = 0;
             loopIndex0 < (theArmy->m_spellCount < 6 ? theArmy->m_spellCount : 6);
             loopIndex0++) {
            spellIndex9++;
            for (; spellIndex9 < 15; spellIndex9++) {
                if (theArmy->m_spellInfluence[spellIndex9])
                    break;
            }
            iconWidget *spellWidget = new iconWidget(
                static_cast<short>(loopIndex0 * spacing0 + spellX3),
                static_cast<short>(spellY3 + 14), 0, 0, const_cast<char *>("spellinl.icn"),
                static_cast<short>(spellIndex9), 0, static_cast<short>(loopIndex0 + 200), 16, 1);
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
            theGroup->m_troopTypes[groupIndex] = static_cast<signed char>(iViewArmyUpgradeToType);
    }
    BaseFree(details9, GFILE, GSAVELINE + 0x164);
    delete m_viewArmyWindow;
}

// @early-stop
// Relocation-masked comparison is identical for the full 0x3f5-byte span;
// both objects contain the same 52 relocation sites and objdiff reports 100%.
VA(0x0047b2cf, 0x3f5)
int ViewArmyHandler(tag_message &msg)
{
    int goldCost6;
    int resourceType0;
    int resourceCost5;

    gbDismissArmy = 0;
    gbUpgradeArmy = 0;
    // Retail reserves a second short word before the aligned upgrade-cost locals.
    short frameDelay0 = 5;
    short frameOffset1;

    if (msg.type == 0x200) {
        switch (msg.field4) {
        case 13:
            switch (msg.field8) {
            case 0x7800:
            case 0x7801:
                gpWindowManager->m_dialogResult = msg.field8;
                msg.field8 = 10;
                msg.field4 = msg.field8;
                return 2;
            case 0x7803:
                NormalDialog(
                    const_cast<char *>("Are you sure you want to dismiss this army?"),
                    2, -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == 0x7805) {
                    gbDismissArmy = 1;
                    msg.field8 = 10;
                    msg.field4 = msg.field8;
                    return 2;
                }
                break;
            case 500:
                goldCost6 =
                    (gMonsterDatabase[iViewArmyUpgradeToType].cost -
                     gMonsterDatabase[iViewArmyType].cost) *
                    iViewArmyNumTroops * 2;
                if (iViewArmyUpgradeToType == MONSTER_BLACK_DRAGON) {
                    resourceType0 = RES_SULFUR;
                    resourceCost5 = iViewArmyNumTroops * 2;
                } else if (iViewArmyUpgradeToType == MONSTER_TITAN) {
                    resourceType0 = RES_GEMS;
                    resourceCost5 = iViewArmyNumTroops * 2;
                } else {
                    resourceType0 = -1;
                    resourceCost5 = 0;
                }
                if (gpCurPlayer->m_resources[RES_GOLD] >= goldCost6 &&
                    (resourceType0 == -1 ||
                     gpCurPlayer->m_resources[resourceType0] >= resourceCost5)) {
                    NormalDialog(
                        const_cast<char *>(
                            "Your troops can be upgraded, but it will cost you dearly.  "
                            "Do you wish to upgrade them?"),
                        2, -1, -1, 6, goldCost6, resourceType0,
                        resourceCost5, -1, 0);
                    if (gpWindowManager->m_dialogResult == 0x7805) {
                        gpCurPlayer->m_resources[RES_GOLD] -= goldCost6;
                        if (resourceType0 != -1)
                            gpCurPlayer->m_resources[resourceType0] -= resourceCost5;
                        gbUpgradeArmy = 1;
                        msg.field8 = 10;
                        msg.field4 = msg.field8;
                        return 2;
                    }
                } else {
                    NormalDialog(
                        const_cast<char *>("You can't afford to upgrade your troops!"),
                        1, -1, -1, 6, goldCost6, resourceType0,
                        resourceCost5, -1, 0);
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
        msg.type = 0x200;
        msg.field4 = 4;
        msg.field8 = 5;
        iViewArmyFrame =
            (iViewArmyFrame + 1) %
            sViewArmyMonFrameInfo.animationFrameCount[ARMY_ANIMATION_WALK];
        msg.field18 =
            sViewArmyMonFrameInfo.animationFrames[ARMY_ANIMATION_WALK]
                                                 [iViewArmyFrame];
        gpGame->m_viewArmyWindow->BroadcastMessage(msg);
        msg.field4 = 52;
        msg.field18 =
            sViewArmyMonFrameInfo.walkXOffsets[iViewArmyFrame] *
                viewArmyFacingWIPXMod +
            viewArmyBaseX;
        gpGame->m_viewArmyWindow->BroadcastMessage(msg);
        gpGame->m_viewArmyWindow->DrawWindow(1, 0, 0x7fff);
        glTimers[0] = static_cast<int>(
            KBTickCount() +
            sViewArmyMonFrameInfo.walkDuration * 0.1 /
                sViewArmyMonFrameInfo.animationFrameCount[ARMY_ANIMATION_WALK]);
    }
    return 1;
}

// @early-stop
// Relocation-masked comparison is identical for all 0x671 bytes (133 relocation
// sites in both objects). The objdiff residual is delinked local-label identity.
VA(0x0047b6c4, 0x671)
int game::GetRandomNumTroops(int monsterType)
{
    switch (monsterType) {
    case 0: return Random(40, 80);
    case 1: return Random(20, 30);
    case 2: return Random(20, 30);
    case 3: return Random(20, 30);
    case 4: return Random(20, 30);
    case 5: return Random(12, 25);
    case 6: return Random(12, 25);
    case 7: return Random(10, 18);
    case 8: return Random(8, 16);
    case 9: return Random(6, 12);
    case 10: return Random(6, 10);
    case 11: return Random(25, 40);
    case 12: return Random(15, 30);
    case 13: return Random(15, 30);
    case 14: return Random(20, 35);
    case 15: return Random(12, 25);
    case 16: return Random(10, 20);
    case 17: return Random(7, 10);
    case 18: return Random(7, 10);
    case 19: return Random(5, 7);
    case 20: return Random(25, 45);
    case 21: return Random(12, 25);
    case 22: return Random(10, 22);
    case 23: return Random(15, 30);
    case 24: return Random(12, 28);
    case 25: return Random(10, 25);
    case 26: return Random(10, 20);
    case 27: return Random(8, 15);
    case 28: return Random(7, 12);
    case 29: return Random(20, 50);
    case 30: return Random(15, 30);
    case 31: return Random(12, 25);
    case 32: return Random(10, 16);
    case 33: return Random(9, 16);
    case 34: return Random(7, 10);
    case 35: return Random(4, 7);
    case 36: return Random(3, 7);
    case 37: return Random(3, 7);
    case 38: return Random(20, 50);
    case 39: return Random(15, 30);
    case 40: return Random(10, 25);
    case 41: return Random(10, 22);
    case 42: return Random(10, 16);
    case 43: return Random(8, 12);
    case 44: return Random(7, 11);
    case 45: return Random(5, 8);
    case 46: return Random(3, 7);
    case 47: return Random(20, 50);
    case 48: return Random(15, 30);
    case 49: return Random(15, 30);
    case 50: return Random(10, 25);
    case 51: return Random(10, 25);
    case 52: return Random(8, 12);
    case 53: return Random(8, 12);
    case 54: return Random(6, 10);
    case 55: return Random(6, 10);
    case 56: return Random(4, 8);
    case 57: return Random(20, 40);
    case 58: return Random(12, 25);
    case 59: return Random(10, 20);
    case 60: return Random(5, 10);
    case 61: return Random(12, 20);
    case 62: return Random(13, 25);
    case 63: return Random(13, 25);
    case 64: return Random(13, 25);
    case 65: return Random(13, 25);
    default: return 3;
    }
}

VA(0x0047bd35, 0x3f)
void game::TurnOnAIMusic(void)
{
    gpSoundManager->StopAllSamples(1);
    gpSoundManager->SwitchAmbientMusic(0x1c);
    *(int *)((char *)gpSoundManager + 0x684) = 0;
}

VA(0x0047bd74, 0x25)
void game::TurnOffAIMusic(void)
{
    *(int *)((char *)gpSoundManager + 0x684) = 1;
}

// @early-stop
// The reconstructed body realigns after each of the first two flag clears and thereafter
// matches instruction-for-instruction. Retail expands each clear into a 0x1e-byte longer
// address/load/and/address/store sequence (114 relocations versus 112); direct, bitfield,
// accessor, and volatile spellings either collapse it or over-expand it under this /Od TU.
VA(0x0047bd99, 0x596)
void game::NextPlayer(void)
{
    int humanCount;
    int index;

    m_heroRecs[gpCurPlayer->m_availableHeroIds[0]].m_eventFlags =
        m_heroRecs[gpCurPlayer->m_availableHeroIds[0]].m_eventFlags &
        ~HERO_EVENT_WEEKLY_VISIT;
    m_heroRecs[gpCurPlayer->m_availableHeroIds[1]].m_eventFlags =
        m_heroRecs[gpCurPlayer->m_availableHeroIds[1]].m_eventFlags &
        ~HERO_EVENT_WEEKLY_VISIT;
    iCurHourGlassPhase = 0;

    if (gbThisNetHumanPlayer[giCurPlayer] && gConfig.autosave) {
        humanCount = 0;
        for (index = 0; index < 6; index++) {
            if (m_playerDead[index] == 0 && gbHumanPlayer[index])
                humanCount++;
        }
        SaveGame(const_cast<char *>("AUTOSAVE"), 1, 0);
    }

    gpAdvManager->m_currentSampleSet = 0;
    if (gpGame->m_players[giCurPlayer].daysLeft > 0)
        gpGame->m_players[giCurPlayer].daysLeft--;
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

    gpCurPlayer = reinterpret_cast<playerData *>(&gpGame->m_players[giCurPlayer]);
    giCurPlayerBit = static_cast<unsigned char>(1 << giCurPlayer);
    for (index = 0; index < m_players[giCurPlayer].heroCount; index++) {
        hero *currentHero = &m_heroRecs[m_players[giCurPlayer].heroes[index]];
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
            gbThisNetGotAdventureControl = 0;
            int remotePlayer = gbGamePosToNetPos[giCurPlayer];
            if (!gpGame->TransmitSaveGame(remotePlayer, 0, 0))
                ShutDown(0);
        }
        if (giBottomViewOverride == 6)
            giBottomViewOverride = 0;
    } else {
        SetNoDialogMenus(1);
        gpInputManager->Flush();
        gbAllBlack = 1;
        gpAdvManager->CheckSetEvilInterface(1, giCurPlayer);
        gbAllBlack = 0;
        if (gbBlackoutPlayer && giNumHumanPlayers > 1) {
            sprintf(gText, "%s's turn.", cPlayerNames[giCurPlayer]);
            WaitForPlayer(gText, giCurPlayer);
        }
        if (gbThisNetHumanPlayer[giCurPlayer])
            CancelComputerScreen();
        giCurWatchPlayerBit = giCurPlayerBit;
        giCurWatchPlayer = giCurPlayer;
    }

    if (gbThisNetHumanPlayer[giCurPlayer] && gbRemoteOn &&
        m_day != 1 && giForceSwitchMusic == -1) {
        gpSoundManager->SwitchAmbientMusic(21);
        giForceSwitchMusic = KBTickCount();
        gpSoundManager->m_samplesReady = 0;
    }
    if (m_day == 1 && giCurTurn != 1)
        gpSoundManager->m_samplesReady = 0;

    DoNewTurn();
    CheckEndGame(0, 0);
    if (gbThisNetHumanPlayer[giCurPlayer] &&
        gpSoundManager->m_samplesReady == 0 && giForceSwitchMusic == -1) {
        gpSoundManager->m_samplesReady = 1;
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[gpAdvManager->m_currentTerrain]);
        gpAdvManager->SetEnvironmentOrigin(
            gpAdvManager->m_mapOriginX + 7,
            gpAdvManager->m_mapOriginY + 7, 1);
    }
    if (gbThisNetHumanPlayer[giCurPlayer])
        gpAdvManager->ForceNewHover();
}

// @early-stop
// Relocation-masked comparison is identical for the full 0x432-byte span; both objects
// contain 25 relocation sites and objdiff reports 100%.
VA(0x0047c32f, 0x432)
int game::ComputeDailyGold(int player)
{
    int heroIndex;
    int gold = 0;
    int index;

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
            if (m_castleRecs[index].m_buildings & 0x20)
                gold += 250;
            else
                gold += 1000;
            if (m_castleRecs[index].m_buildings & 0x80)
                gold += 250;
            if (m_castleRecs[index].m_type == 3 &&
                (m_castleRecs[index].m_buildings & 0x2000))
                gold += 500;
        }
    }

    gold += reinterpret_cast<class playerData *>(&m_players[player])->NumOfGivenArtifact(30) * 1000;
    gold += reinterpret_cast<class playerData *>(&m_players[player])->NumOfGivenArtifact(31) * 750;
    gold += reinterpret_cast<class playerData *>(&m_players[player])->NumOfGivenArtifact(32) * 500;
    gold += reinterpret_cast<class playerData *>(&m_players[player])->NumOfGivenArtifact(7) * 10000;
    gold += reinterpret_cast<class playerData *>(&m_players[player])->NumOfGivenArtifact(69) * -250;

    for (heroIndex = 0; heroIndex < m_players[player].heroCount; heroIndex++) {
        gold += gEstatesGoldLevel[
            gpGame->m_heroRecs[m_players[player].heroes[heroIndex]]
                .m_secondarySkills[HERO_SKILL_ESTATES]];
    }

    if (!gbHumanPlayer[player]) {
        if (gpGame->m_difficulty == 0)
            gold = static_cast<int>(gold * 0.75);
        if (gpGame->m_difficulty == 1) {
        }
        if (gpGame->m_difficulty == 2)
            gold = static_cast<int>(gold * 1.29);
        if (gpGame->m_difficulty == 3)
            gold = static_cast<int>(gold * 1.45);
        if (gpGame->m_difficulty == 4)
            gold = static_cast<int>(gold * 1.6);
    }

    if (m_playerHandicap[player] == 1)
        gold = static_cast<int>(gold * 0.15);
    else if (m_playerHandicap[player] == 2)
        gold = static_cast<int>(gold * 0.3);
    return gold;
}

// @early-stop
// Frame/slots and 30 relocations are exact. The only residual is target 0x47ce94..0x47cec2:
// the commutative handicap sum loads gpGame secondary income before this->primary income;
// this partial /Od TU loads the same two operands in the opposite order, then realigns at fild.
VA(0x0047c761, 0x9aa)
void game::PerDay(void)
{
    int maxSpellPoints9;
    int player;
    int resource8;
    int income13;
    int dailyIncome0;
    int resourceType1;
    hero *currentHero6;
    int restoredSpellPoints13;
    hero *townHero12;
    town *currentTown4;
    double penaltyRate9;

    for (player = 0; player < gpGame->m_playerCount; player++) {
        for (resource8 = 0; resource8 < 7; resource8++) {
            gpGame->m_players[player].secondaryResources[resource8] =
                -m_players[player].resources[resource8];
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
                m_players[m_mines[player].owner].resources[resourceType1] += dailyIncome0;
        }
    }

    for (player = 0; player < GAME_TOWN_COUNT; player++)
        m_castleRecs[player].m_unknown55++;

    for (player = 0; player < m_playerCount; player++) {
        m_players[player].resources[RES_SULFUR] +=
            reinterpret_cast<playerData *>(&m_players[player])->NumOfGivenArtifact(
                ARTIFACT_ENDLESS_POUCH_SULFUR);
        m_players[player].resources[RES_MERCURY] +=
            reinterpret_cast<playerData *>(&m_players[player])->NumOfGivenArtifact(
                ARTIFACT_ENDLESS_VIAL_MERCURY);
        m_players[player].resources[RES_GEMS] +=
            reinterpret_cast<playerData *>(&m_players[player])->NumOfGivenArtifact(
                ARTIFACT_ENDLESS_POUCH_GEMS);
        m_players[player].resources[RES_WOOD] +=
            reinterpret_cast<playerData *>(&m_players[player])->NumOfGivenArtifact(
                ARTIFACT_ENDLESS_CORD_WOOD);
        m_players[player].resources[RES_ORE] +=
            reinterpret_cast<playerData *>(&m_players[player])->NumOfGivenArtifact(
                ARTIFACT_ENDLESS_CART_ORE);
        m_players[player].resources[RES_CRYSTAL] +=
            reinterpret_cast<playerData *>(&m_players[player])->NumOfGivenArtifact(
                ARTIFACT_ENDLESS_POUCH_CRYSTAL);
        m_players[player].resources[RES_GOLD] += ComputeDailyGold(player);
    }

    if (xIsPlayingExpansionCampaign && xCampaign.HasAward(2))
        m_players[0].resources[RES_WOOD] += 2;

    for (player = 0; player < m_playerCount; player++) {
        if (!gbHumanPlayer[player]) {
            if (gpGame->m_difficulty >= GAME_DIFFICULTY_HARD) {
                m_players[player].resources[RES_WOOD]++;
                m_players[player].resources[RES_ORE]++;
            }
            if (gpGame->m_difficulty >= GAME_DIFFICULTY_EXPERT && m_day >= 1 && m_day <= 6)
                m_players[player].resources[m_day - 1]++;
            if (gpGame->m_difficulty >= GAME_DIFFICULTY_IMPOSSIBLE && m_day >= 1 && m_day <= 6)
                m_players[player].resources[m_day - 1]++;
            if (gpGame->m_players[player].unknown0f == 1 && m_day >= 1 && m_day <= 6)
                m_players[player].resources[m_day - 1]++;
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
        m_heroRecs[player].m_eventFlags = m_heroRecs[player].m_eventFlags & 0xfffeffff;

    for (player = 0; player < gpGame->m_playerCount; player++) {
        for (resource8 = 0; resource8 < 6; resource8++) {
            if (m_playerHandicap[player]) {
                if (m_playerHandicap[player] == 1)
                    penaltyRate9 = 0.15;
                else
                    penaltyRate9 = 0.30;
                m_players[player].resources[resource8] -= static_cast<int>(
                    (gpGame->m_players[player].secondaryResources[resource8] +
                     m_players[player].resources[resource8]) * penaltyRate9);
            }
        }
    }

    for (player = 0; player < gpGame->m_playerCount; player++) {
        for (resource8 = 0; resource8 < 7; resource8++) {
            gpGame->m_players[player].secondaryResources[resource8] +=
                m_players[player].resources[resource8];
        }
    }

    for (player = 0; player < GAME_HERO_COUNT; player++) {
        currentHero6 = &m_heroRecs[player];
        restoredSpellPoints13 = currentHero6->m_spellPoints;
        maxSpellPoints9 = currentHero6->Stats(3) * 10;
        restoredSpellPoints13 += currentHero6->m_secondarySkills[8] + 1;
        if (currentHero6->HasArtifact(ARTIFACT_POWER_RING))
            restoredSpellPoints13 += 2;
        if (restoredSpellPoints13 > maxSpellPoints9)
            restoredSpellPoints13 = maxSpellPoints9;
        if (currentHero6->m_spellPoints < restoredSpellPoints13)
            currentHero6->m_spellPoints = static_cast<short>(restoredSpellPoints13);
        if (currentHero6->m_eventFlags & 0x1000)
            currentHero6->m_eventFlags = currentHero6->m_eventFlags - 0x1000U;
    }

    for (player = 0; player < GAME_TOWN_COUNT; player++) {
        currentTown4 = GetTown(player);
        if (!(currentTown4->m_buildings & 1))
            continue;
        if (currentTown4->m_occupyingHeroId != -1) {
            townHero12 = GetHero(currentTown4->m_occupyingHeroId);
            maxSpellPoints9 = townHero12->Stats(3) * 10;
            if (townHero12->m_spellPoints < maxSpellPoints9)
                townHero12->m_spellPoints = static_cast<short>(maxSpellPoints9);
        }
    }
}

// @early-stop
// 99.09%: frame and control flow are byte-exact. The six hero-slot index residuals
// differ only in when a commutative subtraction is issued, and the map-height loop
// differs only in compare operand order; both are the documented TU-cumulative /Od
// operand-order pattern. The other residuals are delinked switch labels/constants;
// the full function has the retail 95 relocations and matching external targets.
VA(0x0047d10b, 0x199d)
void game::PerWeek(void)
{
    int heroClass18 = 0;
    int outerIndex5;
    int innerIndex3;
    int mapY5;
    int mapX8;
    townSlot *castle37;
    int growth13;
    int desiredClass1;
    int monsterIncrease16;
    int monsterCount36;
    hero *weeklyHero4;

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
        castle37 = reinterpret_cast<townSlot *>(GetTown(outerIndex5));
        for (innerIndex3 = WEEKLY_FIRST_DWELLING; innerIndex3 <= WEEKLY_LAST_DWELLING;
             innerIndex3++) {
            if (castle37->buildings & (1 << innerIndex3)) {
                growth13 = gMonsterDatabase[
                    gDwellingType[castle37->race][innerIndex3 - WEEKLY_FIRST_DWELLING]].growth;
                if (castle37->buildings & 0x10)
                    growth13 += 2;
                if (innerIndex3 == WEEKLY_FIRST_DWELLING && (castle37->buildings & 0x800))
                    growth13 += 8;
                if (castle37->owner == -1)
                    growth13 /= 2;
                if (castle37->owner >= 0 &&
                    castle37->dwellingGrowth[innerIndex3 - WEEKLY_FIRST_DWELLING] == 0 &&
                    !gbHumanPlayer[castle37->owner]) {
                    if (gpGame->m_difficulty == GAME_DIFFICULTY_HARD)
                        growth13 = static_cast<int>(growth13 * 1.20);
                    if (gpGame->m_difficulty == GAME_DIFFICULTY_EXPERT)
                        growth13 = static_cast<int>(growth13 * 1.32);
                    if (gpGame->m_difficulty == GAME_DIFFICULTY_IMPOSSIBLE)
                        growth13 = static_cast<int>(growth13 * 1.44);
                }
                if (giWeekType == 1 &&
                    gDwellingType[castle37->race][innerIndex3 - WEEKLY_FIRST_DWELLING] ==
                        giWeekTypeExtra)
                    growth13 += 5;
                castle37->dwellingGrowth[innerIndex3 - WEEKLY_FIRST_DWELLING] += growth13;
            }
        }
    }

    for (outerIndex5 = 0; outerIndex5 < GAME_PLAYER_COUNT; outerIndex5++) {
        for (innerIndex3 = 0; innerIndex3 < 2; innerIndex3++) {
            if (innerIndex3 == 1) {
                heroClass18 =
                    m_heroRecs[gpGame->m_players[outerIndex5].availableHeroes[0]].m_cursorType;
            }
            heroClass18 = (Random(1, 5) + heroClass18) % 6;
            desiredClass1 = heroClass18;
            if (innerIndex3 == 0 &&
                m_setupPlayerRace[gcColorToSetupPos[m_players[outerIndex5].color]] <
                    GAME_PLAYER_COUNT) {
                desiredClass1 = m_setupPlayerRace[gcColorToSetupPos[m_players[outerIndex5].color]];
            }

            if (gpGame->m_availableHeroes[
                    (innerIndex3 - outerIndex5 +
                     outerIndex5 * (sizeof(playerData) + 1))[
                        gpGame->m_players[0].availableHeroes]] ==
                WEEKLY_AVAILABLE_HERO) {
                if (gpGame->m_heroRecs[
                        (innerIndex3 - outerIndex5 +
                         outerIndex5 * (sizeof(playerData) + 1))[
                            gpGame->m_players[0].availableHeroes]]
                        .m_eventFlags & WEEKLY_HERO_RESERVED_FLAG)
                    continue;
            }
            {
                if (gpGame->m_availableHeroes[
                        (innerIndex3 - outerIndex5 +
                         outerIndex5 * (sizeof(playerData) + 1))[
                            gpGame->m_players[0].availableHeroes]] ==
                    WEEKLY_AVAILABLE_HERO)
                    gpGame->m_availableHeroes[
                        (innerIndex3 - outerIndex5 +
                         outerIndex5 * (sizeof(playerData) + 1))[
                            gpGame->m_players[0].availableHeroes]] =
                        -1;
                if (innerIndex3 == 1 && !gbHumanPlayer[outerIndex5])
                    desiredClass1 = -1;
                int useDifficultyBonus3 =
                    !gbHumanPlayer[outerIndex5] && gpGame->m_difficulty > 0;
                (innerIndex3 - outerIndex5 +
                 outerIndex5 * (sizeof(playerData) + 1))[
                    gpGame->m_players[0].availableHeroes] =
                    static_cast<signed char>(gpGame->GetNewHeroId(
                        outerIndex5, desiredClass1, useDifficultyBonus3));
                m_availableHeroes[(innerIndex3 - outerIndex5 +
                                   outerIndex5 * (sizeof(playerData) + 1))[
                    gpGame->m_players[0].availableHeroes]] =
                    WEEKLY_AVAILABLE_HERO;
            }
        }
    }

    for (mapY5 = 0; MAP_HEIGHT > mapY5; mapY5++) {
        for (mapX8 = 0; mapX8 < MAP_WIDTH; mapX8++) {
            switch (WORLDMAP->Row(mapY5)[mapX8].triggerType) {
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_MONSTER: {
                monsterCount36 = WORLDMAP->GetCell(mapX8, mapY5)->w4hi & 0xfff;
                monsterIncrease16 = monsterCount36 / 7;
                if (Random(1, 7) <= static_cast<int>(monsterCount36 % 7))
                    monsterIncrease16++;
                monsterCount36 += monsterIncrease16;
                if (monsterCount36 > WEEKLY_MONSTER_LIMIT)
                    monsterCount36 = WEEKLY_MONSTER_LIMIT;
                WORLDMAP->GetCell(mapX8, mapY5)->w4hi =
                    (WORLDMAP->GetCell(mapX8, mapY5)->w4hi & 0x1000) | monsterCount36;
                break;
            }
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_ARTESIAN_SPRING:
                WORLDMAP->GetCell(mapX8, mapY5)->w4hi = 1;
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_WATER_WHEEL:
                if (WORLDMAP->GetCell(mapX8, mapY5)->w4hi != 0xff)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi = 2;
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_MAGIC_GARDEN:
                WORLDMAP->GetCell(mapX8, mapY5)->w4hi =
                    Random(0, 1) ? 7 : 6;
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_WINDMILL:
                WORLDMAP->GetCell(mapX8, mapY5)->w4hi = Random(1, 5);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_ARCHER_HOUSE:
                if (WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_GROWTH_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(2, 4);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_GOBLIN_HUT:
                if (WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_GROWTH_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(3, 6);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_DWARF_COTTAGE:
                if (WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_GROWTH_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(2, 4);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_PEASANT_HUT:
                if (WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_GROWTH_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(5, 10);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_LOG_CABIN:
                if (WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_GROWTH_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(5, 10);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_DESERT_TENT:
                if (WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_GROWTH_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(1, 3);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_WAGON_CAMP:
                if (WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_GROWTH_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(3, 6);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_TREE_HOUSE:
                if (WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_GROWTH_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(4, 8);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_SIRENS:
                if (WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_GROWTH_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(3, 6);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_WATCH_TOWER:
                if (WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_GROWTH_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(1, 4);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_RUINS:
                if (WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_GROWTH_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(1, 3);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_TREE_CITY:
                if (WORLDMAP->GetCell(mapX8, mapY5)->w4hi < 0x1fe1)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(10, 20);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_CAVE:
                if (WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_GROWTH_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(3, 6);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_EXCAVATION:
                if (WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_GROWTH_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(4, 8);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_HALFLING_HOLE:
                if (WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_GROWTH_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(5, 10);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_TROLL_BRIDGE:
                if (!(WORLDMAP->GetCell(mapX8, mapY5)->w4hi & 0x80) &&
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_DRAGON_CITY_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(1, 3);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_CITY_OF_DEAD:
                if (!(WORLDMAP->GetCell(mapX8, mapY5)->w4hi & 0x80) &&
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_DRAGON_CITY_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += Random(1, 3);
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_DRAGON_CITY:
                if (!(WORLDMAP->GetCell(mapX8, mapY5)->w4hi & 0x80) &&
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi < WEEKLY_DRAGON_CITY_LIMIT)
                    WORLDMAP->GetCell(mapX8, mapY5)->w4hi += 1;
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_EXPANSION_DWELLING:
                WeeklyRecruitSite(WORLDMAP->GetCell(mapX8, mapY5));
                break;
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_EXPANSION_OBJECT:
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
                static_cast<unsigned int>(weeklyHero4->m_eventFlags) -
                WEEKLY_HERO_VISIT_FLAG;
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
void game::WeeklyRecruitSite(mapCell *cell)
{
    int type = cell->w4hi;
    type &= WEEKLY_RECRUIT_TYPE_MASK;
    int recruitCount = cell->w4hi;
    recruitCount >>= WEEKLY_RECRUIT_COUNT_SHIFT;
    int packed;

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
    cell->w4hi = packed | 0;
}

VA(0x0047ebd5, 0x6f)
void game::WeeklyGenericSite(mapCell *cell)
{
    int type = cell->w4hi;
    type &= 0x3f;
    switch (type) {
    case 4:
        cell->w4hi = type;
        break;
    }
}

// @early-stop
// Exact 0x375-byte body and 30 relocation sites. The typed dwelling/monster
// references resolve to retail's folded 0x4fca35 and 0x4faeb7 local symbols.
VA(0x0047ec44, 0x375)
void game::PerMonth(void)
{
    mapCell *cell0;
    int mapX8;
    int mapY5;
    int townIndex0;
    int building4;
    int growth9;
    townSlot *castle10;
    int firstCount5;
    int secondCount4;

    m_month++;
    townIndex0 = Random(MONTH_ROLL_MIN, MONTH_ROLL_MAX);
    if (townIndex0 <= MONTH_NORMAL_ROLL_MAX) {
        giMonthType = MONTH_TYPE_NORMAL;
        giMonthTypeExtra = Random(MONTH_NORMAL_NAME_MIN, MONTH_NORMAL_NAME_MAX);
    } else if (townIndex0 <= MONTH_CREATURE_ROLL_MAX) {
        giMonthType = MONTH_TYPE_CREATURE;
        giMonthTypeExtra = giMonType[
            Random(MONTH_CREATURE_LIST_MIN, MONTH_CREATURE_LIST_MAX)];
    } else {
        giMonthType = MONTH_TYPE_PLAGUE;
    }

    for (townIndex0 = 0; townIndex0 < GAME_TOWN_COUNT; townIndex0++) {
        for (building4 = WEEKLY_FIRST_DWELLING;
             building4 <= WEEKLY_LAST_DWELLING; building4++) {
            castle10 = reinterpret_cast<townSlot *>(GetTown(townIndex0));
            if (castle10->buildings & (1 << building4)) {
                growth9 = gMonsterDatabase[
                    gDwellingType[castle10->race][building4 - WEEKLY_FIRST_DWELLING]].growth;
                if (castle10->buildings & MONTH_WELL_BUILDING)
                    growth9 += MONTH_WELL_GROWTH;
                if (building4 == WEEKLY_FIRST_DWELLING &&
                    (castle10->buildings & MONTH_FIRST_DWELLING_BONUS_BUILDING))
                    growth9 += MONTH_FIRST_DWELLING_GROWTH;

                if (giMonthType == MONTH_TYPE_CREATURE &&
                    gDwellingType[castle10->race][building4 - WEEKLY_FIRST_DWELLING] ==
                        giMonthTypeExtra)
                    castle10->dwellingGrowth[building4 - WEEKLY_FIRST_DWELLING] *= 2;

                if (giMonthType == MONTH_TYPE_PLAGUE) {
                    castle10->dwellingGrowth[building4 - WEEKLY_FIRST_DWELLING] -= growth9;
                    if (castle10->dwellingGrowth[building4 - WEEKLY_FIRST_DWELLING] < 0)
                        castle10->dwellingGrowth[building4 - WEEKLY_FIRST_DWELLING] = 0;
                    castle10->dwellingGrowth[building4 - WEEKLY_FIRST_DWELLING] =
                        castle10->dwellingGrowth[building4 - WEEKLY_FIRST_DWELLING] >> 1;
                }
            }
        }
    }

    if (giMonthType == MONTH_TYPE_CREATURE) {
        for (mapX8 = 0; mapX8 < MAP_WIDTH; mapX8++) {
            for (mapY5 = 0; mapY5 < MAP_HEIGHT; mapY5++) {
                cell0 = gpAdvManager->GetCell(mapX8, mapY5);
                if (cell0->triggerType == 0 && !cell0->w4b && !cell0->w4a &&
                    giGroundToTerrain[cell0->tile] != 0) {
                    if (Random(MONTH_MONSTER_SPAWN_MIN, MONTH_MONSTER_SPAWN_MAX) ==
                        MONTH_MONSTER_SPAWN_ROLL) {
                        cell0->triggerType = MONTH_MONSTER_TRIGGER;
                        cell0->objTileset = MONTH_MONSTER_TILESET;
                        cell0->objIndex = static_cast<unsigned char>(giMonthTypeExtra);
                        firstCount5 = GetRandomNumTroops(giMonthTypeExtra);
                        secondCount4 = GetRandomNumTroops(giMonthTypeExtra);
                        cell0->w4hi = ((firstCount5 | 0) + secondCount4) | 0;
                        if (Random(MONTH_MONSTER_SPAWN_MIN,
                                   MONTH_MONSTER_GUARD_ROLL_MAX) <
                            MONTH_MONSTER_GUARD_CHANCE)
                            cell0->w4hi |= MONTH_MONSTER_GUARD_FLAG;
                    }
                }
            }
        }
    }

    gpAdvManager->CompleteDraw(0);
}

// @early-stop
// The complete control flow and both relocations align. The AST-permuted bounds spelling
// improves the match to 99.19%: this 0x472-byte body materializes y + 1 with a one-byte inc,
// while retail's 0x476-byte body compares y directly and retains a five-byte continuation.
VA(0x0047efb9, 0x476)
void game::ConvertObject(int left, int top, int right, int bottom,
                         int oldTileset, int oldFirstIndex, int oldLastIndex,
                         int newTileset, int newFirstIndex,
                         int oldTrigger, int newTrigger)
{
    int x;
    int y;
    mapCell *cell;
    mapCellExtra *extra;

    for (x = left; right >= x; x++) {
        for (y = top; bottom >= y; y++) {
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && MAP_HEIGHT >= y + 1) {
                cell = WORLDMAP->GetCell(x, y);
                if (cell->objIndex != static_cast<unsigned char>(-1) &&
                    cell->objTileset == oldTileset &&
                    cell->objIndex >= oldFirstIndex && cell->objIndex <= oldLastIndex) {
                    cell->objTileset = static_cast<unsigned char>(newTileset);
                    cell->objIndex = static_cast<unsigned char>(
                        cell->objIndex - oldFirstIndex + newFirstIndex);
                }
                if ((cell->triggerType & 0x7f) == oldTrigger)
                    cell->triggerType = static_cast<unsigned char>(
                        (cell->triggerType & 0x80) | newTrigger);

                if (cell->extra != 0 &&
                    WORLDMAP->Extra(cell->extra)->objIndex != static_cast<unsigned char>(-1))
                    extra = WORLDMAP->Extra(cell->extra);
                else
                    extra = 0;
                while (extra != 0) {
                    if (extra->objTileset == oldTileset &&
                        extra->objIndex >= oldFirstIndex && extra->objIndex <= oldLastIndex) {
                        extra->objTileset = static_cast<unsigned char>(newTileset);
                        extra->objIndex = static_cast<unsigned char>(
                            extra->objIndex - oldFirstIndex + newFirstIndex);
                    }
                    if (extra->index != 0 &&
                        WORLDMAP->Extra(extra->index)->objIndex != static_cast<unsigned char>(-1))
                        extra = WORLDMAP->Extra(extra->index);
                    else
                        extra = 0;
                }

                if (cell->ovlIndex != static_cast<unsigned char>(-1) &&
                    cell->ovlTileset == oldTileset &&
                    cell->ovlIndex >= oldFirstIndex && cell->ovlIndex <= oldLastIndex) {
                    cell->ovlTileset = static_cast<unsigned char>(newTileset);
                    cell->ovlIndex = static_cast<unsigned char>(
                        cell->ovlIndex - oldFirstIndex + newFirstIndex);
                }
                if (cell->extra != 0 &&
                    WORLDMAP->Extra(cell->extra)->ovlIndex != static_cast<unsigned char>(-1))
                    extra = WORLDMAP->Extra(cell->extra);
                else
                    extra = 0;
                while (extra != 0) {
                    if (extra->ovlTileset == oldTileset &&
                        extra->ovlIndex >= oldFirstIndex && extra->ovlIndex <= oldLastIndex) {
                        extra->ovlTileset = static_cast<unsigned char>(newTileset);
                        extra->ovlIndex = static_cast<unsigned char>(
                            extra->ovlIndex - oldFirstIndex + newFirstIndex);
                    }
                    if (extra->index != 0 &&
                        WORLDMAP->Extra(extra->index)->ovlIndex != static_cast<unsigned char>(-1))
                        extra = WORLDMAP->Extra(extra->index);
                    else
                        extra = 0;
                }
            }
        }
    }
}

VA(0x0047f42f, 0x1c2)
void game::RandomizeTown(int x, int y, int)
{
    int unused6[2];  // Two unaddressed words are present in the retail /Od frame.
    int townId0 = GetTownId(x, y);
    townSlot *castle0 = reinterpret_cast<townSlot *>(GetTown(townId0));
    mapTownExtra *extra = reinterpret_cast<mapTownExtra *>(
        ppMapExtra[WORLDMAP->GetCell(x, y)->w4hi]);
    int race0;

    if (extra->color == RANDOM_TOWN_UNOWNED_COLOR)
        race0 = Random(RANDOM_TOWN_RACE_MIN, RANDOM_TOWN_RACE_MAX);
    else
        race0 = m_setupPlayerRace[gcColorToSetupPos[extra->color]];

    castle0->unknown55 = RANDOM_TOWN_AGE;
    ConvertObject(x + RANDOM_TOWN_LEFT, y + RANDOM_TOWN_TOP,
                  x + RANDOM_TOWN_RIGHT, y + RANDOM_TOWN_BOTTOM,
                  RANDOM_TOWN_SOURCE_TILESET,
                  RANDOM_TOWN_OBJECT_SOURCE_FIRST, RANDOM_TOWN_OBJECT_SOURCE_LAST,
                  RANDOM_TOWN_OBJECT_TILESET, race0 << RANDOM_TOWN_RACE_FRAME_SHIFT,
                  RANDOM_TOWN_FIRST_TRIGGER, RANDOM_TOWN_TRIGGER);
    ConvertObject(x + RANDOM_TOWN_LEFT, y + RANDOM_TOWN_TOP,
                  x + RANDOM_TOWN_RIGHT, y + RANDOM_TOWN_BOTTOM,
                  RANDOM_TOWN_SOURCE_TILESET,
                  RANDOM_TOWN_OVERLAY_SOURCE_FIRST, RANDOM_TOWN_OVERLAY_SOURCE_LAST,
                  RANDOM_TOWN_OVERLAY_TILESET, race0 << RANDOM_TOWN_RACE_FRAME_SHIFT,
                  RANDOM_TOWN_FIRST_TRIGGER, RANDOM_TOWN_TRIGGER);
    ConvertObject(x + RANDOM_TOWN_LEFT, y + RANDOM_TOWN_TOP,
                  x + RANDOM_TOWN_RIGHT, y + RANDOM_TOWN_BOTTOM,
                  RANDOM_TOWN_SOURCE_TILESET,
                  RANDOM_TOWN_OBJECT_SOURCE_FIRST, RANDOM_TOWN_OBJECT_SOURCE_LAST,
                  RANDOM_TOWN_OBJECT_TILESET, race0 << RANDOM_TOWN_RACE_FRAME_SHIFT,
                  RANDOM_TOWN_SECOND_TRIGGER, RANDOM_TOWN_TRIGGER);
    ConvertObject(x + RANDOM_TOWN_LEFT, y + RANDOM_TOWN_TOP,
                  x + RANDOM_TOWN_RIGHT, y + RANDOM_TOWN_BOTTOM,
                  RANDOM_TOWN_SOURCE_TILESET,
                  RANDOM_TOWN_OVERLAY_SOURCE_FIRST, RANDOM_TOWN_OVERLAY_SOURCE_LAST,
                  RANDOM_TOWN_OVERLAY_TILESET, race0 << RANDOM_TOWN_RACE_FRAME_SHIFT,
                  RANDOM_TOWN_SECOND_TRIGGER, RANDOM_TOWN_TRIGGER);
    m_castleRecs[townId0].m_type = static_cast<signed char>(race0);
}

// @early-stop
// Exact 0x619-byte span and 22 relocation sites. The remaining /Od differences
// are operand evaluation in the inlined GetCell(x + 1, ...) accessors and the
// columnOffset + x expressions. Both operand spellings were tested unchanged;
// the residual is TU-cumulative compiler state, not a behavioral difference.
VA(0x0047f5f1, 0x619)
void game::RandomizeMine(int x, int y)
{
    unsigned char objectFrame1;
    int mineId;
    int mineType29;
    int terrain3 = giGroundToTerrain[WORLDMAP->GetCell(x, y)->tile];
    int columnOffset4;
    int retry4;
    int rowOffset0;
    unsigned char mineFrame36;
    int triggerType19;

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

    WORLDMAP->GetCell(x, y)->objIndex = objectFrame1;
    WORLDMAP->GetCell(x + 1, y)->objIndex = objectFrame1 + 1;
    WORLDMAP->GetCell(x, y - 1)->ovlIndex = mineFrame36;
    WORLDMAP->GetCell(x + 1, y - 1)->ovlIndex = mineFrame36 + 1;

    if (mineType29 == 1) {
        WORLDMAP->GetCell(x + 1, y)->m_objType |= 1;
        triggerType19 = 1;
    } else if (mineType29 == 0) {
        triggerType19 = 29;
    } else {
        m_worldMap.ChangeTilesetIndex(WORLDMAP->GetCell(x + 1, y),
                                      x + 1, y, 29, mineType29 - 2, 0, -1);
        triggerType19 = 23;
    }

    mineId = GetMineId(x, y);
    for (rowOffset0 = 0; rowOffset0 < 2; rowOffset0++) {
        for (columnOffset4 = 0; columnOffset4 < 2; columnOffset4++) {
            if ((WORLDMAP->GetCell(columnOffset4 + x, y - rowOffset0)->triggerType & 0x7f) > 0)
                if ((WORLDMAP->GetCell(columnOffset4 + x, y - rowOffset0)->triggerType & 0x7f) <= 0x30)
                    continue;
            WORLDMAP->GetCell(columnOffset4 + x, y - rowOffset0)->w4hi = mineId;
            WORLDMAP->GetCell(columnOffset4 + x, y - rowOffset0)->triggerType = triggerType19;
        }
    }
    WORLDMAP->GetCell(x, y)->triggerType |= 0x80;
    m_mines[mineId].resourceType = static_cast<signed char>(mineType29);
}

VA(0x0047fc0a, 0xc6)
void game::InitRandomArtifacts(void)
{
    int xx;
    memset((char *)this + 0x6136, 0, 0x67);
    int x;
    for (x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            mapCell *cell = WORLDMAP->Row(y) + x;
            if (((unsigned char *)cell)[9] == 0xa9)
                ((char *)this)[0x6136 + (((unsigned char *)cell)[3] >> 1)] = 1;
        }
    }
}

VA(0x0047fcd0, 0x17f)
int game::GetRandomArtifactId(int levelMask, int allowCursed)
{
    int attempts = 0;
    int artifact;

    while (1) {
        if (xIsExpansionMap)
            artifact = Random(RANDOM_ARTIFACT_FIRST, RANDOM_ARTIFACT_EXPANSION_LAST);
        else
            artifact = Random(RANDOM_ARTIFACT_FIRST, RANDOM_ARTIFACT_BASE_LAST);

        if (!(levelMask & gArtifactLevel[artifact]))
            continue;
        if (artifact == ARTIFACT_EDITOR_ANY_ULTIMATE ||
            artifact == ARTIFACT_EDITOR_UNUSED_84 ||
            artifact == ARTIFACT_EDITOR_UNUSED_85 ||
            artifact == ARTIFACT_EDITOR_UNUSED_86 ||
            artifact == ARTIFACT_RANDOM_SPELL_SCROLL ||
            artifact == ARTIFACT_BREASTPLATE_ANDURAN ||
            artifact == ARTIFACT_BATTLE_GARB ||
            artifact == ARTIFACT_HELMET_ANDURAN ||
            artifact == ARTIFACT_SWORD_ANDURAN ||
            artifact == ARTIFACT_SPHERE_NEGATION)
            continue;
        if (attempts++ < RANDOM_ARTIFACT_UNIQUE_RETRIES &&
            m_randomArtifacts[artifact])
            continue;
        if (IsCursedItem(artifact)) {
            if (!allowCursed)
                continue;
            if (Random(RANDOM_ARTIFACT_FIRST, RANDOM_ARTIFACT_CURSED_ROLL_MAX) <
                RANDOM_ARTIFACT_CURSED_REJECT_CHANCE)
                continue;
        }
        if (m_mapHeader.victoryCondition != VICTORY_CONDITION_ARTIFACT ||
            m_mapHeader.victoryConditionValue - VICTORY_ARTIFACT_ID_OFFSET != artifact)
            break;
    }

    m_randomArtifacts[artifact] = 1;
    return artifact;
}

VA(0x0047fe4f, 0x68)
int IsCursedItem(int item)
{
    if (item == 0x10 || item == 0x46 || item == 0x45 || item == 0x57 ||
        item == 0x59 || item == 0x5c || item == 0x5d)
        return 1;
    return 0;
}

VA(0x0047feb7, 0x1ef)
void game::RandomizeHeroPool(void)
{
    for (int heroId = 0; heroId < RANDOM_HERO_COUNT; heroId++) {
        m_heroRecs[heroId].m_experience =
            Random(RANDOM_HERO_EXPERIENCE_MIN, RANDOM_HERO_EXPERIENCE_MAX) +
            RANDOM_HERO_EXPERIENCE_BASE;
        SetRandomHeroArmies(heroId, RANDOM_HERO_NORMAL_ARMY);
        m_heroRecs[heroId].m_remainingMobility = m_heroRecs[heroId].CalcMobility();
        m_heroRecs[heroId].m_mobility = m_heroRecs[heroId].m_remainingMobility;
        m_heroRecs[heroId].m_randomSeed = static_cast<unsigned char>(
            Random(RANDOM_HERO_SEED_MIN, RANDOM_HERO_SEED_MAX));
        m_heroRecs[heroId].m_enabled = RANDOM_HERO_ENABLED;

        if (m_heroRecs[heroId].m_cursorType == HERO_CLASS_SORCERESS)
            m_heroRecs[heroId].m_spells[RANDOM_HERO_SORCERESS_STARTING_SPELL] =
                RANDOM_HERO_STARTING_SPELL_KNOWN;
        else if (m_heroRecs[heroId].m_cursorType == HERO_CLASS_WARLOCK)
            m_heroRecs[heroId].m_spells[RANDOM_HERO_WARLOCK_STARTING_SPELL] =
                RANDOM_HERO_STARTING_SPELL_KNOWN;
        else if (m_heroRecs[heroId].m_cursorType == HERO_CLASS_NECROMANCER)
            m_heroRecs[heroId].m_spells[RANDOM_HERO_NECROMANCER_STARTING_SPELL] =
                RANDOM_HERO_STARTING_SPELL_KNOWN;
        else if (m_heroRecs[heroId].m_cursorType == HERO_CLASS_WIZARD)
            m_heroRecs[heroId].m_spells[RANDOM_HERO_WIZARD_STARTING_SPELL] =
                RANDOM_HERO_STARTING_SPELL_KNOWN;
    }
}

VA(0x004800a6, 0x378)
void game::SetRandomHeroArmies(int heroId, int strongArmy)
{
    armyGroup *army2 = &m_heroRecs[heroId].m_army;
    int armySlot7 = 0;
    RandomHeroArmyRange armyTable7[HERO_CLASS_COUNT][RANDOM_HERO_ARMY_OPTION_COUNT] = {
        {{RANDOM_HERO_KNIGHT_FIRST, 30, 50},
         {RANDOM_HERO_KNIGHT_SECOND, 3, 5},
         {RANDOM_HERO_KNIGHT_THIRD, 2, 4}},
        {{RANDOM_HERO_BARBARIAN_FIRST, 15, 25},
         {RANDOM_HERO_BARBARIAN_SECOND, 3, 5},
         {RANDOM_HERO_BARBARIAN_THIRD, 2, 3}},
        {{RANDOM_HERO_SORCERESS_FIRST, 10, 20},
         {RANDOM_HERO_SORCERESS_SECOND, 2, 4},
         {RANDOM_HERO_SORCERESS_THIRD, 1, 2}},
        {{RANDOM_HERO_WARLOCK_FIRST, 6, 10},
         {RANDOM_HERO_WARLOCK_SECOND, 2, 4},
         {RANDOM_HERO_WARLOCK_THIRD, 1, 2}},
        {{RANDOM_HERO_WIZARD_FIRST, 6, 10},
         {RANDOM_HERO_WIZARD_SECOND, 2, 4},
         {RANDOM_HERO_WIZARD_THIRD, 1, 2}},
        {{RANDOM_HERO_NECROMANCER_FIRST, 6, 10},
         {RANDOM_HERO_NECROMANCER_SECOND, 2, 4},
         {RANDOM_HERO_NECROMANCER_THIRD, 1, 2}}
    };
    int selected9[3];
    int index9;
    int minimum5;
    int maximum5;

    selected9[0] = RANDOM_HERO_STACK_SELECTED;
    selected9[1] = RANDOM_HERO_FIRST_STACK_CHANCE +
        (strongArmy ? RANDOM_HERO_FIRST_STACK_BONUS_CHANCE : 0) >
        Random(RANDOM_HERO_PERCENT_MIN, RANDOM_HERO_PERCENT_MAX);
    selected9[2] = Random(RANDOM_HERO_PERCENT_MIN, RANDOM_HERO_PERCENT_MAX) <
        RANDOM_HERO_SECOND_STACK_CHANCE +
        (strongArmy ? RANDOM_HERO_SECOND_STACK_BONUS_CHANCE : 0);
    if (!selected9[2])
        selected9[1] = RANDOM_HERO_STACK_SELECTED;

    for (index9 = 0; index9 < RANDOM_HERO_ARMY_SLOT_COUNT; index9++) {
        army2->m_creatureTypes[index9] = RANDOM_HERO_EMPTY_CREATURE;
        army2->m_creatureCounts[index9] = RANDOM_HERO_EMPTY_COUNT;
    }

    for (index9 = 0; index9 < RANDOM_HERO_ARMY_SELECTION_COUNT; index9++) {
        if (selected9[index9]) {
            army2->m_creatureTypes[armySlot7] = static_cast<signed char>(
                armyTable7[m_heroRecs[heroId].m_cursorType][index9].creature);
            minimum5 = armyTable7[m_heroRecs[heroId].m_cursorType][index9].minimum *
                       RANDOM_HERO_COUNT_SCALE;
            maximum5 = armyTable7[m_heroRecs[heroId].m_cursorType][index9].maximum *
                           RANDOM_HERO_COUNT_SCALE +
                       RANDOM_HERO_COUNT_ROUNDING;
            if (strongArmy)
                minimum5 = ((minimum5 | 0) + maximum5) / 2;
            army2->m_creatureCounts[armySlot7] = static_cast<short>(
                Random(minimum5, maximum5) / RANDOM_HERO_COUNT_SCALE);
            armySlot7++;
        }
    }
}

// @early-stop
// Exact 0x746-byte span and 56 relocation sites. The retained source-hash build
// was relocation-masked byte-identical; after reconstructing its exact predecessor,
// the live /Od output differs only in commutative MAP_HEIGHT/MAP_WIDTH loop-bound
// load order. Reversed comparisons emit the same sequence (TU-cumulative state).
VA(0x0048041e, 0x746)
void game::ProcessRandomObjects(void)
{
    int maxValue17;
    int x10;
    int mineIndex8;
    int y8;
    int artifactId18;
    int minValue7;
    mapCell *cell6;
    int randomType0;
    int randomObjectType3;

    giUABaseX = -1;
    giUABaseY = -1;
    giUARadius = 0;
    for (mineIndex8 = 0; mineIndex8 < 7; mineIndex8++)
        RandMineQty[mineIndex8] = 0;

    for (y8 = 0; MAP_HEIGHT > y8; y8++) {
        for (x10 = 0; x10 < MAP_WIDTH; x10++) {
            cell6 = WORLDMAP->GetCell(x10, y8);
            switch (cell6->triggerType) {
            case 0xac:
                giUABaseX = static_cast<short>(x10);
                giUABaseY = static_cast<short>(y8);
                giUARadius = static_cast<short>(cell6->w4hi);
                cell6->triggerType = 0;
                cell6->objTileset = 0;
                cell6->objIndex = -1;
                break;
            case 0xb0:
                RandomizeTown(x10, y8, 0);
                break;
            case 0xb1:
                RandomizeTown(x10, y8, 1);
                break;
            case 0xaf:
                minValue7 = 80;
                maxValue17 = 2000;
                goto randomMonster;
            case 0xb3:
                minValue7 = 0;
                maxValue17 = 400;
                goto randomMonster;
            case 0xb4:
                minValue7 = 400;
                maxValue17 = 1000;
                goto randomMonster;
            case 0xb5:
                minValue7 = 1000;
                maxValue17 = 2500;
                goto randomMonster;
            case 0xb6:
                minValue7 = 2500;
                maxValue17 = 100000;
                goto randomMonster;
randomMonster:
                if (cell6->objTileset == 12 &&
                    cell6->objIndex >= 0x43 && cell6->objIndex <= 0x46) {
                    randomObjectType3 = cell6->objIndex + 0x70;
                    switch (randomObjectType3) {
                    case 0xb3:
                        minValue7 = 0;
                        maxValue17 = 400;
                        goto monsterBoundsReady;
                    case 0xb4:
                        minValue7 = 400;
                        maxValue17 = 1000;
                        goto monsterBoundsReady;
                    case 0xb5:
                        minValue7 = 1000;
                        maxValue17 = 2500;
                        goto monsterBoundsReady;
                    case 0xb6:
                        minValue7 = 2500;
                        maxValue17 = 100000;
                        goto monsterBoundsReady;
                    }
                }
monsterBoundsReady:
                cell6->triggerType = 0x98;
                cell6->objIndex = static_cast<unsigned char>(Random(0, 65));
                while (gMonsterDatabase[cell6->objIndex].randomValue <= minValue7 ||
                       gMonsterDatabase[cell6->objIndex].randomValue >= maxValue17)
                    cell6->objIndex = static_cast<unsigned char>(Random(0, 65));
                break;
            case 0xae:
                cell6->triggerType = 0x9b;
                randomType0 = Random(0, 6);
                ConvertObject(x10 - 1, y8, x10 - 1, y8,
                              0x2e, 0x10, 0x10, 0x2e, randomType0 * 2, -1, -1);
                ConvertObject(x10, y8, x10, y8,
                              0x2e, 0x11, 0x11, 0x2e, randomType0 * 2 + 1, -1, -1);
                switch (randomType0) {
                case 0:
                case 2:
                    cell6->w4hi = Random(8, 16);
                    break;
                case 6:
                    cell6->w4hi = Random(5, 10);
                    break;
                default:
                    cell6->w4hi = Random(3, 7);
                    break;
                }
                break;
            case 0xad:
                artifactId18 = GetRandomArtifactId(14, 0);
                cell6->triggerType = 0xa9;
                ConvertObject(x10 - 1, y8, x10 - 1, y8,
                              11, 0xa2, 0xa2, 11, artifactId18 * 2, -1, -1);
                ConvertObject(x10, y8, x10, y8,
                              11, 0xa3, 0xa3, 11, artifactId18 * 2 + 1, -1, -1);
                break;
            case 0xf4:
                artifactId18 = GetRandomArtifactId(8, 0);
                cell6->triggerType = 0xa9;
                ConvertObject(x10 - 1, y8, x10 - 1, y8,
                              11, 0xa6, 0xa6, 11, artifactId18 * 2, -1, -1);
                ConvertObject(x10, y8, x10, y8,
                              11, 0xa7, 0xa7, 11, artifactId18 * 2 + 1, -1, -1);
                break;
            case 0xf5:
                artifactId18 = GetRandomArtifactId(4, 0);
                cell6->triggerType = 0xa9;
                ConvertObject(x10 - 1, y8, x10 - 1, y8,
                              11, 0xa8, 0xa8, 11, artifactId18 * 2, -1, -1);
                ConvertObject(x10, y8, x10, y8,
                              11, 0xa9, 0xa9, 11, artifactId18 * 2 + 1, -1, -1);
                break;
            case 0xf6:
                artifactId18 = GetRandomArtifactId(2, 0);
                cell6->triggerType = 0xa9;
                ConvertObject(x10 - 1, y8, x10 - 1, y8,
                              11, 0xaa, 0xaa, 11, artifactId18 * 2, -1, -1);
                ConvertObject(x10, y8, x10, y8,
                              11, 0xab, 0xab, 11, artifactId18 * 2 + 1, -1, -1);
                break;
            case 0xb2:
                RandomizeMine(x10, y8);
                break;
            }
        }
    }
}

// @early-stop
// Logic and frame slots are byte-exact. The only residual is the TU-cumulative /Od
// polarity/load order of cutoff <= visibility: retail emits cmp cutoff,visibility; jg,
// while this partial TU emits the relationally equivalent cmp visibility,cutoff; jl.
// Relational swaps, negation, an empty else arm, |0 steering, and the fixed AST permuter
// do not alter it; the other loop/index order differences disappeared as header state landed.
VA(0x00480b64, 0x230)
void game::SetVisibility(int x, int y, int player, int radius)
{
    int col;
    int cutoff;
    int row;
    unsigned char mask = static_cast<unsigned char>(1 << player);
    int visibility;

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
                int distance = static_cast<int>(sqrt(static_cast<double>(
                    (x - col) * (x - col) + (y - row) * (y - row))));
                if (distance < radius) {
                    mapExtra[(MAP_WIDTH | 0) * row + col] |= mask;
                }
            }
        }
    } else {
        for (row = y - radius; row <= y + radius; row++) {
            for (col = x - radius; col <= x + radius; col++) {
                visibility = radius - abs(y - row) + radius - abs(x - col);
                if (visibility >= cutoff && col >= 0 && row >= 0 &&
                    col < MAP_WIDTH && row < MAP_HEIGHT) {
                    mapExtra[(MAP_WIDTH | 0) * row + col] |= mask;
                }
            }
        }
    }
}

// @early-stop
// Logic + frame slots byte-exact; residual is 3 commutative operand-load swaps (the
// inner-loop test y<MAP_HEIGHT and the two y*MAP_WIDTH index multiplies load the OTHER
// operand into eax first). Not source-steerable (operand order / reversed compare /
// extra temp all tested - no effect): it is the TU-cumulative /Od eval-order parity of
// the partial GAME TU (most preceding functions are still placeholders, so the temp
// counter is off from retail). Same class as the ExperienceValueOfStack @early-stop;
// aligns when GAME is fuller.
VA(0x00480d94, 0xd8)
void game::MakeAllWaterVisible(int player)
{
    char mask = (char)(1 << player);
    int x;
    int y;
    for (x = 0; x < MAP_WIDTH; x++) {
        for (y = 0; y < MAP_HEIGHT; y++) {
            if (giGroundToTerrain[WORLDMAP->Row(y)[x].tile] == 0)
                mapExtra[y * MAP_WIDTH + x] |= mask;
        }
    }
}

VA(0x00480e6c, 0xfc)
void game::GiveArmy(armyGroup *group, int type, int count, int slot)
{
    int tmp;
    int i;
    if (slot >= 0) {
        i = slot;
        ((char *)group)[i] = (char)type;
        i[(short *)((char *)group + 5)] = 0;
    } else {
        for (i = 0; i < 5; i++) {
            if (((signed char *)group)[i] == type)
                break;
        }
        if (i >= 5) {
            for (i = 0; i < 5; i++) {
                if (((signed char *)group)[i] < 0) {
                    i[(short *)((char *)group + 5)] = 0;
                    break;
                }
            }
        }
        if (i >= 5)
            return;
    }
    ((char *)group)[i] = (char)type;
    i[(short *)((char *)group + 5)] += count;
}

// @early-stop
// ~96%: only the operand-load order of one ((signed char*)group)[i] read differs
// (retail loads i then group; we load group then i) — the movsbl(%eax,%ecx) bytes are
// identical (commutative address), only the two preceding movs swap. Proven to compile
// byte-exact in isolation; the flip is a TU-global eval-order effect of the partial GAME
// TU. Re-check when GAME is fuller.
VA(0x00480f68, 0x91)
int game::ExperienceValueOfStack(armyGroup *group, hero *h)
{
    int exp = 0;
    int i;
    for (i = 0; i < 5; i++) {
        if (((short *)((char *)group + 5))[i] > 0) {
            exp += gCreatureInfo[((signed char *)group)[i]].value * ((short *)((char *)group + 5))[i];
        }
    }
    if (h != 0)
        exp += 0x1f4;
    return exp;
}

VA(0x00480ff9, 0x126)
int game::GetLuck(hero *h, class army *, town *castle)
{
    int luck;
    if (h == 0)
        return LUCK_NEUTRAL;
    luck = LUCK_NEUTRAL;
    if (h->HasArtifact(ARTIFACT_RABBIT_FOOT))
        luck++;
    if (h->HasArtifact(ARTIFACT_GOLDEN_HORSESHOE))
        luck++;
    if (h->HasArtifact(ARTIFACT_GAMBLERS_LUCKY_COIN))
        luck++;
    if (h->HasArtifact(ARTIFACT_FOUR_LEAF_CLOVER))
        luck++;
    if (h->HasArtifact(ARTIFACT_MASTHEAD) &&
        (h->m_eventFlags & HERO_EVENT_EMBARKED)) {
        luck++;
    }
    luck += h->m_luck;
    luck += h->m_secondarySkills[HERO_SKILL_LUCK];
    if (luck < LUCK_MINIMUM)
        luck = LUCK_MINIMUM;
    if (luck > LUCK_MAXIMUM)
        luck = LUCK_MAXIMUM;
    if (h->HasArtifact(ARTIFACT_BATTLE_GARB))
        luck = LUCK_MAXIMUM;
    if (castle != 0 && castle->m_type == TOWN_TYPE_SORCERESS &&
        (castle->m_buildings & TOWN_BUILDING_RAINBOW)) {
        luck += LUCK_RAINBOW_BONUS;
    }
    return luck;
}

// @early-stop
// Logic + frame slots byte-exact (col/row/mask + nested x/y land on retail's -0x4..-0x14
// via the {} block); residual is the same TU-cumulative /Od eval-order parity as
// MakeAllWaterVisible - the inner-loop test and the y*MAP_WIDTH multiplies load the other
// operand first. Aligns when GAME is fuller.
VA(0x0048111f, 0xf1)
void game::SetupAdjacentMons(void)
{
    int col;
    int row;
    unsigned char mask = 0x7f;
    {
        int x;
        int y;
        for (x = 0; x < MAP_WIDTH; x++) {
            for (y = 0; y < MAP_HEIGHT; y++) {
                if (gpAdvManager->FindAdjacentMonster(x, y, &col, &row, -1, -1))
                    mapExtra[y * MAP_WIDTH + x] |= 0x80;
                else
                    mapExtra[y * MAP_WIDTH + x] &= mask;
            }
        }
    }
}

VA(0x00481210, 0x61)
void game::CancelComputerScreen(void)
{
    TurnOffAIMusic();
    bShowIt = 1;
    int i;
    for (i = 1; i <= 6; i++) {
        gpWindowManager->BroadcastMessage(0x200, 6, i, 0x4008);
    }
}

VA(0x00481271, 0xed)
void game::ShowComputerScreen(void)
{
    if (gConfig.blackoutComputer) {
        int saved = gbThisNetHumanPlayer[giCurPlayer];
        gbThisNetHumanPlayer[giCurPlayer] = 1;
        int i;
        for (i = 1; i <= 6; i++)
            gpWindowManager->BroadcastMessage(0x200, 5, i, 0x4008);
        gbAllBlack = 1;
        gpAdvManager->CompleteDraw(1);
        gpAdvManager->UpdateHeroLocators(1, 1);
        gpAdvManager->UpdateTownLocators(1, 1);
        gpAdvManager->UpdBottomView(1, 1, 1);
        gpAdvManager->UpdateScreen(0, 1);
        gbAllBlack = 0;
        gbThisNetHumanPlayer[giCurPlayer] = (signed char)saved;
    }
    ShowHeroesLogo();
}

VA(0x0048135e, 0xa0)
void game::ShowHeroesLogo(void)
{
    if (*(int *)((char *)gpAdvManager + 0x37a) == 0) {
        *(int *)((char *)gpAdvManager + 0x37a) = 1;
        icon *theIcon = gpResourceManager->GetIcon("herologo.icn");
        IconToBitmap(theIcon, *(bitmap **)((char *)gpWindowManager + 0x46),
                     0x1e0, 0x10, 0, 0, 0, 0, 0x280, 0x1e0, 0);
        gpWindowManager->UpdateScreenRegion(0x1e0, 0x10, 0x90, 0x90);
        gpResourceManager->Dispose((resource *)theIcon);
    }
}

VA(0x004813fe, 0x143)
void game::WaitForPlayer(char *text, int player)
{
    if (gbBlackoutPlayer && giNumHumanPlayers > 1 && !gbRemoteOn) {
        gpMouseManager->SetPointer(0);
        gbAllBlack = 1;
        giBottomViewOverrideEndTime = KBTickCount() + WAIT_BOTTOM_VIEW_TIMEOUT;
        giBottomViewOverride = gbThisNetHumanPlayer[giCurPlayer] != 0;
        gpSoundManager->m_samplesReady = 1;
        gpSoundManager->SwitchAmbientMusic(WAIT_AMBIENT_MUSIC);
        gpAdvManager->CompleteDraw(1);
        gpAdvManager->UpdateHeroLocators(1, 1);
        gpAdvManager->UpdateTownLocators(1, 1);
        gpAdvManager->UpdateScreen(0, 1);
        ShowHeroesLogo();
        gbAllBlack = 0;
        NormalDialog(text, 1, -1, -1, WAIT_DIALOG_TYPE,
                     static_cast<signed char>(gpGame->m_players[player].color),
                     -1, 0, -1, 0);
        gpSoundManager->SwitchAmbientMusic(-1);
    }
}

// @early-stop
// Computation byte-exact; residual is 2 inline-accessor jmp$+0 brackets the /Ob1
// expander places leading (after the ternary test) where retail places them trailing
// (after the Extra() body) - the documented /Od /Ob1 block-boundary artifact, identical
// in kind to EDITOR/mapcell GetNewCellExtra*. See docs/patterns/inline-accessors.md.
VA(0x00481541, 0x104)
int game::HasLateOverlay(int col, int row)
{
    mapCell *cell = WORLDMAP->Row(row) + col;
    if (cell->ovlFlag1)
        return 1;
    mapCellExtra *extra = cell->extra ? WORLDMAP->Extra(cell->extra) : 0;
    while (extra) {
        if (extra->ovlFlag1)
            return 1;
        extra = extra->index ? WORLDMAP->Extra(extra->index) : 0;
    }
    return 0;
}

// @early-stop
// Instruction stream, frame slots, and traversal are byte-exact after excluding two
// five-byte Extra() inline continuation jumps. Both jumps exist on each side, but C2
// places ours trailing and retail's leading; there are no relocations in either function.
VA(0x00481645, 0x120)
void game::ConvertFlagToLateOverlay(int col, int row)
{
    mapCell *cell = WORLDMAP->Row(row) + col;
    if (cell->ovlTileset == MAP_TILESET_FLAG)
        cell->ovlFlag1 = 1;
    mapCellExtra *extra = cell->extra ? WORLDMAP->Extra(cell->extra) : 0;
    while (extra) {
        if (extra->ovlTileset == MAP_TILESET_FLAG)
            extra->ovlFlag1 = 1;
        extra = extra->index ? WORLDMAP->Extra(extra->index) : 0;
    }
}

// @early-stop
// Instruction stream, frame slots, and traversal are byte-exact after excluding two
// five-byte Extra() inline continuation jumps. Both jumps exist on each side, but C2
// places ours trailing and retail's leading; there are no relocations in either function.
VA(0x00481765, 0x13b)
int game::HasObjectTilesetIndex(int col, int row, int tileset, int index)
{
    mapCell *cell = WORLDMAP->Row(row) + col;
    if (cell->objTileset == tileset && cell->objIndex == index)
        return 1;
    mapCellExtra *extra = cell->extra ? WORLDMAP->Extra(cell->extra) : 0;
    while (extra) {
        if (extra->objTileset == tileset && extra->objIndex == index)
            return 1;
        extra = extra->index ? WORLDMAP->Extra(extra->index) : 0;
    }
    return 0;
}

// @early-stop
// Twin of HasLateOverlay: computation byte-exact; only the 2 inline-accessor jmp$+0
// brackets (Extra()) are placed leading vs retail's trailing - the same /Od /Ob1
// block-boundary artifact. See docs/patterns/inline-accessors.md.
VA(0x004818a0, 0x112)
void game::ConvertAllToLateOverlay(int col, int row)
{
    mapCell *cell = WORLDMAP->Row(row) + col;
    if (cell->ovlIndex != 0xff)
        cell->ovlFlag1 = 1;
    mapCellExtra *extra = cell->extra ? WORLDMAP->Extra(cell->extra) : 0;
    while (extra) {
        if (extra->ovlIndex != 0xff)
            extra->ovlFlag1 = 1;
        extra = extra->index ? WORLDMAP->Extra(extra->index) : 0;
    }
}

// @early-stop
// Logic is complete. The residual is one coupled TU-cumulative /Od lowering choice:
// retail evaluates the packed w4hi lvalue first and reserves two hidden temporary words;
// this partial TU evaluates townId first and omits them. The same parity flips the three
// inner MAP_WIDTH comparisons. Direct packed-word spellings, |0 steering, relational
// swaps, and the fixed AST permuter did not reproduce the retail field-first lowering;
// anonymous source padding would conceal rather than reconstruct this compiler state.
VA(0x004819b2, 0x295)
void game::ProcessMapExtra(void)
{
    int row;
    int col;
    int townId;
    mapCell *cell;

    for (row = 0; row < MAP_HEIGHT; row++) {
        for (col = 0; MAP_WIDTH > col; col++) {
            cell = WORLDMAP->Row(row) + col;
            switch (cell->triggerType) {
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_CASTLE:
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_RANDOM_TOWN:
            case MAP_EVENT_ACTION_FLAG | MAP_EVENT_RANDOM_CASTLE:
                townId = GetTownId(col, row);
                m_castleRecs[townId].m_extraIndex = cell->w4hi;
                cell->w4hi = townId;
                break;
            }
        }
    }

    for (row = 0; row < MAP_HEIGHT; row++) {
        for (col = 0; MAP_WIDTH > col; col++) {
            cell = WORLDMAP->Row(row) + col;
            if (cell->triggerType == (MAP_EVENT_ACTION_FLAG | MAP_EVENT_MINE) &&
                row > 0 && HasLateOverlay(col, row - 1)) {
                ConvertFlagToLateOverlay(col, row);
            }
            if (cell->triggerType == (MAP_EVENT_ACTION_FLAG | MAP_EVENT_ALCHEMIST_LAB)) {
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
void game::SetupTowns(void)
{
    char defaultDwellingRoll[12];
    signed char usedSpells[65];
    int spellsPerLevel[5];
    int townIndex;
    int slot;
    int owner;
    int building;
    int spellLevel;
    int spellSlot;
    int spell;
    int roll;
    int attempts;
    int spellValue;
    int combatSpells;
    int dwellingCount;
    unsigned int extraIndex;
    townSlot *castle;
    mapTownExtra *extra;

    for (townIndex = 0; townIndex < GAME_TOWN_COUNT; townIndex++) {
        if (!m_castleRecs[townIndex].m_onMap)
            continue;
        castle = reinterpret_cast<townSlot *>(GetTown(townIndex));

        extraIndex = castle->m_extraIndex;
        extra = reinterpret_cast<mapTownExtra *>(ppMapExtra[extraIndex]);
        if (extra->color == -1)
            owner = -1;
        else
            owner = gcColorToPlayerPos[extra->color];
        ClaimTown(townIndex, owner, 0);
        castle->m_originalOwner = static_cast<signed char>(owner);

        if (extra->hasCustomArmy) {
            for (slot = 0; slot < 5; slot++) {
                castle->m_army.m_troopCounts[slot] = extra->troopCounts[slot];
                if (static_cast<short>(castle->m_army.m_troopCounts[slot]) > 0)
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
            castle->buildings =
                (gTownEligibleBuildMask[castle->race] & extra->buildings) |
                (castle->buildings & 0x60);
            castle->unknown1c = extra->mageGuildLevel;
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
            castle->buildings |= 0x80000;
            if (!gbHumanPlayer[castle->owner] && dwellingCount == 1 && Random(1, 10) < 4)
                dwellingCount++;
            if (--dwellingCount != 0)
                castle->buildings |= 0x100000;
            dwellingCount--;
            castle->unknown1c = 0;
        }

        for (building = 25; building <= 30; building++) {
            if (castle->buildings & (1 << building)) {
                if (building == 30)
                    castle->buildings &= -553648129;
                else
                    castle->buildings &= -1 - (1 << (building - 5));
            }
        }
        for (building = 19; building <= 30; building++) {
            if (castle->buildings & (1 << building)) {
                castle->dwellingGrowth[building - 19] =
                    gMonsterDatabase[gDwellingType[castle->race][building - 19]].growth;
            }
        }
        if (castle->buildings & 1) {
            for (slot = 1; slot <= castle->unknown1c; slot++) {
                castle->m_spellCounts[slot] = gSpellLimits[slot - 1];
                if (castle->race == 4 && (castle->buildings & 0x2000))
                    castle->m_spellCounts[slot]++;
            }
        }
        if (extra->hasShrine)
            castle->buildings |= 0x8000;
        castle->m_unknown37 = extra->unknown28;
        strcpy(castle->m_name, extra->name);

        memset(usedSpells, 0, 65);
        for (spellLevel = 0; spellLevel < 5; spellLevel++) {
            spellsPerLevel[spellLevel] = 0;
            for (spellSlot = 0; spellSlot < 4; spellSlot++)
                reinterpret_cast<signed char *>(castle->m_spells)
                    [spellLevel * 4 + spellSlot] = -1;
        }

        if (castle->race == 5 && castle->owner != -1 && !gbHumanPlayer[castle->owner]) {
            if (Random(0, 100) < 50)
                spell = 35;
            else
                spell = 36;
            spellLevel = gsSpellInfo[spell].level - 1;
            castle->m_spells[spellLevel][spellsPerLevel[spellLevel]] =
                static_cast<signed char>(spell);
            spellsPerLevel[spellLevel]++;
        }

        roll = Random(0, 100);
        if (roll < 25)
            spell = 23;
        else if (roll < 50)
            spell = 24;
        else if (roll < 75)
            spell = 22;
        else
            spell = 5;
        spellLevel = gsSpellInfo[spell].level - 1;
        castle->m_spells[spellLevel][spellsPerLevel[spellLevel]] =
            static_cast<signed char>(spell);
        spellsPerLevel[spellLevel]++;

        roll = Random(0, 100);
        if (roll < 20)
            spell = 25;
        else if (roll < 40)
            spell = 2;
        else if (roll < 60)
            spell = 0;
        else if (roll < 80)
            spell = 32;
        else
            spell = 33;
        spellLevel = gsSpellInfo[spell].level - 1;
        castle->m_spells[spellLevel][spellsPerLevel[spellLevel]] =
            static_cast<signed char>(spell);
        spellsPerLevel[spellLevel]++;

        for (spellLevel = 0; spellLevel < 5; spellLevel++) {
            combatSpells = 0;
            for (spellSlot = 0; spellSlot < 4; spellSlot++) {
                if (reinterpret_cast<signed char *>(castle->m_spells)
                        [spellLevel * 4 + spellSlot] != -1) {
                    usedSpells[reinterpret_cast<signed char *>(castle->m_spells)
                                   [spellLevel * 4 + spellSlot]] = 1;
                } else {
                    attempts = 0;
                    do {
                        spell = Random(0, 64);
                        while (gsSpellInfo[spell].level - 1 != spellLevel)
                            spell = Random(0, 64);
                        if (castle->owner != -1 && !gbHumanPlayer[castle->owner])
                            spellValue =
                                (gsSpellInfo[spell].attributes & 1 ? 4 : 1) *
                                    gsSpellInfo[spell].aiValue +
                                50;
                        else
                            spellValue = 1500;
                        if (spell == 56)
                            spellValue = 1500;
                    } while ((combatSpells == 1 &&
                              (gsSpellInfo[spell].attributes & 4)) ||
                             gsSpellInfo[spell].raceChance[castle->race] < Random(0, 10) ||
                             attempts++ > 500 || usedSpells[spell] ||
                             spellValue < Random(1, 1500));
                    if (gsSpellInfo[spell].attributes & 4)
                        combatSpells++;
                    reinterpret_cast<signed char *>(castle->m_spells)
                        [spellLevel * 4 + spellSlot] =
                        static_cast<signed char>(spell);
                    usedSpells[spell] = 1;
                }
            }
        }
        BaseFree(ppMapExtra[extraIndex], GFILE,
                 *reinterpret_cast<const short *>("\xef\x18") + 0xee);
        ppMapExtra[extraIndex] = 0;
    }
}

// @early-stop
// reloc-masked: identical frame/instruction stream and all 23 relocation sites align;
// only the folded identities of the line-number and source-file string literals differ.
VA(0x00482547, 0x774)
void game::ProcessOnMapHeroes(void)
{
    signed char usedHeroes[GAME_HERO_COUNT];
    int pass;
    int mapY;
    int mapX;
    int armySlot;
    int artifactSlot;
    int skillSlot;
    int heroClass;
    int heroId;
    unsigned int extraIndex;
    signed char isJail;
    hero *mapHero;
    mapHeroExtra *extra;
    mapCell *cell;
    mapCell *townCell;
    townSlot *occupiedTown;

    memset(usedHeroes, 0, GAME_HERO_COUNT);
    for (pass = 0; pass < 3; pass++) {
        for (mapY = 0; mapY < MAP_HEIGHT; mapY++) {
            for (mapX = 0; mapX < MAP_WIDTH; mapX++) {
                cell = &WORLDMAP->Row(mapY)[mapX];
                if ((cell->triggerType & MAP_EVENT_TYPE_MASK) == MAP_EVENT_HERO ||
                    cell->triggerType == (MAP_EVENT_ACTION_FLAG | MAP_EVENT_JAIL)) {

                if ((cell->triggerType & MAP_EVENT_TYPE_MASK) == MAP_EVENT_JAIL)
                    isJail = 1;
                else
                    isJail = 0;
                extraIndex = cell->w4hi;
                extra = reinterpret_cast<mapHeroExtra *>(ppMapExtra[extraIndex]);

                if (pass == 0) {
                    if (extra->hasCustomHero && extra->heroId < GAME_HERO_COUNT &&
                        !usedHeroes[extra->heroId]) {
                        usedHeroes[extra->heroId] = 1;
                        extra->hasAssignedHero = 1;
                    } else {
                        extra->hasAssignedHero = 0;
                    }
                    if (isJail) {
                        extra->owner = -1;
                    } else {
                        extra->owner = static_cast<signed char>(cell->objIndex / 7);
                        heroClass = gcColorToPlayerPos[extra->owner];
                        extra->owner = static_cast<signed char>(heroClass);
                    }
                }

                if (pass == 1) {
                    if (isJail) {
                        heroClass = extra->heroClass;
                    } else {
                        heroClass = cell->objIndex % 7;
                        if (heroClass == 6) {
                            heroClass = m_setupPlayerRace[
                                gcColorToSetupPos[gpGame->m_players[extra->owner].color]];
                        }
                    }

                    if (extra->hasAssignedHero) {
                        mapHero = GetHero(extra->heroId);
                        mapHero->m_cursorType = static_cast<unsigned char>(heroClass);
                    } else {
                        heroId = RandomScan(usedHeroes, heroClass * 9, 9, 1000, 0);
                        if (heroId == -1) {
                            heroId = RandomScan(usedHeroes, 0, GAME_HERO_COUNT, 10000, 0);
                            heroClass = heroId / 9;
                        }
                        usedHeroes[heroId] = 1;
                        mapHero = GetHero(heroId);
                        mapHero->m_cursorType = static_cast<unsigned char>(heroClass);
                        if (extra->hasCustomHero && extra->heroId >= GAME_HERO_COUNT)
                            mapHero->m_portrait = extra->heroId;
                        extra->heroId = static_cast<signed char>(heroId);
                    }
                }

                if (pass == 2) {
                    mapHero = GetHero(extra->heroId);
                    if (!isJail && extra->hasPatrol) {
                        mapHero->m_patrolX = static_cast<signed char>(mapX);
                        mapHero->m_patrolY = static_cast<signed char>(mapY);
                        mapHero->m_patrolRadius = extra->patrolRadius;
                    }
                    if (extra->hasCustomArmy) {
                        for (armySlot = 0; armySlot < 5; armySlot++) {
                            mapHero->m_army.m_troopCounts[armySlot] =
                                extra->troopCounts[armySlot];
                            if (static_cast<short>(mapHero->m_army.m_troopCounts[armySlot]) > 0)
                                mapHero->m_army.m_troopTypes[armySlot] =
                                    extra->troopTypes[armySlot];
                            else
                                mapHero->m_army.m_troopTypes[armySlot] = -1;
                        }
                    }
                    for (artifactSlot = 0; artifactSlot < 3; artifactSlot++) {
                        if (extra->artifacts[artifactSlot] >= 0)
                            GiveArtifact(mapHero, extra->artifacts[artifactSlot], 1, -1);
                    }
                    if (extra->hasCustomName)
                        strcpy(mapHero->m_name, extra->name);
                    mapHero->m_experience = 0;
                    gpAdvManager->GiveExperience(mapHero, extra->experience, 1);
                    mapHero->CheckLevel();
                    mapHero->m_x = mapX;
                    mapHero->m_y = mapY;

                    if (isJail) {
                        mapHero->m_owner = -1;
                        m_availableHeroes[extra->heroId] = 0x41;
                    } else {
                        mapHero->m_owner = extra->owner;
                        m_availableHeroes[extra->heroId] = mapHero->m_owner;
                        m_players[mapHero->m_owner]
                            .heroes[m_players[mapHero->m_owner].heroCount] = mapHero->m_id;
                        m_players[mapHero->m_owner].heroCount++;
                    }

                    if (!isJail && mapY > 0) {
                        townCell = &WORLDMAP->Row(mapY - 1)[mapX];
                        if (townCell->triggerType ==
                            (MAP_EVENT_ACTION_FLAG | MAP_EVENT_CASTLE)) {
                            mapHero->m_patrolY--;
                            mapHero->m_y--;
                            occupiedTown = reinterpret_cast<townSlot *>(
                                GetTown(GetTownId(mapX, mapY - 1)));
                            occupiedTown->occupyingHeroId = mapHero->m_id;
                        }
                    }

                    if (isJail) {
                        cell->w4hi = extra->heroId;
                    } else {
                        cell->objTileset = 0;
                        cell->objIndex = 0xff;
                        cell->w4hi = 0;
                        cell->triggerType = 0;
                    }

                    if (extra->hasCustomSkills) {
                        mapHero->m_secondarySkillCount = 0;
                        for (skillSlot = 0; skillSlot < 14; skillSlot++) {
                            mapHero->m_secondarySkills[skillSlot] = 0;
                            mapHero->m_secondarySkillOrder[skillSlot] = 0;
                        }
                        for (skillSlot = 0; skillSlot < 8; skillSlot++) {
                            if (extra->skillTypes[skillSlot] != -1) {
                                mapHero->GiveSS(extra->skillTypes[skillSlot],
                                                extra->skillLevels[skillSlot]);
                            }
                        }
                    }
                    if (!isJail) {
                        SetVisibility(mapHero->m_x, mapHero->m_y, mapHero->m_owner,
                                      giVisRange[mapHero->m_secondarySkills[3]]);
                    }
                    BaseFree(ppMapExtra[extraIndex], GFILE,
                             *reinterpret_cast<const short *>("\xef\x18") + 0xdd);
                    ppMapExtra[extraIndex] = 0;
                }
                }
            }
        }
    }
}

// @early-stop
// Frame layout and all seven relocations are exact. The 0x8-byte size residual is one
// five-byte inlined hero-bounds continuation plus three bytes of equivalent commutative
// packed-record index arithmetic; every ownership, repair, and army check realigns.
VA(0x00482cbb, 0x55e)
void game::CheckHeroConsistency(void)
{
    hero *mapHero3;
    mapCell *cell1;
    int x11;
    int y8;
    int player3;
    int slot1;
    int total26 = 0;
    int consistent13;
    town *occupiedTown9;

    for (player3 = 0; player3 < m_playerCount; player3++) {
        if (m_playerDead[player3] != 0)
            continue;
        total26 += m_players[player3].heroCount;
        for (slot1 = 0; slot1 < m_players[player3].heroCount; slot1++) {
            if (m_heroRecs[m_players[player3].heroes[slot1]].m_owner != player3)
                consistent13 = 0;
        }
    }

    for (player3 = 0; player3 < m_playerCount; player3++) {
        if (m_playerDead[player3] == 0) {
            for (slot1 = 0; slot1 < 2; slot1++) {
                if ((m_availableHeroes[m_players[player3].availableHeroes[slot1]] >= 0 &&
                     m_availableHeroes[m_players[player3].availableHeroes[slot1]] <= 5) ||
                    (total26 < 40 &&
                     m_availableHeroes[m_players[player3].availableHeroes[slot1]] == -1)) {
                    m_players[player3].availableHeroes[slot1] =
                        static_cast<signed char>(GetNewHeroId(player3, -1, 0));
                    m_availableHeroes[m_players[player3].availableHeroes[slot1]] = 64;
                }
            }
        }
    }

    for (x11 = 0; x11 < MAP_WIDTH; x11++) {
        for (y8 = 0; y8 < MAP_HEIGHT; y8++) {
            cell1 = gpAdvManager->GetCell(x11, y8);
            if (cell1->triggerType == 0xaa) {
                if (cell1->w4hi >= 0 && cell1->w4hi < 54) {
                    mapHero3 = &m_heroRecs[cell1->w4hi];
                    if (mapHero3->m_x != x11 || mapHero3->m_y != y8) {
                        cell1->triggerType = 0;
                        cell1->w4hi = 0;
                    }
                    if (mapHero3->m_owner < 0 || mapHero3->m_owner >= 6) {
                        if (mapHero3->m_locationType == 0xa3) {
                            occupiedTown9 = GetCastle(mapHero3->m_occupiedTown);
                            occupiedTown9->m_occupyingHeroId = -1;
                        }
                        if (mapHero3->m_x == x11 && mapHero3->m_y == y8) {
                            RestoreCell(mapHero3->m_x, mapHero3->m_y,
                                        mapHero3->m_locationType,
                                        mapHero3->m_occupiedTown, 0, 1);
                        } else {
                            cell1->triggerType = 0;
                            cell1->w4hi = 0;
                        }
                    }
                } else {
                    cell1->triggerType = 0;
                }
            }
        }
    }

    for (player3 = 0; player3 < 54; player3++) {
        for (slot1 = 0; slot1 < 5; slot1++) {
            if (m_heroRecs[player3].m_army.m_troopTypes[slot1] == -1 ||
                m_heroRecs[player3].m_army.m_creatureCounts[slot1] < 0)
                m_heroRecs[player3].m_army.m_creatureCounts[slot1] = 0;
        }
    }
    for (player3 = 0; player3 < 72; player3++) {
        for (slot1 = 0; slot1 < 5; slot1++) {
            if (m_castleRecs[player3].m_army.m_troopTypes[slot1] == -1 ||
                m_castleRecs[player3].m_army.m_creatureCounts[slot1] < 0)
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
// Exact 0x71e-byte span with the same 94 relocation sites. Five masked bytes
// differ: +0x524/+0x527/+0x529 reverse the packet/batch-bound loads, and
// +0x640/+0x643 reverse the fileData/transmitData equality loads. Both source
// operand orders emit identically; these are commutative /Od TU-state choices.
VA(0x00483219, 0x71e)
int game::TransmitSaveGame(int remotePlayer, int player, int useCurrentSave)
{
    int success;
    int samplesReady;
    char filename[456];
    unsigned char *transmitData;
    int batchCount;
    int packet;
    int packetCount;
    int unused484;
    int oldTrack;
    int file;
    int unused3c12;
    int batch;
    int *header;
    int packetsInBatch;
    unsigned int transmitCrc;
    char *reply;
    void *acknowledged;
    unsigned int fileCrc;
    int unused1c3;
    int result;
    int unused140;
    int fileSize;
    int chunkSize;
    unsigned char *fileData;
    int done;

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
    oldTrack = static_cast<signed char>(gpSoundManager->m_currentTrack);
    gpSoundManager->SwitchAmbientMusic(-1);
    gpSoundManager->m_samplesReady = samplesReady;

    LogStr(const_cast<char *>("Transmit Game Start"));
    if (gpAdvManager->m_active == 1)
        BVResMsg(const_cast<char *>("Sending Data"), -1, 0);
    AiPrint(const_cast<char *>("Transmit Start - Compressing"));

    acknowledged = BaseAlloc(5000, GFILE, GTRANSMITLINE + 0x2b);
    memset(acknowledged, 0, 5000);
    SaveGame(gConfig.rmtSCName, 0, 0);
    if (!gbUseDiffCompression)
        useCurrentSave = 1;
    CreateDiffFile(gConfig.rmtSLName, gConfig.rmtSCName, gConfig.rmtSDName,
                   remotePlayer, useCurrentSave);
    sprintf(filename, "%s%s", ".\\DATA\\", gConfig.rmtSDName);
    fileSize = FileSize(filename);
    LogInt(const_cast<char *>("PostDiffFileSize"), fileSize,
           -999, -999, -999, -999, -999, -999);

    header = static_cast<int *>(BaseAlloc(0x100, GFILE, GTRANSMITLINE + 0x3f));
    if (gbUseRegularCompression)
        transmitData = static_cast<unsigned char *>(
            BaseAlloc(fileSize + 2000, GFILE, GTRANSMITLINE + 0x41));
    fileData = static_cast<unsigned char *>(
        BaseAlloc(fileSize + 2000, GFILE, GTRANSMITLINE + 0x42));

    file = _open(filename, 0x8000);
    if (file == -1)
        FileError(filename);
    if (file == -1) {
        goto transmitCleanup;
    }
    {
        _read(file, fileData, fileSize);
        _close(file);
        fileCrc = calc_crc_long(fileData, fileSize);
        if (gbUseRegularCompression)
            fileSize = EncodeData(reinterpret_cast<char *>(transmitData),
                                  reinterpret_cast<char *>(fileData), fileSize);
        else
            transmitData = fileData;

        AiPrint(const_cast<char *>("Transmit Start - Sending"));
        if (gbUseRegularCompression)
            transmitCrc = calc_crc_long(transmitData, fileSize);
        else
            transmitCrc = fileCrc;
        LogInt(const_cast<char *>("Send"), fileSize, transmitCrc,
               -999, -999, -999, -999, -999);

        header[0] = fileSize;
        header[1] = fileCrc;
        header[2] = transmitCrc;
        header[3] = player;
        result = TransmitAndWait(reinterpret_cast<char *>(header), remotePlayer,
                                 16, 1, 2, &reply);
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
                for (packet = batch * 100;
                     packet < batch * 100 + packetsInBatch; packet++) {
                    PollSound();
                    CheckDoMain(0, 1);
                    if (!static_cast<char *>(acknowledged)[packet]) {
                        if (packet + 1 == packetCount)
                            chunkSize = fileSize - packet * 200;
                        else
                            chunkSize = 200;
                        *reinterpret_cast<short *>(header) = static_cast<short>(packet);
                        memcpy(reinterpret_cast<char *>(header) + 2,
                               transmitData + packet * 200, chunkSize);
                        result = TransmitRemoteData(reinterpret_cast<char *>(header),
                                                    remotePlayer, chunkSize + 2,
                                                    3, 0, 1, -1);
                        if (!result)
                            ShutDown(0);
                    }
                }
                LogStr(const_cast<char *>("PreWait"));
                *reinterpret_cast<short *>(header) = static_cast<short>(batch * 100);
                result = TransmitAndWait(reinterpret_cast<char *>(header), remotePlayer,
                                         2, 4, 5, &reply);
                LogStr(const_cast<char *>("PostWait"));
                if (!result)
                    ShutDown(0);
                for (packet = 0; packetsInBatch > packet; packet++) {
                    if (reply[packet + 9] > 0)
                        static_cast<char *>(acknowledged)[batch * 100 + packet] = 1;
                }
                done = 1;
                for (packet = batch * 100;
                     packet < batch * 100 + packetsInBatch; packet++) {
                    if (!static_cast<char *>(acknowledged)[packet])
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
        BaseFree(header, GFILE, GTRANSMITLINE + 199);
    if (transmitData)
        BaseFree(transmitData, GFILE, GTRANSMITLINE + 200);
    if (fileData && fileData != transmitData)
        BaseFree(fileData, GFILE, GTRANSMITLINE + 0xc9);
    if (acknowledged)
        BaseFree(acknowledged, GFILE, GTRANSMITLINE + 0xca);

    AiPrint(const_cast<char *>("Transmit End"));
    if (gpAdvManager->m_active == 1) {
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

// @early-stop
// Relocation-masked comparison is identical for all 0x68d bytes (102 relocation
// sites in both objects).
VA(0x00483937, 0x68d)
int game::ReceiveSaveGame(int dataSize, int expectedCrc, int expectedTransmitCrc,
                          int remotePlayer)
{
    int unused20819;
    int success;
    int samplesReady;
    char filename[452];
    unsigned char *incomingData;
    int index;
    int oldTrack;
    int file;
    int computedCrc;
    int finished;
    int receivedCrc;
    unsigned char *ackBuffer;
    char *packet;
    int result;
    long lastPacketTime;
    char *received;
    int packetStart;
    unsigned char *decodedData;

    LogInt(const_cast<char *>("RSG1"), remotePlayer,
           -999, -999, -999, -999, -999, -999);
    LogStr(const_cast<char *>("Receive"));
    AiPrint(const_cast<char *>("Receive Start - Getting Data"));
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
    if (gpAdvManager->m_active == 1)
        BVResMsg(const_cast<char *>("Receiving Data"), -1, 0);

    samplesReady = gpSoundManager->m_samplesReady;
    oldTrack = static_cast<signed char>(gpSoundManager->m_currentTrack);
    gpSoundManager->m_samplesReady = 1;
    gpSoundManager->SwitchAmbientMusic(-1);
    gpSoundManager->m_samplesReady = samplesReady;

    LogStr(const_cast<char *>("Begin Transmit Init Confirm"));
    result = TransmitRemoteData(0, remotePlayer, 0, 2, 1, 1, -1);
    LogStr(const_cast<char *>("End Transmit Init Confirm"));
    if (!result)
        ShutDown(0);

    received = static_cast<char *>(BaseAlloc(5000, GFILE, GRECEIVELINE + 0x33));
    memset(received, 0, 5000);
    if (gbUseRegularCompression)
        decodedData = static_cast<unsigned char *>(
            BaseAlloc(700000, GFILE, GRECEIVELINE + 0x37));
    ackBuffer = static_cast<unsigned char *>(
        BaseAlloc(0x100, GFILE, GRECEIVELINE + 0x39));
    incomingData = static_cast<unsigned char *>(
        BaseAlloc(dataSize + 2000, GFILE, GRECEIVELINE + 0x3a));

    lastPacketTime = KBTickCount();
    LogInt(const_cast<char *>("FW2"), remotePlayer,
           -999, -999, -999, -999, -999, -999);
    while (!finished) {
        PollSound();
        CheckDoMain(0, 1);
        if (KBTickCount() > lastPacketTime + 90000) {
            NormalDialog(const_cast<char *>("Error receiving data.  Keep trying?"),
                         2, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gpWindowManager->m_dialogResult == 0x7805)
                lastPacketTime = KBTickCount();
            else
                ShutDown(0);
        }

        packet = GetRemoteData(1);
        if (packet && (packet[5] == 2 || packet[5] == 3)) {
            lastPacketTime = KBTickCount();
            switch (packet[6]) {
            case 3:
                packetStart = *reinterpret_cast<short *>(packet + 9);
                received[packetStart] = 1;
                memcpy(incomingData + packetStart * 200, packet + 11,
                       *reinterpret_cast<short *>(packet + 7) - 2);
                break;
            case 4:
                packetStart = *reinterpret_cast<short *>(packet + 9);
                for (index = packetStart; index < packetStart + 100; index++)
                    *(ackBuffer + index - packetStart) = received[index];
                LogInt(const_cast<char *>("FW3"), remotePlayer,
                       -999, -999, -999, -999, -999, -999);
                result = TransmitRemoteData(reinterpret_cast<char *>(ackBuffer),
                                            remotePlayer, 200, 5, 1, 1, -1);
                if (!result)
                    ShutDown(0);
                break;
            case 6:
                finished = 1;
                break;
            }
        }
    }

    AiPrint(const_cast<char *>("Receive Start - Decompressing Data"));
    receivedCrc = calc_crc_long(incomingData, dataSize);
    LogInt(const_cast<char *>("Receive"), dataSize, receivedCrc,
           expectedTransmitCrc, -999, -999, -999, -999);
    if (gbUseRegularCompression) {
        dataSize = DecodeData(reinterpret_cast<char *>(decodedData),
                              reinterpret_cast<char *>(incomingData), dataSize);
        computedCrc = calc_crc_long(decodedData, dataSize);
    } else {
        decodedData = incomingData;
        computedCrc = receivedCrc;
    }
    LogInt(const_cast<char *>("Receive"), dataSize, computedCrc,
           expectedCrc, -999, -999, -999, -999);

    sprintf(filename, "%s%s", ".\\DATA\\", gConfig.rmtRDName);
    file = _open(filename, 0x8301, 0x80);
    if (file == -1)
        FileError(filename);
    _write(file, decodedData, dataSize);
    _close(file);
    success = 1;

    if (received)
        BaseFree(received, GFILE, GRECEIVELINE + 0xa1);
    if (ackBuffer)
        BaseFree(ackBuffer, GFILE, GRECEIVELINE + 0xa2);
    if (incomingData)
        BaseFree(incomingData, GFILE, GRECEIVELINE + 0xa3);
    if (decodedData && incomingData != decodedData)
        BaseFree(decodedData, GFILE, GRECEIVELINE + 0xa4);

    CreateJoinFile(gConfig.rmtRLName, gConfig.rmtRDName, gConfig.rmtRCName);
    AiPrint(const_cast<char *>("Receive End"));
    if (gpAdvManager->m_active == 1) {
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

// @early-stop
// Both objects have an exact 0x455-byte span and 102 relocation sites. The sole masked
// byte difference is the +0x32 local epilogue-branch displacement, a delinked-label identity.
VA(0x00483fc4, 0x455)
void game::DoNewTurn(void)
{
    char musicFile18[16];
    char lowerName19[52];
    int musicTrack2;

    CheckForTimeEvent();
    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        CheckEndGame(0, 0);
    } else {
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
                sprintf(gText, cNewTurn[0], cPlayerNames[giCurPlayer],
                        gpCurPlayer->m_daysLeft);
            }
            NormalDialog(gText, 1, -1, -1, 9,
                         gpGame->GetPlayerColor(static_cast<signed char>(giCurPlayer)),
                         -1, 0, -1, 0);
        }

        if (gpCurPlayer->m_heroCount > 0) {
            gpAdvManager->SetHeroContext(gpCurPlayer->NextHero(0), 0);
        } else if (gpCurPlayer->m_townCount > 0) {
            gpAdvManager->SetTownContext(gpCurPlayer->m_townIds[0]);
        }
        gpAdvManager->CheckDimNextHeroBut();

        if (m_day == 1 &&
            (m_month != 1 || m_week != 1 || m_day != 1)) {
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
                        lowerName19[0] -= 0x20;
                        sprintf(gText, cNewTurn[3],
                                gArmyNames[giMonthTypeExtra], lowerName19);
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
                        lowerName19[0] -= 0x20;
                        sprintf(gText, cNewTurn[6],
                                gArmyNames[giWeekTypeExtra], lowerName19);
                    }
                }
                gpSoundManager->PlayAmbientMusic(musicTrack2, 0, -1);
                gpMouseManager->SetPointer(0);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                gpSoundManager->SwitchAmbientMusic(
                    giTerrainToMusicTrack[gpAdvManager->m_currentTerrain]);
            }
        }
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[gpAdvManager->m_currentTerrain]);
        gpAdvManager->SetEnvironmentOrigin(
            gpAdvManager->m_mapOriginX + 7,
            gpAdvManager->m_mapOriginY + 7, 1);
    }
}

VA(0x00484419, 0x58)
int game::GetBoatsBuilt(void)
{
    int count = 0;
    int i;
    for (i = 0; i < GAME_BOAT_COUNT; i++) {
        if (m_boatSlots[i] != -1)
            count++;
    }
    return count;
}

VA(0x00484471, 0x9c)
int game::GetNumThievesGuilds(int color)
{
    int num = 0;
    int i;
    for (i = 0; i < ((signed char *)this)[color * 283 + 0x4e0]; i++) {
        if (*(int *)((char *)gpGame + ((SThievesData *)this)->list[color][i] * 100 + 0xb6b) & 2)
            num++;
    }
    return num;
}

VA(0x0048450d, 0x113)
int game::CalcDifficultyRating(void)
{
    int notused;
    int rating = 0;
    if (m_difficulty == 0)
        rating += 0x32;
    else if (m_difficulty == 1)
        rating += 0x50;
    else if (m_difficulty == 2)
        rating += 0x64;
    else if (m_difficulty == 3)
        rating += 0x78;
    else if (m_difficulty == 4)
        rating += 0x8c;
    if (m_mapHeader.difficulty == 0)
        ;
    else if (m_mapHeader.difficulty == 1)
        rating += 0x14;
    else if (m_mapHeader.difficulty == 2)
        rating += 0x28;
    else if (m_mapHeader.difficulty == 3)
        rating += 0x50;
    return rating;
}

// @early-stop
// The frame and every non-jump instruction/operand match. Retail is exactly 45 bytes
// longer: nine five-byte local continuation/trampoline jumps; no jump table is present.
VA(0x00484620, 0x1ea)
int CalcBaseScore(int days)
{
    int score = GAME_SCORE_BASE;

    if (gpGame->m_mapHeader.width == GAME_SCORE_MAP_EXTRA_LARGE)
        days = static_cast<int>(days * 1.2);
    else if (gpGame->m_mapHeader.width == GAME_SCORE_MAP_LARGE)
        days = static_cast<int>(days * 1.1);
    else if (gpGame->m_mapHeader.width == GAME_SCORE_MAP_MEDIUM) {
    } else if (gpGame->m_mapHeader.width == GAME_SCORE_MAP_SMALL)
        days = static_cast<int>(days * 0.95);

    if (days <= GAME_SCORE_FIRST_TIER) {
        score -= -(-days);
    } else {
        score -= GAME_SCORE_FIRST_TIER;
        if (days <= GAME_SCORE_SECOND_TIER) {
            score = static_cast<int>(
                score - (days - GAME_SCORE_FIRST_TIER) * 0.5);
        } else {
            score = static_cast<int>(score - 30.0);
            if (days <= GAME_SCORE_THIRD_TIER) {
                score = static_cast<int>(
                    score - (days - GAME_SCORE_SECOND_TIER) * 0.25);
            } else {
                score = static_cast<int>(score - 60.0);
                score = static_cast<int>(
                    score - (days - GAME_SCORE_THIRD_TIER) * 0.125);
            }
        }
    }

    if (score < GAME_SCORE_MINIMUM)
        score = GAME_SCORE_MINIMUM;
    return score;
}

// @early-stop
// ~99.9%: logic byte-exact (matched 100% standalone); tiny residual is the same
// TU-cumulative /Od operand-load-order parity as the other GAME parks — should resolve
// as the surrounding GAME functions are reconstructed.
VA(0x0048480a, 0xb5)
void game::RestoreCell(int x, int y, int obj, int barrier, mapCell *passedCell, int p6)
{
    mapCell *cell;
    if (passedCell)
        cell = passedCell;
    else
        cell = gpAdvManager->GetCell(x, y);
    if (y > 0 && obj == 0xa3 &&
        ((unsigned char *)gpAdvManager->GetCell(x, y - 1))[9] != 0x23) {
        ((char *)cell)[9] = 0;
        cell->w4hi = 0;
    } else {
        ((char *)cell)[9] = (char)obj;
        cell->w4hi = barrier;
    }
}

// @early-stop
// Condition (3-term &&), reinit, and realloc (BaseFree/BaseAlloc/memset) all byte-exact;
// residual is 2 redundant jumps retail /Od emits for the empty then-branch of the
// if/else - an end-of-function trampoline (jmp $+0 class) plus a dead `jmp realloc` -
// that my build collapses to one direct jmp. A /Od jump-layout artifact of the empty
// then; not behaviorally meaningful.
VA(0x004848bf, 0xe3)
void game::SetMapSize(int w, int h)
{
    if (MAP_HEIGHT == h && MAP_WIDTH == w && bMapInitialized) {
    } else {
        bMapInitialized = 1;
        MAP_WIDTH = w;
        MAP_HEIGHT = h;
        gpSearchArray->Init();
    }
    if (mapExtra)
        BaseFree(mapExtra, GFILE, *(short *)"\x0d\x1d" + 0xc);
    mapExtra = (unsigned char *)BaseAlloc(MAP_WIDTH * MAP_HEIGHT, GFILE, *(short *)"\x0d\x1d" + 0xd);
    memset(mapExtra, 0, MAP_WIDTH * MAP_HEIGHT);
}

// @early-stop
// Logic + frame slots byte-exact; residual is the operand-eval order of the two
// `flags |= <extracted len bits>` ORs: retail loads `flags` into al first (then keeps
// len in ecx and pulls the shifted byte via ch), my build evaluates the value first and
// ORs flags from memory. Same TU-cumulative /Od eval-order parity as MakeAllWaterVisible;
// not source-steerable. Aligns when GAME is fuller.
VA(0x004849a2, 0x100)
void WriteDiffHeaderInfo(unsigned char cmd, int len, unsigned char *buf, int *pos)
{
    unsigned char flags = 0;
    flags = (cmd << 7) | flags;
    if (len > 0x1fff) {
        flags |= 0x40;
        flags |= (len & 0x2f0000) >> 16;
        unsigned short word = (unsigned short)(len & 0xffff);
        buf[*pos] = flags;
        *(unsigned short *)(buf + *pos + 1) = word;
        *pos += 3;
    } else if (len > 0x1f) {
        flags |= 0x20;
        flags |= (len >> 8) & 0x1f;
        unsigned char lo = (unsigned char)len;
        buf[*pos] = flags;
        buf[*pos + 1] = lo;
        *pos += 2;
    } else {
        flags |= (unsigned char)len;
        buf[*pos] = flags;
        (*pos)++;
    }
}

VA(0x00484aa2, 0xab)
int GetSkipCopyLen(unsigned char *buf, int *pos)
{
    unsigned char b = buf[*pos];
    int len;
    if (b & 0x40) {
        len = b & 0x3f;
        len <<= 16;
        len |= *(unsigned short *)(buf + *pos + 1);
        *pos += 3;
    } else if (b & 0x20) {
        len = b & 0x1f;
        len <<= 8;
        len |= buf[*pos + 1];
        *pos += 2;
    } else {
        len = b & 0x1f;
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
void CreateDiffFile(char *oldName, char *joinName, char *diffName,
                    int remotePlayer, int forceWhole)
{
    unsigned char *diffData6;
    // Retail's debug frame contains two otherwise unused words before joinData29.
    int unusedFirst0;
    int unusedSecond4;
    unsigned char *joinData29;
    int joinSize36;
    int joinFile1;
    int copyLength28;
    long startTime11;
    int diffSize29;
    unsigned char *oldData13;
    int oldSize37;
    int compareOffset4;
    int position1;
    int sendWhole4;
    int oldFile17;

    startTime11 = KBTickCount();
    oldData13 = 0;
    joinData29 = 0;
    diffData6 = 0;
    oldSize37 = 0;
    joinSize36 = 0;
    diffSize29 = 0;
    sendWhole4 = 0;

    if (forceWhole ||
        (iLastDiffSendTo != -1 && remotePlayer != iLastDiffSendTo))
        sendWhole4 = 1;
    iLastDiffSendTo = remotePlayer;

    sprintf(gText, "%s%s", ".\\DATA\\", joinName);
    joinSize36 = FileSize(gText);
    joinData29 = static_cast<unsigned char *>(
        BaseAlloc(joinSize36, GFILE, GDIFFLINE + 0x18));
    sprintf(gText, "%s%s", ".\\DATA\\", joinName);
    joinFile1 = _open(gText, 0x8000);
    if (joinFile1 == -1)
        FileError(gText);
    _read(joinFile1, joinData29, joinSize36);
    _close(joinFile1);
    LogInt(const_cast<char *>("Orig Join CRC"),
           calc_crc_long(joinData29, joinSize36), joinSize36,
           -999, -999, -999, -999, -999);

    if (!forceWhole) {
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        oldSize37 = FileSize(gText);
        oldData13 = static_cast<unsigned char *>(
            BaseAlloc(oldSize37, GFILE, GDIFFLINE + 0x2d));
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        oldFile17 = _open(gText, 0x8000);
        if (oldFile17 == -1)
            FileError(gText);
        _read(oldFile17, oldData13, oldSize37);
        _close(oldFile17);
    }

    diffData6 = static_cast<unsigned char *>(
        BaseAlloc((oldSize37 > joinSize36 ? oldSize37 : joinSize36) + 5000,
                  GFILE, GDIFFLINE + 0x37));
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
            if (position1 + copyLength28 >= oldSize37 ||
                position1 + copyLength28 >= joinSize36) {
                copyLength28 = oldSize37 - position1;
                WriteDiffHeaderInfo(1, copyLength28, diffData6, &diffSize29);
                memcpy(diffData6 + diffSize29,
                       joinData29 + position1, copyLength28);
                diffSize29 += copyLength28;
                position1 += copyLength28;
                copyLength28 = 0;
                break;
            }
            if (oldData13[position1 + copyLength28] ==
                joinData29[position1 + copyLength28]) {
                compareOffset4 = 1;
                while (position1 + compareOffset4 + copyLength28 < oldSize37 &&
                       position1 + compareOffset4 + copyLength28 < joinSize36 &&
                       oldData13[position1 + compareOffset4 + copyLength28] ==
                           joinData29[position1 + compareOffset4 + copyLength28])
                    compareOffset4++;
                if (compareOffset4 <= 3) {
                    copyLength28 += compareOffset4;
                    compareOffset4 = 0;
                    continue;
                } else {
                    if (copyLength28 != 0) {
                        WriteDiffHeaderInfo(1, copyLength28,
                                            diffData6, &diffSize29);
                        memcpy(diffData6 + diffSize29,
                               joinData29 + position1, copyLength28);
                        diffSize29 += copyLength28;
                        position1 += copyLength28;
                        copyLength28 = 0;
                    }
                    WriteDiffHeaderInfo(0, compareOffset4,
                                        diffData6, &diffSize29);
                    position1 += compareOffset4;
                    compareOffset4 = 0;
                }
            } else {
                while (position1 + copyLength28 < oldSize37 &&
                       position1 + copyLength28 < joinSize36 &&
                       oldData13[position1 + copyLength28] !=
                           joinData29[position1 + copyLength28])
                    copyLength28++;
            }
        }
    }

    sprintf(gText, "%s%s", ".\\DATA\\", diffName);
    joinFile1 = _open(gText, 0x8301, 0x80);
    if (joinFile1 == -1)
        FileError(gText);
    _write(joinFile1, diffData6, diffSize29);
    _close(joinFile1);

    sprintf(gText, "%s%s", ".\\DATA\\", oldName);
    joinFile1 = _open(gText, 0x8301, 0x80);
    if (joinFile1 == -1)
        FileError(gText);
    _write(joinFile1, joinData29, joinSize36);
    _close(joinFile1);

    if (oldData13 != 0)
        BaseFree(oldData13, GFILE, GDIFFLINE + 0xa1);
    if (joinData29 != 0)
        BaseFree(joinData29, GFILE, GDIFFLINE + 0xa3);
    if (diffData6 != 0)
        BaseFree(diffData6, GFILE, GDIFFLINE + 0xa5);
    return;
}

// @early-stop
// Logic and all 0x34-frame slots match. At +0x1e8..+0x1f4 only, /Od emits the
// equivalent TU-cumulative loop test with the two operand loads reversed.
VA(0x00485107, 0x3ce)
void CreateJoinFile(char *oldName, char *diffName, char *joinName)
{
    unsigned char *oldData13 = 0;
    unsigned char *diffData5 = 0;
    unsigned char *joinData9 = 0;
    int joinSize37 = 0;
    int diffSize1;
    int copyLength9;
    int diffFile2;
    int oldSize10;
    unsigned char copyFlag16;
    int position1;
    int joinFile0;

    sprintf(gText, "%s%s", ".\\DATA\\", diffName);
    diffSize1 = FileSize(gText);
    diffData5 = static_cast<unsigned char *>(
        BaseAlloc(diffSize1, GFILE, GDIFFLINE + 0xd));
    sprintf(gText, "%s%s", ".\\DATA\\", diffName);
    diffFile2 = _open(gText, 0x8000);
    if (diffFile2 == -1)
        FileError(gText);
    _read(diffFile2, diffData5, diffSize1);
    _close(diffFile2);

    joinData9 = static_cast<unsigned char *>(
        BaseAlloc(GAME_JOIN_BUFFER_SIZE, GFILE, GDIFFLINE + 0x16));
    if (diffData5[0] == 0) {
        memcpy(joinData9, diffData5 + GAME_JOIN_HEADER_SIZE,
               diffSize1 - GAME_JOIN_HEADER_SIZE);
        joinSize37 = diffSize1 - GAME_JOIN_HEADER_SIZE;
    } else {
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        oldSize10 = FileSize(gText);
        oldData13 = static_cast<unsigned char *>(
            BaseAlloc(oldSize10, GFILE, GDIFFLINE + 0x21));
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        diffFile2 = _open(gText, 0x8000);
        if (diffFile2 == -1)
            FileError(gText);
        _read(diffFile2, oldData13, oldSize10);
        _close(diffFile2);
        memcpy(joinData9, oldData13, oldSize10);

        position1 = GAME_JOIN_HEADER_SIZE;
        while (diffSize1 > position1) {
            copyFlag16 = diffData5[position1] >> 7;
            copyLength9 = GetSkipCopyLen(diffData5, &position1);
            if (copyFlag16) {
                memcpy(joinData9 + joinSize37,
                       diffData5 + position1, copyLength9);
                joinSize37 += copyLength9;
                position1 += copyLength9;
            } else {
                joinSize37 += copyLength9;
            }
        }
    }

    sprintf(gText, "%s%s", ".\\DATA\\", joinName);
    joinFile0 = _open(gText, 0x8301, 0x80);
    if (joinFile0 == -1)
        FileError(gText);
    _write(joinFile0, joinData9, joinSize37);
    _close(joinFile0);
    LogInt(const_cast<char *>("New Join CRC"),
           calc_crc_long(joinData9, joinSize37), joinSize37,
           GAME_JOIN_LOG_UNUSED, GAME_JOIN_LOG_UNUSED, GAME_JOIN_LOG_UNUSED,
           GAME_JOIN_LOG_UNUSED, GAME_JOIN_LOG_UNUSED);

    sprintf(gText, "%s%s", ".\\DATA\\", oldName);
    joinFile0 = _open(gText, 0x8301, 0x80);
    if (joinFile0 == -1)
        FileError(gText);
    _write(joinFile0, joinData9, joinSize37);
    _close(joinFile0);

    if (oldData13)
        BaseFree(oldData13, GFILE, GDIFFLINE + 0x53);
    if (diffData5)
        BaseFree(diffData5, GFILE, GDIFFLINE + 0x55);
    if (joinData9)
        BaseFree(joinData9, GFILE, GDIFFLINE + 0x57);
}

VA(0x004854d5, 0x5d)
int game::HeroIDToHeroPos(playerData *pd, int heroId)
{
    int i;
    for (i = 0; i < ((signed char *)pd)[1]; i++) {
        if (((signed char *)pd)[i + 4] == heroId)
            return i;
    }
    return -1;
}

VA(0x00485532, 0x5d)
int game::TownIDToTownPos(playerData *pd, int townId)
{
    int i;
    for (i = 0; i < ((signed char *)pd)[0x44]; i++) {
        if (((signed char *)pd)[i + 0x47] == townId)
            return i;
    }
    return -1;
}

VA(0x0048558f, 0x79f)
void game::SetupNewRumour(void)
{
    char rumourBuffer6[100];
    long categoryStats7[6];
    signed char categoryOrder2[8];
    rumourEventExtra *event4;
    int eventIndex11;
    int attempts13;
    int category10;
    int roll2;
    int direction9;
    if (m_rumourEventCount != 0 &&
        Random(0, 9) < static_cast<int>(m_rumourEventCount)) {
        attempts13 = 0;
        while (attempts13++ < 200) {
            if (m_rumourEventCount > 1)
                eventIndex11 = Random(0, m_rumourEventCount - 1);
            else
                eventIndex11 = 0;
            event4 = reinterpret_cast<rumourEventExtra *>(
                ppMapExtra[m_rumourEventIndices[eventIndex11]]);
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
                        sprintf(m_rumour, "%s has found the most obelisks.",
                                cPlayerNames[categoryOrder2[0]]);
                    else if (category10 == 7)
                        sprintf(m_rumour, "%s has found the most artifacts.",
                                cPlayerNames[categoryOrder2[0]]);
                    else if (category10 == 8)
                        sprintf(m_rumour, "%s has the most powerful forces.",
                                cPlayerNames[categoryOrder2[0]]);
                    else
                        sprintf(m_rumour, "%s earns the most gold.",
                                cPlayerNames[categoryOrder2[0]]);
                    return;
                }
            }
            goto ultimateRumour;
        } else {
ultimateRumour:
            category10 = Random(0, 100);
            if (category10 < 33) {
                // Retail uses the X coordinate for both threshold axes.
                if (!(m_mapHeader.width * 0.33 <= m_ultimateArtifactX ||
                      m_mapHeader.height * 0.33 <= m_ultimateArtifactX)) {
                    direction9 = 7;
                } else if (!(m_mapHeader.width * 0.33 <= m_ultimateArtifactX ||
                             m_ultimateArtifactX <= m_mapHeader.height * 0.66)) {
                    direction9 = 5;
                } else if (!(m_mapHeader.width * 0.33 <= m_ultimateArtifactX)) {
                    direction9 = 6;
                } else if (!(m_ultimateArtifactX <= m_mapHeader.width * 0.66 ||
                             m_mapHeader.height * 0.33 <= m_ultimateArtifactX)) {
                    direction9 = 1;
                } else if (!(m_ultimateArtifactX <= m_mapHeader.width * 0.66 ||
                             m_ultimateArtifactX <= m_mapHeader.height * 0.66)) {
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
                sprintf(m_rumour, "The ultimate artifact may be found %s.",
                        cDirections[direction9]);
            } else if (category10 < 66) {
                sprintf(m_rumour,
                        "The ultimate artifact may be found in the %s regions of the world.",
                        cRumourTerrainDescriptions[giGroundToTerrain[
                            gpAdvManager->GetCell(m_ultimateArtifactX,
                                                  m_ultimateArtifactY)->tile]]);
            } else {
                sprintf(m_rumour, "The ultimate artifact is really the %s.",
                        gArtifactNames[m_ultimateArtifactId]);
            }
        }
    }
}

VA(0x00485d2e, 0xd9)
EventExtra *GetMapEvent(int x, int y)
{
    int i;
    for (i = 0; i < *(unsigned short *)((char *)gpGame + 0x657b); i++) {
        EventExtra *ev = reinterpret_cast<EventExtra *>(ppMapExtra[*(unsigned short *)((char *)gpGame + 0x657d + i * 2)]);
        if (*(unsigned short *)((char *)ev + 0x26) == x &&
            *(unsigned short *)((char *)ev + 0x28) == y &&
            ((signed char *)ev)[0x25] != 0 &&
            ((unsigned char *)ev)[0x2b + PlayerEventByte(giCurPlayer)] != 0)
            return ev;
    }
    return 0;
}

// @early-stop
// Frame/CFG/logic match. Only +0xc..+0x37 (calendar term register order) and
// +0x19d..+0x286 (four equivalent player-resource address orders) are TU-cumulative.
VA(0x00485e07, 0x34c)
void game::CheckForTimeEvent(void)
{
    int dayNumber6;
    int eventIndex11;
    int resourceIndex27;
    timeEventExtra *event15;
    int primaryType14;
    int secondaryType18;
    int primaryAmount12;
    int secondaryAmount9;
    int resourceAmount7;

    dayNumber6 = (m_week - 1) * TIME_EVENT_DAYS_PER_WEEK +
                 (m_month - 1) * TIME_EVENT_DAYS_PER_MONTH + m_day;
    for (eventIndex11 = 0; eventIndex11 < m_timeEventCount; eventIndex11++) {
        event15 = static_cast<timeEventExtra *>(
            ppMapExtra[m_timeEventIndices[eventIndex11]]);
        if (((gbHumanPlayer[giCurPlayer] && event15->appliesToHuman) ||
             (!gbHumanPlayer[giCurPlayer] && event15->appliesToComputer)) &&
            event15->players[
                GetPlayerColor(static_cast<signed char>(giCurPlayer))] &&
            (event15->firstDay == dayNumber6 ||
             (event15->repeatInterval != 0 &&
              event15->firstDay < dayNumber6 &&
              (dayNumber6 - event15->firstDay) %
                      event15->repeatInterval == 0))) {
            primaryType14 = -1;
            primaryAmount12 = 0;
            secondaryType18 = -1;
            secondaryAmount9 = 0;
            for (resourceIndex27 = 0;
                 resourceIndex27 < TIME_EVENT_RESOURCE_COUNT;
                 resourceIndex27++) {
                resourceAmount7 = event15->resources[resourceIndex27];
                if (gpGame->m_players[giCurPlayer].resources[resourceIndex27] <
                    -resourceAmount7) {
                    resourceAmount7 =
                        -gpGame->m_players[giCurPlayer].resources[resourceIndex27];
                }
                gpGame->m_players[giCurPlayer].resources[resourceIndex27] +=
                    event15->resources[resourceIndex27];
                if (gpGame->m_players[giCurPlayer].resources[resourceIndex27] < 0)
                    gpGame->m_players[giCurPlayer].resources[resourceIndex27] = 0;
                if (resourceAmount7 != 0) {
                    if (primaryType14 != -1) {
                        secondaryType18 = primaryType14;
                        secondaryAmount9 = primaryAmount12;
                    }
                    primaryType14 = resourceIndex27;
                    primaryAmount12 = resourceAmount7;
                }
            }
            if (primaryType14 >= 0 &&
                primaryType14 <= TIME_EVENT_RESOURCE_COUNT - 1 &&
                primaryAmount12 < 0) {
                primaryAmount12 -= TIME_EVENT_RESOURCE_PENALTY;
            }
            if (secondaryType18 >= 0 &&
                secondaryType18 <= TIME_EVENT_RESOURCE_COUNT - 1 &&
                secondaryAmount9 < 0) {
                secondaryAmount9 -= TIME_EVENT_RESOURCE_PENALTY;
            }
            if (gbThisNetHumanPlayer[giCurPlayer]) {
                NormalDialog(event15->message, 1, -1, -1,
                             primaryType14, primaryAmount12,
                             secondaryType18, secondaryAmount9, -1, 0);
            }
        }
    }
}

// @early-stop
// Frame/loops match; +0xc5..+0xe8 is only the equivalent packed expression
// heroIndex + player * 283. Direct, commuted, accessor, and AST variants did not steer it.
VA(0x00486153, 0x143)
void CheckValidAvailableHeroes(void)
{
    int candidatePlayer0;
    int heroIndex5;
    int availableSlot13;
    int heroPlayer26;

    for (heroPlayer26 = 0; heroPlayer26 < gpGame->m_playerCount; heroPlayer26++) {
        for (heroIndex5 = 0;
             heroIndex5 < gpGame->m_players[heroPlayer26].heroCount;
             heroIndex5++) {
            for (candidatePlayer0 = 0;
                 candidatePlayer0 < gpGame->m_playerCount;
                 candidatePlayer0++) {
                for (availableSlot13 = 0;
                     availableSlot13 < AVAILABLE_HERO_SLOTS;
                     availableSlot13++) {
                    if (gpGame->m_players[candidatePlayer0]
                            .availableHeroes[availableSlot13] ==
                        gpGame->m_players[heroPlayer26].heroes[heroIndex5]) {
                        gpGame->m_players[candidatePlayer0]
                            .availableHeroes[availableSlot13] =
                            static_cast<signed char>(
                                gpGame->GetNewHeroId(heroPlayer26, -1, 0));
                    }
                }
            }
        }
    }
}

VA(0x00486296, 0xab)
int CalcFileCRC(char *filename)
{
    long size = FileSize(filename);
    char *block = (char *)BaseAlloc(size, GFILE, *(short *)"\x5e\x1f" + 3);
    int hand = _open(filename, 0x8000);
    if (hand == -1)
        FileError(filename);
    _read(hand, block, size);
    int crc = calc_crc_long((unsigned char *)block, size);
    _close(hand);
    BaseFree(block, GFILE, *(short *)"\x5e\x1f" + 0xe);
    return crc;
}

// @early-stop
// All bytes except +0x8d..+0x99 match; /Od reverses the equivalent index/size
// loop-test load order and branch polarity. Relational and AST variants did not steer it.
VA(0x00486341, 0x153)
void CompressTest2(void)
{
    int dataSize2;
    long encodedSize14;
    long decodedSize17;
    char *sourceData6;
    int sourceCrc0;
    char *decodedData6;
    int index7;
    int decodedCrc5;
    int sourceCrcCheck7;
    char *encodedData6;

    dataSize2 = Random(COMPRESSION_TEST_RANDOM_SIZE_MIN,
                       COMPRESSION_TEST_RANDOM_SIZE_MAX);
    sourceData6 = static_cast<char *>(BaseAlloc(
        dataSize2 + COMPRESSION_TEST_RANDOM_BUFFER_EXTRA,
        GFILE, GCOMPRESSTEST2LINE + 7));
    encodedData6 = static_cast<char *>(BaseAlloc(
        dataSize2 + COMPRESSION_TEST_RANDOM_BUFFER_EXTRA,
        GFILE, GCOMPRESSTEST2LINE + 8));
    decodedData6 = static_cast<char *>(BaseAlloc(
        dataSize2 + COMPRESSION_TEST_RANDOM_BUFFER_EXTRA,
        GFILE, GCOMPRESSTEST2LINE + 9));
    for (index7 = 0; index7 < dataSize2; index7++)
        sourceData6[index7] = static_cast<char>(Random(0, 255));
    sourceCrc0 = calc_crc_long(
        reinterpret_cast<unsigned char *>(sourceData6), dataSize2);
    encodedSize14 = EncodeData(encodedData6, sourceData6, dataSize2);
    decodedSize17 = DecodeData(decodedData6, encodedData6, encodedSize14);
    decodedCrc5 = calc_crc_long(
        reinterpret_cast<unsigned char *>(decodedData6), dataSize2);
    sourceCrcCheck7 = calc_crc_long(
        reinterpret_cast<unsigned char *>(sourceData6), dataSize2);
    BaseFree(sourceData6, GFILE, GCOMPRESSTEST2LINE + 0x1a);
    BaseFree(encodedData6, GFILE, GCOMPRESSTEST2LINE + 0x1b);
    BaseFree(decodedData6, GFILE, GCOMPRESSTEST2LINE + 0x1c);
}

VA(0x00486494, 0x1be)
void CompressTest(void)
{
    long fileSize7;
    long encodedSize14;
    long decodedSize17;
    char *sourceData6;
    int sourceCrc0;
    char *decodedData6;
    int fileHandle4;
    int decodedCrc5;
    int sourceCrcCheck7;
    char *encodedData6;
    char filename3[COMPRESSION_TEST_FILENAME_SIZE];

    LogStr(const_cast<char *>("C1"));
    strcpy(filename3, "c:\\TEMP\\Z.DIF");
    fileSize7 = FileSize(filename3);
    sourceData6 = static_cast<char *>(BaseAlloc(
        fileSize7 + COMPRESSION_TEST_FILE_BUFFER_EXTRA,
        GFILE, GCOMPRESSTESTLINE + 9));
    encodedData6 = static_cast<char *>(BaseAlloc(
        fileSize7 + COMPRESSION_TEST_FILE_BUFFER_EXTRA,
        GFILE, GCOMPRESSTESTLINE + 10));
    decodedData6 = static_cast<char *>(BaseAlloc(
        fileSize7 + COMPRESSION_TEST_FILE_BUFFER_EXTRA,
        GFILE, GCOMPRESSTESTLINE + 0xb));
    LogStr(const_cast<char *>("C2"));
    fileHandle4 = _open(filename3, 0x8000);
    if (fileHandle4 == -1)
        FileError(filename3);
    _read(fileHandle4, sourceData6, fileSize7);
    LogStr(const_cast<char *>("C3"));
    sourceCrc0 = calc_crc_long(
        reinterpret_cast<unsigned char *>(sourceData6), fileSize7);
    LogStr(const_cast<char *>("C4"));
    _close(fileHandle4);
    LogStr(const_cast<char *>("C5"));
    encodedSize14 = EncodeData(encodedData6, sourceData6, fileSize7);
    LogStr(const_cast<char *>("C6"));
    decodedSize17 = DecodeData(decodedData6, encodedData6, encodedSize14);
    LogStr(const_cast<char *>("C7"));
    decodedCrc5 = calc_crc_long(
        reinterpret_cast<unsigned char *>(decodedData6), fileSize7);
    sourceCrcCheck7 = calc_crc_long(
        reinterpret_cast<unsigned char *>(sourceData6), fileSize7);
    BaseFree(sourceData6, GFILE, GCOMPRESSTESTLINE + 0x24);
    BaseFree(encodedData6, GFILE, GCOMPRESSTESTLINE + 0x25);
    BaseFree(decodedData6, GFILE, GCOMPRESSTESTLINE + 0x26);
    LogStr(const_cast<char *>("C8"));
}

VA(0x00486652, 0x53)
void CompressTest3(void)
{
    char buf[40];
    int i;
    for (i = 0; i < 0x64; i++) {
        sprintf(buf, "Test # %d", i);
        AiPrint(buf);
        CompressTest2();
    }
}

// @early-stop
// Logic + frame slots byte-exact (count/cell function-scope, col/row/castle in the {}
// block -> retail's -0x4..-0x14); residual is the same TU-cumulative /Od eval-order
// parity - both loop tests (row<MAP_HEIGHT, col<MAP_WIDTH) load the global into eax
// first in retail vs the loop var in my build. Aligns when GAME is fuller.
VA(0x004866a5, 0x119)
int game::CountShrines(int player)
{
    if (xIsExpansionMap == 0)
        return 0;
    int count = 0;
    mapCell *cell;
    {
        int col;
        int row;
        town *castle;
        for (row = 0; row < MAP_HEIGHT; row++) {
            for (col = 0; col < MAP_WIDTH; col++) {
                cell = WORLDMAP->Row(row) + col;
                if (((unsigned char *)cell)[9] == 0xa3) {
                    castle = GetCastle(cell->w4hi);
                    if (((signed char *)castle)[1] == player &&
                        (*(int *)((char *)castle + 0x18) & 4) &&
                        ((signed char *)castle)[3] == 5)
                        count++;
                }
            }
        }
    }
    return count;
}

// ---- globals (definitions, RVA order) ----
DATA(0x004f70e0) int gbGameOver;
DATA(0x004f7550) signed char giMonType[] = {
    0, 0x11, 0x15, 0x2a, 0x0f, 0x19, 0x34, 0x0e, 0x1d, 0x1e, 0x1b, 0x36
};
DATA(0x004f7a08) char bMapInitialized;
DATA(0x005280e8) int iViewArmyNumTroops;
DATA(0x005280ec) signed char *gbNGHeroType;
DATA(0x005280f8) SMonFrameInfo sViewArmyMonFrameInfo;
DATA(0x00528430) short giUABaseX;
DATA(0x00528434) short giUABaseY;
DATA(0x00528438) int giEndSequence;
DATA(0x0052843c) int gbDismissArmy;
DATA(0x00528440) signed char *gbNGHuman;
DATA(0x00528448) int iViewArmyFrame;
DATA(0x0052844c) int gbAllowUpgrade;
DATA(0x00528450) int iViewArmyType;
DATA(0x00528454) class hero *viewSpellsHero;
DATA(0x00528458) int gbUpgradeArmy;
DATA(0x00528460) short RandMineQty[8];
DATA(0x00528470) char gcCurMapName[16];
DATA(0x00528480) signed char *gbNGDifficulty;
DATA(0x00528488) int iViewArmyUpgradeToType;
DATA(0x0052848c) int viewArmyBaseX;
DATA(0x00528490) int viewArmyBaseY;
DATA(0x00528498) signed char *gbNGColor;
DATA(0x005284a0) short giUARadius;
DATA(0x005284a8) signed char *gbNGPlayerPos;
DATA(0x005284b0) int viewArmyFacingWIPXMod;
