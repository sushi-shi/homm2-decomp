#ifndef HOMM2_BASE_MISC_TYPES_H
#define HOMM2_BASE_MISC_TYPES_H

#include <va.h>

#define MISC_REGISTRY_KEY "SOFTWARE\\New World Computing\\Heroes of Might and Magic 2\\1.0"
#define MISC_PCX_SOURCE_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\Misc.cpp"

template <int TextSize>
union SMiscTextSlot {
    char text[TextSize];
    unsigned long alignmentValue;
};

struct SMiscMemoryText {
    SMiscTextSlot<sizeof("KBAlloc    Size %d   Ptr %d   File %s  Line %d")> allocationFormat;
    SMiscTextSlot<sizeof("Free ")> freeLabel;
    SMiscTextSlot<sizeof("NULL POINTER")> nullPointer;
    SMiscTextSlot<sizeof("MemEntries Below 0")> entryUnderflow;
    SMiscTextSlot<sizeof("KBFree    Size %d   Ptr %d   File %s  Line %d")> freeFormat;
    SMiscTextSlot<sizeof("Bad Delete,  File '%13s'  Line % 4d, ptr %12d")> badDeleteFormat;
    SMiscTextSlot<sizeof("Total Memory Leaks")> leakCountLabel;
    SMiscTextSlot<sizeof("Memory Leak,  File '%13s'  Line % 4d, ptr %12d   size %6d")> leakFormat;
    SMiscTextSlot<sizeof("Mem Left %dK")> memoryStatusFormat;
    SMiscTextSlot<sizeof("Assert statement failed in module %s, line %d.  Do you wish to abort the program?")> assertMessage;
    SMiscTextSlot<sizeof("Assert Failure")> assertTitle;
};
SIZE(SMiscMemoryText, 0x180);

struct SMiscInstallDefaultsText {
    SMiscTextSlot<sizeof("AUTO")> autoLoadName;
    SMiscTextSlot<sizeof("AUTO")> autoSaveName;
};
SIZE(SMiscInstallDefaultsText, 0x10);

struct SMiscGameDefaultsText {
    SMiscTextSlot<sizeof("The Unknown Hero")> unknownHeroName;
    SMiscTextSlot<sizeof("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")> uniqueIdAlphabet;
};
SIZE(SMiscGameDefaultsText, 0x3c);

struct SMiscReadFileText {
    SMiscTextSlot<sizeof("HEROES2.CFG")> configFilename;
    SMiscTextSlot<sizeof("%s")> stringFormat;
    SMiscTextSlot<sizeof("rb")> binaryMode;
    SMiscTextSlot<sizeof("")> cdRomPathDefault;
    SMiscTextSlot<sizeof("")> appPathDefault;
};
SIZE(SMiscReadFileText, 0x1c);

