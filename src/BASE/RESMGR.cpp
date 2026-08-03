#include <va.h>
#include <io.h>
#include <string.h>
#include <SOURCE/KB.h>
#include <stdio.h>
#include <errno.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/baseManager.h>
#include <BASE/resource.h>
#include <BASE/icon.h>
#include <BASE/MIDIWrap.h>
#include <BASE/sample.h>
#include <BASE/tileset.h>
#include <BASE/font.h>
#include <BASE/bitmap.h>
#include <BASE/palette.h>

#define RETAIL_FILE "e:\\Users\\igorl\\VSS\\HMM\\HMM2\\Source\\Base\\RESMGR.CPP"

H2_ENUM_BEGIN(ResourceConstant)
    INVALID_FILE            = -1,
    LOAD_SUCCESS            = 0,
    LOAD_ERROR              = 3,
    ENTRY_BYTES             = 0xc,
    EVIL_TRANSLATION_COUNT  = 37,
    BACKDROP_ROW_BYTES      = 640,
    BINARY_OPEN_MODE        = 0x8000,
    SAMPLE_VOLUME           = 127,
    FILE_COUNT_BUFFER_WORDS = 2,
    POSITION_STACK_DEPTH    = 10
H2_ENUM_END(ResourceConstant)

VA(0x004b7ee0, 0xb8)
resourceManager::resourceManager(void) : baseManager() {
    i32 aggregateIndex;
    m_active = false;
    m_resourceListHead = NULL;
    m_expunging = 0;
    strcpy(m_lastFileName, "");
    m_lastFileId = 0;
    for (aggregateIndex = 0; aggregateIndex < RESOURCE_MANAGER_AGGREGATE_LIMIT; aggregateIndex++) {
        m_aggregateFd[aggregateIndex] = INVALID_FILE;
        m_aggregateDir[aggregateIndex] = NULL;
        m_aggregateEntryCount[aggregateIndex] = 0;
    }
    m_numAggregates = 0;
    m_curAggregate = 0;
}

VA(0x004b7fa0, 0x92)
void resourceManager::GetBackdrop(char* name, class bitmap* backdrop, i32 useIcon) {
    if (useIcon) {
        icon* backdropIcon = GetIcon(name);
        backdropIcon->DrawToBuffer(0, 0, 0, ICON_DRAW_NORMAL);
        Dispose(backdropIcon);
    } else {
        PointToFile(MakeId(name, 1));
        ReadWord();
        ReadWord();
        ReadWord();
        ReadBlock(
            reinterpret_cast<i8*>(backdrop->m_pixels),
            backdrop->m_width * backdrop->m_height
        );
    }
}

VA(0x004b8040, 0xc0)
void resourceManager::GetBackdropAtLoc(
    char* filename,
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
                (curRow * BACKDROP_ROW_BYTES)
                    + reinterpret_cast<i8*>(destination->m_pixels) + destinationX,
                width
            );
        }
    }
}

