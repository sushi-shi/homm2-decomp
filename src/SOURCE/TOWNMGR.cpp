// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\TOWNMGR.OBJ   from: (directly linked into exe)
// functions: 33   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/message.h>
#include <BASE/Misc.h>
#include <BASE/border.h>
#include <BASE/executive.h>
#include <BASE/font.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/icon.h>
#include <BASE/iconWidget.h>
#include <BASE/resourceManager.h>
#include <BASE/soundManager.h>
#include <BASE/textWidget.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/Castle.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/GAME.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/RECRUIT.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/bankBox.h>
#include <SOURCE/game.h>
#include <SOURCE/HERO.h>
#include <SOURCE/hero.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/playerData.h>
#include <SOURCE/philAI.h>
#include <SOURCE/recruitUnit.h>
#include <SOURCE/strip.h>
#include <SOURCE/town.h>
#include <SOURCE/townObject.h>
#include <SOURCE/townManager.h>
#include <SOURCE/TOWNMGR.h>
#include <SOURCE/tradpost.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DATA(0x004eb080) static const i8
    gTownObjectOrder[FACTION_COUNT][TOWN_BUILDING_COUNT] = {
        {
            TOWN_OBJECT_SECOND_WELL,
            TOWN_OBJECT_CASTLE_UPGRADE,
            TOWN_OBJECT_CASTLE,
            TOWN_OBJECT_SPECIAL_BUILDING,
            TOWN_OBJECT_LEFT_TURRET,
            TOWN_OBJECT_RIGHT_TURRET,
            TOWN_OBJECT_CAPTAIN_QUARTERS,
            TOWN_OBJECT_MOAT,
            TOWN_OBJECT_MARKETPLACE,
            TOWN_OBJECT_THIEVES_GUILD,
            TOWN_OBJECT_DWELLING_2,
            TOWN_OBJECT_UPGRADED_DWELLING_2,
            TOWN_OBJECT_DWELLING_5,
            TOWN_OBJECT_UPGRADED_DWELLING_5,
            TOWN_OBJECT_TAVERN,
            TOWN_OBJECT_MAGE_GUILD,
            TOWN_OBJECT_DWELLING_6,
            TOWN_OBJECT_UPGRADED_DWELLING_6,
            TOWN_OBJECT_KNIGHT_LEFT_OVERLAY,
            TOWN_OBJECT_KNIGHT_RIGHT_OVERLAY,
            TOWN_OBJECT_RACE_OVERLAY,
            TOWN_OBJECT_DOCK,
            TOWN_OBJECT_BOAT,
            TOWN_OBJECT_DWELLING_1,
            TOWN_OBJECT_DWELLING_3,
            TOWN_OBJECT_UPGRADED_DWELLING_3,
            TOWN_OBJECT_DWELLING_4,
            TOWN_OBJECT_UPGRADED_DWELLING_4,
            TOWN_OBJECT_STATUE,
            TOWN_OBJECT_WELL,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE
        },
        {
            TOWN_OBJECT_RACE_OVERLAY,
            TOWN_OBJECT_KNIGHT_LEFT_OVERLAY,
            TOWN_OBJECT_SPECIAL_BUILDING,
            TOWN_OBJECT_SECOND_WELL,
            TOWN_OBJECT_DWELLING_6,
            TOWN_OBJECT_THIEVES_GUILD,
            TOWN_OBJECT_CAPTAIN_QUARTERS,
            TOWN_OBJECT_DWELLING_5,
            TOWN_OBJECT_UPGRADED_DWELLING_5,
            TOWN_OBJECT_CASTLE_UPGRADE,
            TOWN_OBJECT_CASTLE,
            TOWN_OBJECT_MOAT,
            TOWN_OBJECT_MARKETPLACE,
            TOWN_OBJECT_DWELLING_2,
            TOWN_OBJECT_UPGRADED_DWELLING_2,
            TOWN_OBJECT_DWELLING_3,
            TOWN_OBJECT_BARBARIAN_OVERLAY,
            TOWN_OBJECT_DWELLING_1,
            TOWN_OBJECT_DWELLING_4,
            TOWN_OBJECT_UPGRADED_DWELLING_4,
            TOWN_OBJECT_MAGE_GUILD,
            TOWN_OBJECT_KNIGHT_RIGHT_OVERLAY,
            TOWN_OBJECT_TAVERN,
            TOWN_OBJECT_DOCK,
            TOWN_OBJECT_BOAT,
            TOWN_OBJECT_WELL,
            TOWN_OBJECT_LEFT_TURRET,
            TOWN_OBJECT_RIGHT_TURRET,
            TOWN_OBJECT_STATUE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE
        },
        {
            TOWN_OBJECT_SPECIAL_BUILDING,
            TOWN_OBJECT_DWELLING_6,
            TOWN_OBJECT_DWELLING_5,
            TOWN_OBJECT_CASTLE_UPGRADE,
            TOWN_OBJECT_CASTLE,
            TOWN_OBJECT_LEFT_TURRET,
            TOWN_OBJECT_RIGHT_TURRET,
            TOWN_OBJECT_MOAT,
            TOWN_OBJECT_CAPTAIN_QUARTERS,
            TOWN_OBJECT_DWELLING_3,
            TOWN_OBJECT_UPGRADED_DWELLING_3,
            TOWN_OBJECT_DWELLING_1,
            TOWN_OBJECT_MAGE_GUILD,
            TOWN_OBJECT_DOCK,
            TOWN_OBJECT_BOAT,
            TOWN_OBJECT_DWELLING_4,
            TOWN_OBJECT_UPGRADED_DWELLING_4,
            TOWN_OBJECT_WELL,
            TOWN_OBJECT_MARKETPLACE,
            TOWN_OBJECT_DWELLING_2,
            TOWN_OBJECT_UPGRADED_DWELLING_2,
            TOWN_OBJECT_THIEVES_GUILD,
            TOWN_OBJECT_TAVERN,
            TOWN_OBJECT_KNIGHT_LEFT_OVERLAY,
            TOWN_OBJECT_SECOND_WELL,
            TOWN_OBJECT_STATUE,
            TOWN_OBJECT_RACE_OVERLAY,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE
        },
        {
            TOWN_OBJECT_DWELLING_5,
            TOWN_OBJECT_DWELLING_3,
            TOWN_OBJECT_CASTLE_UPGRADE,
            TOWN_OBJECT_CASTLE,
            TOWN_OBJECT_LEFT_TURRET,
            TOWN_OBJECT_RIGHT_TURRET,
            TOWN_OBJECT_CAPTAIN_QUARTERS,
            TOWN_OBJECT_RACE_OVERLAY,
            TOWN_OBJECT_MOAT,
            TOWN_OBJECT_TAVERN,
            TOWN_OBJECT_THIEVES_GUILD,
            TOWN_OBJECT_MAGE_GUILD,
            TOWN_OBJECT_MARKETPLACE,
            TOWN_OBJECT_STATUE,
            TOWN_OBJECT_DOCK,
            TOWN_OBJECT_BOAT,
            TOWN_OBJECT_SECOND_WELL,
            TOWN_OBJECT_DWELLING_1,
            TOWN_OBJECT_SPECIAL_BUILDING,
            TOWN_OBJECT_DWELLING_4,
            TOWN_OBJECT_UPGRADED_DWELLING_4,
            TOWN_OBJECT_DWELLING_2,
            TOWN_OBJECT_UPGRADED_DWELLING_6,
            TOWN_OBJECT_ALTERNATE_UPGRADED_DWELLING_6,
            TOWN_OBJECT_DWELLING_6,
            TOWN_OBJECT_WELL,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE
        },
        {
            TOWN_OBJECT_UPGRADED_DWELLING_6,
            TOWN_OBJECT_DWELLING_6,
            TOWN_OBJECT_THIEVES_GUILD,
            TOWN_OBJECT_CAPTAIN_QUARTERS,
            TOWN_OBJECT_RACE_OVERLAY,
            TOWN_OBJECT_DWELLING_5,
            TOWN_OBJECT_UPGRADED_DWELLING_5,
            TOWN_OBJECT_CASTLE_UPGRADE,
            TOWN_OBJECT_CASTLE,
            TOWN_OBJECT_MOAT,
            TOWN_OBJECT_DWELLING_3,
            TOWN_OBJECT_UPGRADED_DWELLING_3,
            TOWN_OBJECT_DWELLING_1,
            TOWN_OBJECT_DWELLING_4,
            TOWN_OBJECT_MAGE_GUILD,
            TOWN_OBJECT_TAVERN,
            TOWN_OBJECT_DOCK,
            TOWN_OBJECT_BOAT,
            TOWN_OBJECT_WELL,
            TOWN_OBJECT_DWELLING_2,
            TOWN_OBJECT_SPECIAL_BUILDING,
            TOWN_OBJECT_MARKETPLACE,
            TOWN_OBJECT_LEFT_TURRET,
            TOWN_OBJECT_RIGHT_TURRET,
            TOWN_OBJECT_STATUE,
            TOWN_OBJECT_SECOND_WELL,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE
        },
        {
            TOWN_OBJECT_SPECIAL_BUILDING,
            TOWN_OBJECT_TAVERN,
            TOWN_OBJECT_CASTLE,
            TOWN_OBJECT_LEFT_TURRET,
            TOWN_OBJECT_RIGHT_TURRET,
            TOWN_OBJECT_MOAT,
            TOWN_OBJECT_CAPTAIN_QUARTERS,
            TOWN_OBJECT_THIEVES_GUILD,
            TOWN_OBJECT_DWELLING_6,
            TOWN_OBJECT_DWELLING_1,
            TOWN_OBJECT_DWELLING_3,
            TOWN_OBJECT_UPGRADED_DWELLING_3,
            TOWN_OBJECT_MAGE_GUILD,
            TOWN_OBJECT_RACE_OVERLAY,
            TOWN_OBJECT_DOCK,
            TOWN_OBJECT_BOAT,
            TOWN_OBJECT_DWELLING_5,
            TOWN_OBJECT_UPGRADED_DWELLING_5,
            TOWN_OBJECT_DWELLING_2,
            TOWN_OBJECT_UPGRADED_DWELLING_2,
            TOWN_OBJECT_DWELLING_4,
            TOWN_OBJECT_UPGRADED_DWELLING_4,
            TOWN_OBJECT_SECOND_WELL,
            TOWN_OBJECT_CASTLE_UPGRADE,
            TOWN_OBJECT_WELL,
            TOWN_OBJECT_MARKETPLACE,
            TOWN_OBJECT_STATUE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE,
            TOWN_OBJECT_NONE
        }
    };

// @data-layout-note Retail's initialized TOWNMGR contribution is
// 0xee750..0xef4c8 (0xd78); candidate .data is 0xd77. The 84 candidate owners
// cover every candidate byte: sBuildingInfo, two signed-short function line
// bases, and 81 private literals. Owner-wise translation is byte-exact with
// SHA-256 8bd72565ea607f957ee8d39c3c3f1e087251f2ae67d401d1d336aefcccf94fa.
// Retail interleaves the line bases at 0xef174 and 0xef3b4, while VC4.2 hoists
// them to candidate offsets 0x6c0 and 0x6c4. The three repeated port/build
// filenames at 0xeeeac, 0xeeebc, and 0xeeecc are fixed by their code relocation
// sites. Retail has one terminal zero byte. The 0xc0 object-order table and
// 0xc townManager vtable sections are assigned at 0xeb080 and 0xeb140; the
// latter occupies a retail 0x10 contribution with four zero tail bytes. There
// is no TOWNMGR BSS contribution. Do not add padding, fallback identities,
// aliases, cursor adjustments, or section pragmas to force allocation order.
DATA(0x004ee750) SBuildingInfo
    sBuildingInfo[FACTION_COUNT][TOWN_BUILDING_COUNT] = {
        { // Knight
            { 0, 397, 46, 84, 138 },
            { 5, 0, 130, 53, 63 },
            { 5, 345, 114, 83, 62 },
            { 5, 531, 214, 113, 42 },
            { 0, 188, 214, 39, 42 },
            { 0, 69, 108, 67, 55 },
            { 5, 0, 49, 286, 116 },
            { 0, 478, 193, 46, 63 },
            { 5, 7, 33, 0, 0 },
            { 5, 134, 37, 0, 0 },
            { 0, 219, 138, 120, 30 },
            { 0, 286, 102, 88, 22 },
            { 0, 0, 146, 311, 30 },
            { 0, 0, 78, 251, 22 },
            { 9, 531, 211, 113, 45 },
            { 0, 293, 107, 59, 35 },
            { 5, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 5, 192, 163, 69, 52 },
            { 0, 135, 149, 73, 32 },
            { 5, 240, 166, 91, 66 },
            { 0, 323, 174, 102, 69 },
            { 7, 48, 176, 104, 80 },
            { 0, 445, 50, 195, 157 },
            { 0, 135, 149, 73, 32 },
            { 5, 240, 166, 91, 66 },
            { 0, 323, 174, 102, 69 },
            { 7, 48, 176, 104, 80 },
            { 0, 445, 50, 195, 157 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 }
        },
        { // Barbarian
            { 8, 346, 22, 54, 120 },
            { 0, 466, 94, 87, 47 },
            { 0, 0, 161, 136, 85 },
            { 5, 505, 199, 138, 56 },
            { 0, 268, 189, 50, 66 },
            { 0, 44, 109, 87, 52 },
            { 6, 0, 0, 214, 175 },
            { 0, 463, 154, 38, 81 },
            { 0, 10, 58, 0, 0 },
            { 0, 118, 45, 0, 0 },
            { 0, 217, 166, 67, 43 },
            { 0, 240, 106, 73, 34 },
            { 5, 115, 138, 182, 42 },
            { 0, 210, 80, 197, 61 },
            { 9, 505, 199, 138, 56 },
            { 5, 206, 99, 46, 42 },
            { 5, 0, 0, 0, 0 },
            { 5, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 0, 290, 138, 58, 45 },
            { 0, 145, 195, 76, 52 },
            { 0, 557, 48, 83, 83 },
            { 5, 496, 136, 138, 64 },
            { 5, 318, 174, 131, 54 },
            { 5, 407, 0, 113, 106 },
            { 0, 145, 195, 76, 52 },
            { 0, 0, 0, 0, 0 },
            { 5, 496, 136, 138, 64 },
            { 5, 318, 174, 131, 54 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 }
        },
        { // Sorceress
            { 0, 279, 0, 63, 168 },
            { 5, 423, 167, 87, 50 },
            { 5, 490, 141, 148, 91 },
            { 5, 0, 208, 178, 48 },
            { 0, 335, 205, 45, 29 },
            { 0, 104, 130, 59, 42 },
            { 5, 0, 0, 201, 179 },
            { 0, 152, 163, 28, 65 },
            { 0, 98, 99, 0, 0 },
            { 0, 151, 98, 0, 0 },
            { 0, 404, 122, 69, 45 },
            { 5, 131, 185, 71, 53 },
            { 0, 0, 171, 272, 23 },
            { 0, 152, 0, 236, 84 },
            { 9, 0, 208, 178, 48 },
            { 5, 223, 122, 37, 52 },
            { 5, 0, 0, 0, 0 },
            { 5, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 5, 472, 59, 111, 92 },
            { 5, 338, 146, 93, 61 },
            { 0, 51, 164, 106, 40 },
            { 0, 198, 178, 143, 71 },
            { 0, 263, 226, 296, 30 },
            { 0, 179, 0, 84, 119 },
            { 5, 338, 146, 93, 61 },
            { 0, 51, 164, 106, 40 },
            { 0, 198, 178, 143, 71 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 }
        },
        { // Warlock
            { 0, 586, 18, 54, 150 },
            { 0, 520, 103, 64, 54 },
            { 0, 476, 96, 82, 55 },
            { 5, 517, 200, 123, 56 },
            { 0, 342, 205, 67, 51 },
            { 0, 298, 135, 72, 31 },
            { 5, 241, 18, 181, 150 },
            { 0, 478, 161, 37, 63 },
            { 0, 311, 84, 0, 0 },
            { 0, 359, 83, 0, 0 },
            { 0, 386, 171, 71, 40 },
            { 6, 60, 32, 63, 186 },
            { 5, 211, 166, 301, 21 },
            { 0, 0, 160, 59, 96 },
            { 9, 517, 200, 123, 56 },
            { 5, 418, 83, 53, 84 },
            { 5, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 6, 0, 64, 48, 50 },
            { 0, 237, 168, 78, 87 },
            { 0, 492, 50, 53, 39 },
            { 0, 139, 163, 190, 83 },
            { 0, 82, 92, 178, 68 },
            { 0, 92, 0, 64, 257 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 0, 139, 163, 190, 83 },
            { 0, 0, 0, 0, 0 },
            { 0, 92, 0, 64, 257 },
            { 0, 92, 0, 64, 257 },
            { 0, 0, 0, 0, 0 }
        },
        { // Wizard
            { 0, 570, 0, 70, 126 },
            { 5, 505, 50, 51, 49 },
            { 6, 0, 149, 118, 76 },
            { 5, 0, 206, 206, 50 },
            { 0, 249, 139, 28, 33 },
            { 0, 58, 60, 49, 42 },
            { 5, 0, 0, 200, 99 },
            { 0, 464, 45, 24, 72 },
            { 0, 30, 17, 0, 0 },
            { 0, 128, 17, 0, 0 },
            { 0, 255, 163, 108, 53 },
            { 0, 237, 208, 137, 49 },
            { 0, 0, 90, 223, 14 },
            { 0, 297, 95, 109, 78 },
            { 9, 0, 206, 206, 50 },
            { 0, 210, 52, 28, 35 },
            { 5, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 5, 467, 181, 38, 30 },
            { 0, 231, 68, 192, 36 },
            { 5, 152, 130, 96, 60 },
            { 0, 593, 184, 51, 31 },
            { 0, 411, 0, 49, 167 },
            { 0, 160, 0, 178, 67 },
            { 0, 0, 0, 0, 0 },
            { 5, 152, 130, 96, 60 },
            { 0, 0, 0, 0, 0 },
            { 0, 411, 0, 49, 167 },
            { 0, 160, 0, 178, 67 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 }
        },
        { // Necromancer
            { 5, 557, 17, 85, 191 },
            { 0, 275, 124, 62, 77 },
            { 0, 455, 39, 51, 103 },
            { 5, 500, 220, 141, 36 },
            { 0, 215, 213, 29, 41 },
            { 0, 333, 115, 47, 70 },
            { 5, 289, 10, 134, 164 },
            { 0, 365, 154, 41, 93 },
            { 0, 330, 47, 0, 0 },
            { 0, 360, 46, 0, 0 },
            { 0, 412, 193, 98, 61 },
            { 6, 263, 181, 90, 65 },
            { 0, 258, 171, 193, 19 },
            { 0, 0, 0, 640, 63 },
            { 9, 500, 220, 141, 36 },
            { 0, 441, 77, 22, 99 },
            { 5, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 0, 396, 177, 71, 35 },
            { 0, 110, 174, 141, 45 },
            { 5, 0, 28, 241, 142 },
            { 0, 20, 107, 124, 129 },
            { 0, 221, 127, 66, 84 },
            { 6, 464, 72, 105, 124 },
            { 0, 110, 174, 141, 45 },
            { 5, 0, 28, 241, 142 },
            { 0, 0, 107, 144, 129 },
            { 0, 223, 45, 65, 166 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 }
        }
    };

