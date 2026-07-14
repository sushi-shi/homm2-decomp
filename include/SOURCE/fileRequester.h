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

typedef enum FileRequesterMode {
    FILE_REQUESTER_MAP_GAME = 1,
    FILE_REQUESTER_LOAD_GAME = 2,
    FILE_REQUESTER_SAVE_GAME = 3,
    FILE_REQUESTER_MAP = 4
} FileRequesterMode;

typedef enum FileRequesterControlId {
    FILE_REQUESTER_SCROLL_KNOB = 3,
    FILE_REQUESTER_FILENAME_ENTRY = 15,
    FILE_REQUESTER_CANCEL = 0x7801,
    FILE_REQUESTER_OK = 0x7802,
    FILE_REQUESTER_FILTER_SMALL = 100,
    FILE_REQUESTER_FILTER_MEDIUM = 101,
    FILE_REQUESTER_FILTER_LARGE = 102,
    FILE_REQUESTER_FILTER_XLARGE = 103,
    FILE_REQUESTER_FILTER_ALL = 104
} FileRequesterControlId;

typedef enum FileRequesterMapSizeFilter {
    FILE_REQUESTER_MAP_SIZE_SMALL = 0,
    FILE_REQUESTER_MAP_SIZE_MEDIUM = 1,
    FILE_REQUESTER_MAP_SIZE_LARGE = 2,
    FILE_REQUESTER_MAP_SIZE_XLARGE = 3,
    FILE_REQUESTER_MAP_SIZE_ALL = 4,
    FILE_REQUESTER_MAP_SIZE_COUNT = 5
} FileRequesterMapSizeFilter;

enum {
    FILE_REQUESTER_PATH_SIZE = 300,
    FILE_REQUESTER_NAME_SIZE = 0x15f,
    FILE_REQUESTER_EXTENSION_SIZE = 5
};

struct FileRequesterName {
    char text[FILE_REQUESTER_NAME_SIZE];
};

struct FileRequesterExtension {
    char text[FILE_REQUESTER_EXTENSION_SIZE];
};

#pragma pack(push, 1)  // recovered layout is byte-packed
class fileRequester : public baseManager {
public:
    heroWindow *m_window;                        // +0x036
    int m_x;                                     // +0x03a
    int m_y;                                     // +0x03e
    FileRequesterMode m_mode;                    // +0x042
    char m_filePattern[FILE_REQUESTER_PATH_SIZE]; // +0x046
    char m_directory[FILE_REQUESTER_PATH_SIZE];   // +0x172
    FileRequesterName *m_fileNames;              // +0x29e
    FileRequesterExtension *m_extensions;        // +0x2a2
    SMapHeader *m_mapHeaders;                     // +0x2a6
    void *m_previousMenu;                         // +0x2aa
    char m_defaultExtension[FILE_REQUESTER_EXTENSION_SIZE]; // +0x2ae
    char m_filename[FILE_REQUESTER_NAME_SIZE];    // +0x2b3
    int m_fileCount;                              // +0x412
    int m_topIndex;                               // +0x416
    int m_selectedIndex;                          // +0x41a
    int m_result;                                 // +0x41e
    char m_listState[9];                          // +0x422
    iconWidget *m_scrollKnob;                     // +0x42b
    // --- constructors ---
    fileRequester(int, int, int, char *, char *, char *);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    int InitializeFiles(char *, char *, int);
    int MapExistsForFilter(int);
    void SetupFiles(void);
    void CleanUpData(void);
    void SetOK(int);
    void DoKnob(void);
    void Update(int);
    char * GetFilename(void);
};
#pragma pack(pop)
SIZE(fileRequester, 0x42f);
// ---- globals (declarations, RVA order) ----
extern int giMapSizeFilter;
extern char *cFRDummy;
extern float fGutterMinY;
extern float fGutterTravelLength;
extern int iMaxListSize;

#endif // HOMM2_SOURCE_FILEREQUESTER_H
