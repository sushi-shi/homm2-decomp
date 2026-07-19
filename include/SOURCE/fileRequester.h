#ifndef HOMM2_SOURCE_FILEREQUESTER_H
#define HOMM2_SOURCE_FILEREQUESTER_H
#include <va.h>
#include "../BASE/baseManager.h"
struct tag_message;
class heroWindow;
class iconWidget;
struct SMapHeader;

H2_ENUM_CLASS_BEGIN(FileRequesterMode)
    FILE_REQUESTER_MAP_GAME = 1,
    FILE_REQUESTER_LOAD_GAME = 2,
    FILE_REQUESTER_SAVE_GAME = 3,
    FILE_REQUESTER_MAP = 4
H2_ENUM_CLASS_END(FileRequesterMode)

H2_ENUM_BEGIN(FileRequesterControlId)
    FILE_REQUESTER_PATH_SIZE = 300,
    FILE_REQUESTER_NAME_SIZE = 0x15f,
    FILE_REQUESTER_EXTENSION_SIZE = 5,
    FILE_REQUESTER_LOCAL_NAME_SIZE = 352,
    FILE_REQUESTER_LOCAL_EXTENSION_SIZE = 208,
    FILE_REQUESTER_UPDATE_STORAGE_SIZE = 380,
    FILE_REQUESTER_FILENAME_INITIAL_CLEAR_SIZE = 9,
    FILE_REQUESTER_LIST_RANGE_SIZE = 20,
    FILE_REQUESTER_SCROLL_KNOB_HALF_HEIGHT = 9,
    FILE_REQUESTER_GUTTER_SCALE = 100,
    FILE_REQUESTER_HELP_MODIFIER = 2,
    FILE_REQUESTER_EXTENSION_PLAYER_DIGIT = 3,
    FILE_REQUESTER_DEBUG_ALLOW_PLAYER_MISMATCH = 2,
    FILE_REQUESTER_SELECTION_NONE = -1,
    FILE_REQUESTER_MAIN_CONTINUE = 1,
    FILE_REQUESTER_MAIN_CLOSE = 2,
    FILE_REQUESTER_SCROLL_UP = 1,
    FILE_REQUESTER_SCROLL_DOWN = 2,
    FILE_REQUESTER_SCROLL_KNOB = 3,
    FILE_REQUESTER_SCROLL_GUTTER = 14,
    FILE_REQUESTER_FILENAME_ENTRY = 15,
    FILE_REQUESTER_FILENAME_LABEL = 16,
    FILE_REQUESTER_LIST_TEXT_FIRST = 20,
    FILE_REQUESTER_MAP_NAME = 0x50,
    FILE_REQUESTER_MAP_PLAYER_COUNT = 0x51,
    FILE_REQUESTER_MAP_SIZE = 0x52,
    FILE_REQUESTER_MAP_DIFFICULTY_ICON = 0x53,
    FILE_REQUESTER_MAP_DIFFICULTY_TEXT = 0x54,
    FILE_REQUESTER_MAP_DESCRIPTION = 0x55,
    FILE_REQUESTER_MAP_VICTORY = 0x56,
    FILE_REQUESTER_MAP_LOSS = 0x57,
    FILE_REQUESTER_MAP_SIZE_ICON_FIRST = 200,
    FILE_REQUESTER_MAP_PLAYER_ICON_FIRST = 220,
    FILE_REQUESTER_MAP_VICTORY_ICON_FIRST = 240,
    FILE_REQUESTER_MAP_LOSS_ICON_FIRST = 260,
    FILE_REQUESTER_CANCEL = 0x7801,
    FILE_REQUESTER_OK = 0x7802,
    FILE_REQUESTER_FILTER_SMALL = 100,
    FILE_REQUESTER_FILTER_MEDIUM = 101,
    FILE_REQUESTER_FILTER_LARGE = 102,
    FILE_REQUESTER_FILTER_XLARGE = 103,
    FILE_REQUESTER_FILTER_ALL = 104