// @match-note 90.65%: construction semantics, exact 0x30 frame, and all 12/12
// relocations agree. fileName/w/h/x/buildingId_h/tempY/y occupy retail
// -0x10/-0x14/-0x18/-0x1c/-0x20/-0x24/-0x28, followed by the compiler new
// temporary and this at -0x2c/-0x30. The first relocation-masked raw byte
// differs at +0x36: retail 08 versus ours 0c, the displacement operand of the
// mov eax,[ebp+...] at +0x34. Retail forms
// each SBuildingInfo address with townType*0x120 in eax and buildingId*9 in
// ecx; this compiler state selects the commutative reverse register order for
// all five field reads. Direct indexing, inverted subscript, and explicit
// typed pointer-add spellings select the same order. A guarded 20-trial
// post-95 TU-state sweep found several disposable 99.7685% candidates but no
// audited exact closure; every candidate failed the strict size/identity guard.
// Revisit only after a material TU-state change.
VA(0x00413900, 0x16a)
townObject::townObject(i32 townType, i32 buildingId, char *iconBaseName)
{
    char fileName[TOWN_OBJECT_FILENAME_SIZE];
    i32 x;
    i32 y;
    i32 tempY;
    i32 w;
    i32 h;
    i32 buildingId_h;

    m_animationFrame = 0;
    m_icon = 0;
    m_border = 0;
    m_visible = 1;
    m_animationFrameCount =
        sBuildingInfo[townType][buildingId].animationFrameCount;
    x = sBuildingInfo[townType][buildingId].x;
    y = sBuildingInfo[townType][buildingId].y;
    w = sBuildingInfo[townType][buildingId].width;
    h = sBuildingInfo[townType][buildingId].height;
    buildingId_h = buildingId;
    m_buildingId = buildingId_h;
    sprintf(fileName, "%s.icn", iconBaseName);
    m_icon = gpResourceManager->GetIcon(fileName);
    if (buildingId_h != TOWN_OBJECT_NONE) {
        m_border = new border(x, y, w, h, static_cast<i16>(buildingId_h),
                              TOWN_OBJECT_BORDER_Z_ORDER, 0, 0);
        if (m_border == 0)
            MemError();
    }
}

VA(0x00413a6a, 0x60)
townObject::~townObject()
{
    if (m_border != 0)
        delete m_border;
    gpResourceManager->Dispose(m_icon);
}

VA(0x00413aca, 0x437)
void townObject::Draw(i32 advanceAnimation)
{
    i32 baseFrame;

    if (m_visible == 0)
        return;
    if (m_buildingId == TOWN_OBJECT_CASTLE_UPGRADE)
        return;
    if (m_buildingId == TOWN_OBJECT_KNIGHT_LEFT_OVERLAY &&
        gpTownManager->m_town->m_type == FACTION_KNIGHT &&
        (!(gpTownManager->m_town->m_buildings & TOWN_RENDER_KNIGHT_LEFT_GATE) ||
         (!(gpTownManager->m_town->m_buildings &
            TOWN_RENDER_KNIGHT_LEFT_FIRST_OPTION) &&
          !(gpTownManager->m_town->m_buildings &
            TOWN_RENDER_KNIGHT_LEFT_SECOND_OPTION) &&
          !(gpTownManager->m_town->m_buildings &
            TOWN_RENDER_KNIGHT_LEFT_THIRD_OPTION))))
        return;
    if (m_buildingId == TOWN_OBJECT_KNIGHT_RIGHT_OVERLAY &&
        gpTownManager->m_town->m_type == FACTION_KNIGHT &&
        (!(gpTownManager->m_town->m_buildings & TOWN_RENDER_KNIGHT_RIGHT_GATE) ||
         (!(gpTownManager->m_town->m_buildings &
            TOWN_RENDER_KNIGHT_RIGHT_FIRST_OPTION) &&
          !(gpTownManager->m_town->m_buildings &
            TOWN_RENDER_KNIGHT_RIGHT_SECOND_OPTION))))
        return;
    if (m_buildingId == TOWN_OBJECT_KNIGHT_RIGHT_OVERLAY &&
        gpTownManager->m_town->m_type == FACTION_BARBARIAN &&
        (!(gpTownManager->m_town->m_buildings &
           TOWN_RENDER_BARBARIAN_RIGHT_GATE) ||
         !(gpTownManager->m_town->m_buildings &
           TOWN_RENDER_BARBARIAN_RIGHT_OPTION)))
        return;
    if (m_buildingId == TOWN_OBJECT_BARBARIAN_OVERLAY &&
        gpTownManager->m_town->m_type == FACTION_BARBARIAN &&
        (!(gpTownManager->m_town->m_buildings &
           TOWN_RENDER_BARBARIAN_OVERLAY_GATE) ||
         !(gpTownManager->m_town->m_buildings &
           TOWN_RENDER_BARBARIAN_OVERLAY_OPTION)))
        return;
    if (gpTownManager->m_town->m_type == FACTION_SORCERESS &&
        m_buildingId == TOWN_OBJECT_KNIGHT_LEFT_OVERLAY &&
        (!(gpTownManager->m_town->m_buildings &
           TOWN_RENDER_SORCERESS_LEFT_GATE) ||
         !(gpTownManager->m_town->m_buildings &
           TOWN_RENDER_SORCERESS_LEFT_OPTION)))
        return;
    if (gpTownManager->m_town->m_type == FACTION_SORCERESS &&
        (m_buildingId == TOWN_OBJECT_SORCERESS_LEFT_OVERLAY ||
         m_buildingId == TOWN_OBJECT_SORCERESS_RIGHT_OVERLAY) &&
        (gpTownManager->m_town->m_buildings &
         TOWN_RENDER_SORCERESS_LEFT_GATE) &&
        (gpTownManager->m_town->m_buildings &
         TOWN_RENDER_SORCERESS_LEFT_OPTION))
        return;
    if (m_buildingId == TOWN_OBJECT_RACE_OVERLAY &&
        (gpTownManager->m_town->m_type == FACTION_NECROMANCER ||
         gpTownManager->m_town->m_type == FACTION_WARLOCK ||
         gpTownManager->m_town->m_type == FACTION_SORCERESS ||
         gpTownManager->m_town->m_type == FACTION_KNIGHT) &&
        ((gpTownManager->m_town->m_buildings &
          TOWN_RENDER_RACE_OVERLAY_FIRST_OPTION) ||
         (gpTownManager->m_town->m_buildings &
          TOWN_RENDER_RACE_OVERLAY_SECOND_OPTION)))
        return;
    if (m_buildingId == TOWN_OBJECT_DOCK &&
        (gpTownManager->m_town->m_buildings &
         TOWN_RENDER_DOCK_GATE))
        return;

    if (m_buildingId == TOWN_OBJECT_PRIMARY_ANIMATION) {
        if (gpTownManager->m_town->m_type == FACTION_NECROMANCER) {
            baseFrame =
                2 * (3 * (gpTownManager->m_town->m_buildState - 1));
        } else {
            baseFrame = gpTownManager->m_town->m_buildState - 1;
        }
        m_icon->DrawToBuffer(0, 0, baseFrame, 0);
        if (m_animationFrameCount != 0) {
            if (gpTownManager->m_town->m_type == FACTION_BARBARIAN &&
                gpTownManager->m_town->m_buildState <
                    TOWN_BARBARIAN_ANIMATION_BUILD_STATE)
                return;
            m_icon->DrawToBuffer(0, 0,
                                 m_animationFrame + baseFrame + 1, 0);
            if (advanceAnimation == 1) {
                ++m_animationFrame;
                if (m_animationFrame == m_animationFrameCount)
                    m_animationFrame = 0;
            }
        }
        return;
    }

    m_icon->DrawToBuffer(0, 0, 0, 0);
    if (m_animationFrameCount != 0) {
        m_icon->DrawToBuffer(0, 0, m_animationFrame + 1, 0);
        if (advanceAnimation == 1) {
            ++m_animationFrame;
            if (m_animationFrame == m_animationFrameCount)
                m_animationFrame = 0;
        }
    }
}

VA(0x00413f01, 0x68)
townManager::townManager(void)
{
    m_town = 0;
    m_heroWindow0 = 0;
    m_unknownC6 = 0;
    m_selectedBuilding = TOWN_SELECTED_BUILDING_NONE;
    m_castleDialogActive = 0;
}

VA(0x00413f69, 0x1a0)
void townManager::SetupExtraStuff(void)
{
    m_town->m_buildings &= TOWN_EXTRA_DYNAMIC_CLEAR_MASK;
    if (m_town->m_type == FACTION_WIZARD)
        m_town->m_buildings |= TOWN_EXTRA_RACE_FIRST_MASK;
    if (m_town->m_type == FACTION_SORCERESS) {
        m_town->m_buildings |= TOWN_EXTRA_RACE_FIRST_MASK;
        m_town->m_buildings |= TOWN_EXTRA_RACE_SECOND_MASK;
    }
    if (m_town->m_type == FACTION_KNIGHT) {
        m_town->m_buildings |= TOWN_EXTRA_RACE_SECOND_MASK;
        m_town->m_buildings |= TOWN_EXTRA_RACE_THIRD_MASK;
    }
    if (m_town->m_type == FACTION_BARBARIAN) {
        m_town->m_buildings |= TOWN_EXTRA_RACE_SECOND_MASK;
        m_town->m_buildings |= TOWN_EXTRA_RACE_THIRD_MASK;
        m_town->m_buildings |= TOWN_EXTRA_RACE_LAST_MASK;
    }
    if ((m_town->m_type == FACTION_WARLOCK ||
         m_town->m_type == FACTION_KNIGHT ||
         m_town->m_type == FACTION_BARBARIAN ||
         m_town->m_type == FACTION_NECROMANCER) &&
        m_town->CanBuildDock())
        m_town->m_buildings |= TOWN_EXTRA_RACE_FIRST_MASK;
    if ((m_town->m_buildings & TOWN_BUILDING_DOCK) &&
        gpAdvManager->GetCell(m_town->m_boatX,
                              m_town->m_boatY)->m_triggerType != 0)
        m_town->m_buildings |= TOWN_EXTRA_DOCK_GRAPHIC_MASK;
    else
        m_town->m_buildings &= ~TOWN_EXTRA_DOCK_GRAPHIC_MASK;
}

VA(0x00414109, 0x1ef)
i32 townManager::Open(i32 id)
{
    gpGame->CheckHeroConsistency();
    if (gConfig.useOpera != CONFIG_OPERA_DISABLED || gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI)
        gpSoundManager->SwitchAmbientMusic(townTheme[m_town->m_type]);
    PollSound();
    m_townWindow = new heroWindow(0, 0, "townwind.bin");
    if (m_townWindow == 0)
        MemError();
    glTimers[0] = KBTickCount() + TOWN_REDRAW_INTERVAL;
    m_lastTownType = TOWN_LAST_TYPE_UNINITIALIZED;
    m_castleDialogActive = 0;
    m_recruitResult = 0;
    m_lastHoverId = TOWN_HOVER_NONE;
    m_lastHoverSubId = 0;
    m_townObjectCount = 0;
    m_unknownC6 = 0;
    m_garrisonStrip = 0;
    m_heroStrip = 0;
    m_selectedStrip = 0;
    m_swapStrip = 0;
    m_pendingStrip = 0;
    m_bankBox = 0;
    m_backgroundIcon = 0;
    SetupExtraStuff();
    SetupTown();
    KBChangeMenu(hmnuTown);
    gpMouseManager->SetPointer("advmice.mse", 0, TOWN_MAP_CHANGE_UNUSED);
    m_messageMask = TOWN_MANAGER_EVENT_MASK;
    m_priority = id;
    m_active = 1;
    strcpy(m_name, "townManager");
    gpWindowManager->FadeScreen(0, TOWN_FADE_STEPS, 0);
    return 0;
}

VA(0x004142f8, 0x77)
void townManager::ChangeTown(void)
{
    tag_message message;

    if (gConfig.useOpera != CONFIG_OPERA_DISABLED || gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI)
        gpSoundManager->SwitchAmbientMusic(townTheme[m_town->m_type]);
    SetupExtraStuff();
    SetupTown();
    message.type = MESSAGE_WIDGET;
    message.payload.widget.id = TOWN_WIDGET_ID_NONE;
    SetCommandAndText(message);
}

// @early-stop
// Jump-only proof: the exact 0x58 frame/slots, all 85 ordered relocations, and
// every non-jump opcode and operand agree. The 0x950-byte base stream is exactly
// ten bytes short of retail's 0x95a bytes: two five-byte jump-to-next inline
// continuations at retail +0x58e and +0x692 account for the complete delta.
// Color() regressed; color and occupying-hero OD_STEER(value) spellings were stagnant.
// Revisit only after either accessor, source/TU/header, or comparison epoch changes.
VA(0x0041436f, 0x95a)
void townManager::SetupTown(void)
{
    tag_message message;
    i32 objectOrder;
    i32 objectId;
    i32 crestFrame;

    sprintf(gText, GetTownName(m_town->m_id));
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = TOWN_WIDGET_SET_TEXT;
    message.payload.widget.id = TOWN_WINDOW_TEXT_CONTROL;
    message.payload.widget.data.text = gText;
    m_townWindow->BroadcastMessage(message);
    strcpy(gText, "Town Screen");
    message.payload.widget.id = TOWN_CONTROL_STATUS_TEXT;
    message.payload.widget.data.text = gText;
    m_townWindow->BroadcastMessage(message);
    m_townWindow->DrawWindow(0, TOWN_WINDOW_DRAW_WIDTH,
                             TOWN_WINDOW_DRAW_RIGHT);

    if (gpCurPlayer->m_townCount == 1) {
        message.payload.widget.command = TOWN_WIDGET_DISABLE;
        message.payload.widget.data.value = TOWN_WIDGET_DISABLED_VALUE;
        message.payload.widget.id = TOWN_CONTROL_PREVIOUS_TOWN;
        m_townWindow->BroadcastMessage(message);
        message.payload.widget.id = TOWN_CONTROL_NEXT_TOWN;
        m_townWindow->BroadcastMessage(message);
        message.payload.widget.command = TOWN_WIDGET_ENABLE;
        message.payload.widget.data.value = TOWN_WIDGET_ENABLED_VALUE;
        message.payload.widget.id = TOWN_CONTROL_PREVIOUS_TOWN;
        m_townWindow->BroadcastMessage(message);
        message.payload.widget.id = TOWN_CONTROL_NEXT_TOWN;
        m_townWindow->BroadcastMessage(message);
    }

    if (m_town->m_type != m_lastTownType) {
        if (m_lastTownType != TOWN_LAST_TYPE_NONE)
            UnloadTown();
        m_bankBox =
            new bankBox(TOWN_BANK_BOX_X, TOWN_GARRISON_STRIP_Y, gpCurPlayer);
        if (m_bankBox == 0)
            MemError();
        sprintf(gText, "townbkg%d.icn", m_town->m_type);
        m_backgroundIcon = gpResourceManager->GetIcon(gText);
        m_townObjectCount = 0;
        for (objectOrder = 0; objectOrder < TOWN_BUILDING_COUNT;
             ++objectOrder) {
            objectId = gTownObjectOrder[m_town->m_type][objectOrder];
            if (objectId != TOWN_OBJECT_NONE) {
                sprintf(gText, "%s%s",
                        gTownPrefixNames[m_town->m_type],
                        gTownObjNames[objectId]);
                m_townObjects[m_townObjectCount] =
                    new townObject(m_town->m_type, objectId, gText);
                if (m_townObjects[m_townObjectCount] == 0)
                    MemError();
                if (m_townObjects[m_townObjectCount]->m_border != 0) {
                    if (!(m_town->m_buildings & (1L << objectId))) {
                        m_townObjects[m_townObjectCount]->m_border->m_flags &=
                            ~TOWN_OBJECT_BORDER_ENABLED;
                        m_townObjects[m_townObjectCount]->m_visible = 0;
                    }
                    m_townWindow->AddWidget(
                        m_townObjects[m_townObjectCount]->m_border,
                        TOWN_WIDGET_INSERT_DEFAULT);
                }
                ++m_townObjectCount;
            }
        }
        gpWindowManager->AddWindow(m_townWindow, 0, 1);
    } else {
        m_townObjectCount = 0;
        for (objectOrder = 0; objectOrder < TOWN_BUILDING_COUNT;
             ++objectOrder) {
            i32 existingObjectId =
                gTownObjectOrder[m_town->m_type][objectOrder];
            if (existingObjectId != TOWN_OBJECT_NONE) {
                if (m_townObjects[m_townObjectCount]->m_border != 0) {
                    if (!(m_town->m_buildings &
                          (1L << existingObjectId))) {
                        m_townObjects[m_townObjectCount]->m_border->m_flags &=
                            ~TOWN_OBJECT_BORDER_ENABLED;
                        m_townObjects[m_townObjectCount]->m_visible = 0;
                    } else {
                        m_townObjects[m_townObjectCount]->m_border->m_flags |=
                            TOWN_OBJECT_BORDER_ENABLED;
                        m_townObjects[m_townObjectCount]->m_visible = 1;
                    }
                }
                ++m_townObjectCount;
            }
        }
        if (m_heroStrip != 0)
            delete m_heroStrip;
        m_heroStrip = 0;
        if (m_garrisonStrip != 0)
            delete m_garrisonStrip;
        m_garrisonStrip = 0;
    }

    crestFrame = gpCurPlayer->m_color;
    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
        crestFrame *= 4;
        crestFrame += gpGame->GetHero(m_town->m_occupyingHeroId)->m_portrait;
    } else {
        crestFrame += TOWN_EMPTY_HERO_PORTRAIT_OFFSET;
    }
    m_garrisonStrip = new strip(
        0, TOWN_GARRISON_STRIP_Y,
        static_cast<i8>(
            m_town->m_occupyingHeroId == TOWN_OCCUPYING_HERO_NONE
                ? TOWN_CREST_FRAME_WITHOUT_HERO
                : TOWN_CREST_FRAME_WITH_HERO),
        gpResourceManager->MakeId("crest.icn", TOWN_ICON_RESOURCE_TYPE),
        gpCurPlayer->m_color,
        &m_town->m_army, TOWN_GARRISON_FIRST_CONTROL, 0, -1);
    if (m_garrisonStrip == 0)
        MemError();

    if (m_town->m_occupyingHeroId != TOWN_OCCUPYING_HERO_NONE) {
        sprintf(gText, "port%04d.icn",
                gpGame->GetHero(m_town->m_occupyingHeroId)->m_portrait);
        m_heroStrip = new strip(
            0, TOWN_HERO_STRIP_Y, TOWN_HERO_STRIP_FRAME_COUNT,
            gpResourceManager->MakeId(gText, TOWN_ICON_RESOURCE_TYPE), 0,
            &gpGame->GetHero(m_town->m_occupyingHeroId)->m_army,
            TOWN_HERO_FIRST_CONTROL, 0, -1);
        if (m_heroStrip == 0)
            MemError();
        if (m_town->m_buildings & TOWN_BUILDING_MAGE_GUILD)
            m_town->GiveSpells(0);
    } else if (m_town->m_buildings & TOWN_BUILDING_CAPTAIN_QUARTERS) {
        sprintf(gText, "port%04d.icn",
                m_town->m_type + TOWN_PORTRAIT_FRAME_BASE);
        m_heroStrip = new strip(
            0, TOWN_HERO_STRIP_Y, TOWN_HERO_STRIP_FRAME_COUNT,
            gpResourceManager->MakeId(gText, TOWN_ICON_RESOURCE_TYPE),
            0, 0, -1, 0,
            gpCurPlayer->m_color);
        if (m_heroStrip == 0)
            MemError();
    } else {
        m_heroStrip = new strip(
            0, TOWN_HERO_STRIP_Y, TOWN_HERO_STRIP_FRAME_COUNT,
            gpResourceManager->MakeId("strip.icn", TOWN_ICON_RESOURCE_TYPE),
            TOWN_HERO_STRIP_FRAME_COUNT, 0, -1, 0, -1);
        if (m_heroStrip == 0)
            MemError();
    }

    m_lastTownType = m_town->m_type;
    m_pendingStrip = 0;
    m_swapStrip = m_pendingStrip;
    m_selectedStrip = m_swapStrip;
    m_pendingArmySlot = TOWN_ARMY_SLOT_NONE;
    m_swapArmySlot = m_pendingArmySlot;
    m_selectedArmySlot = m_swapArmySlot;
    DrawTown(0, 0);
    gpWindowManager->UpdateScreenRegion(0, 0, TOWN_SCREEN_WIDTH,
                                         TOWN_SCREEN_HEIGHT);
}

