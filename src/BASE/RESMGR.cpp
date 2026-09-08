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
#include <bit>
#include <algorithm>
#include <PLATFORM/Binary.h>
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
#include <BASE/bmap2.h>
#include <BASE/palette.h>

typedef enum ResourceConstant {
    INVALID_FILE            = -1,
    LOAD_SUCCESS            = 0,
    LOAD_ERROR              = 3,
    EVIL_TRANSLATION_COUNT  = 37,
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
    m_curEntry = -1;
    m_reserved = 0;
}

void resourceManager::GetBackdrop(const char* name, class bitmap* backdrop, i32 useIcon) {
    if (useIcon) {
        icon* backdropIcon = GetIcon(name);
        backdropIcon->DrawToBuffer(0, 0, 0, ICON_DRAW_NORMAL);
        Dispose(backdropIcon);
    } else {
        bitmap* image = GetBitmap(name);
        BlitBitmap(image, 0, 0, image->m_width, image->m_height, backdrop, 0, 0);
        Dispose(image);
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
    if (useIcon != 0) {
        backdropIcon = GetIcon(filename);
        backdropIcon->DrawToBuffer(destinationX, destinationY, 0, ICON_DRAW_NORMAL);
        Dispose(backdropIcon);
    } else {
        bitmap* image = GetBitmap(filename);
        BlitBitmap(image, 0, 0, image->m_width, image->m_height,
                   destination, destinationX, destinationY);
        Dispose(image);
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
        if (m_aggregateDir[aggregateIndex] != NULL) {
            H2_FREE(m_aggregateDir[aggregateIndex]);
            m_aggregateDir[aggregateIndex] = nullptr;
        }
        m_aggregateEntryCount[aggregateIndex] = 0;
        if (m_aggregateFd[aggregateIndex] != INVALID_FILE) {
            platform::FileClose(m_aggregateFd[aggregateIndex]);
            m_aggregateFd[aggregateIndex] = INVALID_FILE;
        }
    }
    m_numAggregates = 0;
    m_curEntry = -1;
    m_active = false;
}

i32 resourceManager::LoadAggregateHeader(
    const char* aggregateName,
    bool locale,
    bool required
) {
    i32 aggregateFp;
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
    m_curEntry = -1;
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
    std::vector<aggEntry> entries;
    std::string error;
    if (!resources::ReadAggDirectory(platform::Files(), aggregateFp, entries, error))
        return rejectAggregate(error.c_str());
    const std::size_t directoryBytes = entries.size() * sizeof(aggEntry);
    auto* directory = static_cast<aggEntry*>(H2_ALLOC(directoryBytes));
    if (directory == nullptr)
        return rejectAggregate("cannot allocate entry table");
    std::copy(entries.begin(), entries.end(), directory);
    m_aggregateDir[m_curAggregate] = directory;
    m_aggregateEntryCount[m_curAggregate] = static_cast<i32>(entries.size());
    ++m_numAggregates;
    return LOAD_SUCCESS;
}

void resourceManager::PointToFile(u32l fileId) {
    m_curEntry = -1;
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
    const i32 offset = m_aggregateDir[m_curAggregate][entry].offset;
    if (platform::FileSeek(m_aggregateFd[m_curAggregate], offset) != offset) {
        ShutDown("Cannot seek to an AGG member.");
        return;
    }
    m_curEntry = entry;
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

namespace {

i32 lastEntryZ[POSITION_STACK_DEPTH];

}

void resourceManager::SavePosition(void) {
    if (iSaveCtr < 0 || iSaveCtr >= POSITION_STACK_DEPTH) {
        ShutDown("Invalid AGG position stack push.");
        return;
    }
    lastPositionZ[iSaveCtr] = m_aggregateFd[m_curAggregate] != INVALID_FILE
        ? platform::FileTell(m_aggregateFd[m_curAggregate]) : -1;
    lastAggZ[iSaveCtr] = m_curAggregate;
    lastEntryZ[iSaveCtr] = m_curEntry;
    ++iSaveCtr;
}

void resourceManager::RestorePosition(void) {
    if (iSaveCtr <= 0 || iSaveCtr > POSITION_STACK_DEPTH) {
        ShutDown("Invalid AGG position stack pop.");
        return;
    }
    --iSaveCtr;
    m_curAggregate = lastAggZ[iSaveCtr];
    m_curEntry = lastEntryZ[iSaveCtr];
    const i32 position = static_cast<i32>(lastPositionZ[iSaveCtr]);
    if (position >= 0
        && platform::FileSeek(m_aggregateFd[m_curAggregate], position) != position)
        ShutDown("Cannot restore an AGG member position.");
}

i8 resourceManager::ReadByte(void) {
    i8 value = 0;
    ReadBlock(&value, sizeof(value));
    return value;
}

i16 resourceManager::ReadWord(void) {
    u8 bytes[2]{};
    ReadBlock(bytes, sizeof(bytes));
    u16 value = 0;
    platform::binary::ReadU16(bytes, sizeof(bytes), 0, value);
    return std::bit_cast<i16>(value);
}

i32l resourceManager::ReadLong(void) {
    u8 bytes[4]{};
    ReadBlock(bytes, sizeof(bytes));
    u32 value = 0;
    platform::binary::ReadU32(bytes, sizeof(bytes), 0, value);
    return std::bit_cast<i32>(value);
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
    if (destination == nullptr
        || std::memchr(destination, 0, RESOURCE_MANAGER_READ13_BYTES) == nullptr)
        ShutDown("Unterminated resource name in an AGG member.");
}

void resourceManager::ReadBlock(void* destination, u32l size) {
    PollSound();
    const bool selected = m_curAggregate >= 0
        && m_curAggregate < RESOURCE_MANAGER_AGGREGATE_LIMIT
        && m_aggregateDir[m_curAggregate] != nullptr
        && m_curEntry >= 0 && m_curEntry < m_aggregateEntryCount[m_curAggregate];
    if (!selected || size > static_cast<u32l>(std::numeric_limits<i32>::max())
        || !resources::ReadAggMember(platform::Files(), m_aggregateFd[m_curAggregate],
            m_aggregateDir[m_curAggregate][m_curEntry], destination, static_cast<u32>(size))) {
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE,
            "Invalid or incomplete AGG member read: '%s'", m_lastFileName);
        ShutDown(gText);
        return;
    }
    PollSound();
}

i32 iSaveCtr = 0;
i32 lastAggZ[POSITION_STACK_DEPTH];
i32l lastPositionZ[POSITION_STACK_DEPTH];
