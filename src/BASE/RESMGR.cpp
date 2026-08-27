#include <Ints.h>
#include <BASE/Utf8.h>
#include <PLATFORM/Strings.h>
#include <PLATFORM/File.h>
#include <PLATFORM/Platform.h>
#include <string.h>
#include <SOURCE/KB.h>
#include <SOURCE/Localization.h>
#include <stdio.h>
#include <errno.h>
#include <limits>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/baseManager.h>
#include <BASE/resource.h>
#include <BASE/icon.h>
#include <BASE/sample.h>
#include <BASE/tileset.h>
#include <BASE/font.h>
#include <BASE/bitmap.h>
#include <BASE/palette.h>

typedef enum ResourceConstant {
    INVALID_FILE            = -1,
    LOAD_SUCCESS            = 0,
    LOAD_ERROR              = 3,
    ENTRY_BYTES             = 0xc,
    EVIL_TRANSLATION_COUNT  = 37,
    BACKDROP_ROW_BYTES      = 640,
    FILE_COUNT_BUFFER_WORDS = 2,
    POSITION_STACK_DEPTH    = 10
} ResourceConstant;

resourceManager::resourceManager(void) : baseManager() {
    i32 aggregateIndex;
    m_active = false;
    m_resourceListHead = NULL;
    m_expunging = false;
    strcpy(m_lastFileName,
           "");
    m_lastFileId = 0;
    for (aggregateIndex = 0; aggregateIndex < RESOURCE_MANAGER_AGGREGATE_LIMIT; aggregateIndex++) {
        m_aggregateFd[aggregateIndex] = INVALID_FILE;
        m_aggregateDir[aggregateIndex] = NULL;
        m_aggregateEntryCount[aggregateIndex] = 0;
    }
    m_numAggregates = 0;
    m_curAggregate = 0;
    m_reserved = 0;
}

void resourceManager::GetBackdrop(const char* name, class bitmap* backdrop, i32 useIcon) {
    if (useIcon) {
        icon* backdropIcon = GetIcon(name);
        backdropIcon->DrawToBuffer(0, 0, 0, ICON_DRAW_NORMAL);
        Dispose(backdropIcon);
    } else {
        PointToFile(MakeId(name, 1));
        ReadWord();
        ReadWord();
        ReadWord();
        ReadBlock(backdrop->m_pixels, backdrop->m_width * backdrop->m_height);
    }
}

void resourceManager::GetBackdropAtLoc(
    const char* filename,
    class bitmap* destination,
    i32 destinationX,
    i32 destinationY,
    i32 useIcon
) {
    icon* backdropIcon;
    i32 width;
    i32 imageHeight;
    i32 curRow;
    if (useIcon != 0) {
        backdropIcon = GetIcon(filename);
        backdropIcon->DrawToBuffer(destinationX, destinationY, 0, ICON_DRAW_NORMAL);
        Dispose(backdropIcon);
    } else {
        PointToFile(MakeId(filename, 1));
        ReadWord();
        width = ReadWord();
        imageHeight = ReadWord();
        for (curRow = destinationY; curRow < destinationY + imageHeight; curRow++) {
            ReadBlock(
                destination->m_pixels + curRow * BACKDROP_ROW_BYTES + destinationX,
                width
            );
        }
    }
}

class palette* resourceManager::GetPalette(const char* name) {
    u32l id = MakeId(name, 1);
    resource* r = Query(id);
    if (r != NULL) {
        r->m_refCount++;
        return static_cast<palette*>(r);
    } else {
        r = new palette(id);
        AddResource(r);
        return static_cast<palette*>(r);
    }
}

class bitmap* resourceManager::GetBitmap(const char* name) {
    u32l fileId = MakeId(name, 1);
    resource* r = Query(fileId);
    if (r != NULL) {
        r->m_refCount++;
        return static_cast<bitmap*>(r);
    } else {
        r = new bitmap(fileId);
        AddResource(r);
        return static_cast<bitmap*>(r);
    }
}

class icon* resourceManager::GetIcon(const char* name) {
    return GetIcon(MakeId(name, 1));
}

class icon* resourceManager::GetIcon(u32l resourceId) {
    icon* iconPointer = static_cast<icon*>(Query(resourceId));
    if (iconPointer != NULL) {
        iconPointer->m_refCount++;
        return iconPointer;
    } else {
        iconPointer = new icon(resourceId);
        AddResource(iconPointer);
        return iconPointer;
    }
}

class tileset* resourceManager::GetTileset(const char* name) {
    u32l id = MakeId(name, 1);
    resource* r = Query(id);
    if (r != NULL) {
        r->m_refCount++;
        return static_cast<tileset*>(r);
    } else {
        r = new tileset(id);
        AddResource(r);
        return static_cast<tileset*>(r);
    }
}