VA(0x00414cc9, 0x1cf)
void townManager::UnloadTown(void)
{
    i32 index_i;

    if (m_bankBox != 0)
        delete m_bankBox;
    m_bankBox = 0;
    if (m_heroStrip != 0)
        delete m_heroStrip;
    m_heroStrip = 0;
    if (m_garrisonStrip != 0)
        delete m_garrisonStrip;
    m_garrisonStrip = 0;
    for (index_i = 0; index_i < m_townObjectCount; ++index_i) {
        m_townWindow->RemoveWidget(m_townObjects[index_i]->m_border);
        delete m_townObjects[index_i];
        m_townObjects[index_i] = 0;
    }
    if (m_backgroundIcon != 0) {
        gpResourceManager->Dispose(m_backgroundIcon);
        m_backgroundIcon = 0;
    }
}

VA(0x00414e98, 0xca)
void townManager::Close(void)
{
    UnloadTown();
    if (m_townWindow != 0) {
        gpWindowManager->RemoveWindow(m_townWindow);
        delete m_townWindow;
    }
    m_townWindow = 0;
    if (gConfig.useOpera != CONFIG_OPERA_DISABLED || gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI)
        gpSoundManager->SwitchAmbientMusic(TOWN_MUSIC_STOP);
    gpWindowManager->FadeScreen(TOWN_FADE_OUT, TOWN_FADE_STEPS, 0);
    gpMouseManager->SetPointer(TOWN_POINTER_DEFAULT);
    m_active = 0;
    m_town->m_buildings &= TOWN_CLOSE_DYNAMIC_CLEAR_MASK;
}

VA(0x00414f62, 0x3b9)
void townManager::SetArmyCommand(i32 qualifier)
{
    i32 cantMoveLastArmy;
    i32 sameType;

    m_command = TOWN_WIDGET_ID_NONE;
    cantMoveLastArmy = 0;
    if (m_swapStrip->m_army->GetNumArmies() == 1 &&
        &m_swapStrip[0] == m_heroStrip && m_pendingStrip != m_swapStrip)
        cantMoveLastArmy = 1;

    if (m_pendingStrip == m_swapStrip &&
        m_swapArmySlot == m_pendingArmySlot) {
        sprintf(m_statusText, cTownCommand[TOWN_TEXT_VIEW_ARMY],
                gArmyNames[
                    m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot]]);
        m_command = TOWN_ARMY_COMMAND_VIEW;
    } else {
        sameType = 0;
        if ((m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] | 0) ==
            m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot])
            sameType = 1;
        if (sameType) {
            if (qualifier != 0) {
                sprintf(m_statusText,
                        cTownCommand[TOWN_TEXT_REDISTRIBUTE_ARMY],
                        gArmyNames[m_swapStrip->m_army
                                       ->m_creatureTypes[m_swapArmySlot]]);
                m_command = TOWN_ARMY_COMMAND_SPLIT;
            } else if (cantMoveLastArmy) {
                strcpy(m_statusText,
                       cTownCommand[TOWN_TEXT_CANNOT_COMBINE_LAST_ARMY]);
                return;
            } else {
                sprintf(m_statusText,
                        cTownCommand[TOWN_TEXT_COMBINE_ARMIES],
                        gArmyNames[m_swapStrip->m_army
                                       ->m_creatureTypes[m_swapArmySlot]]);
                m_command = TOWN_ARMY_COMMAND_MERGE;
            }
        } else if (qualifier != 0 &&
                   m_pendingStrip->m_army
                           ->m_creatureTypes[m_pendingArmySlot] ==
                       ARMY_GROUP_EMPTY_SLOT) {
            sprintf(m_statusText,
                    cTownCommand[TOWN_TEXT_REDISTRIBUTE_TO_EMPTY_SLOT],
                    gArmyNames[m_swapStrip->m_army
                                   ->m_creatureTypes[m_swapArmySlot]]);
            m_command = TOWN_ARMY_COMMAND_SPLIT;
        }
    }

    if (m_command != TOWN_WIDGET_ID_NONE)
        return;
    if (m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot] ==
        ARMY_GROUP_EMPTY_SLOT) {
        if (cantMoveLastArmy) {
            strcpy(m_statusText,
                   cTownCommand[TOWN_TEXT_CANNOT_MOVE_LAST_ARMY]);
            return;
        } else {
            sprintf(m_statusText, cTownCommand[TOWN_TEXT_MOVE_ARMY],
                    gArmyNames[m_swapStrip->m_army
                                   ->m_creatureTypes[m_swapArmySlot]]);
            m_command = TOWN_ARMY_COMMAND_SWAP;
        }
    } else {
        sprintf(
            m_statusText, cTownCommand[TOWN_TEXT_EXCHANGE_ARMIES],
            gArmyNames[m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot]],
            gArmyNames[m_pendingStrip->m_army
                           ->m_creatureTypes[m_pendingArmySlot]]);
        m_command = TOWN_ARMY_COMMAND_SWAP;
    }
}

// @early-stop
// Permanent delinker-label artifact: all 0x5c5 relocation-masked bytes match.
// Manual llvm-objdump audit finds 73/73 relocations: 55 external plus 18 local
// jump-table entries. Retail absolute operands prove cTownCommand indices
// 8, 9, and 11 through 27, plus gWellExtraNames, gSpecialBuildingNames, and
// gDwellingType-19; the helper
// truncates at the delinked local jump-table symbol and reports only 2 base.
VA(0x0041531b, 0x5c5)
void townManager::SetCommandAndText(struct tag_message &message)
{
    i32 objectId = message.payload.widget.id;

    m_command = TOWN_WIDGET_ID_NONE;
    switch (objectId) {
    case TOWN_CONTROL_CLOSE:
        strcpy(m_statusText, cTownCommand[TOWN_TEXT_EXIT]);
        break;
    case TOWN_WIDGET_ID_NONE:
    case TOWN_EMPTY_STATUS_CONTROL_FIRST:
    case TOWN_EMPTY_STATUS_CONTROL_LAST:
        strcpy(m_statusText, cTownCommand[TOWN_TEXT_EMPTY_STATUS]);
        break;
    case TOWN_GARRISON_SLOT_FIRST:
    case TOWN_GARRISON_SLOT_FIRST + 1:
    case TOWN_GARRISON_SLOT_FIRST + 2:
    case TOWN_GARRISON_SLOT_FIRST + 3:
    case TOWN_GARRISON_SLOT_LAST:
        if (m_swapArmySlot != TOWN_ARMY_SLOT_NONE) {
            m_pendingStrip = m_garrisonStrip;
            m_pendingArmySlot = objectId - TOWN_GARRISON_SLOT_FIRST;
            SetArmyCommand(message.payload.widget.parameter &
                           TOWN_ARMY_QUALIFIER_MASK);
        } else {
            m_selectedStrip = m_garrisonStrip;
            m_selectedArmySlot = objectId - TOWN_GARRISON_SLOT_FIRST;
            if (m_selectedStrip->m_army
                    ->m_creatureTypes[m_selectedArmySlot] ==
                ARMY_GROUP_EMPTY_SLOT) {
                strcpy(m_statusText, cTownCommand[TOWN_TEXT_EMPTY_SLOT]);
            } else {
                sprintf(m_statusText, cTownCommand[TOWN_TEXT_SELECT_ARMY],
                        gArmyNames[m_selectedStrip->m_army
                                       ->m_creatureTypes[m_selectedArmySlot]]);
                m_command = TOWN_ARMY_COMMAND_SELECT;
            }
        }
        break;
    case TOWN_HERO_FIRST_CONTROL:
        strcpy(m_statusText, cTownCommand[TOWN_TEXT_VIEW_HERO]);
        m_command = TOWN_ARMY_COMMAND_VIEW_HERO;
        break;
    case TOWN_HERO_SLOT_FIRST:
    case TOWN_HERO_SLOT_FIRST + 1:
    case TOWN_HERO_SLOT_FIRST + 2:
    case TOWN_HERO_SLOT_FIRST + 3:
    case TOWN_HERO_SLOT_LAST:
        if (m_swapArmySlot != TOWN_ARMY_SLOT_NONE) {
            m_pendingStrip = m_heroStrip;
            m_pendingArmySlot = objectId - TOWN_HERO_SLOT_FIRST;
            SetArmyCommand(message.payload.widget.parameter &
                           TOWN_ARMY_QUALIFIER_MASK);
        } else {
            m_selectedStrip = m_heroStrip;
            m_selectedArmySlot = objectId - TOWN_HERO_SLOT_FIRST;
            if (m_selectedStrip->m_army == 0 ||
                m_selectedStrip->m_army
                        ->m_creatureTypes[m_selectedArmySlot] ==
                    ARMY_GROUP_EMPTY_SLOT) {
                strcpy(m_statusText, cTownCommand[TOWN_TEXT_EMPTY_SLOT]);
                m_command = TOWN_WIDGET_ID_NONE;
            } else {
                sprintf(m_statusText, cTownCommand[TOWN_TEXT_SELECT_ARMY],
                        gArmyNames[m_selectedStrip->m_army
                                       ->m_creatureTypes[m_selectedArmySlot]]);
                m_command = TOWN_ARMY_COMMAND_SELECT;
            }
        }
        break;
    case TOWN_OBJECT_MAGE_GUILD:
        strcpy(m_statusText, cTownCommand[TOWN_TEXT_MAGE_GUILD]);
        break;
    case TOWN_OBJECT_THIEVES_GUILD:
        strcpy(m_statusText, cTownCommand[TOWN_TEXT_THIEVES_GUILD]);
        break;
    case TOWN_OBJECT_TAVERN:
        if (m_town->m_type == FACTION_NECROMANCER)
            strcpy(m_statusText, xNecromancerShrine);
        else
            strcpy(m_statusText, cTownCommand[TOWN_TEXT_TAVERN]);
        break;
    case TOWN_OBJECT_DOCK:
    case TOWN_OBJECT_BOAT:
        strcpy(m_statusText, cTownCommand[TOWN_TEXT_DOCK]);
        break;
    case TOWN_OBJECT_WELL:
        strcpy(m_statusText, cTownCommand[TOWN_TEXT_WELL]);
        break;
    case TOWN_OBJECT_TENT:
        strcpy(m_statusText, cTownCommand[TOWN_TEXT_TENT]);
        break;
    case TOWN_OBJECT_CASTLE:
        strcpy(m_statusText, cTownCommand[TOWN_TEXT_CASTLE]);
        break;
    case TOWN_OBJECT_STATUE:
        strcpy(m_statusText, cTownCommand[TOWN_TEXT_STATUE]);
        break;
    case TOWN_OBJECT_LEFT_TURRET:
        strcpy(m_statusText, cTownCommand[TOWN_TEXT_LEFT_TURRET]);
        break;
    case TOWN_OBJECT_RIGHT_TURRET:
        strcpy(m_statusText, cTownCommand[TOWN_TEXT_RIGHT_TURRET]);
        break;
    case TOWN_OBJECT_MOAT:
        strcpy(m_statusText, cTownCommand[TOWN_TEXT_MOAT]);
        break;
    case TOWN_OBJECT_MARKETPLACE:
        strcpy(m_statusText, cTownCommand[TOWN_TEXT_MARKETPLACE]);
        break;
    case TOWN_OBJECT_CAPTAIN_QUARTERS:
        strcpy(m_statusText, cTownCommand[TOWN_TEXT_CAPTAIN_QUARTERS]);
        break;
    case TOWN_OBJECT_SPECIAL_BUILDING:
        strcpy(m_statusText, gSpecialBuildingNames[m_town->m_type]);
        break;
    case TOWN_OBJECT_SECOND_WELL:
        strcpy(m_statusText, gWellExtraNames[m_town->m_type]);
        break;
    case TOWN_OBJECT_DWELLING_1:
    case TOWN_OBJECT_DWELLING_2:
    case TOWN_OBJECT_DWELLING_3:
    case TOWN_OBJECT_DWELLING_4:
    case TOWN_OBJECT_DWELLING_5:
    case TOWN_OBJECT_DWELLING_6:
    case TOWN_OBJECT_UPGRADED_DWELLING_2:
    case TOWN_OBJECT_UPGRADED_DWELLING_3:
    case TOWN_OBJECT_UPGRADED_DWELLING_4:
    case TOWN_OBJECT_UPGRADED_DWELLING_5:
    case TOWN_OBJECT_UPGRADED_DWELLING_6:
    case TOWN_OBJECT_ALTERNATE_UPGRADED_DWELLING_6:
        sprintf(
            m_statusText, cTownCommand[TOWN_TEXT_RECRUIT],
            gArmyNames[gDwellingType[m_town->m_type]
                                    [objectId - TOWN_OBJECT_DWELLING_1]]);
        break;
    }
    ShowText(m_statusText);
}

VA(0x004158e0, 0x7d)
void townManager::ShowText(char *)
{
    tag_message message;

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = TOWN_WIDGET_SET_TEXT;
    message.payload.widget.id = TOWN_CONTROL_STATUS_TEXT;
    message.payload.widget.data.text = m_statusText;
    m_townWindow->BroadcastMessage(message);
    m_townWindow->DrawWindow(TOWN_STATUS_DRAW_LEFT, TOWN_STATUS_DRAW_WIDTH,
                             TOWN_STATUS_DRAW_RIGHT);
    gpWindowManager->UpdateScreenRegion(
        TOWN_STATUS_REGION_X, TOWN_STATUS_REGION_Y, TOWN_STATUS_REGION_WIDTH,
        TOWN_STATUS_REGION_HEIGHT);
}

