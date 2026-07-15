// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\RESMGR.OBJ   from: .\basewin.lib
// functions: 31   data: 4
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

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
VA(0x004c7fa0, 0xdb)
resourceManager::resourceManager(void) : baseManager()
{
    int aggregateIndex;
    m_active = 0;
    m_resourceListHead = 0;
    m_expunging = 0;
    strcpy(m_lastFileName, "");
    m_lastFileId = 0;
    for (aggregateIndex = 0; aggregateIndex < RESOURCE_MANAGER_AGGREGATE_LIMIT; aggregateIndex++) {
        m_aggregateFd[aggregateIndex] = RESOURCE_MANAGER_INVALID_FILE;
        m_aggregateDir[aggregateIndex] = 0;
        m_aggregateEntryCount[aggregateIndex] = 0;
    }
    m_numAggregates = 0;
    m_curAggregate = 0;
}

VA(0x004c8080, 0xa2)
void resourceManager::GetBackdrop(char *name, class bitmap *backdrop, int useIcon)
{
    if (useIcon) {
        icon *backdropIcon = GetIcon(name);
        backdropIcon->DrawToBuffer(0, 0, 0, 0);
        Dispose(backdropIcon);
    } else {
        PointToFile(MakeId(name, 1));
        ReadWord();
        ReadWord();
        ReadWord();
        ReadBlock(reinterpret_cast<signed char *>(backdrop->m_pixels),
                  backdrop->m_width * backdrop->m_height);
    }
}

// Complete /Od frame/slots and CFG. Before the semantic ResourceManagerConstant
// header enum this body was byte-exact. Under the combined TU state the first raw
// difference is the +0x46 branch displacement caused by a two-byte row-loop delta;
// the first instruction difference is +0x93, where retail evaluates imageHeight
// first and ours destinationY first. The row address similarly evaluates row*640
// versus the pixel base first. All 9 external targets and their order agree; the
// final ReadBlock relocation is consequently +0xbf versus retail +0xbd. Swapping
// both commutative source orders and spelling 640 directly do not alter the code.
// Revisit after shared RESMGR header state changes.
VA(0x004c8130, 0xd2)
void resourceManager::GetBackdropAtLoc(char *filename, class bitmap *destination,
                                       int destinationX, int destinationY,
                                       int useIcon)
{
    icon *backdropIcon;
    int dataWidth;
    int imageHeight;
    int row;
    if (useIcon != 0) {
        backdropIcon = GetIcon(filename);
        backdropIcon->DrawToBuffer(destinationX, destinationY, 0, 0);
        Dispose(backdropIcon);
    } else {
        PointToFile(MakeId(filename, 1));
        ReadWord();
        dataWidth = ReadWord();
        imageHeight = ReadWord();
        for (row = destinationY; row < destinationY + imageHeight; row++) {
            ReadBlock(reinterpret_cast<signed char *>(destination->m_pixels) +
                          (row * RESOURCE_MANAGER_BACKDROP_ROW_BYTES) + destinationX,
                      dataWidth);
        }
    }
}

VA(0x004c8210, 0x97)
class palette * resourceManager::GetPalette(char *name)
{
    unsigned long id = MakeId(name, 1);
    resource *r = Query(id);
    if (r != 0) {
        r->m_refCount++;
        return static_cast<palette *>(r);
    } else {
        r = new palette(id);
        AddResource(r);
        return static_cast<palette *>(r);
    }
}

VA(0x004c82b0, 0x97)
class bitmap * resourceManager::GetBitmap(char *name)
{
    unsigned long id = MakeId(name, 1);
    resource *r = Query(id);
    if (r != 0) {
        r->m_refCount++;
        return static_cast<bitmap *>(r);
    } else {
        r = new bitmap(id);
        AddResource(r);
        return static_cast<bitmap *>(r);
    }
}

VA(0x004c8350, 0x2f)
class icon * resourceManager::GetIcon(char *name) { return GetIcon(MakeId(name, 1)); }

VA(0x004c8380, 0x86)
class icon *resourceManager::GetIcon(unsigned long resourceId)
{
    icon *iconPointer = static_cast<icon *>(Query(resourceId));
    if (iconPointer != 0) {
        iconPointer->m_refCount++;
        return iconPointer;
    } else {
        iconPointer = new icon(resourceId);
        AddResource(iconPointer);
        return iconPointer;
    }
}

VA(0x004c8410, 0x97)
class tileset * resourceManager::GetTileset(char *name)
{
    unsigned long id = MakeId(name, 1);
    resource *r = Query(id);
    if (r != 0) {
        r->m_refCount++;
        return static_cast<tileset *>(r);
    } else {
        r = new tileset(id);
        AddResource(r);
        return static_cast<tileset *>(r);
    }
}