class mouse* resourceManager::GetMouse(const char*) {
    return NULL;
}

class font* resourceManager::GetFont(const char* name) {
    u32l resourceId = MakeId(name, 1);
    resource* fontEntry = Query(resourceId);
    if (fontEntry != NULL) {
        fontEntry->m_refCount++;
        return static_cast<font*>(fontEntry);
    } else {
        fontEntry = new font(resourceId);
        AddResource(fontEntry);
        return static_cast<font*>(fontEntry);
    }
}

class sample* resourceManager::GetSample(const char* name) {
    u32l fileId = MakeId(name, 1);
    resource* r = Query(fileId);
    if (r != NULL) {
        r->m_refCount++;
        return static_cast<sample*>(r);
    } else {
        r = new sample(name);
        AddResource(r);
        return static_cast<sample*>(r);
    }
}

void resourceManager::Dispose(class resource* resourceToDispose) {
    if (m_expunging != 0)
        return;
    if (resourceToDispose != NULL) {
        resourceToDispose->m_refCount--;
        if (resourceToDispose->m_refCount > 0) {
            return;
        } else {
            RemoveResource(resourceToDispose);
            delete resourceToDispose;
        }
    }
}

void resourceManager::AddResource(class resource* newResource) {
    if (m_resourceListHead == NULL) {
        m_resourceListHead = newResource;
        m_resourceListHead->m_next = NULL;
    } else {
        newResource->m_next = m_resourceListHead;
        m_resourceListHead = newResource;
    }
}

void resourceManager::Expunge(void) {
    m_expunging = true;
    resource* cur = m_resourceListHead;
    resource* next = NULL;
    while (cur != NULL) {
        next = cur->m_next;
        RemoveResource(cur);
        delete cur;
        cur = next;
    }
    m_expunging = false;
}

class resource* resourceManager::Query(u32l resourceId) {
    resource* cursorResource = m_resourceListHead;
    while (cursorResource != NULL && static_cast<u32l>(cursorResource->m_id) != resourceId) {
        cursorResource = cursorResource->m_next;
    }
    return cursorResource;
}

MessageDispatchResult resourceManager::Main(struct tag_message&) {
    return MESSAGE_DISPATCH_CONTINUE;
}

i32 resourceManager::Open(i32 priority) {
    if (localization::UsesResourceOverlay()) {
        const i32 localeExpansion =
            LoadAggregateHeader(EXPANSION_AGGREGATE_NAME, true, false);
        const i32 localeBase = LoadAggregateHeader(DEFAULT_AGGREGATE_NAME, true, false);
        if (localeExpansion != LOAD_SUCCESS && localeBase != LOAD_SUCCESS) {
            platform::Host().Log(
                platform::LogLevel::Warning,
                "resources: HOMM2_LOCALE_DATA contains no usable AGG archive"
            );
            localization::RejectResourceProfile(
                "the selected locale overlay contains no usable AGG archive"
            );
        }
        m_reserved = m_numAggregates;
    }
    if (LoadAggregateHeader(EXPANSION_AGGREGATE_NAME) != LOAD_SUCCESS)
        return LOAD_ERROR;
    if (LoadAggregateHeader(DEFAULT_AGGREGATE_NAME) != LOAD_SUCCESS)
        return LOAD_ERROR;
    m_messageMask = BASE_MANAGER_ACCEPT_RESOURCE;
    m_priority = priority;
    m_active = true;
    strcpy(m_name, "resourceManager");
    m_resourceListHead = NULL;
    return LOAD_SUCCESS;
}

void resourceManager::DisableLocaleAggregates(void) {
    for (i32 aggregateIndex = 0; aggregateIndex < m_reserved; ++aggregateIndex) {
        if (m_aggregateDir[aggregateIndex] != NULL) {
            H2_FREE(m_aggregateDir[aggregateIndex]);
            m_aggregateDir[aggregateIndex] = NULL;
            m_aggregateEntryCount[aggregateIndex] = 0;
        }
        if (m_aggregateFd[aggregateIndex] != INVALID_FILE) {
            platform::FileClose(m_aggregateFd[aggregateIndex]);
            m_aggregateFd[aggregateIndex] = INVALID_FILE;
        }
    }
    m_reserved = 0;
}

void resourceManager::RemoveResource(class resource* resourceToRemove) {
    if (m_resourceListHead == resourceToRemove) {
        m_resourceListHead = resourceToRemove->m_next;
        return;
    }
    resource* previousResource = m_resourceListHead;
    while (previousResource != NULL && previousResource->m_next != resourceToRemove) {
        previousResource = previousResource->m_next;
    }
    if (previousResource == NULL) {
        return;
    } else {
        previousResource->m_next = resourceToRemove->m_next;
    }
}