// @semantic: Explicit-range comparison excluding the 0x38-byte jump
// table has 1306 instructions on both sides, an identical opcode multiset, the
// exact 0x250 frame/0x1830 size, and all 215/215 ordered relocations. Three
// /Ob1 continuation jumps move between six otherwise aligned spans. A bounded
// 512-candidate AST pass and integral-global-read pass retained no canonical
// mutation. Revisit only with a shared inline-accessor/TU-state discovery.
VA(0x0041595d, 0x1830)
i32 townManager::Main(tag_message &message)
{
    char description_b[400];
    i32 exitTown_i = 0;
    i32 quickView_k = (message.payload.widget.parameter & TOWN_QUICK_VIEW_MODIFIER) != 0;
    i32 debugBuilding_e;
    i32 index_i;
    i32 marketplaceCount_m;
    baseManager *dialogManager_d;
    SAMPLE2 buildSample_m;
    i32 total;
    i32 unusedTownValue;

    if (giDebugBuildingToBuild != -1) {
        debugBuilding_e = giDebugBuildingToBuild;
        giDebugBuildingToBuild = -1;
        if (debugBuilding_e == TOWN_DEBUG_BUILD_ALL) {
            for (index_i = 0; index_i < TOWN_BUILDING_COUNT; ++index_i) {
                if ((gTownEligibleBuildMask[m_town->m_type] & (1L << index_i)) ||
                    index_i == TOWN_COMMAND_CASTLE)
                    BuildObj(index_i);
            }
        } else if ((gTownEligibleBuildMask[m_town->m_type] &
                    (1L << static_cast<u8>(debugBuilding_e))) ||
                   debugBuilding_e == TOWN_COMMAND_CASTLE) {
            BuildObj(debugBuilding_e);
        }
    }

    if (KBTickCount() > glTimers[0]) {
        DrawTown(1, 1);
        glTimers[0] = KBTickCount() + TOWN_REDRAW_INTERVAL;
    }

    switch (message.type) {
    case MESSAGE_WIDGET:
        switch (message.payload.widget.command) {
        case TOWN_INPUT_SELECT:
        case TOWN_INPUT_ALTERNATE_SELECT:
        switch (message.payload.widget.id) {
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
            if (quickView_k) {
                QuickViewRecruit(
                    m_town, message.payload.widget.id - TOWN_COMMAND_FIRST_DWELLING);
            } else {
                DrawTown(1, 1);
                dialogManager_d = new recruitUnit(
                    m_town,
                    message.payload.widget.id - TOWN_COMMAND_FIRST_DWELLING, 1);
                if (dialogManager_d == 0)
                    MemError();
                gpExec->DoDialog(dialogManager_d);
                delete dialogManager_d;
            }
            break;

        case TOWN_COMMAND_CASTLE:
            if (quickView_k) {
                goto showBuildingInformation;
            }
            {
                m_heroWindow0 = new heroWindow(0, 0, "caslwind.bin");
                if (m_heroWindow0 == 0)
                    MemError();
                SetupCastle(m_heroWindow0, 0);
                m_castleDialogActive = 1;
                m_recruitResult = 0;
                gpWindowManager->DoDialog(
                    m_heroWindow0, CastleHandler, 0);
                m_castleDialogActive = 0;
                delete m_heroWindow0;

                if (m_recruitResult != 0) {
                    RedrawTownScreen();
                    gpWindowManager->SaveFizzleSource(0, 0x100,
                                                       0x228, 0xcc);
                    delete m_heroStrip;
                    sprintf(gText, "port%04d.icn",
                            m_recruitHero->m_portrait);
                    m_heroStrip = new strip(
                        0, 0x163, 3,
                        gpResourceManager->MakeId(gText, 1),
                        0, &m_recruitHero->m_army, 0x7a, 0, -1);
                    if (m_heroStrip == 0)
                        MemError();
                    buildSample_m = NULL_SAMPLE2;
                    buildSample_m = LoadPlaySample("buildtwn.82M");
                    hero *townHero = gpGame->GetHero(
                        m_town->m_occupyingHeroId);
                    i32 fizzleWidth = 0x228;
                    m_townWindow->DrawWindow(0);
                    m_garrisonStrip->DrawIcons(0);
                    m_heroStrip->DrawIcons(0);
                    gpWindowManager->FizzleForward(
                        0, 0x100, fizzleWidth, 0xcc, -1, 0, 0);
                    WaitEndSample(buildSample_m, -1);
                    m_recruitResult = 0;
                    gpWindowManager->ReleaseFizzleSource();
                } else {
                    if (m_selectedBuilding == 15 &&
                        m_town->m_occupyingHeroId == -1) {
                        if (m_heroStrip != 0)
                            delete m_heroStrip;
                        m_heroStrip = 0;
                        sprintf(gText, "port%04d.icn",
                                m_town->m_type + 0x5a);
                        m_heroStrip = new strip(
                            0, 0x163, 3,
                            gpResourceManager->MakeId(gText, 1),
                            0, 0, -1, 0, gpCurPlayer->m_color);
                        if (m_heroStrip == 0)
                            MemError();
                    }
                    RedrawTownScreen();
                    if (m_selectedBuilding != -1)
                        BuildObj(m_selectedBuilding);
                }
            }
            break;

        case TOWN_COMMAND_MAGE_GUILD:
            if (quickView_k) {
                goto showBuildingInformation;
            }
            {
                if (m_town->m_occupyingHeroId != -1 &&
                    !gpGame->GetHero(m_town->m_occupyingHeroId)
                         ->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                    if (gpGame->GetHero(m_town->m_occupyingHeroId)
                            ->NumArtifacts() == TOWN_MAX_ARTIFACTS) {
                        NormalDialog(
                            "You must purchase a spell book to use the mage guild, but you currently have no room for a spell book.  Try giving one of your artifacts to another hero.",
                            1, -1, -1, -1, 0, -1, 0, -1, 0);
                    } else if (gpCurPlayer->m_resources[RES_GOLD] <
                               TOWN_SPELL_BOOK_COST) {
                        NormalDialog(
                            "To cast spells, you must first buy a spell book for 500 gold.  Unfortunately, you seem to be a little short of cash at the moment.",
                            1, -1, -1, 7, ARTIFACT_MAGIC_BOOK,
                            -1, 0, -1, 0);
                    } else {
                        NormalDialog(
                            "To cast spells, you must first buy a spell book for 500 gold.  Do you wish to buy one?",
                            2, -1, -1, 7, ARTIFACT_MAGIC_BOOK,
                            -1, 0, -1, 0);
                        if (gpWindowManager->m_dialogResult ==
                            TOWN_DIALOG_BUY_SPELL_BOOK) {
                            GiveArtifact(
                                gpGame->GetHero(m_town->m_occupyingHeroId),
                                ARTIFACT_MAGIC_BOOK, 1, -1);
                            gpCurPlayer->m_resources[RES_GOLD] -=
                                TOWN_SPELL_BOOK_COST;
                            m_bankBox->Update(1);
                            m_townWindow->DrawWindow();
                            m_town->GiveSpells(0);
                        }
                    }
                } else {
                    m_heroWindow0 = new heroWindow(0, 0, "magewind.bin");
                    if (m_heroWindow0 == 0)
                        MemError();
                    SetWinText(m_heroWindow0, 17);
                    SetupMage(m_heroWindow0);
                    gpWindowManager->DoDialog(
                        m_heroWindow0, MageGuildHandler, 0);
                    delete m_heroWindow0;
                }
                m_town->GiveSpells(0);
                RedrawTownScreen();
            }
            break;

        case TOWN_COMMAND_WELL:
            if (quickView_k) {
                goto showBuildingInformation;
            }
            {
                m_heroWindow0 = new heroWindow(0, 0, "wellwind.bin");
                if (m_heroWindow0 == 0)
                    MemError();
                SetupWell(m_heroWindow0);
                gpWindowManager->DoDialog(
                    m_heroWindow0, TrueFalseDialogHandler, 0);
                delete m_heroWindow0;
                RedrawTownScreen();
            }
            break;

        case TOWN_COMMAND_THIEVES_GUILD:
            if (quickView_k) {
                goto showBuildingInformation;
            }
            {
                m_heroWindow0 = new heroWindow(0, 0, "thiefwin.bin");
                if (m_heroWindow0 == 0)
                    MemError();
                SetWinText(m_heroWindow0, 14);
                SetupThievesGuild(m_heroWindow0, -1);
                gpWindowManager->DoDialog(
                    m_heroWindow0, TrueFalseDialogHandler, 0);
                delete m_heroWindow0;
                RedrawTownScreen();
            }
            break;

        case TOWN_COMMAND_TAVERN:
            if (quickView_k) {
                goto showBuildingInformation;
            }
            {
                if (m_town->m_type == FACTION_NECROMANCER) {
                    sprintf(description_b,
                            GetBuildingInfo(m_town->m_type,
                                            message.payload.widget.id, 1));
                    NormalDialog(description_b, 1, -1, -1,
                                 m_town->m_type + 19, message.payload.widget.id,
                                 -1, 0, -1, 0);
                } else {
                    DoTavern();
                }
            }
            break;

        case TOWN_COMMAND_UPGRADE_CASTLE:
            if (quickView_k) {
                goto showBuildingInformation;
            }
            {
                if (m_town->m_unknown37 != 0) {
                    NormalDialog(
                        "This town may not be upgraded to a castle.",
                        1, -1, -1, -1, 0, -1, 0, -1, 0);
                } else {
                    if (BuyBuild(TOWN_COMMAND_CASTLE,
                                 CanBuy(m_town, TOWN_COMMAND_CASTLE) == 0,
                                 quickView_k))
                        BuildObj(TOWN_COMMAND_CASTLE);
                }
            }
            break;

        case TOWN_COMMAND_DOCK:
            if (quickView_k) {
                goto showBuildingInformation;
            }
            {
                gpWindowManager->BroadcastMessage(
                    MESSAGE_WIDGET, 5, TOWN_CONTROL_CLOSE,
                    TOWN_INTERFACE_BROADCAST_FLAGS);
                if (gpGame->GetBoatsBuilt() < TOWN_MAX_BOATS &&
                    gpAdvManager->GetCell(m_town->m_boatX,
                                          m_town->m_boatY)->m_triggerType == 0) {
                    m_heroWindow0 =
                        new heroWindow(0xb1, 0x14, "shipwind.bin");
                    if (m_heroWindow0 == 0)
                        MemError();
                    SetWinText(m_heroWindow0, 12);
                    if (gpGame->m_players[giCurPlayer].m_resources[RES_GOLD] <
                            TOWN_BOAT_GOLD_COST ||
                        gpGame->m_players[giCurPlayer].m_resources[RES_WOOD] <
                            TOWN_BOAT_WOOD_COST) {
                        message.type = MESSAGE_WIDGET;
                        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
                        message.payload.widget.id = TOWN_DIALOG_BUILD_BOAT;
                        message.payload.widget.data.value = 0x1000;
                        m_heroWindow0->BroadcastMessage(message);
                        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
                        message.payload.widget.data.value = 2;
                        m_heroWindow0->BroadcastMessage(message);
                    }
                    gpWindowManager->DoDialog(
                        m_heroWindow0, TrueFalseDialogHandler, 0);
                    delete m_heroWindow0;
                    if (gpWindowManager->m_dialogResult ==
                        TOWN_DIALOG_BUILD_BOAT) {
                        if (gpGame->CreateBoat(m_town->m_boatX,
                                               m_town->m_boatY, 0) != -1) {
                            BuildObj(14);
                            gpGame->m_players[giCurPlayer]
                                .m_resources[RES_GOLD] -= TOWN_BOAT_GOLD_COST;
                            gpGame->m_players[giCurPlayer]
                                .m_resources[RES_WOOD] -= TOWN_BOAT_WOOD_COST;
                            m_bankBox->Update(1);
                        } else {
                            unusedTownValue = 0;
                            LogStr("Can't create boat!");
                        }
                    }
                } else {
                    NormalDialog("Cannot build another boat.",
                                 1, 0xd0, 0x28, -1, 0, -1, 0, -1, 0);
                }
                gpWindowManager->BroadcastMessage(
                    MESSAGE_WIDGET, 6, TOWN_CONTROL_CLOSE,
                    TOWN_INTERFACE_BROADCAST_FLAGS);
            }
            break;

        case TOWN_COMMAND_TRADING_POST:
            if (quickView_k) {
                goto showBuildingInformation;
            }
            {
                marketplaceCount_m = 0;
                for (index_i = 0; index_i < gpCurPlayer->m_townCount; ++index_i) {
                    if (gpGame->GetTown(gpCurPlayer->TownId(index_i))
                            ->m_buildings & TOWN_BUILDING_MARKETPLACE)
                        ++marketplaceCount_m;
                }
                if (marketplaceCount_m > 10)
                    marketplaceCount_m = 10;
                DoTradingPost(1, fTradingPostEfficency[marketplaceCount_m]);
                RedrawTownScreen();
            }
            break;

        case 7:
        case 8:
        case 9:
        case 11:
        case 12:
        case 13:
        case 15:
        showBuildingInformation:
            if (quickView_k) {
                sprintf(description_b,
                        GetBuildingInfo(m_town->m_type,
                                        message.payload.widget.id, 1));
                NormalDialog(description_b, 4, -1, -1,
                             m_town->m_type + 19, message.payload.widget.id,
                             -1, 0, -1, 0);
            } else {
                sprintf(description_b,
                        GetBuildingInfo(m_town->m_type, message.payload.widget.id, 1));
                NormalDialog(description_b, 1, -1, -1,
                             m_town->m_type + 19, message.payload.widget.id,
                             -1, 0, -1, 0);
            }
            break;

        case TOWN_CONTROL_CLOSE:
            if (!quickView_k)
                SetCommandAndText(message);
            break;

        default:
            if (quickView_k) {
                i32 armySelected = 0;
                hero *viewedHero;
                if (message.payload.widget.id >= TOWN_GARRISON_SLOT_FIRST &&
                    message.payload.widget.id <= TOWN_GARRISON_SLOT_LAST) {
                    m_selectedStrip = m_garrisonStrip;
                    m_selectedArmySlot =
                        message.payload.widget.id - TOWN_GARRISON_SLOT_FIRST;
                    armySelected = 1;
                }
                if (message.payload.widget.id >= TOWN_HERO_SLOT_FIRST &&
                    message.payload.widget.id <= TOWN_HERO_SLOT_LAST) {
                    m_selectedStrip = m_heroStrip;
                    m_selectedArmySlot =
                        message.payload.widget.id - TOWN_HERO_SLOT_FIRST;
                    armySelected = 1;
                }
                if (armySelected &&
                    m_selectedStrip->m_army
                            ->m_creatureTypes[m_selectedArmySlot] != -1) {
                    if (m_selectedStrip == m_heroStrip)
                        viewedHero = gpGame->GetHero(
                            m_town->m_occupyingHeroId);
                    else
                        viewedHero = 0;
                    gpGame->ViewArmy(
                        TOWN_ARMY_VIEW_X, TOWN_ARMY_VIEW_Y,
                        m_selectedStrip->m_army
                            ->m_creatureTypes[m_selectedArmySlot],
                        m_selectedStrip->m_army
                            ->m_creatureCounts[m_selectedArmySlot],
                        m_town, 1, 1, 1, viewedHero, 0,
                        m_selectedStrip->m_army, m_selectedArmySlot);
                    m_bankBox->Update(1);
                }
            } else {
                DoCommand(m_command);
                SetCommandAndText(message);
            }
            break;
        }
        break;

        case TOWN_INPUT_DESELECT:
            switch (message.payload.widget.id) {
            case TOWN_CONTROL_CLOSE:
                if (quickView_k)
                    break;
                ++exitTown_i;
                break;

            case TOWN_CONTROL_PREVIOUS_TOWN:
            case TOWN_CONTROL_NEXT_TOWN:
                if (m_town->m_owner != giCurPlayer)
                    break;
                if (gpCurPlayer->m_townCount <= 1)
                    break;
                {
                    i32 townPosition =
                        gpGame->TownIDToTownPos(gpCurPlayer, m_town->m_id);
                    townPosition = (townPosition +
                                    (message.payload.widget.id == TOWN_CONTROL_PREVIOUS_TOWN
                                         ? -1
                                         : 1) +
                                    gpCurPlayer->m_townCount) %
                                   gpCurPlayer->m_townCount;
                    m_town = gpGame->GetTown(
                        gpCurPlayer->TownId(townPosition));
                    ChangeTown();
                }
                break;
            }
            break;
        }
        break;

    case MESSAGE_MOUSE_MOVE:
        gpWindowManager->ConvertToHover(message);
        if (message.payload.hover.id == m_lastHoverId &&
            message.payload.hover.modifiers == m_lastHoverSubId)
            return 1;
        m_lastHoverId = message.payload.hover.id;
        m_lastHoverSubId = message.payload.hover.modifiers;
        SetCommandAndText(message);
        return 1;

    case MESSAGE_KEY_UP:
        switch (message.payload.keyboard.keyCode) {
        case TOWN_KEY_SHIFT_LEFT:
        case TOWN_KEY_SHIFT_RIGHT:
            ShiftQualChange();
            break;
        }
        break;

    case MESSAGE_KEY_DOWN:
        switch (message.payload.keyboard.keyCode) {
        case TOWN_KEY_SHIFT_LEFT:
        case TOWN_KEY_SHIFT_RIGHT:
            ShiftQualChange();
            break;

        case 1:
            ++exitTown_i;
            break;
        }
        break;
    }

    if (exitTown_i == 1) {
        message.type = MESSAGE_EXECUTIVE;
        message.payload.widget.command = 1;
        return 2;
    }
    return 1;
}