VA(0x004c84b0, 0x1a)
class mouse * resourceManager::GetMouse(char *) { return 0; }

VA(0x004c84d0, 0x97)
class font * resourceManager::GetFont(char *name)
{
    unsigned long resourceId = MakeId(name, 1);
    resource *fontEntry = Query(resourceId);
    if (fontEntry != 0) {
        fontEntry->m_refCount++;
        return static_cast<font *>(fontEntry);
    } else {
        fontEntry = new font(resourceId);
        AddResource(fontEntry);
        return static_cast<font *>(fontEntry);
    }
}

VA(0x004c8570, 0x9d)
class sample *resourceManager::GetSample(char *name)
{
    unsigned long id = MakeId(name, 1);
    resource *r = Query(id);
    if (r != 0) {
        r->m_refCount++;
        return static_cast<sample *>(r);
    } else {
        r = new sample(name, 0, 0x7f, 1);
        AddResource(r);
        return static_cast<sample *>(r);
    }
}

VA(0x004c8610, 0x97)
class MIDIWrap *resourceManager::GetMIDIWrap(char *name)
{
    unsigned long id = MakeId(name, 1);
    resource *r = Query(id);
    if (r != 0) {
        r->m_refCount++;
        return static_cast<MIDIWrap *>(r);
    } else {
        r = new MIDIWrap(name);
        AddResource(r);
        return static_cast<MIDIWrap *>(r);
    }
}

VA(0x004c86b0, 0x87)
void resourceManager::Dispose(class resource *resourceToDispose)
{
    if (m_expunging != 0)
        return;
    if (resourceToDispose != 0) {
        resourceToDispose->m_refCount--;
        if (resourceToDispose->m_refCount > 0) {
            return;
        } else {
            RemoveResource(resourceToDispose);
            delete resourceToDispose;
        }
    }
}

VA(0x004c8740, 0x55)
void resourceManager::AddResource(class resource *newResource)
{
    if (m_resourceListHead == 0) {
        m_resourceListHead = newResource;
        m_resourceListHead->m_next = 0;
    } else {
        newResource->m_next = m_resourceListHead;
        m_resourceListHead = newResource;
    }
}

VA(0x004c87a0, 0x8b)
void resourceManager::Expunge(void)
{
    m_expunging = 1;
    resource *currentResource = m_resourceListHead;
    resource *nextResource = 0;
    while (currentResource != 0) {
        nextResource = currentResource->m_next;
        RemoveResource(currentResource);
        delete currentResource;
        currentResource = nextResource;
    }
    m_expunging = 0;
}

VA(0x004c8830, 0x4b)
class resource *resourceManager::Query(unsigned long resourceId)
{
    resource *cursorResource = m_resourceListHead;
    while (cursorResource != 0 && cursorResource->m_id != resourceId) {
        cursorResource = cursorResource->m_next;
    }
    return cursorResource;
}

VA(0x004c8880, 0x1a)
int resourceManager::Main(struct tag_message &) { return RESOURCE_MANAGER_SUCCESS; }

VA(0x004c88a0, 0xab)
int resourceManager::Open(int priority)
{
    if (LoadAggregateHeader(EXPANSION_AGGREGATE_NAME) != RESOURCE_MANAGER_SUCCESS)
        return RESOURCE_MANAGER_ERROR;
    if (LoadAggregateHeader(DEFAULT_AGGREGATE_NAME) != RESOURCE_MANAGER_SUCCESS)
        return RESOURCE_MANAGER_ERROR;
    m_messageMask = RESOURCE_MANAGER_MESSAGE_MASK;
    m_priority = priority;
    m_active = 1;
    strcpy(m_name, "resourceManager");
    m_resourceListHead = 0;
    return RESOURCE_MANAGER_SUCCESS;
}

VA(0x004c8950, 0x88)
void resourceManager::RemoveResource(class resource *resourceToRemove)
{
    if (m_resourceListHead == resourceToRemove) {
        m_resourceListHead = resourceToRemove->m_next;
        return;
    }
    resource *previousResource = m_resourceListHead;
    while (previousResource != 0 && previousResource->m_next != resourceToRemove) {
        previousResource = previousResource->m_next;
    }
    if (previousResource == 0) {
        return;
    } else {
        previousResource->m_next = resourceToRemove->m_next;
    }
}