void resourceManager::Close(void) {
    i32 aggregateIndex;
    if (m_active != 1)
        return;
    Expunge();
    m_resourceListHead = NULL;
    for (aggregateIndex = 0; aggregateIndex < RESOURCE_MANAGER_AGGREGATE_LIMIT; aggregateIndex++) {
        if (m_aggregateDir[aggregateIndex] != NULL)
            H2_FREE(m_aggregateDir[aggregateIndex]);
        if (m_aggregateFd[aggregateIndex] != INVALID_FILE) {
            platform::FileClose(m_aggregateFd[aggregateIndex]);
            m_aggregateFd[aggregateIndex] = INVALID_FILE;
        }
    }
    m_numAggregates = 0;
    m_active = false;
}

i32 resourceManager::LoadAggregateHeader(
    const char* aggregateName,
    bool locale,
    bool required
) {
    i16 fpCountBuffer[FILE_COUNT_BUFFER_WORDS] = {};
    i32 aggregateFp;
    u32 directoryBytes;
    if (m_numAggregates >= RESOURCE_MANAGER_AGGREGATE_LIMIT) {
        utf8::Format(
            gText, GLOBAL_TEXT_BUFFER_SIZE,
            "Only %d .AGG files can be used at once.",
            RESOURCE_MANAGER_AGGREGATE_LIMIT
        );
        ShutDown(gText);
        return LOAD_ERROR;
    }
    aggregateFp = locale
        ? platform::FileOpenLocale(aggregateName)
        : platform::FileOpen(aggregateName, platform::FileMode::Read);
    if (aggregateFp == INVALID_FILE) {
        if (!required)
            return LOAD_ERROR;
        utf8::Format(
            gText, GLOBAL_TEXT_BUFFER_SIZE,
            localization::Tr("resource.file.open_failed"),
            aggregateName
        );
        ShutDown(gText);
        return LOAD_ERROR;
    }
    m_curAggregate = m_numAggregates;
    m_aggregateFd[m_curAggregate] = aggregateFp;
    const auto rejectAggregate = [&](const char* reason) {
        snprintf(
            gText,
            GLOBAL_TEXT_BUFFER_SIZE,
            "Invalid aggregate '%s': %s",
            aggregateName,
            reason
        );
        platform::FileClose(m_aggregateFd[m_curAggregate]);
        m_aggregateFd[m_curAggregate] = INVALID_FILE;
        m_aggregateEntryCount[m_curAggregate] = 0;
        if (required) {
            ShutDown(gText);
        } else {
            platform::Host().Log(platform::LogLevel::Warning, gText);
        }
        return LOAD_ERROR;
    };
    if (!platform::FileReadExact(
            m_aggregateFd[m_curAggregate], fpCountBuffer, sizeof(fpCountBuffer[0])
        )) {
        return rejectAggregate("missing entry count");
    }
    m_aggregateEntryCount[m_curAggregate] = fpCountBuffer[0];
    const i32 aggregateLength = platform::FileLength(m_aggregateFd[m_curAggregate]);
    if (m_aggregateEntryCount[m_curAggregate] <= 0
        || aggregateLength < static_cast<i32>(sizeof(fpCountBuffer[0]))
        || m_aggregateEntryCount[m_curAggregate]
               > (aggregateLength - static_cast<i32>(sizeof(fpCountBuffer[0]))) / ENTRY_BYTES) {
        return rejectAggregate("entry table exceeds the file");
    }
    directoryBytes = static_cast<u32>(m_aggregateEntryCount[m_curAggregate] * ENTRY_BYTES);
    m_aggregateDir[m_curAggregate] = static_cast<aggEntry*>(H2_ALLOC(directoryBytes));
    if (m_aggregateDir[m_curAggregate] == NULL
        || directoryBytes > static_cast<u32>(std::numeric_limits<i32>::max())
        || !platform::FileReadExact(
            m_aggregateFd[m_curAggregate],
            m_aggregateDir[m_curAggregate],
            static_cast<i32>(directoryBytes)
        )) {
        if (m_aggregateDir[m_curAggregate] != NULL) {
            H2_FREE(m_aggregateDir[m_curAggregate]);
            m_aggregateDir[m_curAggregate] = NULL;
        }
        return rejectAggregate("truncated entry table");
    }
    ++m_numAggregates;
    return LOAD_SUCCESS;
}