// @semantic: Exact 0x4e3 size, 0x18 frame, all six command bodies/CFG,
// retail -0x4/-0x8/-0xc/-0x10 local slots, and 23/23 ordered relocation owners
// and addends. Fifteen of sixteen external relocation offsets are exact; the
// first gpGame site is five bytes late because the inlined GetHero continuation
// is before its body here but after it in retail. Relocation-masked raw proof
// leaves 90 bytes in the evaluation-order spans 0x6b-0xa3, 0xc7-0xce,
// 0x207-0x233, 0x2ba-0x2e7, 0x32f-0x346, and 0x37b-0x3cf. Both hero-strip
// equality orders compile identically. Combined-condition and compound-add
// forms were worse; the explicit loop break and read/add/write are retail.
// A 295-candidate value-neutral AST run found no exact-size improvement; its
// 98.9088% best adds a spurious five-byte m_dialogResult inline continuation.
VA(0x0041718d, 0x4e3)
void townManager::DoCommand(i32 command)
{
    hero *viewedHero;
    i32 dismissAllowed;
    i32 slot;
    i32 oldValue;

    switch (command) {
    case TOWN_ARMY_COMMAND_SELECT:
        m_swapStrip = m_selectedStrip;
        m_swapArmySlot = m_selectedArmySlot;
        m_swapStrip->m_selectedSlot = m_swapArmySlot;
        m_swapStrip->Draw();
        break;

    case TOWN_ARMY_COMMAND_VIEW:
        if (m_selectedStrip == m_heroStrip)
            viewedHero = gpGame->GetHero(m_town->m_occupyingHeroId);
        else
            viewedHero = 0;
        if (m_castleDialogActive == 1 ||
            (m_selectedStrip == m_heroStrip &&
             m_selectedStrip->m_army->GetNumArmies() == 1))
            dismissAllowed = 1;
        else
            dismissAllowed = 0;
        gpGame->ViewArmy(
            TOWN_ARMY_VIEW_X, TOWN_ARMY_VIEW_Y,
            m_selectedStrip->m_army->m_creatureTypes[m_selectedArmySlot],
            m_selectedStrip->m_army->m_creatureCounts[m_selectedArmySlot],
            m_town, dismissAllowed, 1, 0, viewedHero, 0,
            m_selectedStrip->m_army, m_selectedArmySlot);
        m_bankBox->Update(1);
        if (gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM) {
            m_selectedStrip->m_army
                ->m_creatureTypes[m_selectedArmySlot] = -1;
            m_selectedStrip->m_army
                ->m_creatureCounts[m_selectedArmySlot] = 0;
        }
        ResetStrips();
        break;

    case TOWN_ARMY_COMMAND_MERGE:
        if (m_pendingStrip != m_swapStrip &&
            m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] !=
                m_pendingStrip->m_army
                    ->m_creatureTypes[m_pendingArmySlot]) {
            for (slot = 0; slot < TOWN_ARMY_SLOT_COUNT; ++slot) {
                if (m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] ==
                    m_pendingStrip->m_army->m_creatureTypes[slot])
                    break;
            }
            if (slot < TOWN_ARMY_SLOT_COUNT)
                m_pendingArmySlot = slot;
        }
        m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] =
            m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] +
            m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot];
        m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] = -1;
        m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] = 0;
        ResetStrips();
        break;

    case TOWN_ARMY_COMMAND_SWAP:
        oldValue = m_pendingStrip->m_army
                        ->m_creatureCounts[m_pendingArmySlot];
        m_pendingStrip->m_army->m_creatureCounts[m_pendingArmySlot] =
            m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot];
        m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] = oldValue;
        oldValue = m_pendingStrip->m_army
                        ->m_creatureTypes[m_pendingArmySlot];
        m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot] =
            m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot];
        m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] = oldValue;
        ResetStrips();
        break;

    case TOWN_ARMY_COMMAND_VIEW_HERO:
        HeroView(m_town->m_occupyingHeroId, 1, 0);
        RedrawTownScreen();
        gpWindowManager->FadeScreen(0, 8, 0);
        break;

    case TOWN_ARMY_COMMAND_SPLIT:
        SplitArmy();
        ResetStrips();
        break;
    }
    m_lastHoverId = -1;
}

VA(0x00417670, 0xad)
void townManager::RedrawTownScreen(void)
{
    tag_message message;

    DrawTown(0, 1);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = TOWN_CONTROL_STATUS_TEXT;
    message.payload.widget.data.text = m_statusText;
    m_townWindow->BroadcastMessage(message);
    m_townWindow->DrawWindow(0);
    m_garrisonStrip->DrawIcons(0);
    m_heroStrip->DrawIcons(0);
    m_bankBox->Update(0);
    gpWindowManager->UpdateScreenRegion(0, 0, 0x280, 0x1e0);
}

// @early-stop
// Relocation-masked instruction streams are identical with the exact 0x40
// frame and 27/27 relocations; only delinked string-literal symbol identities
// differ (splitwin.bin, prompt/army labels, and numeric format).
VA(0x0041771d, 0x374)
void townManager::SplitArmy(void)
{
    i16 unusedValue = 1;
    i16 unusedDialogValue = 4;
    i32 sameCreature;
    tag_message message;

    m_heroWindow1 = new heroWindow(0xb1, 0x14, "splitwin.bin");
    if (m_heroWindow1 == 0)
        MemError();
    m_splitAmount = 0;
    m_splitMaximum = m_swapStrip->m_army
                         ->m_creatureCounts[m_swapArmySlot];
    message.type = MESSAGE_WIDGET;
    if (m_pendingStrip->m_army == m_swapStrip->m_army) {
        sprintf(gText, "Move how many troops?");
    } else {
        sprintf(gText, "Move how many %s troops from %s to %s?",
                gArmyNames[m_swapStrip->m_army
                               ->m_creatureTypes[m_swapArmySlot]],
                m_swapStrip == m_heroStrip ? "Hero's Army" : "Garrison",
                m_pendingStrip == m_heroStrip ? "Hero's Army" : "Garrison");
    }
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = 1;
    message.payload.widget.data.text = gText;
    m_heroWindow1->BroadcastMessage(message);
    sprintf(gText, "%d", m_splitAmount);
    message.payload.widget.id = 4;
    message.payload.widget.data.text = gText;
    m_heroWindow1->BroadcastMessage(message);
    gpWindowManager->DoDialog(m_heroWindow1, SplitArmyHandler, 0);
    delete m_heroWindow1;
    if (gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM) {
        sameCreature = 0;
        if (m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot] ==
            m_pendingStrip->m_army->m_creatureTypes[m_pendingArmySlot])
            sameCreature = 1;
        if (sameCreature != 0) {
            m_pendingStrip->m_army
                ->m_creatureCounts[m_pendingArmySlot] +=
                m_splitAmount;
        } else {
            m_pendingStrip->m_army
                ->m_creatureTypes[m_pendingArmySlot] =
                m_swapStrip->m_army->m_creatureTypes[m_swapArmySlot];
            m_pendingStrip->m_army
                ->m_creatureCounts[m_pendingArmySlot] =
                m_splitAmount;
        }
        m_swapStrip->m_army->m_creatureCounts[m_swapArmySlot] -=
            m_splitAmount;
    }
}

VA(0x00417a91, 0x21)
void townManager::ShiftQualChange(void)
{
    gpInputManager->ForceMouseMove();
}

VA(0x00417ab2, 0xb0)
void townManager::ResetStrips(void)
{
    if (m_swapStrip != 0)
        m_swapStrip->m_selectedSlot = -1;
    if (m_pendingStrip != 0)
        m_pendingStrip->m_selectedSlot = -1;
    m_heroStrip->Draw();
    m_garrisonStrip->Draw();
    m_swapStrip = m_pendingStrip = 0;
    m_swapArmySlot = m_pendingArmySlot = -1;
}

VA(0x00417b62, 0x7b)
void townManager::Toggle(i32 building)
{
    i32 index;

    if (m_town->m_buildings & (1L << building)) {
        for (index = 0; index < m_townObjectCount; ++index) {
            if (m_townObjects[index]->m_buildingId == building)
                m_townObjects[index]->m_visible ^= 1;
        }
    }
}

VA(0x00417bdd, 0xc0)
void townManager::DrawTown(i32 updateScreen, i32 drawFlags)
{
    i32 index;

    PollSound();
    m_backgroundIcon->DrawToBuffer(0, 0, 0, 0);
    PollSound();
    for (index = 0; index < m_townObjectCount; ++index) {
        m_townObjects[index]->Draw(drawFlags);
        PollSound();
    }
    m_townWindow->DrawWindow(0, 0x88, 0x89);
    PollSound();
    if (updateScreen != 0)
        BlitBitmapToScreen(gpWindowManager->m_screen,
                           0, 0, 0x280, 0x100, 0, 0);
    PollSound();
}

// @early-stop 99.29%: exact 0xf35 span, 801 instructions, 0x14c frame, every
// visible slot/CFG edge, and all 102/102 relocation sites and targets agree.
// The 54 unmasked bytes are five value-neutral evaluation-order spans:
// +0x2f5..+0x303 and +0x326..+0x334 form neutral-cost addresses index-first
// versus building-first; +0xb98..+0xb9f, +0xc18..+0xc1f and +0xca1..+0xca8
// reverse commutative widget-offset/spacing loads. Operand/subscript reversal
// was byte-neutral, flattening shortened both neutral accesses, and bounded
// 31-name index plus 32-name offset AST searches retained no gain.
VA(0x00417c9d, 0xf35)
i32 townManager::BuyBuild(i32 building, i32 cannotBuy, i32 quickView)
{
    static i16 sourceLineBase = 0x0948;
    u32l prerequisiteMask_c;
    i32 prerequisiteCount_p;
    i16 dialogLeft_a;
    i32 mageLevel_k;
    char *amountText_n[TOWN_RESOURCE_COUNT];
    i32 windowY_m;
    i32 bottomRowCount_o;
    i32 resourcesInRow_l;
    i32 resourceCount_a;
    i16 dialogWidth_e;
    i32 entryWidth_o;
    i32 costCount_o;
    i16 dialogButtonCount_m;
    iconWidget *resourceWidgets_m[TOWN_RESOURCE_COUNT];
    i32 xStart_b;
    i32 index_h;
    widget *descriptionWidget_g;
    i32 windowHeight_a;
    i16 costs_e[8];
    i32 dwelling_k;
    i32 topRowCount_c;
    tag_message message_m;
    i8 resourceTypes_o[8];
    i32 windowRows_b;
    i32 row_l;
    icon *resourceIcon_c;
    i16 dialogControl_g;
    i16 dialogResult_b;
    i32 rowWidth_h;
    i32 lineCount_j;
    heroWindow *window_a;
    i16 dialogHeight_f;
    i16 dialogButtonWidth_l;
    textWidget *amountWidgets_b[TOWN_RESOURCE_COUNT];
    i32 rowResourceTypes_a[4];
    i32 x_d;
    i32 spacing_h;
    i32 rowY_o;
    char iconName_o[16];
    i32 widgetIndex_f;
    char *description_b;

    mageLevel_k = 0;
    index_h = 0;
    costCount_o = 0;
    description_b = static_cast<char *>(BaseAlloc(
        400, "I:\\Projects\\Heroes\\Prog\\SOURCE\\TOWNMGR.CPP",
        sourceLineBase + 8));

    for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
        costs_e[index_h] = -1;
        resourceTypes_o[index_h] = static_cast<i8>(costs_e[index_h]);
    }

    dwelling_k = -1;
    if (building >= 19 && building <= 30)
        dwelling_k = building - 19;

    if (building == TOWN_COMMAND_TAVERN &&
        m_town->m_type == FACTION_NECROMANCER) {
        for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
            if (xShrineBuildingCost[index_h] > 0) {
                resourceTypes_o[costCount_o] = static_cast<i8>(index_h);
                costs_e[costCount_o] = static_cast<i16>(xShrineBuildingCost[index_h]);
                ++costCount_o;
            }
        }
    } else if (building == TOWN_COMMAND_MAGE_GUILD) {
        mageLevel_k = gpTownManager->m_town->m_buildState;
        for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
            if (gMageBuildingCosts[
                    mageLevel_k + 1 < TOWN_MAGE_GUILD_MAX_LEVEL
                        ? mageLevel_k + 1
                        : TOWN_MAGE_GUILD_MAX_LEVEL][index_h] > 0) {
                resourceTypes_o[costCount_o] = static_cast<i8>(index_h);
                costs_e[costCount_o] = static_cast<i16>(
                    gMageBuildingCosts[
                        mageLevel_k + 1 < TOWN_MAGE_GUILD_MAX_LEVEL
                            ? mageLevel_k + 1
                            : TOWN_MAGE_GUILD_MAX_LEVEL][index_h]);
                ++costCount_o;
            }
        }
    } else if (building == TOWN_COMMAND_SPECIAL_BUILDING) {
        for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
            if (gSpecialBuildingCosts[gpTownManager->m_town->m_type][index_h] > 0) {
                resourceTypes_o[costCount_o] = static_cast<i8>(index_h);
                costs_e[costCount_o] = static_cast<i16>(
                    gSpecialBuildingCosts[gpTownManager->m_town->m_type][index_h]);
                ++costCount_o;
            }
        }
    } else if (building <= TOWN_COMMAND_LAST_NEUTRAL_BUILDING) {
        for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
            if (gNeutralBuildingCosts[building][index_h] > 0) {
                resourceTypes_o[costCount_o] = static_cast<i8>(index_h);
                costs_e[costCount_o] = static_cast<i16>(
                    gNeutralBuildingCosts[building][index_h]);
                ++costCount_o;
            }
        }
    } else {
        for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
            if (gDwellingCosts[gpTownManager->m_town->m_type]
                              [dwelling_k][index_h] > 0) {
                resourceTypes_o[costCount_o] = static_cast<i8>(index_h);
                costs_e[costCount_o] = static_cast<i16>(
                    gDwellingCosts[gpTownManager->m_town->m_type]
                                  [dwelling_k][index_h]);
                ++costCount_o;
            }
        }
    }

    dialogWidth_e = 0x50;
    dialogHeight_f = 0x28;
    dialogLeft_a = 0x20;
    dialogControl_g = 0x121;
    dialogResult_b = 0;
    dialogButtonWidth_l = 2;
    dialogButtonCount_m = 3;
    widgetIndex_f = 0;
    resourceCount_a = 0;
    topRowCount_c = 0;
    bottomRowCount_o = 0;

    for (index_h = 0; index_h < TOWN_RESOURCE_COUNT; ++index_h) {
        if (resourceTypes_o[index_h] != -1)
            ++resourceCount_a;
    }
    if (resourceCount_a <= 4) {
        topRowCount_c = resourceCount_a;
    } else if (resourceCount_a == 5) {
        topRowCount_c = 2;
        bottomRowCount_o = 3;
    } else if (resourceCount_a == 6) {
        topRowCount_c = 3;
        bottomRowCount_o = 3;
    } else if (resourceCount_a == TOWN_RESOURCE_COUNT) {
        topRowCount_c = 3;
        bottomRowCount_o = 4;
    }

    sprintf(description_b, GetBuildingInfo(m_town->m_type, building, 0));
    if (dwelling_k >= 0) {
        prerequisiteCount_p = 0;
        prerequisiteMask_c = gHierarchyMask[m_town->m_type][dwelling_k];
        for (index_h = 0; index_h < 32; ++index_h) {
            if (prerequisiteMask_c & (1L << index_h)) {
                if (prerequisiteCount_p == 0)
                    strcat(description_b, "\n\nRequires:");
                ++prerequisiteCount_p;
                strcat(description_b, "\n");
                strcat(description_b, GetBuildingName(m_town->m_type, index_h));
            }
        }
        if (m_town->m_type == FACTION_NECROMANCER &&
            building == TOWN_COMMAND_NECROMANCER_MAGE_GUILD_PREREQUISITE &&
            m_town->m_buildState <= 2)
            strcat(description_b, "\nLevel 2 Mage Guild");
    }
    strcat(description_b, "\n ");

    lineCount_j = bigFont->LineLength(description_b, 0xf0);
    windowY_m = 0x97;
    windowHeight_a = windowY_m;
    windowHeight_a += lineCount_j << 4;
    if (resourceCount_a <= 4)
        windowHeight_a += 0x2c;
    else
        windowHeight_a += 0x58;
    if (quickView == 0)
        windowHeight_a += 0x27;
    windowRows_b = (windowHeight_a - 0x45) / 0x2d;
    if (windowRows_b < 3)
        windowRows_b = 3;
    if (windowRows_b > 6)
        windowRows_b = 6;

    sprintf(gText, "buybuil%d.bin", windowRows_b);
    window_a = new heroWindow(0x9e, 0x10, gText);
    if (window_a == 0)
        MemError();

    message_m.type = MESSAGE_WIDGET;
    message_m.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    message_m.payload.widget.id = 2;
    sprintf(iconName_o, "cstl%s.icn", cHeroTypeShortName[m_town->m_type]);
    message_m.payload.widget.data.text = iconName_o;
    window_a->BroadcastMessage(message_m);
    message_m.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
    message_m.payload.widget.id = 2;
    message_m.payload.widget.data.value = building;
    window_a->BroadcastMessage(message_m);

    if (building == TOWN_COMMAND_MAGE_GUILD) {
        sprintf(gText, "Mage Guild, Level %d",
                mageLevel_k + 1 < TOWN_MAGE_GUILD_MAX_LEVEL
                    ? mageLevel_k + 1
                    : TOWN_MAGE_GUILD_MAX_LEVEL);
    } else {
        strcpy(gText, GetBuildingName(m_town->m_type, building));
    }
    message_m.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message_m.payload.widget.id = 3;
    message_m.payload.widget.data.text = gText;
    window_a->BroadcastMessage(message_m);

    descriptionWidget_g = new textWidget(
        0x2b, static_cast<i16>(windowY_m + 0x18), 0xf0,
        static_cast<i16>(lineCount_j << 4), description_b, "bigfont.fnt",
        1, -1, 8, 1);
    if (descriptionWidget_g == 0)
        MemError();
    window_a->AddWidget(descriptionWidget_g, -1);

    widgetIndex_f = 0;
    resourceIcon_c = gpResourceManager->GetIcon("resource.icn");
    for (row_l = 0; row_l < 2; ++row_l) {
        rowY_o = row_l * 0x2c + lineCount_j * 0x10 + windowY_m + 0xc;
        if (row_l == 0)
            resourcesInRow_l = topRowCount_c;
        else
            resourcesInRow_l = bottomRowCount_o;
        if (resourcesInRow_l > 0) {
            rowWidth_h = 0;
            costCount_o = widgetIndex_f;
            for (index_h = 0; index_h < 4; ++index_h) {
                if (index_h < resourcesInRow_l) {
                    while (resourceTypes_o[costCount_o] == -1)
                        ++costCount_o;
                    rowResourceTypes_a[index_h] = resourceTypes_o[costCount_o];
                    ++costCount_o;
                } else {
                    rowResourceTypes_a[index_h] = -1;
                }
            }
            for (index_h = 0; index_h < resourcesInRow_l; ++index_h) {
                rowWidth_h += GetIconEntry(
                    resourceIcon_c, rowResourceTypes_a[index_h])->w;
            }
            spacing_h = (0x100 - rowWidth_h) / (resourcesInRow_l + 1);
            xStart_b = spacing_h + 0x20;
            x_d = xStart_b;
            for (index_h = 0; index_h < resourcesInRow_l; ++index_h) {
                entryWidth_o = GetIconEntry(
                    resourceIcon_c, rowResourceTypes_a[index_h])->w;
                amountText_n[widgetIndex_f] = static_cast<char *>(BaseAlloc(
                    10, "I:\\Projects\\Heroes\\Prog\\SOURCE\\TOWNMGR.CPP",
                    sourceLineBase + 0x128));
                sprintf(amountText_n[widgetIndex_f], "%d", costs_e[widgetIndex_f]);
                i32 widgetXOffset = 0;
                amountWidgets_b[widgetIndex_f] = new textWidget(
                    static_cast<i16>(x_d + widgetXOffset),
                    static_cast<i16>(rowY_o + 0x23),
                    static_cast<i16>(entryWidth_o), 0xc,
                    amountText_n[widgetIndex_f], "smalfont.fnt", 1, -1, 8, 1);
                if (amountWidgets_b[widgetIndex_f] == 0)
                    MemError();
                resourceWidgets_m[widgetIndex_f] = new iconWidget(
                    static_cast<i16>(
                        x_d + widgetXOffset -
                        GetIconEntry(resourceIcon_c,
                                     rowResourceTypes_a[index_h])->x),
                    static_cast<i16>(rowY_o),
                    static_cast<i16>(entryWidth_o), 0xc, "resource.icn",
                    resourceTypes_o[widgetIndex_f], 0, -1, 0x10, 1);
                if (resourceWidgets_m[widgetIndex_f] == 0)
                    MemError();
                window_a->AddWidget(amountWidgets_b[widgetIndex_f], -1);
                window_a->AddWidget(resourceWidgets_m[widgetIndex_f], -1);
                ++widgetIndex_f;
                x_d += spacing_h + entryWidth_o;
            }
        }
    }
    gpResourceManager->Dispose(resourceIcon_c);

    if (quickView == 0)
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET, 5, TOWN_CONTROL_CLOSE,
            TOWN_INTERFACE_BROADCAST_FLAGS);
    m_selectedBuilding = -1;
    if (quickView != 0) {
        message_m.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message_m.payload.widget.data.value = 6;
        message_m.payload.widget.id = TOWN_DIALOG_CONFIRM;
        window_a->BroadcastMessage(message_m);
        message_m.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message_m.payload.widget.data.value = 6;
        message_m.payload.widget.id = 0x7801;
        window_a->BroadcastMessage(message_m);
        message_m.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message_m.payload.widget.data.value = 6;
        message_m.payload.widget.id = 0;
        window_a->BroadcastMessage(message_m);
        gpWindowManager->AddWindow(window_a, -1, 1);
        QuickViewWait();
        gpWindowManager->RemoveWindow(window_a);
    } else {
        if (cannotBuy != 0) {
            message_m.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message_m.payload.widget.id = TOWN_DIALOG_CONFIRM;
            message_m.payload.widget.data.value = 2;
            window_a->BroadcastMessage(message_m);
            message_m.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message_m.payload.widget.id = TOWN_DIALOG_CONFIRM;
            message_m.payload.widget.data.value = 0x1000;
            window_a->BroadcastMessage(message_m);
        }
        gpWindowManager->DoDialog(window_a, TrueFalseDialogHandler, 0);
        if (gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM) {
            m_selectedBuilding = building;
            for (index_h = 0; index_h < resourceCount_a; ++index_h)
                gpCurPlayer->m_resources[resourceTypes_o[index_h]] -= costs_e[index_h];
        }
    }
    if (quickView == 0)
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET, 6, TOWN_CONTROL_CLOSE,
            TOWN_INTERFACE_BROADCAST_FLAGS);
    delete window_a;
    if (quickView != 0) {
        return 0;
    } else {
        return gpWindowManager->m_dialogResult == TOWN_DIALOG_CONFIRM;
    }
}