VA(0x004b8100, 0xbf)
class palette* resourceManager::GetPalette(char* name) {
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

VA(0x004b81c0, 0xbf)
class bitmap* resourceManager::GetBitmap(char* name) {
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

VA(0x004b8280, 0x24)
class icon* resourceManager::GetIcon(char* name) {
    return GetIcon(MakeId(name, 1));
}

VA(0x004b82b0, 0xae)
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

VA(0x004b8360, 0xbf)
class tileset* resourceManager::GetTileset(char* name) {
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

VA(0x004b8420, 0xf)
class mouse* resourceManager::GetMouse(char*) {
    return NULL;
}

VA(0x004b8430, 0xbf)
class font* resourceManager::GetFont(char* name) {
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

VA(0x004b84f0, 0xbf)
class sample* resourceManager::GetSample(char* name) {
    u32l id = MakeId(name, 1);
    resource* r = Query(id);
    if (r != NULL) {
        r->m_refCount++;
        return static_cast<sample*>(r);
    } else {
        r = new sample(name, 0, SAMPLE_VOLUME, 1);
        AddResource(r);
        return static_cast<sample*>(r);
    }
}

VA(0x004b85b0, 0xbf)
class MIDIWrap* resourceManager::GetMIDIWrap(char* name) {
    u32l fileId = MakeId(name, 1);
    resource* r = Query(fileId);
    if (r != NULL) {
        r->m_refCount++;
        return static_cast<MIDIWrap*>(r);
    } else {
        r = new MIDIWrap(name);
        AddResource(r);
        return static_cast<MIDIWrap*>(r);
    }
}

VA(0x004b8670, 0x75)
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

VA(0x004b86f0, 0x43)
void resourceManager::AddResource(class resource* newResource) {
    if (m_resourceListHead == NULL) {
        m_resourceListHead = newResource;
        m_resourceListHead->m_next = NULL;
    } else {
        newResource->m_next = m_resourceListHead;
        m_resourceListHead = newResource;
    }
}

VA(0x004b8740, 0x7e)
void resourceManager::Expunge(void) {
    m_expunging = 1;
    resource* cur = m_resourceListHead;
    resource* next = NULL;
    while (cur != NULL) {
        next = cur->m_next;
        RemoveResource(cur);
        delete cur;
        cur = next;
    }
    m_expunging = 0;
}

VA(0x004b87c0, 0x37)
class resource* resourceManager::Query(u32l resourceId) {
    resource* cursorResource = m_resourceListHead;
    while (cursorResource != NULL && cursorResource->m_id != resourceId) {
        cursorResource = cursorResource->m_next;
    }
    return cursorResource;
}

VA(0x004b8800, 0xf)
MessageDispatchResult resourceManager::Main(struct tag_message&) {
    return MESSAGE_DISPATCH_CONTINUE;
}

VA(0x004b8810, 0x7d)
i32 resourceManager::Open(i32 priority) {
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

VA(0x004b8890, 0x61)
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

VA(0x004b8900, 0xaf)
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
            close(m_aggregateFd[aggregateIndex]);
            m_aggregateFd[aggregateIndex] = INVALID_FILE;
        }
    }
    m_numAggregates = 0;
    m_active = false;
}

VA(0x004b89b0, 0x138)
i32 resourceManager::LoadAggregateHeader(char* aggregateName) {
    i16 fpCountBuffer[FILE_COUNT_BUFFER_WORDS];
    i32 aggregateFp;
    u32 directoryBytes;
    if (m_numAggregates >= RESOURCE_MANAGER_AGGREGATE_LIMIT) {
        sprintf(gText, "Only %d .AGG files can be used at once.", RESOURCE_MANAGER_AGGREGATE_LIMIT);
        ShutDown(gText);
        return LOAD_ERROR;
    }
    aggregateFp = open(aggregateName, BINARY_OPEN_MODE);
    if (aggregateFp == INVALID_FILE) {
        sprintf(gText, "Can't open file: %s", aggregateName);
        ShutDown(gText);
        return LOAD_ERROR;
    }
    m_curAggregate = m_numAggregates;
    m_numAggregates = m_numAggregates + 1;
    m_aggregateFd[m_curAggregate] = aggregateFp;
    read(m_aggregateFd[m_curAggregate], fpCountBuffer, sizeof(i16));
    m_aggregateEntryCount[m_curAggregate] = fpCountBuffer[0];
    directoryBytes = m_aggregateEntryCount[m_curAggregate] * ENTRY_BYTES;
    m_aggregateDir[m_curAggregate] = static_cast<aggEntry*>(H2_ALLOC(directoryBytes));
    read(m_aggregateFd[m_curAggregate], m_aggregateDir[m_curAggregate], directoryBytes);
    return LOAD_SUCCESS;
}

VA(0x004b8af0, 0xf8)
void resourceManager::PointToFile(u32l fileId) {
    char found = 0;
    i32 entry;
    i32 i;
    for (i = 0; i < RESOURCE_MANAGER_AGGREGATE_LIMIT; i++) {
        if (m_aggregateDir[i] != NULL) {
            entry = 0;
            while (entry < m_aggregateEntryCount[i]) {
                if (m_aggregateDir[i][entry].id == fileId) {
                    found = 1;
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
        sprintf(
            gText,
            "ResMgr::PointToFile failure!  ThisFileId:%d  LastFileId:%d  LastFileName:%s",
            fileId,
            m_lastFileId,
            m_lastFileName
        );
        ShutDown(gText);
    }
    i32l position =
        lseek(m_aggregateFd[m_curAggregate], m_aggregateDir[m_curAggregate][entry].offset, 0);
}

VA(0x004b8bf0, 0xd6)
u32l resourceManager::GetFileSize(u32l fileId) {
    char found = 0;
    i32 entry;
    i32 matched;
    i32 i;
    for (i = 0; i < RESOURCE_MANAGER_AGGREGATE_LIMIT; i++) {
        if (m_aggregateDir[i] != NULL) {
            entry = 0;
            while (entry < m_aggregateEntryCount[i]) {
                if (m_aggregateDir[i][entry].id == fileId) {
                    found = 1;
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
        sprintf(
            gText,
            "ResMgr::PointToFile failure!  ThisFileId:%d  LastFileId:%d  LastFileName:%s",
            fileId,
            m_lastFileId,
            m_lastFileName
        );
        ShutDown(gText);
    }
    return m_aggregateDir[matched][entry].size;
}

VA(0x004b8cd0, 0x50)
void resourceManager::SavePosition(void) {
    lastPositionZ[iSaveCtr] = tell(m_aggregateFd[m_curAggregate]);
    lastAggZ[iSaveCtr] = m_curAggregate;
    iSaveCtr = iSaveCtr + 1;
}

VA(0x004b8d20, 0x51)
void resourceManager::RestorePosition(void) {
    iSaveCtr = iSaveCtr - 1;
    m_curAggregate = lastAggZ[iSaveCtr];
    lseek(m_aggregateFd[m_curAggregate], lastPositionZ[iSaveCtr], 0);
}

VA(0x004b8d80, 0x59)
i8 resourceManager::ReadByte(void) {
    H2_ASSERT(m_aggregateFd[m_curAggregate] != INVALID_FILE, RETAIL_FILE, 675);
    i8 value = 0;
    i32 result = read(m_aggregateFd[m_curAggregate], &value, sizeof(value));
    return value;
}

VA(0x004b8de0, 0x5c)
i16 resourceManager::ReadWord(void) {
    H2_ASSERT(m_aggregateFd[m_curAggregate] != INVALID_FILE, RETAIL_FILE, 700);
    i16 value = 0;
    i32 result = read(m_aggregateFd[m_curAggregate], &value, sizeof(value));
    return value;
}

VA(0x004b8e40, 0x5c)
i32l resourceManager::ReadLong(void) {
    H2_ASSERT(m_aggregateFd[m_curAggregate] != INVALID_FILE, RETAIL_FILE, 723);
    i32l value = 0;
    i32 result = read(m_aggregateFd[m_curAggregate], &value, sizeof(value));
    return value;
}

VA(0x004b8ea0, 0xa0)
u32l resourceManager::MakeId(char* name, i32 translate) {
    strcpy(m_lastFileName, name);
    if (gbUseEvilInterface != 0 && translate != 0) {
        for (i32 translatedIndex = 0; translatedIndex < EVIL_TRANSLATION_COUNT;
             translatedIndex++) {
            if (strcmpi(m_lastFileName, cEvilTranslate[translatedIndex][0]) == 0)
                strcpy(m_lastFileName, cEvilTranslate[translatedIndex][1]);
        }
    }
    u32l result = MAKEFILEID(m_lastFileName);
    m_lastFileId = result;
    return result;
}

VA(0x004b8f40, 0x1b)
void resourceManager::Read13(i8* destination) {
    ReadBlock(destination, RESOURCE_MANAGER_READ13_BYTES);
}

VA(0x004b8f60, 0x9b)
void resourceManager::ReadBlock(i8* destination, u32l size) {
    H2_ASSERT(m_aggregateFd[m_curAggregate] != INVALID_FILE, RETAIL_FILE, 775);
    PollSound();
    i32 bytesRead = read(m_aggregateFd[m_curAggregate], destination, size);
    if (bytesRead != size) {
        i32 errorCode = errno;
        sprintf(
            gText,
            "File error - bytes read %d, bytes requested %d, errno %d, last file '%s'",
            bytesRead,
            size,
            errno,
            m_lastFileName
        );
        LogStr(gText);
        i32 debugTrap = 0;
        debugTrap++;
        debugTrap++;
    }
    PollSound();
}




i32 iSaveCtr = 0;
i32 lastAggZ[POSITION_STACK_DEPTH];
i32l lastPositionZ[POSITION_STACK_DEPTH];