void resourceManager::PointToFile(u32l fileId) {
    bchar found = false;
    i32 entry;
    i32 i;
    for (i = 0; i < RESOURCE_MANAGER_AGGREGATE_LIMIT; i++) {
        if (m_aggregateDir[i] != NULL) {
            entry = 0;
            while (entry < m_aggregateEntryCount[i]) {
                if (m_aggregateDir[i][entry].id == fileId) {
                    found = true;
                    m_curAggregate = i;
                    break;
                }
                entry++;
            }
        }
        if (found)
            break;
    }
    if (!found) {
        utf8::Format(
            gText, GLOBAL_TEXT_BUFFER_SIZE,
            "ResMgr::PointToFile failure!  ThisFileId:%d  LastFileId:%d  LastFileName:%s",
            static_cast<i32>(fileId),
            m_lastFileId,
            m_lastFileName
        );
        ShutDown(gText);
        return;
    }
    platform::FileSeek(m_aggregateFd[m_curAggregate], m_aggregateDir[m_curAggregate][entry].offset);
}

u32l resourceManager::GetFileSize(u32l fileId) {
    bchar found = false;
    i32 entry;
    i32 matched;
    i32 i;
    for (i = 0; i < RESOURCE_MANAGER_AGGREGATE_LIMIT; i++) {
        if (m_aggregateDir[i] != NULL) {
            entry = 0;
            while (entry < m_aggregateEntryCount[i]) {
                if (m_aggregateDir[i][entry].id == fileId) {
                    found = true;
                    matched = i;
                    break;
                }
                entry++;
            }
        }
        if (found)
            break;
    }
    if (!found) {
        utf8::Format(
            gText, GLOBAL_TEXT_BUFFER_SIZE,
            "ResMgr::PointToFile failure!  ThisFileId:%d  LastFileId:%d  LastFileName:%s",
            static_cast<i32>(fileId),
            m_lastFileId,
            m_lastFileName
        );
        ShutDown(gText);
        return 0;
    }
    return m_aggregateDir[matched][entry].size;
}

void resourceManager::SavePosition(void) {
    lastPositionZ[iSaveCtr] = platform::FileTell(m_aggregateFd[m_curAggregate]);
    lastAggZ[iSaveCtr] = m_curAggregate;
    iSaveCtr = iSaveCtr + 1;
}

void resourceManager::RestorePosition(void) {
    iSaveCtr = iSaveCtr - 1;
    m_curAggregate = lastAggZ[iSaveCtr];
    platform::FileSeek(m_aggregateFd[m_curAggregate], lastPositionZ[iSaveCtr]);
}

i8 resourceManager::ReadByte(void) {
    H2_ASSERT(m_aggregateFd[m_curAggregate] != INVALID_FILE);
    i8 value = 0;
    ReadBlock(&value, sizeof(value));
    return value;
}

i16 resourceManager::ReadWord(void) {
    H2_ASSERT(m_aggregateFd[m_curAggregate] != INVALID_FILE);
    i16 value = 0;
    ReadBlock(&value, sizeof(value));
    return value;
}

i32l resourceManager::ReadLong(void) {
    H2_ASSERT(m_aggregateFd[m_curAggregate] != INVALID_FILE);
    i32l value = 0;
    ReadBlock(&value, sizeof(value));
    return value;
}

u32l resourceManager::MakeId(const char* name, i32 translate) {
    strcpy(m_lastFileName, name);
    if (gbUseEvilInterface != 0 && translate != 0) {
        for (i32 translatedIndex = 0; translatedIndex < EVIL_TRANSLATION_COUNT;
             translatedIndex++) {
            if (platform::CompareIgnoringCase(m_lastFileName, cEvilTranslate[translatedIndex][0]) == 0)
                strcpy(m_lastFileName, cEvilTranslate[translatedIndex][1]);
        }
    }
    u32l result = MAKEFILEID(m_lastFileName);
    m_lastFileId = result;
    return result;
}

void resourceManager::Read13(void* destination) {
    ReadBlock(destination, RESOURCE_MANAGER_READ13_BYTES);
}

void resourceManager::ReadBlock(void* destination, u32l size) {
    H2_ASSERT(m_aggregateFd[m_curAggregate] != INVALID_FILE);
    PollSound();
    const bool readable = size <= static_cast<u32l>(std::numeric_limits<i32>::max());
    if (!readable
        || !platform::FileReadExact(
            m_aggregateFd[m_curAggregate], destination, static_cast<i32>(size)
        )) {
        if (destination != NULL && readable) {
            memset(destination, 0, static_cast<size_t>(size));
        }
        utf8::Format(
            gText, GLOBAL_TEXT_BUFFER_SIZE,
            "File error - incomplete read, bytes requested %d, errno %d, last file '%s'",
            static_cast<i32>(size),
            errno,
            m_lastFileName
        );
        LogStr(gText);
    }
    PollSound();
}

i32 iSaveCtr = 0;
i32 lastAggZ[POSITION_STACK_DEPTH];
i32l lastPositionZ[POSITION_STACK_DEPTH];