VA(0x00418bd2, 0x3e9)
void townManager::BuildObj(i32 building)
{
    i32 objectIndex_k;
    SLimitData limits_h;
    i32 index_j;
    SAMPLE2 buildSample_b;
    i32 frame_g;

    if ((m_town->m_buildings & (1L << building)) &&
        (building != TOWN_COMMAND_MAGE_GUILD ||
         m_town->m_buildState == TOWN_MAGE_GUILD_MAX_LEVEL)) {
        return;
    }
    if (building == TOWN_COMMAND_DOCK && !m_town->CanBuildDock()) {
        return;
    }
    {
        DrawTown(1, 1);
        m_town->BuildBuilding(building);
        for (index_j = 0; index_j < m_townObjectCount; ++index_j) {
            if (m_town->m_buildings &
                (1L << m_townObjects[index_j]->m_buildingId)) {
                m_townObjects[index_j]->m_visible = 1;
                m_townObjects[index_j]->m_border->m_flags |=
                    TOWN_OBJECT_BORDER_ENABLED;
            } else {
                m_townObjects[index_j]->m_visible = 0;
                m_townObjects[index_j]->m_border->m_flags &=
                    ~TOWN_OBJECT_BORDER_ENABLED;
            }
        }

        objectIndex_k = -1;
        for (index_j = 0; index_j < m_townObjectCount; ++index_j) {
            if (m_townObjects[index_j]->m_buildingId == building)
                objectIndex_k = index_j;
        }

        giMaxExtentY = 0;
        giMaxExtentX = giMaxExtentY;
        giMinExtentX = 0x27f;
        giMinExtentY = 0xff;
        gbComputeExtent = 1;
        gbSaveBiggestExtent = 1;
        gbReturnAfterComputeExtent = 1;
        if (building == TOWN_COMMAND_MAGE_GUILD) {
            if (gpTownManager->m_town->m_type == FACTION_NECROMANCER)
                frame_g = (gpTownManager->m_town->m_buildState - 1) * 3 * 2;
            else
                frame_g = gpTownManager->m_town->m_buildState - 1;
            m_townObjects[objectIndex_k]->m_icon->CombatClipDrawToBuffer(
                0, 0, frame_g, &limits_h, 0, 0, 0, 0);
            if (m_townObjects[objectIndex_k]->m_animationFrameCount != 0)
                m_townObjects[objectIndex_k]->m_icon->CombatClipDrawToBuffer(
                    0, 0, frame_g + 1, &limits_h, 0, 0, 0, 0);
        } else {
            m_townObjects[objectIndex_k]->m_icon->CombatClipDrawToBuffer(
                0, 0, 0, &limits_h, 0, 0, 0, 0);
            if (m_townObjects[objectIndex_k]->m_animationFrameCount != 0)
                m_townObjects[objectIndex_k]->m_icon->CombatClipDrawToBuffer(
                    0, 0, 1, &limits_h, 0, 0, 0, 0);
        }
        gbComputeExtent = 0;
        gbSaveBiggestExtent = 0;
        gbReturnAfterComputeExtent = 0;

        gpWindowManager->SaveFizzleSource(
            giMinExtentX, giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1);
        DrawTown(0, 1);
        buildSample_b = NULL_SAMPLE2;
        buildSample_b = LoadPlaySample("buildtwn.82M");
        gpWindowManager->FizzleForward(
            giMinExtentX, giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1, -1, 0, 0);
        WaitEndSample(buildSample_b, -1);
        PollSound();
        m_selectedBuilding = -1;
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET, 6, TOWN_CONTROL_CLOSE,
            TOWN_INTERFACE_BROADCAST_FLAGS);
        RedrawTownScreen();
    }
}

// @semantic: First raw divergence is the frame byte at +0x5: base 0x5c versus
// retail 0x60. Message(-0x3c), loop/condition CFG, and all 34 ordered relocations
// agree, but retail reserves an unreferenced word at -0x48 and consequently has
// hasLibrary/this at -0x5c/-0x60 versus -0x58/-0x5c. Flattened and typed-row
// spellings and reversed guild-frame assignment order were worse; no padding
// declaration was invented. Revisit only after compiler state, layout evidence,
// or the source/TU/header comparison epoch changes.
VA(0x00418fbb, 0x3d8)
void townManager::SetupMage(heroWindow *window)
{
    i16 unusedZero_f = 0;
    i16 unusedAvailable_j = 1;
    i16 unusedInvalid_c = TOWN_MAGE_SPELL_UNAVAILABLE;
    i16 unusedIconState_h = 2;
    i16 unusedFirstSpell_p = TOWN_MAGE_FIRST_SPELL_CONTROL;
    i16 unusedFirstIcon_m = TOWN_MAGE_FIRST_ICON_CONTROL;
    i16 unusedFirstDescription_o = TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
    i16 unusedGuildIcon_i = TOWN_MAGE_GUILD_ICON_CONTROL;
    i16 unusedDescription_g = TOWN_MAGE_DESCRIPTION_CONTROL;
    tag_message message_b;
    i32 level;
    i32 slot_m;
    i32 spellState_m;
    i32 hasLibrary_k;
    i32 lineCount_m;
    i32 unusedGuildFrame_n;

    message_b.type = MESSAGE_WIDGET;
    if (m_town->m_occupyingHeroId == -1) {
        strcpy(gText, "The above spells are available here.");
        message_b.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        message_b.payload.widget.id = TOWN_MAGE_DESCRIPTION_CONTROL;
        message_b.payload.widget.data.text = gText;
        window->BroadcastMessage(message_b);
    }

    for (level = 0; level < TOWN_MAGE_GUILD_MAX_LEVEL; ++level) {
        for (slot_m = 0; slot_m < TOWN_MAGE_SPELLS_PER_LEVEL; ++slot_m) {
            if (m_town->m_type == FACTION_WIZARD &&
                (m_town->m_buildings & TOWN_WIZARD_LIBRARY_BUILDING_FLAG))
                hasLibrary_k = 1;
            else
                hasLibrary_k = 0;

            if (gSpellLimits[level] + hasLibrary_k <= slot_m) {
                spellState_m = TOWN_MAGE_SPELL_UNAVAILABLE;
            } else if (m_town->m_spellCounts[level + 1] <= slot_m) {
                spellState_m = 1;
            } else {
                spellState_m = 0;
            }

            if (spellState_m == TOWN_MAGE_SPELL_UNAVAILABLE)
                message_b.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            else
                message_b.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message_b.payload.widget.id = level * TOWN_MAGE_SPELLS_PER_LEVEL + slot_m +
                             TOWN_MAGE_FIRST_SPELL_CONTROL;
            message_b.payload.widget.data.value = 4;
            window->BroadcastMessage(message_b);

            if (spellState_m != TOWN_MAGE_SPELL_UNAVAILABLE) {
                message_b.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
                message_b.payload.widget.id = level * TOWN_MAGE_SPELLS_PER_LEVEL + slot_m +
                                 TOWN_MAGE_FIRST_SPELL_CONTROL;
                message_b.payload.widget.data.value = spellState_m;
                window->BroadcastMessage(message_b);
            }

            if (spellState_m != 0) {
                message_b.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
                message_b.payload.widget.data.value = 4;
                message_b.payload.widget.id = level * TOWN_MAGE_SPELLS_PER_LEVEL + slot_m +
                                 TOWN_MAGE_FIRST_ICON_CONTROL;
                window->BroadcastMessage(message_b);
                message_b.payload.widget.id = level * TOWN_MAGE_SPELLS_PER_LEVEL + slot_m +
                                 TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
                window->BroadcastMessage(message_b);
            } else {
                message_b.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
                message_b.payload.widget.id = level * TOWN_MAGE_SPELLS_PER_LEVEL + slot_m +
                                 TOWN_MAGE_FIRST_ICON_CONTROL;
                message_b.payload.widget.data.value = gsSpellInfo[
                    m_town->m_spells[level][slot_m]].iconIndex;
                window->BroadcastMessage(message_b);
                lineCount_m = smallFont->LineLength(
                    gSpellNames[m_town->m_spells[level][slot_m]], 0x4a);
                if (lineCount_m == 1)
                    sprintf(gText, "%s\n[%d]",
                            gSpellNames[m_town->m_spells[level][slot_m]],
                            GetManaCost(m_town->m_spells[level][slot_m], 0));
                else
                    sprintf(gText, "%s  [%d]",
                            gSpellNames[m_town->m_spells[level][slot_m]],
                            GetManaCost(m_town->m_spells[level][slot_m], 0));
                message_b.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
                message_b.payload.widget.id = level * TOWN_MAGE_SPELLS_PER_LEVEL + slot_m +
                                 TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
                message_b.payload.widget.data.text = gText;
                window->BroadcastMessage(message_b);
            }
        }
    }

    message_b.payload.widget.data.value = m_town->m_buildState - 1;
    message_b.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
    message_b.payload.widget.id = TOWN_MAGE_GUILD_ICON_CONTROL;
    unusedGuildFrame_n = message_b.payload.widget.data.value;
    window->BroadcastMessage(message_b);
    sprintf(gText, "magegld%c.icn", cHeroTypeInitial[m_town->m_type]);
    message_b.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    message_b.payload.widget.id = TOWN_MAGE_GUILD_ICON_CONTROL;
    message_b.payload.widget.data.text = gText;
    window->BroadcastMessage(message_b);
}

// @semantic: The 0x28 frame, local slots, select/switch CFG, and all five ordered
// relocations agree. The first raw divergence is the spell-table address:
// current code folds slot_p into the town base before loading level, while retail
// forms base + 4*level and then loads slot_p. Scalar wrappers on level, slot_p,
// and the town pointer, reversed subscripts, and the flat spell-slot view all
// produced the same worse level-before-base order. Revisit after a relevant
// accessor, layout, or TOWNMGR TU-state change.
VA(0x00419393, 0x190)
i32 MageGuildHandler(tag_message &message)
{
    i16 unusedFirstSpell = TOWN_MAGE_FIRST_SPELL_CONTROL;
    i16 unusedFirstIcon = TOWN_MAGE_FIRST_ICON_CONTROL;
    i16 unusedFirstDescription_l = TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
    u32 quickView_f;
    i32 spellSlot;
    i32 level;
    i32 slot_p;
    i32 spell;

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
        case TOWN_INPUT_SELECT:
        case TOWN_INPUT_ALTERNATE_SELECT:
            quickView_f = message.payload.widget.parameter & TOWN_QUICK_VIEW_MODIFIER;
            spellSlot = -1;
            if (message.payload.widget.id >= TOWN_MAGE_FIRST_SPELL_CONTROL &&
                message.payload.widget.id < TOWN_MAGE_FIRST_SPELL_CONTROL +
                                     TOWN_MAGE_GUILD_MAX_LEVEL *
                                         TOWN_MAGE_SPELLS_PER_LEVEL)
                spellSlot = message.payload.widget.id - TOWN_MAGE_FIRST_SPELL_CONTROL;
            if (message.payload.widget.id >= TOWN_MAGE_FIRST_ICON_CONTROL &&
                message.payload.widget.id < TOWN_MAGE_FIRST_ICON_CONTROL +
                                     TOWN_MAGE_GUILD_MAX_LEVEL *
                                         TOWN_MAGE_SPELLS_PER_LEVEL)
                spellSlot = message.payload.widget.id - TOWN_MAGE_FIRST_ICON_CONTROL;
            if (message.payload.widget.id >= TOWN_MAGE_FIRST_DESCRIPTION_CONTROL &&
                message.payload.widget.id < TOWN_MAGE_FIRST_DESCRIPTION_CONTROL +
                                     TOWN_MAGE_GUILD_MAX_LEVEL *
                                         TOWN_MAGE_SPELLS_PER_LEVEL)
                spellSlot = message.payload.widget.id -
                            TOWN_MAGE_FIRST_DESCRIPTION_CONTROL;
            if (spellSlot != -1) {
                level = spellSlot / TOWN_MAGE_SPELLS_PER_LEVEL;
                slot_p = spellSlot % TOWN_MAGE_SPELLS_PER_LEVEL;
                if (level[gpTownManager->m_town->m_spellCounts + 1] <= slot_p)
                    return 1;
                spell = gpTownManager->m_town->m_spells[level][slot_p];
                NormalDialog(gSpellDesc[spell],
                             quickView_f >= 1 ? 4 : 1,
                             -1, -1, 8, spell, -1, 0, -1, 0);
                return 1;
            }
        }
    }
    return EventWindowHandler(message);
}

