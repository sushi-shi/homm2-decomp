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

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\RESMGR.CPP"
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

void resourceManager::GetBackdropAtLoc(
    char* filename,
    class bitmap* destination,
    i32 destinationX,
    i32 destinationY,
    i32 useIcon
) {
    icon* backdropIcon;
    i32 dataWidth;
    i32 imageHeight;
    i32 row;
    if (useIcon != 0) {
        backdropIcon = GetIcon(filename);
        backdropIcon->DrawToBuffer(destinationX, destinationY, 0, ICON_DRAW_NORMAL);
        Dispose(backdropIcon);
    } else {
        PointToFile(MakeId(filename, 1));
        ReadWord();
        dataWidth = ReadWord();
        imageHeight = ReadWord();
        for (row = destinationY; row < imageHeight + destinationY; row++) {
            ReadBlock(
                (row * BACKDROP_ROW_BYTES)
                    + reinterpret_cast<i8*>(destination->m_pixels) + destinationX,
                dataWidth
            );
        }
    }
}

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

class bitmap* resourceManager::GetBitmap(char* name) {
    u32l id = MakeId(name, 1);
    resource* r = Query(id);
    if (r != NULL) {
        r->m_refCount++;
        return static_cast<bitmap*>(r);
    } else {
        r = new bitmap(id);
        AddResource(r);
        return static_cast<bitmap*>(r);
    }
}

class icon* resourceManager::GetIcon(char* name) {
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

class mouse* resourceManager::GetMouse(char*) {
    return NULL;
}

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

class MIDIWrap* resourceManager::GetMIDIWrap(char* name) {
    u32l id = MakeId(name, 1);
    resource* r = Query(id);
    if (r != NULL) {
        r->m_refCount++;
        return static_cast<MIDIWrap*>(r);
    } else {
        r = new MIDIWrap(name);
        AddResource(r);
        return static_cast<MIDIWrap*>(r);
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
    m_expunging = 1;
    resource* currentResource = m_resourceListHead;
    resource* nextResource = NULL;
    while (currentResource != NULL) {
        nextResource = currentResource->m_next;
        RemoveResource(currentResource);
        delete currentResource;
        currentResource = nextResource;
    }
    m_expunging = 0;
}

class resource* resourceManager::Query(u32l resourceId) {
    resource* cursorResource = m_resourceListHead;
    while (cursorResource != NULL && cursorResource->m_id != resourceId) {
        cursorResource = cursorResource->m_next;
    }
    return cursorResource;
}

MessageDispatchResult resourceManager::Main(struct tag_message&) {
    return MESSAGE_DISPATCH_CONTINUE;
}

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
            H2_FREE_AT(m_aggregateDir[aggregateIndex], RETAIL_FILE, 0x1da);
        if (m_aggregateFd[aggregateIndex] != INVALID_FILE) {
            close(m_aggregateFd[aggregateIndex]);
            m_aggregateFd[aggregateIndex] = INVALID_FILE;
        }
    }
    m_numAggregates = 0;
    m_active = false;
}

i32 resourceManager::LoadAggregateHeader(char* aggregateName) {
    i16 fileCountBuffer[FILE_COUNT_BUFFER_WORDS];
    i32 aggregateFile;
    u32 directoryBytes;
    if (m_numAggregates >= RESOURCE_MANAGER_AGGREGATE_LIMIT) {
        sprintf(gText, "Only %d .AGG files can be used at once.", RESOURCE_MANAGER_AGGREGATE_LIMIT);
        ShutDown(gText);
        return LOAD_ERROR;
    }
    aggregateFile = open(aggregateName, BINARY_OPEN_MODE);
    if (aggregateFile == INVALID_FILE) {
        sprintf(gText, "Can't open file: %s", aggregateName);
        ShutDown(gText);
        return LOAD_ERROR;
    }
    m_curAggregate = m_numAggregates;
    m_numAggregates = m_numAggregates + 1;
    m_aggregateFd[m_curAggregate] = aggregateFile;
    read(m_aggregateFd[m_curAggregate], fileCountBuffer, sizeof(i16));
    m_aggregateEntryCount[m_curAggregate] = fileCountBuffer[0];
    directoryBytes = m_aggregateEntryCount[m_curAggregate] * ENTRY_BYTES;
    m_aggregateDir[m_curAggregate] = static_cast<aggEntry*>(H2_ALLOC_AT(directoryBytes, RETAIL_FILE, 542));
    read(m_aggregateFd[m_curAggregate], m_aggregateDir[m_curAggregate], directoryBytes);
    return LOAD_SUCCESS;
}

