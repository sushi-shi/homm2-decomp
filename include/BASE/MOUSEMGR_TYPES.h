#ifndef HOMM2_BASE_MOUSEMGR_TYPES_H
#define HOMM2_BASE_MOUSEMGR_TYPES_H

#include <Ints.h>

#define MOUSE_MANAGER_SOURCE_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\MOUSEMGR.CPP"
#define MOUSE_MANAGER_NAME "mouseManager"
#define MOUSE_MANAGER_ADVENTURE_ICON "ADVMCO.ICN"
#define MOUSE_MANAGER_ADVENTURE_BITMAP "ADVMBW%02d.BMP"

union SMouseManagerName {
    char text[sizeof(MOUSE_MANAGER_NAME)];
    u32l alignmentValue;
};

union SMouseManagerSourceFile {
    char text[sizeof(MOUSE_MANAGER_SOURCE_FILE)];
    u32l alignmentValue;
};

union SMouseManagerIconName {
    char text[sizeof(MOUSE_MANAGER_ADVENTURE_ICON)];
    u32l alignmentValue;
};

union SMouseManagerBitmapName {
    char text[sizeof(MOUSE_MANAGER_ADVENTURE_BITMAP)];
    u32l alignmentValue;
};

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
    char defaultCursorName[sizeof("")];
};

#endif