struct SMiscReadRegistryText {
    SMiscTextSlot<sizeof("")> scratchDefault;
    SMiscTextSlot<sizeof(MISC_REGISTRY_KEY)> key;
    SMiscTextSlot<sizeof("Music Volume")> musicVolumeProbe;
    SMiscTextSlot<sizeof("Music Volume")> musicVolume;
    SMiscTextSlot<sizeof("Sound Volume")> soundVolume;
    SMiscTextSlot<sizeof("Walk Speed")> walkSpeed;
    SMiscTextSlot<sizeof("Computer Walk Speed")> computerWalkSpeed;
    SMiscTextSlot<sizeof("Show Route")> showRoute;
    SMiscTextSlot<sizeof("Blackout Computer")> blackoutComputer;
    SMiscTextSlot<sizeof("Sound Quality")> soundQuality;
    SMiscTextSlot<sizeof("Use Opera")> useOpera;
    SMiscTextSlot<sizeof("Direct Connect Com Port")> directComPort;
    SMiscTextSlot<sizeof("Direct Connect Baud Rate")> directBaudRate;
    SMiscTextSlot<sizeof("Modem Com Port")> modemComPort;
    SMiscTextSlot<sizeof("Modem Baud Rate")> modemBaudRate;
    SMiscTextSlot<sizeof("Modem Init String")> modemInitString;
    SMiscTextSlot<sizeof("Unique System ID")> uniqueSystemId;
    SMiscTextSlot<sizeof("Network Default Name")> networkDefaultName;
    SMiscTextSlot<sizeof("Autosave")> autosave;
    SMiscTextSlot<sizeof("Slow Video")> slowVideo;
    SMiscTextSlot<sizeof("Show Combat Grid")> showCombatGrid;
    SMiscTextSlot<sizeof("Show Combat Mouse Hex")> showCombatMouseHex;
    SMiscTextSlot<sizeof("Combat Shade Level")> combatShadeLevel;
    SMiscTextSlot<sizeof("Combat Army Info Level")> combatArmyInfoLevel;
    SMiscTextSlot<sizeof("Evil Interface Usage")> evilInterfaceUsage;
    SMiscTextSlot<sizeof("Quick Combat Level")> quickCombatLevel;
    SMiscTextSlot<sizeof("Combat Speed")> combatSpeed;
    SMiscTextSlot<sizeof("Auto Combat Use Spells")> autoCombatUseSpells;
    SMiscTextSlot<sizeof("First Map Offset")> firstMapOffset;
    SMiscTextSlot<sizeof("Current Map Offset")> currentMapOffset;
    SMiscTextSlot<sizeof("Show Object Boxes")> showObjectBoxes;
    SMiscTextSlot<sizeof("Editor Screen Animation")> editorScreenAnimation;
    SMiscTextSlot<sizeof("Editor Palette Cycling")> editorPaletteCycling;
    SMiscTextSlot<sizeof("Main Game Show Menu")> mainGameShowMenu;
    SMiscTextSlot<sizeof("Main Game X")> mainGameX;
    SMiscTextSlot<sizeof("Main Game Y")> mainGameY;
    SMiscTextSlot<sizeof("Main Game Width")> mainGameWidth;
    SMiscTextSlot<sizeof("Main Game Height")> mainGameHeight;
    SMiscTextSlot<sizeof("Main Game Full Screen")> mainGameFullScreen;
    SMiscTextSlot<sizeof("Main Game Color Mouse Cursor")> mainGameColorMouseCursor;
    SMiscTextSlot<sizeof("Editor Show Menu")> editorShowMenu;
    SMiscTextSlot<sizeof("Editor X")> editorX;
    SMiscTextSlot<sizeof("Editor Y")> editorY;
    SMiscTextSlot<sizeof("Editor Width")> editorWidth;
    SMiscTextSlot<sizeof("Editor Height")> editorHeight;
    SMiscTextSlot<sizeof("Editor Full Screen")> editorFullScreen;
    SMiscTextSlot<sizeof("Editor Color Mouse Cursor")> editorColorMouseCursor;
    SMiscTextSlot<sizeof("AppPath")> appPath;
    SMiscTextSlot<sizeof("")> appPathDefault;
    SMiscTextSlot<sizeof("CDDrive")> cdDrive;
    SMiscTextSlot<sizeof("")> cdDriveDefault;
};
SIZE(SMiscReadRegistryText, 0x3b0);

struct SMiscRemoteNameText {
    SMiscTextSlot<sizeof("RMT%sRL.BIN")> remoteLocal;
    SMiscTextSlot<sizeof("RMT%sRC.BIN")> remoteCenter;
    SMiscTextSlot<sizeof("RMT%sRD.BIN")> remoteRight;
    SMiscTextSlot<sizeof("RMT%sSL.BIN")> serverLocal;
    SMiscTextSlot<sizeof("RMT%sSC.BIN")> serverCenter;
    SMiscTextSlot<sizeof("RMT%sSD.BIN")> serverRight;
};
SIZE(SMiscRemoteNameText, 0x48);

struct SMiscWriteFileText {
    SMiscTextSlot<sizeof("HEROES2.CFG")> configFilename;
    SMiscTextSlot<sizeof("%s")> stringFormat;
};
SIZE(SMiscWriteFileText, 0x10);