VA(0x00419523, 0x706)
i32 townManager::RecruitHero(i32 availableHeroIndex, i32 cannotRecruit)
{
    i16 unusedTextStateTemp = 1;
    i16 unusedPortraitStateSlot = 2;
    i16 unusedTextControlValue = 3;
    i16 unusedIconStatek = 4;
    i16 unusedDimStatem = 6;
    i16 unusedPortraitControlIndex = 7;
    i16 unusedButtonTextk = 8;
    i16 unusedButtonIconl = 9;
    tag_message messageLocal;
    i32 artifactCountb;
    i32 indexValue;
    i32 townXh;
    i32 townYWork;
    i32 newHeroClassCount;

    m_heroWindow1 = new heroWindow(0xb1, 0x10, "rcrthero.bin");
    if (m_heroWindow1 == 0)
        MemError();
    SetWinText(m_heroWindow1, 0x14);
    m_recruitHero = &gpGame->m_heroRecs[
        gpCurPlayer->AvailableHeroId(availableHeroIndex)];
    m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
    messageLocal.type = MESSAGE_WIDGET;

    if (cannotRecruit != 0) {
        messageLocal.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        messageLocal.payload.widget.data.value = 2;
        messageLocal.payload.widget.id = 8;
        m_heroWindow1->BroadcastMessage(messageLocal);
        messageLocal.payload.widget.id = 9;
        m_heroWindow1->BroadcastMessage(messageLocal);
        messageLocal.payload.widget.id = TOWN_DIALOG_CONFIRM;
        m_heroWindow1->BroadcastMessage(messageLocal);
        messageLocal.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        messageLocal.payload.widget.data.value = 0x1000;
        messageLocal.payload.widget.id = 8;
        m_heroWindow1->BroadcastMessage(messageLocal);
        messageLocal.payload.widget.id = 9;
        m_heroWindow1->BroadcastMessage(messageLocal);
        messageLocal.payload.widget.id = TOWN_DIALOG_CONFIRM;
        m_heroWindow1->BroadcastMessage(messageLocal);
    }

    artifactCountb = 0;
    for (indexValue = 0; indexValue < TOWN_MAX_ARTIFACTS; ++indexValue) {
        if (m_recruitHero->m_artifacts[indexValue] != ARTIFACT_NONE &&
            m_recruitHero->m_artifacts[indexValue] != ARTIFACT_MAGIC_BOOK)
            ++artifactCountb;
    }
    sprintf(gText, "%s is a level %d %s with %d artifacts.",
            m_recruitHero->m_name, m_recruitHero->m_level,
            gAlignmentNames[m_recruitHero->m_cursorType], artifactCountb);
    messageLocal.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    messageLocal.payload.widget.id = 1;
    messageLocal.payload.widget.data.text = gText;
    m_heroWindow1->BroadcastMessage(messageLocal);
    sprintf(gText, "port%04d.icn", m_recruitHero->m_portrait);
    messageLocal.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    messageLocal.payload.widget.id = 2;
    messageLocal.payload.widget.data.text = gText;
    m_heroWindow1->BroadcastMessage(messageLocal);

    m_recruitState = -1;
    gpWindowManager->DoDialog(m_heroWindow1, RecruitHeroHandler, 0);
    delete m_heroWindow1;
    if (m_recruitState != -1) {
        m_recruitState = availableHeroIndex;
        gpCurPlayer->m_resources[RES_GOLD] -= gHeroGoldCost;
        gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] =
            gpCurPlayer->m_availableHeroIds[m_recruitState];
        ++gpCurPlayer->m_heroCount;

        townXh = m_town->m_x;
        townYWork = m_town->m_y;
        m_recruitHero->m_x = townXh;
        m_recruitHero->m_y = townYWork;
        m_recruitHero->m_eventFlags = 0;
        m_recruitHero->m_direction = 2;
        m_recruitHero->m_remainingMobility = m_recruitHero->CalcMobility();
        m_recruitHero->m_mobility = m_recruitHero->m_remainingMobility;
        m_recruitHero->m_locationType =
            gpGame->m_worldMap.GetCell(townXh, townYWork)->m_triggerType;
        m_recruitHero->m_occupiedTown =
            gpGame->m_worldMap.GetCell(townXh, townYWork)->m_objectMetadata;
        gpGame->m_worldMap.GetCell(townXh, townYWork)->m_triggerType =
            (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION);
        gpGame->m_worldMap.GetCell(townXh, townYWork)->m_objectMetadata =
            gpCurPlayer->m_availableHeroIds[m_recruitState];
        SendMapChange(3, m_recruitHero->m_id, townXh, townYWork,
                      TOWN_MAP_CHANGE_UNUSED, 0, 0);
        m_recruitResult = 1;
        m_town->m_occupyingHeroId = m_recruitHero->m_id;
        gpGame->m_availableHeroes[
            gpCurPlayer->m_availableHeroIds[m_recruitState]] =
            static_cast<i8>(giCurPlayer);
        CheckValidAvailableHeroes();
        if (m_town->m_buildings & 1)
            m_town->GiveSpells(0);

        newHeroClassCount =
            gpCurPlayer->m_availableHeroIds[1 - m_recruitState] / 9;
        newHeroClassCount =
            (Random(1, 5) + newHeroClassCount) % TOWN_FACTION_COUNT;
        gpCurPlayer->m_availableHeroIds[m_recruitState] =
            static_cast<i8>(
                gpGame->GetNewHeroId(giCurPlayer, newHeroClassCount, 0));
        gpGame->m_availableHeroes[
            gpCurPlayer->m_availableHeroIds[m_recruitState]] =
            AI_HERO_AVAILABLE_FLAG;
    } else {
        if (m_castleDialogActive != 0)
            SetupCastle(m_heroWindow0, 0);
        if (m_castleDialogActive != 0)
            m_heroWindow0->DrawWindow();
    }

    m_bankBox->Update(1);
    gpWindowManager->BroadcastMessage(
        MESSAGE_WIDGET, 6, TOWN_CONTROL_CLOSE,
        TOWN_INTERFACE_BROADCAST_FLAGS);
    m_recruitHero->m_owner = -1;
    if (m_recruitState != -1)
        m_recruitHero->m_owner = static_cast<char>(giCurPlayer);
    return m_recruitState != -1;
}

VA(0x00419c29, 0x153)
i32 TavernHandler(tag_message &message)
{
    i32 unusedDelay = TOWN_TAVERN_ANIMATION_DELAY;
    i16 unusedFirstFrame = TOWN_TAVERN_FIRST_ANIMATION_FRAME;

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
        case TOWN_INPUT_DESELECT:
            switch (message.payload.widget.id) {
            case 0x7800:
            case 0x7801:
            case TOWN_DIALOG_CONFIRM:
                gpWindowManager->m_dialogResult = message.payload.widget.id;
                message.payload.widget.id = 10;
                message.payload.widget.command = message.payload.widget.id;
                return 2;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    if (static_cast<i32l>(KBTickCount()) > glTimers[0]) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message.payload.widget.id = 2;
        ++gpGame->m_viewArmyResult;
        message.payload.widget.data.value =
            gpGame->m_viewArmyResult % TOWN_TAVERN_ANIMATION_FRAME_COUNT +
            TOWN_TAVERN_FIRST_ANIMATION_FRAME;
        gpTownManager->m_heroWindow0->BroadcastMessage(message);
        gpTownManager->m_heroWindow0->MoveWindow(0, 0);
        glTimers[0] = static_cast<i32>(KBTickCount() +
                                       TOWN_TAVERN_ANIMATION_DELAY);
    }
    return 1;
}

VA(0x00419d7c, 0x110)
void townManager::DoTavern(void)
{
    i32 unusedValue = 0;
    tag_message message;

    m_heroWindow0 = new heroWindow(0xa2, 10, "tavwin.bin");
    if (m_heroWindow0 == 0)
        MemError();
    SetWinText(m_heroWindow0, 0x16);
    sprintf(gText,
            "A generous tip for the barkeep yields the following rumor:\n\n%s",
            gpGame->m_rumour);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = TOWN_TAVERN_RUMOUR_CONTROL;
    message.payload.widget.data.text = gText;
    m_heroWindow0->BroadcastMessage(message);
    gpWindowManager->DoDialog(m_heroWindow0, TavernHandler, 0);
    delete m_heroWindow0;
}

// @semantic: Exact 0x328 size, 0x24 frame, complete amount/clamp,
// confirm/cancel/redraw CFG, and 32/32 resolved relocation owners/addends.
// Retail slots are plusButton -0x4, handled -0x8, unusedAction -0xc,
// minusButton -0x10, amountControl -0x14, and message -0x18; their initializer
// order and the final handled == 1 predicate are byte-exact. Relocation-masked
// raw proof leaves 71 bytes at +0x10c, +0x1b6, and +0x1f1..+0x241: equivalent
// branch destinations around one extra pre-ID-dispatch jump and the confirm
// arm. Removing the empty default shrank incorrectly to 0x323; moving confirm
// before cancel regressed to 91.74%. All 106 safe atomic AST variants found no
// gain. Revisit only with a new dispatch/body placement discovery.
VA(0x00419e8c, 0x328)
i32 SplitArmyHandler(tag_message &message)
{
    i16 plusButton = TOWN_SPLIT_INCREASE_CONTROL;
    i16 minusButton = TOWN_SPLIT_DECREASE_CONTROL;
    i16 amountControl = TOWN_SPLIT_AMOUNT_CONTROL;
    i32 handled = 0;
    i32 unusedAction;

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
        case TOWN_INPUT_SELECT:
            switch (message.payload.widget.id) {
            case TOWN_SPLIT_AMOUNT_CONTROL:
                message.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
                gpTownManager->m_heroWindow1->BroadcastMessage(message);
                gpTownManager->m_splitAmount = atoi(message.payload.widget.data.text);
                if (gpTownManager->m_splitAmount < 0)
                    gpTownManager->m_splitAmount = 0;
                if (gpTownManager->m_splitAmount >=
                    gpTownManager->m_splitMaximum)
                    gpTownManager->m_splitAmount =
                        gpTownManager->m_splitMaximum - 1;
                goto update_amount;
            }
            break;
        case TOWN_INPUT_DESELECT:
            switch (message.payload.widget.id) {
            case TOWN_SPLIT_INCREASE_CONTROL:
                ++gpTownManager->m_splitAmount;
                if (gpTownManager->m_splitAmount >=
                    gpTownManager->m_splitMaximum)
                    gpTownManager->m_splitAmount =
                        gpTownManager->m_splitMaximum - 1;
                goto update_amount;
            case TOWN_SPLIT_DECREASE_CONTROL:
                --gpTownManager->m_splitAmount;
                if (gpTownManager->m_splitAmount < 0)
                    gpTownManager->m_splitAmount = 0;
                goto update_amount;
            case 0x7800:
            case 0x7801:
                gpTownManager->m_splitAmount = 0;
                gpWindowManager->m_dialogResult = message.payload.widget.id;
                handled = 1;
                break;
            case TOWN_DIALOG_CONFIRM:
                if (gpTownManager->m_splitAmount == 0)
                    gpWindowManager->m_dialogResult = 0x7801;
                else
                    gpWindowManager->m_dialogResult = TOWN_DIALOG_CONFIRM;
                handled = 1;
                break;
            default:
                break;
            }
            break;
        }
    }

    if (handled == 1) {
        message.payload.widget.id = 10;
        message.payload.widget.command = message.payload.widget.id;
        return 2;
    }
    return 1;

update_amount:
    sprintf(gText, "%d", gpTownManager->m_splitAmount);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = TOWN_SPLIT_AMOUNT_CONTROL;
    message.payload.widget.data.text = gText;
    gpTownManager->m_heroWindow1->BroadcastMessage(message);
    gpTownManager->m_heroWindow1->DrawWindow(
        1, TOWN_SPLIT_AMOUNT_CONTROL, TOWN_SPLIT_AMOUNT_CONTROL);
    return 1;
}

VA(0x0041a1b4, 0x5cf)
void townManager::SetupWell(heroWindow *window)
{
    i16 unusedFirstIconj = 1;
    i16 unusedFirstNameh = TOWN_WELL_FIRST_NAME_CONTROL;
    i16 unusedFirstMonsterIconSlot = TOWN_WELL_FIRST_MONSTER_ICON_CONTROL;
    i16 unusedFirstCreaturep = TOWN_WELL_FIRST_CREATURE_CONTROL;
    i16 unusedFirstDetailState = TOWN_WELL_FIRST_DETAIL_CONTROL;
    i16 unusedFirstAvailablek = TOWN_WELL_FIRST_AVAILABLE_CONTROL;
    i16 unusedFirstAvailableCountm = TOWN_WELL_FIRST_AVAILABLE_COUNT_CONTROL;
    u8 dwellingTypesValue[8];
    i32 availablen;
    i32 dwellingResult;
    tag_message messaged;
    char iconNameCount[16];
    char detailTextf[40];
    tag_monsterInfo monsterInfoi;
    i32 growthd;

    for (dwellingResult = 0; dwellingResult < TOWN_WELL_DWELLING_COUNT; ++dwellingResult) {
        if (dwellingResult == TOWN_WELL_DWELLING_COUNT - 1 &&
            (m_town->m_buildings &
             (1L << TOWN_WELL_LAST_UPGRADE_BUILDING))) {
            dwellingTypesValue[dwellingResult] = TOWN_WELL_DWELLING_COUNT * 2 - 1;
        } else if (dwellingResult >= 1 &&
                   (m_town->m_buildings &
                    (1L << (dwellingResult + TOWN_WELL_FIRST_UPGRADE_BUILDING)))) {
            dwellingTypesValue[dwellingResult] = static_cast<u8>(
                dwellingResult + TOWN_WELL_FIRST_UPGRADE_OFFSET);
        } else {
            dwellingTypesValue[dwellingResult] = static_cast<u8>(dwellingResult);
        }
    }

    messaged.type = MESSAGE_WIDGET;
    messaged.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    sprintf(iconNameCount, "cstl%s.icn", cHeroTypeShortName[m_town->m_type]);
    messaged.payload.widget.data.text = iconNameCount;
    for (dwellingResult = 0; dwellingResult < TOWN_WELL_DWELLING_COUNT; ++dwellingResult) {
        messaged.payload.widget.id = dwellingResult + 1;
        window->BroadcastMessage(messaged);
    }

    for (dwellingResult = 0; dwellingResult < TOWN_WELL_DWELLING_COUNT; ++dwellingResult) {
        messaged.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        messaged.payload.widget.id = dwellingResult + 1;
        messaged.payload.widget.data.value = dwellingTypesValue[dwellingResult] +
                          TOWN_COMMAND_FIRST_DWELLING;
        window->BroadcastMessage(messaged);
        sprintf(gText, "monh%04d.icn",
                gDwellingType[m_town->m_type][dwellingTypesValue[dwellingResult]]);
        messaged.payload.widget.command = WIDGET_COMMAND_SET_ICON;
        messaged.payload.widget.id = dwellingResult + TOWN_WELL_FIRST_MONSTER_ICON_CONTROL;
        messaged.payload.widget.data.text = gText;
        window->BroadcastMessage(messaged);
    }

    messaged.type = MESSAGE_WIDGET;
    messaged.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    for (dwellingResult = 0; dwellingResult < TOWN_WELL_DWELLING_COUNT; ++dwellingResult) {
        sprintf(gText, GetBuildingName(
            m_town->m_type,
            dwellingTypesValue[dwellingResult] + TOWN_COMMAND_FIRST_DWELLING));
        messaged.payload.widget.id = dwellingResult + TOWN_WELL_FIRST_NAME_CONTROL;
        messaged.payload.widget.data.text = gText;
        window->BroadcastMessage(messaged);

        if (m_town->m_buildings &
            (1L << (dwellingTypesValue[dwellingResult] +
                    TOWN_COMMAND_FIRST_DWELLING))) {
            availablen = m_town->m_garrison[dwellingTypesValue[dwellingResult]];
            sprintf(gText, "Available:");
            messaged.payload.widget.id = dwellingResult + TOWN_WELL_FIRST_AVAILABLE_CONTROL;
            messaged.payload.widget.data.text = gText;
            window->BroadcastMessage(messaged);
            sprintf(gText, "%d", availablen);
            messaged.payload.widget.id = dwellingResult +
                             TOWN_WELL_FIRST_AVAILABLE_COUNT_CONTROL;
            messaged.payload.widget.data.text = gText;
            window->BroadcastMessage(messaged);
        }

        messaged.payload.widget.id = dwellingResult + TOWN_WELL_FIRST_CREATURE_CONTROL;
        strcpy(gText, gArmyNamesPlural[
            gDwellingType[m_town->m_type][dwellingTypesValue[dwellingResult]]]);
        gText[0] -= ' ';
        messaged.payload.widget.data.text = gText;
        window->BroadcastMessage(messaged);
    }

    for (dwellingResult = 0; dwellingResult < TOWN_WELL_DWELLING_COUNT; ++dwellingResult) {
        monsterInfoi = gMonsterDatabase[
            gDwellingType[m_town->m_type][dwellingTypesValue[dwellingResult]]];
        strcpy(gText, "");
        sprintf(detailTextf, "%s%d", cWellDetail[0], monsterInfoi.attack);
        strcat(gText, detailTextf);
        sprintf(detailTextf, "\n%s%d", cWellDetail[1], monsterInfoi.defense);
        strcat(gText, detailTextf);
        sprintf(detailTextf, "\n%s%d", cWellDetail[3], monsterInfoi.damageMin);
        strcat(gText, detailTextf);
        if (monsterInfoi.damageMin != monsterInfoi.damageMax) {
            sprintf(detailTextf, "-%d", monsterInfoi.damageMax);
            strcat(gText, detailTextf);
        }
        sprintf(detailTextf, "\n%s%d", cWellDetail[4], monsterInfoi.hitPoints);
        strcat(gText, detailTextf);
        sprintf(detailTextf, cWellDetail[7], speedText[monsterInfoi.speed]);
        strcat(gText, detailTextf);
        if (m_town->m_buildings &
            (1L << (dwellingTypesValue[dwellingResult] +
                    TOWN_COMMAND_FIRST_DWELLING))) {
            growthd = gMonsterDatabase[
                gDwellingType[m_town->m_type][dwellingTypesValue[dwellingResult]]].growth;
            growthd += TOWN_WELL_BASE_GROWTH_BONUS;
            if (dwellingResult == 0 &&
                (m_town->m_buildings &
                 (1L << TOWN_WELL_FIRST_DWELLING_GROWTH_BUILDING)))
                growthd += TOWN_WELL_FIRST_DWELLING_GROWTH_BONUS;
            sprintf(detailTextf, cWellDetail[8], growthd);
            strcat(gText, detailTextf);
        }
        messaged.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        messaged.payload.widget.id = dwellingResult + TOWN_WELL_FIRST_DETAIL_CONTROL;
        messaged.payload.widget.data.text = gText;
        window->BroadcastMessage(messaged);
    }
}

