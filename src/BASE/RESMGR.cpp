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
VA(0x004c7fa0, 0xdb)
resourceManager::resourceManager(void) : baseManager()
{
    int local_8;
    m_active = 0;
    m_resourceListHead = 0;
    m_expunging = 0;
    strcpy(name, "resourceManager");
    m_lastFileId = 0;
    for (local_8 = 0; local_8 < 2; local_8++) {
        m_aggregateFd[local_8] = -1;
        m_aggregateDir[local_8] = 0;
        m_aggregateEntryCount[local_8] = 0;
    }
    m_numAggregates = 0;
    m_curAggregate = 0;
}

VA(0x004c8080, 0xa2)
void resourceManager::GetBackdrop(char *param_1, class bitmap *param_2, int param_3)
{
    if (param_3 != 0) {
        icon *this_00 = GetIcon(param_1);
        this_00->DrawToBuffer(0, 0, 0, 0);
        Dispose(this_00);
    } else {
        unsigned long uVar1 = MakeId(param_1, 1);
        PointToFile(uVar1);
        ReadWord();
        ReadWord();
        ReadWord();
        ReadBlock(reinterpret_cast<signed char *>(param_2->m_pixels),
                  param_2->m_height * param_2->m_width);
    }
}

VA(0x004c8130, 0xd2)
void resourceManager::GetBackdropAtLoc(char *param_1, class bitmap *param_2, int param_3, int param_4,
                                       int param_5)
{
    short sVar1, sVar2;
    int local_8;
    if (param_5 != 0) {
        icon *this_00 = GetIcon(param_1);
        this_00->DrawToBuffer(param_3, param_4, 0, 0);
        Dispose(this_00);
    } else {
        unsigned long uVar3 = MakeId(param_1, 1);
        PointToFile(uVar3);
        ReadWord();
        sVar1 = ReadWord();
        sVar2 = ReadWord();
        for (local_8 = param_4; local_8 < sVar2 + param_4; local_8++) {
            ReadBlock(reinterpret_cast<signed char *>(local_8 * 0x280 + param_2->m_pixels + param_3),
                      sVar1);
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
class sample *resourceManager::GetSample(char *param_1)
{
    unsigned long uVar1 = MakeId(param_1, 1);
    resource *local_8 = Query(uVar1);
    if (local_8 != 0) {
        local_8->m_refCount++;
        return static_cast<sample *>(local_8);
    } else {
        local_8 = new sample(param_1, 0, 0x7f, 1);
        AddResource(local_8);
        return static_cast<sample *>(local_8);
    }
}

VA(0x004c8610, 0x97)
class MIDIWrap *resourceManager::GetMIDIWrap(char *param_1)
{
    unsigned long uVar1 = MakeId(param_1, 1);
    resource *local_8 = Query(uVar1);
    if (local_8 != 0) {
        local_8->m_refCount++;
        return static_cast<MIDIWrap *>(local_8);
    } else {
        local_8 = new MIDIWrap(param_1);
        AddResource(local_8);
        return static_cast<MIDIWrap *>(local_8);
    }
}

VA(0x004c86b0, 0x87)
void resourceManager::Dispose(class resource *param_1)
{
    if (m_expunging != 0)
        return;
    if (param_1 == 0)
        return;
    param_1->m_refCount--;
    if (param_1->m_refCount <= 0) {
        RemoveResource(param_1);
        if (param_1 != 0)
            delete param_1;
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
    resource *prVar1, *local_8;
    m_expunging = 1;
    prVar1 = m_resourceListHead;
    while (local_8 = prVar1, local_8 != 0) {
        prVar1 = local_8->m_next;
        RemoveResource(local_8);
        delete local_8;
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
    return 0;
}

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
        if (local_8 != 0)
            local_8->m_next = param_1->m_next;
    }
}

VA(0x004c89e0, 0xc8)
void resourceManager::Close(void)
{
    int local_8;
    if (m_active == 1) {
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
}

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
void resourceManager::PointToFile(unsigned long param_1)
{
    char bVar1 = 0;
    int local_10, local_8;
    for (local_10 = 0; local_10 < 2; local_10++) {
        if (m_aggregateDir[local_10] != 0) {
            for (local_8 = 0; local_8 < m_aggregateEntryCount[local_10]; local_8++) {
                if (m_aggregateDir[local_10][local_8].id == param_1) {
                    bVar1 = 1;
                    m_curAggregate = local_10;
                    break;
                }
            }
        }
        if (bVar1)
            break;
    }
    if (!bVar1) {
        sprintf(gText, "ResMgr::PointToFile failure. This resource (%lu) is not in aggregate %s", param_1,
                m_lastFileId, &m_aggregateName);
        ShutDown(gText);
    }
    _lseek(m_aggregateFd[m_curAggregate], m_aggregateDir[m_curAggregate][local_8].offset, 0);
}

VA(0x004c8d20, 0xfa)
unsigned long resourceManager::GetFileSize(unsigned long param_1)
{
    char bVar1 = 0;
    int local_14, local_10, local_8;
    for (local_14 = 0; local_14 < 2; local_14++) {
        if (m_aggregateDir[local_14] != 0) {
            for (local_8 = 0; local_8 < m_aggregateEntryCount[local_14]; local_8++) {
                if (m_aggregateDir[local_14][local_8].id == param_1) {
                    bVar1 = 1;
                    local_10 = local_14;
                    break;
                }
            }
        }
        if (bVar1)
            break;
    }
    if (!bVar1) {
        sprintf(gText, "ResMgr::PointToFile failure. This resource (%lu) is not in aggregate %s", param_1,
                m_lastFileId, &m_aggregateName);
        ShutDown(gText);
    }
    return m_aggregateDir[local_10][local_8].size;
}

VA(0x004c8e20, 0x52)
void resourceManager::SavePosition(void)
{
    lastPositionZ[gResPositionStackIdx] = _tell(m_aggregateFd[m_curAggregate]);
    lastAggZ[gResPositionStackIdx] = m_curAggregate;
    gResPositionStackIdx = gResPositionStackIdx + 1;
}

VA(0x004c8e80, 0x53)
void resourceManager::RestorePosition(void)
{
    gResPositionStackIdx = gResPositionStackIdx - 1;
    m_curAggregate = lastAggZ[gResPositionStackIdx];
    _lseek(m_aggregateFd[m_curAggregate], lastPositionZ[gResPositionStackIdx], 0);
}

VA(0x004c8ee0, 0x81)
signed char resourceManager::ReadByte(void)
{
    int iVar1;
    char local_8[4];
    ProcessAssert(m_aggregateFd[m_curAggregate] != -1, __FILE__, __LINE__);
    local_8[0] = 0;
    iVar1 = _read(m_aggregateFd[m_curAggregate], local_8, 1);
    if (iVar1 == 0)
        _errno();
    return local_8[0];
}

VA(0x004c8f70, 0x84)
short resourceManager::ReadWord(void)
{
    int iVar1;
    short local_8[2];
    ProcessAssert(m_aggregateFd[m_curAggregate] != -1, __FILE__, __LINE__);
    local_8[0] = 0;
    iVar1 = _read(m_aggregateFd[m_curAggregate], local_8, 2);
    if (iVar1 == 0)
        _errno();
    return local_8[0];
}

VA(0x004c9000, 0x84)
long resourceManager::ReadLong(void)
{
    int iVar1;
    long local_8;
    ProcessAssert(m_aggregateFd[m_curAggregate] != -1, __FILE__, __LINE__);
    local_8 = 0;
    iVar1 = _read(m_aggregateFd[m_curAggregate], &local_8, 4);
    if (iVar1 == 0)
        _errno();
    return local_8;
}

VA(0x004c9090, 0xe3)
unsigned long resourceManager::MakeId(char *param_1, int param_2)
{
    int local_c;
    strcpy(&m_aggregateName, param_1);
    if (gbUseEvilInterface != 0 && param_2 != 0) {
        for (local_c = 0; local_c < 0x25; local_c++) {
            if (_strcmpi(&m_aggregateName, cEvilTranslate[local_c][0]) == 0) {
                strcpy(&m_aggregateName, cEvilTranslate[local_c][1]);
            }
        }
    }
    unsigned long uVar1 = MAKEFILEID(&m_aggregateName);
    m_lastFileId = uVar1;
    return uVar1;
}

VA(0x004c9180, 0x26)
void resourceManager::Read13(signed char *param_1)
{
    ReadBlock(param_1, 0xd);
}

VA(0x004c91b0, 0xbd)
void resourceManager::ReadBlock(signed char *param_1, unsigned long param_2)
{
    unsigned long uVar1;
    ProcessAssert(m_aggregateFd[m_curAggregate] != -1, __FILE__, __LINE__);
    PollSound();
    uVar1 = _read(m_aggregateFd[m_curAggregate], param_1, param_2);
    if (uVar1 != param_2) {
        sprintf(gText, "File error: bytes read %d, bytes wanted %d, errno %d, aggregate %s", uVar1,
                param_2, errno, &m_aggregateName);
        LogStr(gText);
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
int iSaveCtr;
int lastAggZ[10];
long lastPositionZ[12];
