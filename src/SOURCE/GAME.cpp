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
#include <SOURCE/KB.h>
#include <SOURCE/REMOTE.h>
#include <io.h>
#include <string.h>
#include <stdio.h>
#include <SOURCE/game.h>
#include <SOURCE/playerData.h>
#include <SOURCE/town.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/advManager.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/EVENTS.h>
#include <EDITOR/mapcell.h>
#include <EDITOR/fullMap.h>
#include <BASE/soundManager.h>
#include <BASE/resourceManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/BITS.h>


// configStruct/SCreatureInfo shared in _types.h; EventExtra/SThievesData in SOURCE/GAME.h.

// GAME's BaseAlloc/BaseFree pass __FILE__ + a source line number. The retail
// encodes the base line as a 2-byte string read via movswl, then adds a per-call
// delta; reproduce byte-exactly. __FILE__ is the original build path (reloc-masked).
#define GFILE const_cast<char *>("I:\\Projects\\Heroes\\Prog\\SOURCE\\GAME.CPP")
#define GSAVELINE (*reinterpret_cast<const short *>("\x94\x02"))
#define GLOADLINE (*reinterpret_cast<const short *>("\x4f\x04"))
#define GMAPLINE (*reinterpret_cast<const short *>("\n"))

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
    _write(file, m_unknown0c, 2);
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
    _write(file, &m_unknown43, 1);
    _write(file, &m_townCount, 1);
    _write(file, &m_currentTown, 1);
    _write(file, &m_townLocatorPage, 1);
    _write(file, m_townIds, 72);
    _write(file, m_resources, 28);
    _write(file, m_secondaryResources, 28);
    _write(file, &m_unknownac, 1);
    _write(file, &m_unknownac, 1);
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
    _read(file, m_unknown0c, 2);
    _read(file, unused, 42);
    _read(file, &gpGame->m_cheated, 1);
    _read(file, &m_cheatValue, 1);
    _read(file, &m_unknown0f, 4);
    _read(file, &m_unknown0e, 1);
    _read(file, &m_unknownab, 1);
    _read(file, &m_canDig, 1);
    _read(file, &m_unknown41, 1);
    _read(file, &m_unknown42, 1);
    _read(file, &m_unknown43, 1);
    _read(file, &m_townCount, 1);
    _read(file, &m_currentTown, 1);
    _read(file, &m_townLocatorPage, 1);
    _read(file, m_townIds, 72);
    _read(file, m_resources, 28);
    _read(file, m_secondaryResources, 28);
    _read(file, &m_unknownac, 1);
    _read(file, &m_unknownac, 1);
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
    int boatIdx = Scan(reinterpret_cast<signed char *>(this) + 0x631d, 0, 0x30);
    if (boatIdx != -1) {
        if (notify == 0)
            SendMapChange(4, 0, x, y, -999, 0, 0);
        reinterpret_cast<char *>(this)[boatIdx + 0x631d] = static_cast<char>(boatIdx);
        char *boat = reinterpret_cast<char *>(this) + boatIdx * 8 + 0x619d;
        boat[0] = static_cast<char>(boatIdx);
        boat[1] = static_cast<char>(x);
        boat[2] = static_cast<char>(y);
        boat[3] = 2;
        boat[7] = static_cast<char>(giCurPlayer);
        mapCell *cell = WORLDMAP->Row(y) + x;
        boat[4] = reinterpret_cast<char *>(cell)[9];
        boat[5] = static_cast<char>(cell->w4hi);
        reinterpret_cast<char *>(cell)[9] = static_cast<char>(0xab);
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
            m_heroRecs[heroId].m_unknown18 >= 54 &&
            m_heroRecs[heroId].m_unknown18 <= 59)
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
    _write(fileInfo, reinterpret_cast<unsigned char *>(this) + 0x2a9, 0x1a4);
    _write(fileInfo, reinterpret_cast<unsigned char *>(this) + 0x44d, 0x41);
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
    _write(fileInfo, &m_unknown48f, 1);
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
    _write(fileInfo, reinterpret_cast<unsigned char *>(this) + 0x631d, 0x30);
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