struct SMiscWriteRegistryText {
    SMiscTextSlot<sizeof("")> scratchDefault;
    SMiscTextSlot<sizeof(MISC_REGISTRY_KEY)> key;
    SMiscTextSlot<sizeof("Music Volume")> musicVolume;
    SMiscTextSlot<sizeof("Sound Volume")> soundVolume;
    SMiscTextSlot<sizeof("Walk Speed")> walkSpeed;
    SMiscTextSlot<sizeof("Computer Walk Speed")> computerWalkSpeed;
    SMiscTextSlot<sizeof("Show Route")> showRoute;
    SMiscTextSlot<sizeof("Blackout Computer")> blackoutComputer;
    SMiscTextSlot<sizeof("Sound Quality")> soundQuality;
    SMiscTextSlot<sizeof("Use Opera")> useOpera;
    SMiscTextSlot<sizeof("Direct Connect Com Port")> directComPort;
    SMiscTextSlot<sizeof("Direct Connect Baud Rate")> directBaudRate;
    SMiscTextSlot<sizeof("Modem Com Port")> modemComPort;
    SMiscTextSlot<sizeof("Modem Baud Rate")> modemBaudRate;
    SMiscTextSlot<sizeof("Modem Init String")> modemInitString;
    SMiscTextSlot<sizeof("Unique System ID")> uniqueSystemId;
    SMiscTextSlot<sizeof("Network Default Name")> networkDefaultName;
    SMiscTextSlot<sizeof("Autosave")> autosave;
    SMiscTextSlot<sizeof("Slow Video")> slowVideo;
    SMiscTextSlot<sizeof("Show Combat Grid")> showCombatGrid;
    SMiscTextSlot<sizeof("Show Combat Mouse Hex")> showCombatMouseHex;
    SMiscTextSlot<sizeof("Combat Shade Level")> combatShadeLevel;
    SMiscTextSlot<sizeof("Combat Army Info Level")> combatArmyInfoLevel;
    SMiscTextSlot<sizeof("Evil Interface Usage")> evilInterfaceUsage;
    SMiscTextSlot<sizeof("Quick Combat Level")> quickCombatLevel;
    SMiscTextSlot<sizeof("Combat Speed")> combatSpeed;
    SMiscTextSlot<sizeof("Auto Combat Use Spells")> autoCombatUseSpells;
    SMiscTextSlot<sizeof("First Map Offset")> firstMapOffset;
    SMiscTextSlot<sizeof("Current Map Offset")> currentMapOffset;
    SMiscTextSlot<sizeof("Show Object Boxes")> showObjectBoxes;
    SMiscTextSlot<sizeof("Editor Screen Animation")> editorScreenAnimation;
    SMiscTextSlot<sizeof("Editor Palette Cycling")> editorPaletteCycling;
    SMiscTextSlot<sizeof("Main Game Show Menu")> mainGameShowMenu;
    SMiscTextSlot<sizeof("Main Game X")> mainGameX;
    SMiscTextSlot<sizeof("Main Game Y")> mainGameY;
    SMiscTextSlot<sizeof("Main Game Width")> mainGameWidth;
    SMiscTextSlot<sizeof("Main Game Height")> mainGameHeight;
    SMiscTextSlot<sizeof("Main Game Full Screen")> mainGameFullScreen;
    SMiscTextSlot<sizeof("Main Game Color Mouse Cursor")> mainGameColorMouseCursor;
    SMiscTextSlot<sizeof("Editor Show Menu")> editorShowMenu;
    SMiscTextSlot<sizeof("Editor X")> editorX;
    SMiscTextSlot<sizeof("Editor Y")> editorY;
    SMiscTextSlot<sizeof("Editor Width")> editorWidth;
    SMiscTextSlot<sizeof("Editor Height")> editorHeight;
    SMiscTextSlot<sizeof("Editor Full Screen")> editorFullScreen;
    SMiscTextSlot<sizeof("Editor Color Mouse Cursor")> editorColorMouseCursor;
};
SIZE(SMiscWriteRegistryText, 0x388);