// @early-stop
// Relocation-masked code bytes agree. The original source-filename literal has a
// local $SG identity instead of retail's named string constant; all 4 ordered
// relocation offsets, types, runtime targets, and addends agree.
VA(0x004c89e0, 0xc8)
void resourceManager::Close(void)
{
    int aggregateIndex;
    if (m_active != 1)
        return;
    Expunge();
    m_resourceListHead = 0;
    for (aggregateIndex = 0; aggregateIndex < RESOURCE_MANAGER_AGGREGATE_LIMIT; aggregateIndex++) {
        if (m_aggregateDir[aggregateIndex] != 0)
            H2_FREE(m_aggregateDir[aggregateIndex], "I:\\Projects\\Heroes\\Prog\\BASE\\RESMGR.CPP", 0x1da);
        if (m_aggregateFd[aggregateIndex] != RESOURCE_MANAGER_INVALID_FILE) {
            _close(m_aggregateFd[aggregateIndex]);
            m_aggregateFd[aggregateIndex] = RESOURCE_MANAGER_INVALID_FILE;
        }
    }
    m_numAggregates = 0;
    m_active = 0;
}

// @early-stop
// Relocation-masked code, frame/slots, and CFG are exact. Only the two diagnostic
// format strings and original source-filename literal have local $SG identities
// instead of retail named constants; all 15 ordered offsets/types/runtime targets
// and addends agree.
VA(0x004c8ab0, 0x143)
int resourceManager::LoadAggregateHeader(char *aggregateName)
{
    short fileCountBuffer[2];
    int aggregateFile;
    unsigned int directoryBytes;
    if (m_numAggregates >= RESOURCE_MANAGER_AGGREGATE_LIMIT) {
        sprintf(gText, "Only %d AGG files can be used at once", RESOURCE_MANAGER_AGGREGATE_LIMIT);
        ShutDown(gText);
        return RESOURCE_MANAGER_ERROR;
    }
    aggregateFile = _open(aggregateName, RESOURCE_MANAGER_BINARY_OPEN_MODE);
    if (aggregateFile == RESOURCE_MANAGER_INVALID_FILE) {
        sprintf(gText, "Can't open file '%s'", aggregateName);
        ShutDown(gText);
        return RESOURCE_MANAGER_ERROR;
    }
    m_curAggregate = m_numAggregates;
    m_numAggregates = m_numAggregates + 1;
    m_aggregateFd[m_curAggregate] = aggregateFile;
    _read(m_aggregateFd[m_curAggregate], fileCountBuffer, sizeof(short));
    m_aggregateEntryCount[m_curAggregate] = fileCountBuffer[0];
    directoryBytes = m_aggregateEntryCount[m_curAggregate] * RESOURCE_MANAGER_ENTRY_BYTES;
    m_aggregateDir[m_curAggregate] = static_cast<aggEntry *>(H2_ALLOC(directoryBytes, "I:\\Projects\\Heroes\\Prog\\BASE\\RESMGR.CPP", 0x21e));
    _read(m_aggregateFd[m_curAggregate], m_aggregateDir[m_curAggregate], directoryBytes);
    return RESOURCE_MANAGER_SUCCESS;
}