// @early-stop
// reloc-masked: identical 0xb44-byte code/frame; 75/75 targets agree, only compiler literal/constant symbol identities differ
VA(0x00472a7b, 0xb44)
void game::SetupOrigData(void)
{
    ClearMapExtra();
    gbIAmGreatest = 0;
    field_0x0 = 1;
    giMonthType = 0;
    giMonthTypeExtra = 0;
    giWeekType = 0;
    giWeekTypeExtra = 0;
    m_cheated = 0;
    gpAdvManager->PurgeMapChangeQueue();
    giMapChangeCtr = 1;
    strcpy(m_saveName, "NEWGAME");
    m_playerCount = 4;
    m_unknown48f = 0;
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
        m_players[i].unknown43 = -1;
        m_players[i].unknown13 = 0;
        memset(m_players[i].unknown0c, -1, 2);
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
        m_heroRecs[i].m_unknown2a = -1;
        m_heroRecs[i].m_unknown29 = m_heroRecs[i].m_unknown2a;
        m_heroRecs[i].m_id = static_cast<signed char>(i);
        m_heroRecs[i].m_unknown18 = static_cast<unsigned char>(i);
        m_heroRecs[i].m_owner = -1;
        m_heroRecs[i].m_direction = 2;
        strcpy(m_heroRecs[i].m_name, gHeroDefaultNames[i]);
        m_heroRecs[i].m_cursorType = static_cast<unsigned char>(i / 9);
        for (j = 0; j < 5; j++)
            reinterpret_cast<unsigned char *>(&m_heroRecs[i].m_attack)[j] =
                gStartingHeroStats[m_heroRecs[i].m_cursorType][j];
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
    memset(reinterpret_cast<unsigned char *>(this) + 0x631d, -1, GAME_BOAT_COUNT);
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
        _read(file, reinterpret_cast<unsigned char *>(this) + 0x2a9, 0x1a4);
        _read(file, reinterpret_cast<unsigned char *>(this) + 0x44d, 0x41);
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
        _read(file, &m_unknown48f, 1);
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
        _read(file, reinterpret_cast<unsigned char *>(this) + 0x631d, 0x30);
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

    if ((m_castleRecs[townId].x > 0 || m_castleRecs[townId].y > 0) &&
        m_castleRecs[townId].owner < 0) {
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
        switch (m_castleRecs[townId].race + jb) {
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
        GiveArmy(reinterpret_cast<armyGroup *>(m_castleRecs[townId].army), idx, cnt, -1);
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
        if (player2 >= static_cast<unsigned char>(gpGame->field_0x2c3)) {
            gbSetupGamePosToRealGamePos[player2] = -1;
        } else {
            if (reinterpret_cast<signed char *>(this)[player2 + 0x45f] == 10)
                gbSetupGamePosToRealGamePos[player2] = static_cast<signed char>(nextHuman6++);
            else
                gbSetupGamePosToRealGamePos[player2] =
                    reinterpret_cast<signed char *>(this)[player2 + 0x45f];
        }
    }
    for (player2 = 0; player2 < GAME_PLAYER_COUNT; player2++) {
        m_players[player2].color = -1;
        gcColorToPlayerPos[player2] = -1;
        gcColorToSetupPos[player2] = -1;
        if (reinterpret_cast<signed char *>(gpGame)[player2 + 0x459] == 7)
            reinterpret_cast<signed char *>(gpGame)[player2 + 0x459] =
                static_cast<signed char>(randomColor2);
        randomColor2 = (randomColor2 + 1) % GAME_PLAYER_COUNT;
    }
    for (player2 = 0; player2 < m_playerCount; player2++)
        gcColorToSetupPos[reinterpret_cast<signed char *>(this)[player2 + 0x44d]] =
            static_cast<signed char>(player2);
    for (player2 = 0; player2 < m_playerCount; player2++)
        m_players[gbSetupGamePosToRealGamePos[player2]].color =
            reinterpret_cast<signed char *>(this)[player2 + 0x44d];
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
    m_unknown48f = 0;
    for (player2 = m_playerCount; player2 < GAME_PLAYER_COUNT; player2++)
        m_playerDead[player2] = 1;

    if (reinterpret_cast<unsigned char *>(this)[0x2c6] == 4 ||
        reinterpret_cast<unsigned char *>(this)[0x2c6] == 2) {
        reinterpret_cast<unsigned char *>(this)[0x2c7] = 1;
        reinterpret_cast<unsigned char *>(this)[0x2c8] = 0;
    }
    if (reinterpret_cast<unsigned char *>(this)[0x2c6] == 4) {
        townIndex9 = 0;
        for (player2 = 0; player2 < GAME_PLAYER_COUNT; player2++) {
            if (reinterpret_cast<unsigned char *>(this)[player2 + 0x2b1] != 0)
                townIndex9++;
            if (*reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x2c9) + 1 == townIndex9) {
                *reinterpret_cast<short *>(reinterpret_cast<unsigned char *>(this) + 0x2d9) =
                    static_cast<short>(player2);
                player2 = 99;
            }
        }
    }
    if (reinterpret_cast<unsigned char *>(this)[0x2c6] == 3)
        reinterpret_cast<unsigned char *>(this)[0x2c7] = 1;

    for (player2 = 0; player2 < m_playerCount; player2++) {
        m_players[player2].unknown40 = 0;
        m_players[player2].unknown41 = -1;
        m_players[player2].unknown42 = -1;
        heroIndex1 = 0;
        selectedTown14 = -1;
        if (reinterpret_cast<unsigned char *>(this)[0x2ce] == 0 &&
            m_players[player2].townCount > 0) {
            for (pass27 = 0; pass27 < 2; pass27++) {
                for (townIndex9 = 0; townIndex9 < m_players[player2].townCount; townIndex9++) {
                    if (selectedTown14 == -1 &&
                        m_castleRecs[(m_players + player2)->towns[townIndex9]].occupyingHeroId == -1 &&
                        ((m_castleRecs[(m_players + player2)->towns[townIndex9]].buildings & 0x40) != 0 ||
                         pass27 == 1))
                        selectedTown14 = townIndex9;
                }
            }
        }
        if (selectedTown14 != -1) {
            m_players[player2].heroes[m_players[player2].heroCount] =
                static_cast<signed char>(GetNewHeroId(
                    player2, m_castleRecs[m_players[player2].towns[selectedTown14]].race, 0));
            m_availableHeroes[m_players[player2].heroes[m_players[player2].heroCount]] =
                static_cast<signed char>(player2);
            m_heroRecs[m_players[player2].heroes[m_players[player2].heroCount]].m_owner =
                static_cast<signed char>(player2);
            m_heroRecs[m_players[player2].heroes[m_players[player2].heroCount]].m_x =
                m_castleRecs[m_players[player2].towns[selectedTown14]].x;
            m_heroRecs[m_players[player2].heroes[m_players[player2].heroCount]].m_y =
                m_castleRecs[m_players[player2].towns[selectedTown14]].y;
            m_castleRecs[m_players[player2].towns[selectedTown14]].occupyingHeroId =
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
                    m_heroRecs[campaignHero15].m_unknown18 = 56;
                } else {
                    m_heroRecs[campaignHero15].m_experience += 5000;
                    m_heroRecs[campaignHero15].CheckLevel();
                    strcpy(m_heroRecs[campaignHero15].m_name, "Brother Brax");
                    m_heroRecs[campaignHero15].m_unknown18 = 59;
                }
                m_players[player2].unknown0c[0] = static_cast<char>(campaignHero15);
                m_availableHeroes[m_players[player2].unknown0c[0]] = 64;
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
                        m_heroRecs[campaignHero15].m_unknown18 = specialPortrait6;
                        m_players[player2].unknown0c[0] = static_cast<char>(campaignHero15);
                        m_availableHeroes[m_players[player2].unknown0c[0]] = 64;
                        heroClass5 = m_heroRecs[campaignHero15].m_cursorType;
                        goto secondHero;
                    }
                }
            }
            heroClass5 = Random(0, 5);
            if (reinterpret_cast<signed char *>(this)[
                    gcColorToSetupPos[m_players[player2].color] + 0x459] < 6)
                heroClass5 = reinterpret_cast<signed char *>(this)[
                    gcColorToSetupPos[m_players[player2].color] + 0x459];
            m_players[player2].unknown0c[0] =
                static_cast<char>(GetNewHeroId(player2, heroClass5, 0));
            m_availableHeroes[m_players[player2].unknown0c[0]] = 64;
        }