void resourceManager::PointToFile(u32l fileId) {
    char isFound = 0;
    i32 entryIndex;
    i32 aggregateIndex;
    for (aggregateIndex = 0; aggregateIndex < RESOURCE_MANAGER_AGGREGATE_LIMIT; aggregateIndex++) {
        if (m_aggregateDir[aggregateIndex] != NULL) {
            entryIndex = 0;
            while (entryIndex < m_aggregateEntryCount[aggregateIndex]) {
                if (m_aggregateDir[aggregateIndex][entryIndex].id == fileId) {
                    isFound = 1;
                    m_curAggregate = aggregateIndex;
                    break;
                }
                entryIndex++;
            }
        }
        if (isFound)
            break;
    }
    if (!isFound) {
        sprintf(
            gText,
            "ResMgr::PointToFile failure!  ThisFileId:%d  LastFileId:%d  LastFileName:%s",
            fileId,
            m_lastFileId,
            m_lastFileName
        );
        ShutDown(gText);
    }
    i32l ignoredPosition =
        lseek(m_aggregateFd[m_curAggregate], m_aggregateDir[m_curAggregate][entryIndex].offset, 0);
}

u32l resourceManager::GetFileSize(u32l fileId) {
    char isFound = 0;
    i32 entryIndex;
    i32 matchedAggregate;
    i32 fileIndex;
    for (fileIndex = 0; fileIndex < RESOURCE_MANAGER_AGGREGATE_LIMIT; fileIndex++) {
        if (m_aggregateDir[fileIndex] != NULL) {
            entryIndex = 0;
            while (entryIndex < m_aggregateEntryCount[fileIndex]) {
                if (m_aggregateDir[fileIndex][entryIndex].id == fileId) {
                    isFound = 1;
                    matchedAggregate = fileIndex;
                    break;
                }
                entryIndex++;
            }
        }
        if (isFound)
            break;
    }
    if (!isFound) {
        sprintf(
            gText,
            "ResMgr::PointToFile failure!  ThisFileId:%d  LastFileId:%d  LastFileName:%s",
            fileId,
            m_lastFileId,
            m_lastFileName
        );
        ShutDown(gText);
    }
    return m_aggregateDir[matchedAggregate][entryIndex].size;
}

void resourceManager::SavePosition(void) {
    lastPositionZ[iSaveCtr] = tell(m_aggregateFd[m_curAggregate]);
    lastAggZ[iSaveCtr] = m_curAggregate;
    iSaveCtr = iSaveCtr + 1;
}

void resourceManager::RestorePosition(void) {
    iSaveCtr = iSaveCtr - 1;
    m_curAggregate = lastAggZ[iSaveCtr];
    lseek(m_aggregateFd[m_curAggregate], lastPositionZ[iSaveCtr], 0);
}

i8 resourceManager::ReadByte(void) {
    H2_ASSERT(m_aggregateFd[m_curAggregate] != INVALID_FILE, RETAIL_FILE, 703);
    i8 value = 0;
    i32 bytesRead = read(m_aggregateFd[m_curAggregate], &value, sizeof(value));
    if (bytesRead == 0) {
        i32 errorCode = errno;
        i32 debugTrap = 0;
        debugTrap++;
        debugTrap++;
    }
    return value;
}

i16 resourceManager::ReadWord(void) {
    H2_ASSERT(m_aggregateFd[m_curAggregate] != INVALID_FILE, RETAIL_FILE, 732);
    i16 value = 0;
    i32 bytesRead = read(m_aggregateFd[m_curAggregate], &value, sizeof(value));
    if (bytesRead == 0) {
        i32 errorCode = errno;
        i32 debugTrap = 0;
        debugTrap++;
        debugTrap++;
    }
    return value;
}

i32l resourceManager::ReadLong(void) {
    H2_ASSERT(m_aggregateFd[m_curAggregate] != INVALID_FILE, RETAIL_FILE, 760);
    i32l value = 0;
    i32 bytesRead = read(m_aggregateFd[m_curAggregate], &value, sizeof(value));
    if (bytesRead == 0) {
        i32 errorCode = errno;
        i32 debugTrap = 0;
        debugTrap++;
        debugTrap++;
    }
    return value;
}

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

void resourceManager::Read13(i8* destination) {
    ReadBlock(destination, RESOURCE_MANAGER_READ13_BYTES);
}

void resourceManager::ReadBlock(i8* destination, u32l size) {
    H2_ASSERT(m_aggregateFd[m_curAggregate] != INVALID_FILE, RETAIL_FILE, 816);
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

#undef RETAIL_FILE
