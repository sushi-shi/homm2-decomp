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
#include <_globals_model.h>
// @early-stop
// reloc-masked: code bytes agree; the manager-name literal is $SG31246 vs retail const_0011e9a0.
VA(0x004c7fa0, 0xdb)
resourceManager::resourceManager(void) : baseManager()
{
    int aggregateIndex;
    m_active = 0;
    m_resourceListHead = 0;
    m_expunging = 0;
    strcpy(name, "resourceManager");
    m_lastFileId = 0;
    for (aggregateIndex = 0; aggregateIndex < 2; aggregateIndex++) {
        m_aggregateFd[aggregateIndex] = -1;
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
        for (row = destinationY; row < (imageHeight | 0) + destinationY; row++) {
            ReadBlock(((row * 640) | 0) +
                          reinterpret_cast<signed char *>(destination->m_pixels) + destinationX,
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
class icon *resourceManager::GetIcon(unsigned long param_1)
{
    resource *local_8 = Query(param_1);
    if (local_8 != 0) {
        local_8->m_refCount++;
        return static_cast<icon *>(local_8);
    } else {
        local_8 = new icon(param_1);
        AddResource(local_8);
        return static_cast<icon *>(local_8);
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

// @early-stop
// reloc-masked: all 54 instructions and all 5 relocation targets agree; only delink span identity remains.
VA(0x004c84d0, 0x97)
class font * resourceManager::GetFont(char *name)
{
    unsigned long id = MakeId(name, 1);
    resource *r = Query(id);
    if (r != 0) {
        r->m_refCount++;
        return static_cast<font *>(r);
    } else {
        r = new font(id);
        AddResource(r);
        return static_cast<font *>(r);
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
void resourceManager::Dispose(class resource *param_1)
{
    if (m_expunging != 0)
        return;
    if (param_1 != 0) {
        param_1->m_refCount--;
        if (param_1->m_refCount > 0) {
            return;
        } else {
            RemoveResource(param_1);
            delete param_1;
        }
    }
}

VA(0x004c8740, 0x55)
void resourceManager::AddResource(class resource *param_1)
{
    if (m_resourceListHead == 0) {
        m_resourceListHead = param_1;
        m_resourceListHead->m_next = 0;
    } else {
        param_1->m_next = m_resourceListHead;
        m_resourceListHead = param_1;
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
class resource *resourceManager::Query(unsigned long param_1)
{
    resource *local_8 = m_resourceListHead;
    while (local_8 != 0 && local_8->m_id != param_1) {
        local_8 = local_8->m_next;
    }
    return local_8;
}

VA(0x004c8880, 0x1a)
int resourceManager::Main(struct tag_message &) { return 0; }

VA(0x004c88a0, 0xab)
int resourceManager::Open(int param_1)
{
    if (LoadAggregateHeader(EXPANSION_AGGREGATE_NAME) != 0)
        return 3;
    if (LoadAggregateHeader(DEFAULT_AGGREGATE_NAME) != 0)
        return 3;
    field_0xc = 0x80;
    field_0x10 = param_1;
    m_active = 1;
    strcpy(name, "resourceManager");
    m_resourceListHead = 0;
    return 0;
}

// @early-stop
// raw bytes: only relative +0x28 differs (local branch target 0x50 vs 0x55); no external relocs.
VA(0x004c8950, 0x88)
void resourceManager::RemoveResource(class resource *param_1)
{
    resource *local_8;
    if (m_resourceListHead == param_1) {
        m_resourceListHead = param_1->m_next;
    } else {
        local_8 = m_resourceListHead;
        while (local_8 != 0 && local_8->m_next != param_1) {
            local_8 = local_8->m_next;
        }
        if (local_8 == 0) {
            return;
        } else {
            local_8->m_next = param_1->m_next;
        }
    }
}

// @early-stop
// reloc-masked: code bytes agree; __FILE__ is $SG31418 vs retail's named constant; all 4 relocs agree.
VA(0x004c89e0, 0xc8)
void resourceManager::Close(void)
{
    int local_8;
    if (m_active != 1)
        return;
    Expunge();
    m_resourceListHead = 0;
    for (local_8 = 0; local_8 < 2; local_8++) {
        if (m_aggregateDir[local_8] != 0)
            BaseFree(m_aggregateDir[local_8], __FILE__, __LINE__);
        if (m_aggregateFd[local_8] != -1) {
            _close(m_aggregateFd[local_8]);
            m_aggregateFd[local_8] = -1;
        }
    }
    m_numAggregates = 0;
    m_active = 0;
}

// @early-stop
// reloc-masked: code bytes agree; three string-pool identities differ; all 15 relocs agree.
VA(0x004c8ab0, 0x143)
int resourceManager::LoadAggregateHeader(char *param_1)
{
    short local_10[2];
    int local_c;
    unsigned int local_8;
    if (m_numAggregates >= 2) {
        sprintf(gText, "Only %d AGG files can be used at once", 2);
        ShutDown(gText);
        return 3;
    }
    local_c = _open(param_1, 0x8000);
    if (local_c == -1) {
        sprintf(gText, "Can't open file '%s'", param_1);
        ShutDown(gText);
        return 3;
    }
    m_curAggregate = m_numAggregates;
    m_numAggregates = m_numAggregates + 1;
    m_aggregateFd[m_curAggregate] = local_c;
    _read(m_aggregateFd[m_curAggregate], local_10, 2);
    m_aggregateEntryCount[m_curAggregate] = local_10[0];
    local_8 = m_aggregateEntryCount[m_curAggregate] * 0xc;
    m_aggregateDir[m_curAggregate] = static_cast<aggEntry *>(BaseAlloc(local_8, __FILE__, __LINE__));
    _read(m_aggregateFd[m_curAggregate], m_aggregateDir[m_curAggregate], local_8);
    return 0;
}

VA(0x004c8c00, 0x11c)
void resourceManager::PointToFile(unsigned long fileId)
{
    char isFound = 0;
    int entryIndex;
    int aggregateIndex;
    for (aggregateIndex = 0; aggregateIndex < 2; aggregateIndex++) {
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
    for (fileIndex = 0; fileIndex < 2; fileIndex++) {
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
    ProcessAssert(m_aggregateFd[m_curAggregate] != -1, __FILE__, 0x2bf);
    signed char value = 0;
    int bytesRead = _read(m_aggregateFd[m_curAggregate], &value, 1);
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
    ProcessAssert(m_aggregateFd[m_curAggregate] != -1, __FILE__, 0x2dc);
    short value = 0;
    int bytesRead = _read(m_aggregateFd[m_curAggregate], &value, 2);
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
    ProcessAssert(m_aggregateFd[m_curAggregate] != -1, __FILE__, 0x2f8);
    long value = 0;
    int bytesRead = _read(m_aggregateFd[m_curAggregate], &value, 4);
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
        for (int translatedIndex = 0; translatedIndex < 37; translatedIndex++) {
            if (_stricmp(m_lastFileName, cEvilTranslate[translatedIndex][0]) == 0)
                strcpy(m_lastFileName, cEvilTranslate[translatedIndex][1]);
        }
    }
    unsigned long result = MAKEFILEID(m_lastFileName);
    m_lastFileId = result;
    return result;
}

VA(0x004c9180, 0x26)
void resourceManager::Read13(signed char *param_1)
{
    ReadBlock(param_1, 0xd);
}

VA(0x004c91b0, 0xbd)
void resourceManager::ReadBlock(signed char *destination, unsigned long size)
{
    ProcessAssert(m_aggregateFd[m_curAggregate] != -1, __FILE__, 0x330);
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