H2_ENUM_END(FileRequesterControlId)

H2_ENUM_CLASS_BEGIN(FileRequesterMapSizeFilter)
    FILE_REQUESTER_MAP_SIZE_SMALL = 0,
    FILE_REQUESTER_MAP_SIZE_MEDIUM = 1,
    FILE_REQUESTER_MAP_SIZE_LARGE = 2,
    FILE_REQUESTER_MAP_SIZE_XLARGE = 3,
    FILE_REQUESTER_MAP_SIZE_ALL = 4,
    FILE_REQUESTER_MAP_SIZE_COUNT = 5
H2_ENUM_CLASS_END(FileRequesterMapSizeFilter)

H2_ENUM_CLASS_BEGIN(FileRequesterKey)
    FILE_REQUESTER_KEY_CYCLE_FILTER = 0x40
H2_ENUM_CLASS_END(FileRequesterKey)

H2_ENUM_CLASS_BEGIN(FileRequesterHelpIndex)
    FILE_REQUESTER_HELP_NONE = -1,
    FILE_REQUESTER_HELP_FILTER_SMALL = 0,
    FILE_REQUESTER_HELP_FILTER_MEDIUM = 1,
    FILE_REQUESTER_HELP_FILTER_LARGE = 2,
    FILE_REQUESTER_HELP_FILTER_XLARGE = 3,
    FILE_REQUESTER_HELP_FILTER_ALL = 4,
    FILE_REQUESTER_HELP_FILENAME = 5,
    FILE_REQUESTER_HELP_OK = 6,
    FILE_REQUESTER_HELP_CANCEL = 7,
    FILE_REQUESTER_HELP_MAP_SIZE = 8,
    FILE_REQUESTER_HELP_PLAYER_COUNT = 9,
    FILE_REQUESTER_HELP_VICTORY = 10,
    FILE_REQUESTER_HELP_LOSS = 11,
    FILE_REQUESTER_HELP_MAP_NAME = 12,
    FILE_REQUESTER_HELP_DESCRIPTION = 13,
    FILE_REQUESTER_HELP_DIFFICULTY = 14
H2_ENUM_CLASS_END(FileRequesterHelpIndex)

struct FileRequesterName {
    char text[FILE_REQUESTER_NAME_SIZE];
};

struct FileRequesterExtension {
    char text[FILE_REQUESTER_EXTENSION_SIZE];
};

#pragma pack(push, 1)
class fileRequester : public baseManager {
public:
    heroWindow* m_window;
    i32 m_x;
    i32 m_y;
    FileRequesterMode m_mode;
    char m_filePattern[FILE_REQUESTER_PATH_SIZE];
    char m_directory[FILE_REQUESTER_PATH_SIZE];
    FileRequesterName* m_fileNames;
    FileRequesterExtension* m_extensions;
    SMapHeader* m_mapHeaders;
    void* m_previousMenu;
    char m_defaultExtension[FILE_REQUESTER_EXTENSION_SIZE];
    char m_filename[FILE_REQUESTER_NAME_SIZE];
    i32 m_fileCount;
    i32 m_topIndex;
    i32 m_selectedIndex;
    i32 m_result;
    char m_listState[9];
    iconWidget* m_scrollKnob;
    fileRequester(i32, i32, FileRequesterMode, char*, char*, char*);
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    i32 InitializeFiles(char*, char*, i32);
    i32 MapExistsForFilter(FileRequesterMapSizeFilter);
    void SetupFiles(void);
    void CleanUpData(void);
    void SetOK(i32);
    void DoKnob(void);
    void Update(i32);
    char* GetFilename(void);
};
#pragma pack(pop)
SIZE(fileRequester, 0x42f);
extern FileRequesterMapSizeFilter giMapSizeFilter;
extern char* cFRDummy;
extern float fGutterMinY;
extern float fGutterTravelLength;
extern i32 iMaxListSize;

#endif