// @semantic: The 0x1b0 frame/slots, complete guild-rank/UI CFG, and all 114
// ordered relocations agree. The first raw divergence at +0x249 loads the loop
// counter before the bound and uses cmp/jle; retail loads the bound first and
// uses cmp/jge. This is not a jump-only wall: at +0x5d3 the equivalent hero-id
// address calculation also uses a different non-jump instruction sequence.
// Direct m_heroCount/m_townCount reads remove three false inline continuations,
// and value-before-position assignment is byte-exact. Positive, negated,
// empty-else, and explicit-continue forms were worse or changed rank
// progression; a prior bounded AST pass found no exact closure. Revisit after
// a rank-loop source-shape or relevant TOWNMGR TU-state discovery.
VA(0x0041a783, 0xf0f)
void townManager::SetupThievesGuild(heroWindow *window, i32 informationLevel)
{
    static i16 sourceLineBase = 0x0e0e;
    i16 unusedRankX_last = 0x102;
    i16 unusedRankWidth_category = 0x44;
    i16 unusedRankY_j = 0x1b;
    i16 unusedRankHeight_player = 0x18;
    i16 unusedRankIconHeight_k = 0x16;
    i16 unusedIconWidth_index = 0x12;
    i16 unusedIconHeight = 0x16;
    i16 unusedPlayerWidth_value = 0x48;
    i32 category_stat;
    i8 categoryOrder_x[TOWN_THIEVES_ORDER_BUFFER_SIZE];
    i32 rank;
    i32 tiedCount_value;
    i32 rankX_m;
    i32 lastAtRank_j;
    i32 firstAtRank_rank;
    widget *iconControl_last;
    char *widgetText_control;
    i32 maxCategories_hero;
    i32 strongestHeroValue_current;
    char statText_k[200];
    i32 armySlot_index;
    i32l categoryStats_m[TOWN_THIEVES_PLAYER_COUNT];
    i32 heroPosition_index;
    i32 heroValue_j;
    i32 strongestCreatureValue;
    i32 strongestHeroPosition_first;
    hero *strongestHero_x;
    i32 unusedThievesValue_s;
    town *playerTown_k;
    i16 unusedFirstRankControl_slot = TOWN_THIEVES_FIRST_RANK_CONTROL;
    i16 unusedFirstPlayerControl_hero = TOWN_THIEVES_FIRST_PLAYER_CONTROL;
    i16 unusedHeroY_p = 0x12c;
    i16 unusedPrimaryStatsY = 0x153;
    i16 unusedPersonalityY_player = 0x18d;
    i16 unusedCreatureY_j = 0x1a2;
    i32 position_current;
    tag_message message_n;
    widget *textControl_icon;
    i32 strongestCreature_m;

    if (informationLevel == -1)
        informationLevel = gpGame->GetNumThievesGuilds(giCurPlayer);

    if (informationLevel >= TOWN_THIEVES_INFO_ALL_CATEGORIES) {
        maxCategories_hero = TOWN_THIEVES_MAX_CATEGORIES;
    } else {
        if (informationLevel == TOWN_THIEVES_INFO_STRONGEST_CREATURE)
            maxCategories_hero = TOWN_THIEVES_CREATURE_CATEGORY_COUNT;
        else if (informationLevel == TOWN_THIEVES_INFO_PERSONALITY)
            maxCategories_hero = TOWN_THIEVES_PERSONALITY_CATEGORY_COUNT;
        else if (informationLevel == TOWN_THIEVES_INFO_PRIMARY_STATS)
            maxCategories_hero = TOWN_THIEVES_PRIMARY_CATEGORY_COUNT;
        else
            maxCategories_hero = TOWN_THIEVES_BASIC_CATEGORY_COUNT;
    }

    for (position_current = gpGame->m_playerCount - gpGame->m_deadPlayerCount;
         position_current < TOWN_THIEVES_PLAYER_COUNT; ++position_current) {
        message_n.type = MESSAGE_WIDGET;
        message_n.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message_n.payload.widget.id = position_current + TOWN_THIEVES_FIRST_RANK_CONTROL;
        message_n.payload.widget.data.value = 4;
        window->BroadcastMessage(message_n);
        message_n.payload.widget.id = position_current + TOWN_THIEVES_FIRST_PLAYER_CONTROL;
        window->BroadcastMessage(message_n);
    }
    for (position_current = gpGame->m_playerCount - gpGame->m_deadPlayerCount;
         position_current < TOWN_THIEVES_PLAYER_COUNT; ++position_current) {
        message_n.type = MESSAGE_WIDGET;
        message_n.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message_n.payload.widget.id = position_current + TOWN_THIEVES_FIRST_PLAYER_CONTROL;
        message_n.payload.widget.data.value = 4;
        window->BroadcastMessage(message_n);
    }

    for (category_stat = 0;
         !(category_stat >= maxCategories_hero); ++category_stat) {
        GetCategoryStats(category_stat, categoryStats_m, categoryOrder_x);
        SortStats(categoryStats_m, categoryOrder_x);
        firstAtRank_rank = 0;
        lastAtRank_j = 0;
        for (rank = 0; rank < TOWN_THIEVES_PLAYER_COUNT; ++rank) {
            if (gpGame->m_playerCount - gpGame->m_deadPlayerCount ==
                firstAtRank_rank)
                break;
            tiedCount_value = 1;
            while (lastAtRank_j + 1 < gpGame->m_playerCount &&
                   categoryStats_m[lastAtRank_j + 1] ==
                       categoryStats_m[lastAtRank_j]) {
                ++tiedCount_value;
                ++lastAtRank_j;
            }
            rankX_m = 0x44 * rank + 0x102 -
                      (tiedCount_value - 1) * 9;
            for (position_current = firstAtRank_rank;
                 !(lastAtRank_j < position_current); ++position_current) {
                iconControl_last = new iconWidget(
                    static_cast<i16>(
                        (position_current - firstAtRank_rank) * 0x12 + rankX_m),
                    static_cast<i16>(category_stat * 0x18 + 0x1b),
                    0x12, 0x16, "townwind.icn",
                    static_cast<i16>(
                        gpGame->m_players[categoryOrder_x[position_current]].m_color +
                        TOWN_THIEVES_RANK_ICON_FRAME_BASE),
                    0, -1, 0x10, 1);
                if (iconControl_last == 0)
                    MemError();
                window->AddWidget(iconControl_last, -1);
            }
            ++lastAtRank_j;
            firstAtRank_rank = lastAtRank_j;
        }
    }

    rank = 0;
    for (position_current = 0;
         position_current <
             gpGame->m_playerCount - gpGame->m_deadPlayerCount;
         ++position_current) {
        while (gpGame->m_playerDead[rank] != 0)
            ++rank;
        sprintf(gText, gColors[gpGame->m_players[rank].m_color]);
        gText[0] -= ' ';
        message_n.type = MESSAGE_WIDGET;
        message_n.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        message_n.payload.widget.id = position_current + TOWN_THIEVES_FIRST_PLAYER_CONTROL;
        message_n.payload.widget.data.text = gText;
        window->BroadcastMessage(message_n);

        if (informationLevel < TOWN_THIEVES_INFO_STRONGEST_HERO) {
        } else {
            strongestHeroPosition_first = -1;
            strongestHeroValue_current = 0;
            for (heroPosition_index = 0;
                 heroPosition_index < gpGame->m_players[rank].m_heroCount;
                 ++heroPosition_index) {
                strongestHero_x =
                    gpGame->GetPlayerHero(rank, heroPosition_index);
                heroValue_j = gpPhilAI->FightValueOfStack(
                    &strongestHero_x->m_army, strongestHero_x, 0, 0, 0, 0);
                if (!(heroValue_j <= strongestHeroValue_current)) {
                    strongestHeroValue_current = heroValue_j;
                    strongestHeroPosition_first = heroPosition_index;
                }
            }

            if (strongestHeroPosition_first != -1) {
                iconControl_last = new iconWidget(
                    static_cast<i16>(position_current * 0x44 + 0xf6),
                    0x12d, 0, 0, "locators.icn", 0x16, 0, -1, 0x10, 1);
                if (iconControl_last == 0)
                    MemError();
                window->AddWidget(iconControl_last, -1);
                iconControl_last = new iconWidget(
                    static_cast<i16>(position_current * 0x44 + 0xed),
                    300, 0, 0, "miniport.icn",
                    gpGame->GetPlayerHero(rank, strongestHeroPosition_first)
                        ->m_portrait,
                    0, -1, 0x10, 1);
                if (iconControl_last == 0)
                    MemError();
                window->AddWidget(iconControl_last, -1);
            }

            if (informationLevel < TOWN_THIEVES_INFO_PRIMARY_STATS) {
            } else {
                if (strongestHeroPosition_first != -1) {
                    strongestHero_x = gpGame->GetPlayerHero(
                        rank, strongestHeroPosition_first);
                    sprintf(gText, "Att.\nDef.\nPower\nKnowl.");
                    widgetText_control = static_cast<char *>(BaseAlloc(
                        strlen(gText) + 1,
                        "I:\\Projects\\Heroes\\Prog\\SOURCE\\TOWNMGR.CPP",
                        sourceLineBase + TOWN_THIEVES_SOURCE_LINE_HERO_LABELS));
                    strcpy(widgetText_control, gText);
                    textControl_icon = new textWidget(
                        static_cast<i16>(position_current * 0x44 + 0xef),
                        0x153, 0x28, 0x30, widgetText_control, "smalfont.fnt",
                        1, -1, 0x200, 0);
                    window->AddWidget(textControl_icon, -1);

                    sprintf(gText, "");
                    for (heroPosition_index = 0;
                         heroPosition_index < TOWN_THIEVES_PRIMARY_STAT_COUNT;
                         ++heroPosition_index) {
                        sprintf(statText_k, "%d\n",
                                strongestHero_x->Stats(heroPosition_index));
                        strcat(gText, statText_k);
                    }
                    widgetText_control = static_cast<char *>(BaseAlloc(
                        strlen(gText) + 1,
                        "I:\\Projects\\Heroes\\Prog\\SOURCE\\TOWNMGR.CPP",
                        sourceLineBase + TOWN_THIEVES_SOURCE_LINE_HERO_STATS));
                    strcpy(widgetText_control, gText);
                    textControl_icon = new textWidget(
                        static_cast<i16>(position_current * 0x44 + 0x11c),
                        0x153, 0xf, 0x30, widgetText_control, "smalfont.fnt",
                        1, -1, 0x200, 0);
                    window->AddWidget(textControl_icon, -1);
                }

                if (informationLevel < TOWN_THIEVES_INFO_PERSONALITY) {
                } else {
                    strcpy(gText, cPersonality[
                        gpGame->m_players[rank].m_aiDifficulty]);
                    widgetText_control = static_cast<char *>(BaseAlloc(
                        strlen(gText) + 1,
                        "I:\\Projects\\Heroes\\Prog\\SOURCE\\TOWNMGR.CPP",
                        sourceLineBase + TOWN_THIEVES_SOURCE_LINE_PERSONALITY));
                    strcpy(widgetText_control, gText);
                    textControl_icon = new textWidget(
                        static_cast<i16>(position_current * 0x44 + 0xe3),
                        0x18d, 0x4a, 0x10, widgetText_control, "smalfont.fnt",
                        1, -1, 8, 1);
                    window->AddWidget(textControl_icon, -1);

                    if (informationLevel <
                        TOWN_THIEVES_INFO_STRONGEST_CREATURE) {
                    } else {
                        strongestCreature_m = -1;
                        strongestCreatureValue = 0;
                        for (heroPosition_index = 0;
                             heroPosition_index <
                             gpGame->m_players[rank].m_townCount;
                             ++heroPosition_index) {
                            playerTown_k =
                                gpGame->GetPlayerTown(rank, heroPosition_index);
                            for (armySlot_index = 0;
                                 armySlot_index < TOWN_ARMY_SLOT_COUNT; ++armySlot_index) {
                                if (playerTown_k->m_army
                                            .m_creatureTypes[armySlot_index] != -1 &&
                                    playerTown_k->m_army
                                            .m_creatureCounts[armySlot_index] > 0 &&
                                    strongestCreatureValue <
                                        gMonsterDatabase[
                                            playerTown_k->m_army
                                                .m_creatureTypes[armySlot_index]]
                                            .fightValue) {
                                    strongestCreature_m = playerTown_k->m_army
                                                            .m_creatureTypes[armySlot_index];
                                    strongestCreatureValue =
                                        gMonsterDatabase[
                                            playerTown_k->m_army
                                                .m_creatureTypes[armySlot_index]]
                                            .fightValue;
                                }
                            }
                        }
                        for (heroPosition_index = 0;
                             heroPosition_index <
                             gpGame->m_players[rank].m_heroCount;
                             ++heroPosition_index) {
                            strongestHero_x =
                                gpGame->GetPlayerHero(rank, heroPosition_index);
                            for (armySlot_index = 0;
                                 armySlot_index < TOWN_ARMY_SLOT_COUNT; ++armySlot_index) {
                                if (strongestHero_x->m_army
                                            .m_creatureTypes[armySlot_index] != -1 &&
                                    strongestHero_x->m_army
                                            .m_creatureCounts[armySlot_index] > 0 &&
                                    strongestCreatureValue <
                                        gMonsterDatabase[
                                            strongestHero_x->m_army
                                                .m_creatureTypes[armySlot_index]]
                                            .fightValue) {
                                    strongestCreature_m = strongestHero_x->m_army
                                                            .m_creatureTypes[armySlot_index];
                                    strongestCreatureValue =
                                        gMonsterDatabase[
                                            strongestHero_x->m_army
                                                .m_creatureTypes[armySlot_index]]
                                            .fightValue;
                                }
                            }
                        }
                        if (strongestCreature_m != -1) {
                            iconControl_last = new iconWidget(
                                static_cast<i16>(
                                    position_current * 0x44 + 0xf4),
                                0x1a2, 0x28, 0x22, "mons32.icn",
                                static_cast<i16>(strongestCreature_m),
                                0, -1, 0x11, 1);
                            if (iconControl_last == 0)
                                MemError();
                            window->AddWidget(iconControl_last, -1);
                        }
                    }
                }
            }
        }
        ++rank;
    }
}

// @semantic: Complete ten-case CFG, exact 0x56a size, 0x2c
// frame/slots, and the exact +0x531/0x28 jump table. All 31 external and 42
// total relocations agree in ordered offset/type/identity/addend. Relocation-
// masked raw proof leaves 38 bytes: four one-byte branches crossing the two
// equivalent playerData::m_heroIds address spans at +0x3df..+0x3f2 and
// +0x47e..+0x491. Reversed subscripts compile identically; direct GetHero was
// worse. The consolidated runner tried all 122 safe atomic AST mutations with
// no gain. Revisit only after a playerData/accessor or TU-state change.
VA(0x0041b692, 0x56a)
void GetCategoryStats(i32 category, i32l * const stats,
                      i8 * const order)
{
    i32 player;
    i32 townIndex_c;
    hero *playerHero_h;
    i32 heroIndex_n;
    i32 townCount_k;
    i32 castleCount_p;
    i32 armyStrength;
    town *playerTown;

    for (player = 0; player < gpGame->m_playerCount; ++player) {
        townCount_k = 0;
        castleCount_p = 0;
        order[player] = static_cast<i8>(player);
        if (gpGame->m_playerDead[player]) {
            stats[player] = TOWN_THIEVES_DEAD_PLAYER_STAT;
        } else {
            switch (category) {
            case TOWN_THIEVES_CATEGORY_TOWNS:
                for (townIndex_c = 0; townIndex_c < GAME_TOWN_COUNT;
                     ++townIndex_c) {
                    if (gpGame->m_castleRecs[townIndex_c].m_owner == player &&
                        (gpGame->m_castleRecs[townIndex_c].m_buildings &
                         TOWN_BUILDING_TENT)) {
                        ++townCount_k;
                    }
                }
                stats[player] = townCount_k;
                break;
            case TOWN_THIEVES_CATEGORY_CASTLES:
                for (townIndex_c = 0; townIndex_c < GAME_TOWN_COUNT;
                     ++townIndex_c) {
                    if (gpGame->m_castleRecs[townIndex_c].m_owner == player &&
                        (gpGame->m_castleRecs[townIndex_c].m_buildings &
                         TOWN_BUILDING_CASTLE)) {
                        ++castleCount_p;
                    }
                }
                stats[player] = castleCount_p;
                break;
            case TOWN_THIEVES_CATEGORY_HEROES:
                stats[player] = gpGame->m_players[player].m_heroCount;
                break;
            case TOWN_THIEVES_CATEGORY_GOLD:
                stats[player] = gpGame->m_players[player].m_resources[RES_GOLD];
                break;
            case TOWN_THIEVES_CATEGORY_WOOD_AND_ORE:
                stats[player] =
                    gpGame->m_players[player].m_resources[RES_WOOD] +
                    gpGame->m_players[player].m_resources[RES_ORE];
                break;
            case TOWN_THIEVES_CATEGORY_RARE_RESOURCES:
                stats[player] =
                    gpGame->m_players[player].m_resources[RES_MERCURY] +
                    gpGame->m_players[player].m_resources[RES_SULFUR] +
                    gpGame->m_players[player].m_resources[RES_CRYSTAL] +
                    gpGame->m_players[player].m_resources[RES_GEMS];
                break;
            case TOWN_THIEVES_CATEGORY_OBELISKS:
                stats[player] = GetNumObelisks(player);
                break;
            case TOWN_THIEVES_CATEGORY_ARTIFACTS:
                stats[player] = 0;
                for (townIndex_c = 0;
                     townIndex_c < gpGame->m_players[player].m_heroCount;
                     ++townIndex_c) {
                    playerHero_h = gpGame->GetHero(
                        gpGame->m_players[player].m_heroIds[townIndex_c]);
                    for (heroIndex_n = 0;
                         heroIndex_n < TOWN_MAX_ARTIFACTS; ++heroIndex_n) {
                        if (playerHero_h->m_artifacts[heroIndex_n] != ARTIFACT_NONE &&
                            playerHero_h->m_artifacts[heroIndex_n] !=
                                ARTIFACT_MAGIC_BOOK) {
                            ++stats[player];
                        }
                    }
                }
                break;
            case TOWN_THIEVES_CATEGORY_ARMY_STRENGTH:
                armyStrength = 0;
                for (heroIndex_n = 0;
                     heroIndex_n < gpGame->m_players[player].m_heroCount;
                     ++heroIndex_n) {
                    playerHero_h =
                        gpGame->GetPlayerHero(player, heroIndex_n);
                    armyStrength += gpPhilAI->FightValueOfStack(
                        &playerHero_h->m_army, playerHero_h, 0, 0, 0, 0);
                }
                for (heroIndex_n = 0;
                     heroIndex_n < gpGame->m_players[player].m_townCount;
                     ++heroIndex_n) {
                    playerTown =
                        gpGame->GetPlayerTown(player, heroIndex_n);
                    if (playerTown->HasGarrison()) {
                        armyStrength += gpPhilAI->FightValueOfStack(
                            &playerTown->m_army, 0, 0, 0, 0, 0);
                    }
                }
                stats[player] = armyStrength;
                break;
            case TOWN_THIEVES_CATEGORY_INCOME:
                stats[player] = gpGame->ComputeDailyGold(player);
                break;
            }
        }
    }
}

VA(0x0041bbfc, 0xd9)
void SortStats(i32l * const stats, i8 * const order)
{
    i32 temporaryOrder;
    i32 secondPlayer;
    i32 firstPlayer;
    i32l tempStat;

    for (firstPlayer = 0; firstPlayer < gpGame->m_playerCount - 1;
         ++firstPlayer) {
        for (secondPlayer = firstPlayer + 1;
             secondPlayer < gpGame->m_playerCount; ++secondPlayer) {
            if (stats[OD_STEER(secondPlayer)] > stats[firstPlayer]) {
                tempStat = stats[firstPlayer];
                stats[firstPlayer] = stats[secondPlayer];
                stats[secondPlayer] = tempStat;
                temporaryOrder = order[firstPlayer];
                order[firstPlayer] = order[secondPlayer];
                order[secondPlayer] = static_cast<i8>(temporaryOrder);
            }
        }
    }
}


// ===== vtable townManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x00414109, 0x1ef)  int townManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x00414e98, 0xca)  void townManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x0041595d, 0x1830)  int townManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(townManager, 0x004eb140);
