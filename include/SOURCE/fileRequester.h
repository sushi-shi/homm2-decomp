#ifndef HOMM2_SOURCE_FILEREQUESTER_H
#define HOMM2_SOURCE_FILEREQUESTER_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 12 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
// forward declarations:
struct tag_message;
class heroWindow;
class iconWidget;
struct SMapHeader;

HOMM2_ENUM_BEGIN(FileRequesterMode)
FILE_REQUESTER_MAP_GAME = 1, FILE_REQUESTER_LOAD_GAME = 2, FILE_REQUESTER_SAVE_GAME = 3,
                             FILE_REQUESTER_MAP = 4 HOMM2_ENUM_END(FileRequesterMode)

                                 typedef enum FileRequesterControlId {
    FILE_REQUESTER_SCROLL_UP = 1,
    FILE_REQUESTER_SCROLL_DOWN = 2,
    FILE_REQUESTER_SCROLL_KNOB = 3,
    FILE_REQUESTER_SCROLL_GUTTER = 14,
    FILE_REQUESTER_FILENAME_ENTRY = 15,
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
} FileRequesterControlId;

HOMM2_ENUM_BEGIN(FileRequesterMapSizeFilter)
FILE_REQUESTER_MAP_SIZE_SMALL = 0,
    FILE_REQUESTER_MAP_SIZE_MEDIUM = 1, FILE_REQUESTER_MAP_SIZE_LARGE = 2,
    FILE_REQUESTER_MAP_SIZE_XLARGE = 3, FILE_REQUESTER_MAP_SIZE_ALL = 4,
    FILE_REQUESTER_MAP_SIZE_COUNT = 5 HOMM2_ENUM_END(FileRequesterMapSizeFilter)

        typedef enum FileRequesterKey {
    FILE_REQUESTER_KEY_CYCLE_FILTER = 0x40
} FileRequesterKey;

typedef enum FileRequesterHelpIndex {
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
} FileRequesterHelpIndex;

enum {
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
    FILE_REQUESTER_EXECUTIVE_CLOSE = 4
};

struct FileRequesterName {
    char text[FILE_REQUESTER_NAME_SIZE];
};

struct FileRequesterExtension {
    char text[FILE_REQUESTER_EXTENSION_SIZE];
};

#pragma pack(push, 1) // recovered layout is byte-packed
class fileRequester : public baseManager {
public:
    heroWindow* m_window;                                   // +0x036
    i32 m_x;                                                // +0x03a
    i32 m_y;                                                // +0x03e
    FileRequesterMode m_mode;                               // +0x042
    char m_filePattern[FILE_REQUESTER_PATH_SIZE];           // +0x046
    char m_directory[FILE_REQUESTER_PATH_SIZE];             // +0x172
    FileRequesterName* m_fileNames;                         // +0x29e
    FileRequesterExtension* m_extensions;                   // +0x2a2
    SMapHeader* m_mapHeaders;                               // +0x2a6
    void* m_previousMenu;                                   // +0x2aa
    char m_defaultExtension[FILE_REQUESTER_EXTENSION_SIZE]; // +0x2ae
    char m_filename[FILE_REQUESTER_NAME_SIZE];              // +0x2b3
    i32 m_fileCount;                                        // +0x412
    i32 m_topIndex;                                         // +0x416
    i32 m_selectedIndex;                                    // +0x41a
    i32 m_result;                                           // +0x41e
    char m_listState[9];                                    // +0x422
    iconWidget* m_scrollKnob;                               // +0x42b
    // --- constructors ---
    fileRequester(i32, i32, i32, char*, char*, char*);
    // --- virtual methods (vtable order) ---
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    // --- methods ---
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
// ---- globals (declarations, RVA order) ----
extern FileRequesterMapSizeFilter giMapSizeFilter;
extern char* cFRDummy;
extern float fGutterMinY;
extern float fGutterTravelLength;
extern i32 iMaxListSize;

#endif // HOMM2_SOURCE_FILEREQUESTER_H