VA(0x004c8c00, 0x11c)
void resourceManager::PointToFile(unsigned long fileId)
{
    char isFound = 0;
    int entryIndex;
    int aggregateIndex;
    for (aggregateIndex = 0; aggregateIndex < RESOURCE_MANAGER_AGGREGATE_LIMIT; aggregateIndex++) {
        if (m_aggregateDir[aggregateIndex] != 0) {
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
        sprintf(gText,
                "ResMgr::PointToFile failure!  ThisFileId:%d  LastFileId:%d  LastFileName:%s",
                fileId, m_lastFileId, m_lastFileName);
        ShutDown(gText);
    }
    long ignoredPosition = _lseek(m_aggregateFd[m_curAggregate],
                                  m_aggregateDir[m_curAggregate][entryIndex].offset, 0);
}

VA(0x004c8d20, 0xfa)
unsigned long resourceManager::GetFileSize(unsigned long fileId)
{
    char isFound = 0;
    int entryIndex;
    int matchedAggregate;
    int fileIndex;
    for (fileIndex = 0; fileIndex < RESOURCE_MANAGER_AGGREGATE_LIMIT; fileIndex++) {
        if (m_aggregateDir[fileIndex] != 0) {
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
        sprintf(gText,
                "ResMgr::PointToFile failure!  ThisFileId:%d  LastFileId:%d  LastFileName:%s",
                fileId, m_lastFileId, m_lastFileName);
        ShutDown(gText);
    }
    return m_aggregateDir[matchedAggregate][entryIndex].size;
}

VA(0x004c8e20, 0x52)
void resourceManager::SavePosition(void)
{
    lastPositionZ[iSaveCtr] = _tell(m_aggregateFd[m_curAggregate]);
    lastAggZ[iSaveCtr] = m_curAggregate;
    iSaveCtr = iSaveCtr + 1;
}

VA(0x004c8e80, 0x53)
void resourceManager::RestorePosition(void)
{
    iSaveCtr = iSaveCtr - 1;
    m_curAggregate = lastAggZ[iSaveCtr];
    _lseek(m_aggregateFd[m_curAggregate], lastPositionZ[iSaveCtr], 0);
}

VA(0x004c8ee0, 0x81)
signed char resourceManager::ReadByte(void)
{
    H2_ASSERT(m_aggregateFd[m_curAggregate] != RESOURCE_MANAGER_INVALID_FILE, "I:\\Projects\\Heroes\\Prog\\BASE\\RESMGR.CPP", 0x2bf);
    signed char value = 0;
    int bytesRead = _read(m_aggregateFd[m_curAggregate], &value, sizeof(value));
    if (bytesRead == 0) {
        int errorCode = errno;
        int debugTrap = 0;
        debugTrap++;
        debugTrap++;
    }
    return value;
}

VA(0x004c8f70, 0x84)
short resourceManager::ReadWord(void)
{
    H2_ASSERT(m_aggregateFd[m_curAggregate] != RESOURCE_MANAGER_INVALID_FILE, "I:\\Projects\\Heroes\\Prog\\BASE\\RESMGR.CPP", 0x2dc);
    short value = 0;
    int bytesRead = _read(m_aggregateFd[m_curAggregate], &value, sizeof(value));
    if (bytesRead == 0) {
        int errorCode = errno;
        int debugTrap = 0;
        debugTrap++;
        debugTrap++;
    }
    return value;
}

VA(0x004c9000, 0x84)
long resourceManager::ReadLong(void)
{
    H2_ASSERT(m_aggregateFd[m_curAggregate] != RESOURCE_MANAGER_INVALID_FILE, "I:\\Projects\\Heroes\\Prog\\BASE\\RESMGR.CPP", 0x2f8);
    long value = 0;
    int bytesRead = _read(m_aggregateFd[m_curAggregate], &value, sizeof(value));
    if (bytesRead == 0) {
        int errorCode = errno;
        int debugTrap = 0;
        debugTrap++;
        debugTrap++;
    }
    return value;
}

VA(0x004c9090, 0xe3)
unsigned long resourceManager::MakeId(char *name, int translate)
{
    strcpy(m_lastFileName, name);
    if (gbUseEvilInterface != 0 && translate != 0) {
        for (int translatedIndex = 0; translatedIndex < RESOURCE_MANAGER_EVIL_TRANSLATION_COUNT; translatedIndex++) {
            if (_stricmp(m_lastFileName, cEvilTranslate[translatedIndex][0]) == 0)
                strcpy(m_lastFileName, cEvilTranslate[translatedIndex][1]);
        }
    }
    unsigned long result = MAKEFILEID(m_lastFileName);
    m_lastFileId = result;
    return result;
}

VA(0x004c9180, 0x26)
void resourceManager::Read13(signed char *destination)
{
    ReadBlock(destination, RESOURCE_MANAGER_READ13_BYTES);
}

// @early-stop
// Relocation-masked code is exact, including the retail comparison load order,
// frame/slots, CFG, and all 12 ordered relocation offsets/types/runtime targets.
// Only the assert filename and file-error format have local $SG identities instead
// of retail's named string constants.
VA(0x004c91b0, 0xbd)
void resourceManager::ReadBlock(signed char *destination, unsigned long size)
{
    H2_ASSERT(m_aggregateFd[m_curAggregate] != RESOURCE_MANAGER_INVALID_FILE, "I:\\Projects\\Heroes\\Prog\\BASE\\RESMGR.CPP", 0x330);
    PollSound();
    int bytesRead = _read(m_aggregateFd[m_curAggregate], destination, size);
    if (bytesRead != size) {
        int errorCode = errno;
        sprintf(gText,
                "File error - bytes read %d, bytes requested %d, errno %d, last file '%s'",
                bytesRead, size, errno, m_lastFileName);
        LogStr(gText);
        int debugTrap = 0;
        debugTrap++;
        debugTrap++;
    }
    PollSound();
}


// ===== vtable resourceManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x004c88a0, 0xab)  int resourceManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004c89e0, 0xc8)  void resourceManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x004c8880, 0x1a)  int resourceManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(resourceManager, 0x004eb9f0);

// ---- globals (definitions, RVA order) ----
DATA(0x0051e99c) int iSaveCtr;
DATA(0x005331e8) int lastAggZ[10];
DATA(0x00533210) long lastPositionZ[12];