secondHero:
        heroClass5 = (Random(1, 5) + heroClass5) % 6;
        m_players[player2].unknown0c[1] =
            static_cast<char>(GetNewHeroId(player2, heroClass5, 0));
        m_availableHeroes[m_players[player2].unknown0c[1]] = 64;
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
                               gInitResourcesHuman[reinterpret_cast<signed char *>(this)[0x465]],
                               28);
                        if (reinterpret_cast<signed char *>(this)[player2 + 0x453] != 0) {
                            for (townIndex9 = 0; townIndex9 < 7; townIndex9++) {
                                double resourceScale;
                                if (reinterpret_cast<signed char *>(this)[player2 + 0x453] == 1)
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
                               gInitResourcesComputer[reinterpret_cast<signed char *>(this)[0x465]],
                               28);
                    }
                }
                SetupAdjacentMons();
                if (reinterpret_cast<unsigned char *>(this)[0x2cb] == 2) {
                    ultimateDistance5 = *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x2cc);
                    ultimateTries4 = *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x2d7);
                    *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x2cc) = 0;
                    if (m_worldMap.GetCell(ultimateDistance5, ultimateTries4)->triggerType == 0xaa)
                        *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x2cc) =
                            m_worldMap.GetCell(ultimateDistance5, ultimateTries4)->w4hi;
                    else {
                        if (m_worldMap.GetCell(ultimateDistance5, ultimateTries4 - 1)->triggerType == 0xaa)
                            *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x2cc) =
                                m_worldMap.GetCell(ultimateDistance5, ultimateTries4 - 1)->w4hi;
                        else
                            reinterpret_cast<unsigned char *>(this)[0x2cb] = 0;
                    }
                }
                if (reinterpret_cast<unsigned char *>(this)[0x2c6] == 2) {
                    ultimateDistance5 = *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x2c9);
                    ultimateTries4 = *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x2d5);
                    *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x2c9) = 0;
                    if (m_worldMap.GetCell(ultimateDistance5, ultimateTries4)->triggerType == 0xaa)
                        *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x2c9) =
                            m_worldMap.GetCell(ultimateDistance5, ultimateTries4)->w4hi;
                    else {
                        if (m_worldMap.GetCell(ultimateDistance5, ultimateTries4 - 1)->triggerType == 0xaa)
                            *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x2c9) =
                                m_worldMap.GetCell(ultimateDistance5, ultimateTries4 - 1)->w4hi;
                        else
                            reinterpret_cast<unsigned char *>(this)[0x2c6] = 0;
                    }
                }
                for (player2 = 0; player2 < m_playerCount; player2++) {
                    heroClass5 = 0;
                    if (reinterpret_cast<signed char *>(this)[
                            gcColorToSetupPos[m_players[player2].color] + 0x459] >= 0 &&
                        reinterpret_cast<signed char *>(this)[
                            gcColorToSetupPos[m_players[player2].color] + 0x459] < 6) {
                        heroClass5 = reinterpret_cast<signed char *>(this)[
                            gcColorToSetupPos[m_players[player2].color] + 0x459];
                    } else {
                        if (!!m_players[player2].townCount) {
                            heroClass5 = gpGame->m_castleRecs[m_players[player2].Town(0)].race;
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
    return &instance->m_castleRecs[index];
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
    _read(file, reinterpret_cast<unsigned char *>(this) + 0x2a9, 0x1a4);
    m_worldMap.Read(file, 1);
    SetMapSize(m_worldMap.width, m_worldMap.height);

    for (i = 0; i < GAME_TOWN_COUNT; i++) {
        _read(file, column, 1);
        _read(file, row, 1);
        _read(file, type, 1);
        if (static_cast<unsigned char>(column[0]) != 0xff) {
            m_castleRecs[i].unknown38 = 1;
            m_castleRecs[i].x = static_cast<unsigned char>(column[0]);
            m_castleRecs[i].y = static_cast<unsigned char>(row[0]);
            m_castleRecs[i].race = static_cast<signed char>(type[0] & 0x7f);
            if (type[0] < 0)
                m_castleRecs[i].buildings |= 0x40;
            else
                m_castleRecs[i].buildings |= 0x20;
        }
    }

    for (i = 0; i < GAME_MINE_COUNT; i++) {
        if (field_0x2a9 == 90 && i >= GAME_TOWN_COUNT) {
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

    field_0x2a9 = 92;
    _read(file, &m_obeliskCount, 1);
    _read(file, reinterpret_cast<unsigned char *>(this) + 0x64d9,
          static_cast<unsigned char>(field_0x44b) * 2);
    *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x64d7) =
        static_cast<unsigned char>(field_0x44b);
    _read(file, reinterpret_cast<unsigned char *>(this) + 0x6517,
          static_cast<unsigned char>(field_0x44c) * 2);
    *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(this) + 0x6515) =
        static_cast<unsigned char>(field_0x44c);
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
    townRec = &m_castleRecs[townId];
    if (townRec->owner == player)
        return;
    townRec->unknown38 = 0;
    if (m_castleOwners[townId] != -1)
        GetCastle(townId)->Deallocate();
    for (i = 0; i < 5; i++) {
        townRec->army[i] = -1;
        *reinterpret_cast<short *>(townRec->army + 5 + i * 2) = 0;
    }
    m_castleRecs[townId].unknown55 = m_castleRecs[townId].owner == -1 ? 2 : 0;
    m_castleRecs[townId].owner = static_cast<signed char>(player);
    m_castleOwners[townId] = static_cast<signed char>(player);
    m_players[player].towns[m_players[player].townCount] = static_cast<signed char>(townId);
    m_players[player].townCount++;

    cell = m_worldMap.GetCell(m_castleRecs[townId].x - 1, m_castleRecs[townId].y);
    m_worldMap.ChangeTilesetIndex(cell, m_castleRecs[townId].x - 1,
                                 m_castleRecs[townId].y, 14,
                                 m_players[static_cast<signed char>(player)].color * 2, 1, -1);
    cell = m_worldMap.GetCell(m_castleRecs[townId].x + 1, m_castleRecs[townId].y);
    m_worldMap.ChangeTilesetIndex(cell, m_castleRecs[townId].x + 1,
                                 m_castleRecs[townId].y, 14,
                                 m_players[static_cast<signed char>(player)].color * 2 + 1, 1, -1);
    if (suppressVisibility != 0)
        return;
    SetVisibility(m_castleRecs[townId].x, m_castleRecs[townId].y,
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

VA(0x00479856, 0x1e2)
int game::ViewSpells(class hero *, int, int (*)(struct tag_message &), int) { return 0; }

VA(0x00479a38, 0x403)
void game::UpdateSpellWidgets(void) {}

VA(0x00479e3b, 0x692)
int ViewSpellsHandler(struct tag_message &) { return 0; }

VA(0x0047a4cd, 0x17c)
int ViewSpecialHandler(struct tag_message &) { return 0; }

VA(0x0047a649, 0xc86)
void game::ViewArmy(int, int, int, int, class town *, int, int, int, class hero *, class army *, class armyGroup *, int) {}

VA(0x0047b2cf, 0x3f5)
int ViewArmyHandler(struct tag_message &) { return 0; }

VA(0x0047b6c4, 0x671)
int game::GetRandomNumTroops(int) { return 0; }

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

VA(0x0047bd99, 0x596)
void game::NextPlayer(void) {}

VA(0x0047c32f, 0x432)
int game::ComputeDailyGold(int) { return 0; }

VA(0x0047c761, 0x9aa)
void game::PerDay(void) {}

VA(0x0047d10b, 0x199d)
void game::PerWeek(void) {}

VA(0x0047eaa8, 0x12d)
void game::WeeklyRecruitSite(class mapCell *) {}

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

VA(0x0047ec44, 0x375)
void game::PerMonth(void) {}

VA(0x0047efb9, 0x476)
void game::ConvertObject(int, int, int, int, int, int, int, int, int, int, int) {}

VA(0x0047f42f, 0x1c2)
void game::RandomizeTown(int, int, int) {}

VA(0x0047f5f1, 0x619)
void game::RandomizeMine(int, int) {}

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
int game::GetRandomArtifactId(int, int) { return 0; }

VA(0x0047fe4f, 0x68)
int IsCursedItem(int item)
{
    if (item == 0x10 || item == 0x46 || item == 0x45 || item == 0x57 ||
        item == 0x59 || item == 0x5c || item == 0x5d)
        return 1;
    return 0;
}

VA(0x0047feb7, 0x1ef)
void game::RandomizeHeroPool(void) {}

VA(0x004800a6, 0x378)
void game::SetRandomHeroArmies(int, int) {}

VA(0x0048041e, 0x746)
void game::ProcessRandomObjects(void) {}

VA(0x00480b64, 0x230)
void game::SetVisibility(int, int, int, int) {}

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
int game::GetLuck(class hero *, class army *, class town *) { return 0; }

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
void game::WaitForPlayer(char *, int) {}

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

VA(0x00481645, 0x120)
void game::ConvertFlagToLateOverlay(int, int) {}

VA(0x00481765, 0x13b)
int game::HasObjectTilesetIndex(int, int, int, int) { return 0; }

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

VA(0x004819b2, 0x295)
void game::ProcessMapExtra(void) {}

VA(0x00481c47, 0x900)
void game::SetupTowns(void) {}

VA(0x00482547, 0x774)
void game::ProcessOnMapHeroes(void) {}

VA(0x00482cbb, 0x55e)
void game::CheckHeroConsistency(void) {}

VA(0x00483219, 0x71e)
int game::TransmitSaveGame(int, int, int) { return 0; }

VA(0x00483937, 0x68d)
int game::ReceiveSaveGame(int, int, int, int) { return 0; }

VA(0x00483fc4, 0x455)
void game::DoNewTurn(void) {}

VA(0x00484419, 0x58)
int game::GetBoatsBuilt(void)
{
    int count = 0;
    int i;
    for (i = 0; i < 0x30; i++) {
        if (((signed char *)this)[i + 0x631d] != -1)
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
    if (((signed char *)this)[0x465] == 0)
        rating += 0x32;
    else if (((signed char *)this)[0x465] == 1)
        rating += 0x50;
    else if (((signed char *)this)[0x465] == 2)
        rating += 0x64;
    else if (((signed char *)this)[0x465] == 3)
        rating += 0x78;
    else if (((signed char *)this)[0x465] == 4)
        rating += 0x8c;
    if (((unsigned char *)this)[0x2ad] == 0)
        ;
    else if (((unsigned char *)this)[0x2ad] == 1)
        rating += 0x14;
    else if (((unsigned char *)this)[0x2ad] == 2)
        rating += 0x28;
    else if (((unsigned char *)this)[0x2ad] == 3)
        rating += 0x50;
    return rating;
}

VA(0x00484620, 0x1ea)
int CalcBaseScore(int) { return 0; }

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

VA(0x00484b4d, 0x5ba)
void CreateDiffFile(char *, char *, char *, int, int) {}

VA(0x00485107, 0x3ce)
void CreateJoinFile(char *, char *, char *) {}

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
void game::SetupNewRumour(void) {}

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

VA(0x00485e07, 0x34c)
void game::CheckForTimeEvent(void) {}

VA(0x00486153, 0x143)
void CheckValidAvailableHeroes(void) {}

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

VA(0x00486341, 0x153)
void CompressTest2(void) {}

VA(0x00486494, 0x1be)
void CompressTest(void) {}

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
DATA(0x004f7550) signed char *giMonType;
DATA(0x004f7a08) char bMapInitialized;
DATA(0x005280e8) int iViewArmyNumTroops;
DATA(0x005280ec) signed char *gbNGHeroType;
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
DATA(0x00528460) short *RandMineQty;
DATA(0x00528470) char *gcCurMapName;
DATA(0x00528480) signed char *gbNGDifficulty;
DATA(0x00528488) int iViewArmyUpgradeToType;
DATA(0x0052848c) int viewArmyBaseX;
DATA(0x00528490) int viewArmyBaseY;
DATA(0x00528498) signed char *gbNGColor;
DATA(0x005284a0) short giUARadius;
DATA(0x005284a8) signed char *gbNGPlayerPos;
DATA(0x005284b0) int viewArmyFacingWIPXMod;
