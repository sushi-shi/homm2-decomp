#ifndef HOMM2_BASE_MOUSEMGR_TYPES_H
#define HOMM2_BASE_MOUSEMGR_TYPES_H

#include <va.h>

#define MOUSE_MANAGER_SOURCE_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\MOUSEMGR.CPP"
#define MOUSE_MANAGER_NAME "mouseManager"
#define MOUSE_MANAGER_ADVENTURE_ICON "ADVMCO.ICN"
#define MOUSE_MANAGER_SPELL_ICON "SPELCO.ICN"
#define MOUSE_MANAGER_COMBAT_ICON "CMSECO.ICN"
#define MOUSE_MANAGER_ADVENTURE_BITMAP "ADVMBW%02d.BMP"
#define MOUSE_MANAGER_SPELL_BITMAP "SPELBW%02d.BMP"
#define MOUSE_MANAGER_COMBAT_BITMAP "CMSEBW%02d.BMP"

union SMouseManagerName {
    char text[sizeof(MOUSE_MANAGER_NAME)];
    u32l alignmentValue;
};
SIZE(SMouseManagerName, 0x10);

union SMouseManagerSourceFile {
    char text[sizeof(MOUSE_MANAGER_SOURCE_FILE)];
    u32l alignmentValue;
};
SIZE(SMouseManagerSourceFile, 0x2c);

union SMouseManagerIconName {
    char text[sizeof(MOUSE_MANAGER_ADVENTURE_ICON)];
    u32l alignmentValue;
};
SIZE(SMouseManagerIconName, 0xc);

union SMouseManagerBitmapName {
    char text[sizeof(MOUSE_MANAGER_ADVENTURE_BITMAP)];
    u32l alignmentValue;
};
SIZE(SMouseManagerBitmapName, 0x10);

struct SMouseManagerStrings {
    SMouseManagerName managerName;
    SMouseManagerSourceFile andMaskDestruction;
    SMouseManagerSourceFile colorBitsDestruction;
    SMouseManagerIconName adventureIcon;
    SMouseManagerIconName spellIcon;
    SMouseManagerIconName combatIcon;
    SMouseManagerSourceFile cursorFrameAssertion;
    SMouseManagerSourceFile cursorSizeAssertion;
    SMouseManagerSourceFile colorBitsAllocation;
    SMouseManagerSourceFile andBitsAllocation;
    SMouseManagerBitmapName adventureBitmap;
    SMouseManagerBitmapName spellBitmap;
    SMouseManagerBitmapName combatBitmap;
    SMouseManagerSourceFile bitmapAssertion;
    SMouseManagerSourceFile cursorAssertion;
    char defaultCursorName[1];
};
SIZE(SMouseManagerStrings, 0x1c8);

#endif // HOMM2_BASE_MOUSEMGR_TYPES_H