struct SMiscCdText {
    SMiscTextSlot<sizeof("A:\\")> rootDrive;
    SMiscTextSlot<sizeof(".\\DATA\\HEROES2.AGG")> dataArchive;
    SMiscTextSlot<sizeof("%s\\heroes2\\anim\\voy24.smk")> configuredAnimationPath;
    SMiscTextSlot<sizeof("%s")> stringFormat;
    SMiscTextSlot<sizeof("open %c: type cdaudio alias CD")> openAudioCommand;
    SMiscTextSlot<sizeof("info CD UPC wait")> audioInfoCommand;
    SMiscTextSlot<sizeof("close CD")> closeAudioCommand;
    SMiscTextSlot<sizeof("%c:\\heroes2\\anim\\voy24.smk")> driveAnimationPath;
    SMiscTextSlot<sizeof(MISC_REGISTRY_KEY)> registryKey;
    SMiscTextSlot<sizeof("%c:")> driveFormat;
    SMiscTextSlot<sizeof("CDDrive")> cdDrive;
    SMiscTextSlot<sizeof("%c:%s")> driveAndPathFormat;
};
SIZE(SMiscCdText, 0xe8);

struct SMiscLogText {
    SMiscTextSlot<sizeof("KB.LOG")> truncateFilename;
    SMiscTextSlot<sizeof("===========New Log==========")> newLogLabel;
    SMiscTextSlot<sizeof("\n")> truncateNewline;
    SMiscTextSlot<sizeof("at+")> appendMode;
    SMiscTextSlot<sizeof("KB.LOG")> appendFilename;
    SMiscTextSlot<sizeof("\n")> appendNewline;
    SMiscTextSlot<sizeof("%s : % 8d % 8d % 8d % 8d % 8d % 8d % 8d")> sevenValueFormat;
    SMiscTextSlot<sizeof("%s : % 8d % 8d % 8d % 8d % 8d % 8d")> sixValueFormat;
    SMiscTextSlot<sizeof("%s : % 8d % 8d % 8d % 8d % 8d")> fiveValueFormat;
    SMiscTextSlot<sizeof("%s : % 8d % 8d % 8d % 8d")> fourValueFormat;
    SMiscTextSlot<sizeof("%s : % 8d % 8d % 8d")> threeValueFormat;
    SMiscTextSlot<sizeof("%s : % 8d % 8d")> twoValueFormat;
    SMiscTextSlot<sizeof("%s : % 8d")> oneValueFormat;
};
SIZE(SMiscLogText, 0xf4);

struct SMiscPcxText {
    SMiscTextSlot<sizeof(MISC_PCX_SOURCE_FILE)> encodedRowAllocation;
    SMiscTextSlot<sizeof(MISC_PCX_SOURCE_FILE)> encodedRowDestruction;
    SMiscTextSlot<sizeof(MISC_PCX_SOURCE_FILE)> outputPaletteAllocation;
    SMiscTextSlot<sizeof(MISC_PCX_SOURCE_FILE)> outputPaletteDestruction;
};
SIZE(SMiscPcxText, 0xa0);

struct SMiscFileText {
    SMiscTextSlot<sizeof("r+b")> readWriteBinaryMode;
};
SIZE(SMiscFileText, 0x4);

struct SMiscDataEntryText {
    SMiscTextSlot<sizeof("advmice.mse")> mouseFilename;
    SMiscTextSlot<sizeof("")> destinationDefault;
    SMiscTextSlot<sizeof("evntwin%d.bin")> windowFilenameFormat;
    SMiscTextSlot<sizeof("")> initialTextDefault;
    SMiscTextSlot<sizeof("buybuild.icn")> iconFilename;
    SMiscTextSlot<sizeof("bigfont.fnt")> fontFilename;
    SMiscTextSlot<sizeof("")> restoredMouseFilename;
};
SIZE(SMiscDataEntryText, 0x44);

struct SMiscText {
    SMiscMemoryText memory;
    SMiscInstallDefaultsText installDefaults;
    SMiscGameDefaultsText gameDefaults;
    SMiscReadFileText readFile;
    SMiscReadRegistryText readRegistry;
    SMiscRemoteNameText remoteNames;
    SMiscWriteFileText writeFile;
    SMiscWriteRegistryText writeRegistry;
    SMiscCdText cd;
    SMiscLogText log;
    SMiscPcxText pcx;
    SMiscFileText file;
    SMiscDataEntryText dataEntry;
};
SIZE(SMiscText, 0xc3c);

#endif // HOMM2_BASE_MISC_TYPES_H
