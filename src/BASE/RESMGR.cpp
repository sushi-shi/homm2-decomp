// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\RESMGR.OBJ   from: .\basewin.lib
// functions: 31   data: 4
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/resourceManager.h>
#include <BASE/resource.h>
#include <BASE/tileset.h>
#include <BASE/font.h>
VA(0x004c7fa0, 0xdb)
resourceManager::resourceManager(void) {}

VA(0x004c8080, 0xa2)
void resourceManager::GetBackdrop(char *, class bitmap *, int) {}

VA(0x004c8130, 0xd2)
void resourceManager::GetBackdropAtLoc(char *, class bitmap *, int, int, int) {}

VA(0x004c8210, 0x97)
class palette * resourceManager::GetPalette(char *) { return 0; }

VA(0x004c82b0, 0x97)
class bitmap * resourceManager::GetBitmap(char *) { return 0; }

VA(0x004c8350, 0x2f)
class icon * resourceManager::GetIcon(char *name) { return GetIcon(MakeId(name, 1)); }

VA(0x004c8380, 0x86)
class icon * resourceManager::GetIcon(unsigned long int) { return 0; }

VA(0x004c8410, 0x97)
class tileset * resourceManager::GetTileset(char *name)
{
    unsigned long id = MakeId(name, 1);
    resource *r = Query(id);
    if (r != 0) {
        r->field_0x6++;
    } else {
        r = new tileset(id);
        AddResource(r);
    }
    return static_cast<tileset *>(r);
}

VA(0x004c84b0, 0x1a)
class mouse * resourceManager::GetMouse(char *) { return 0; }

VA(0x004c84d0, 0x97)
class font * resourceManager::GetFont(char *name)
{
    unsigned long id = MakeId(name, 1);
    resource *r = Query(id);
    if (r != 0) {
        r->field_0x6++;
    } else {
        r = new font(id);
        AddResource(r);
    }
    return static_cast<font *>(r);
}

VA(0x004c8570, 0x9d)
class sample * resourceManager::GetSample(char *) { return 0; }

VA(0x004c8610, 0x97)
class MIDIWrap * resourceManager::GetMIDIWrap(char *) { return 0; }

VA(0x004c86b0, 0x87)
void resourceManager::Dispose(class resource *) {}

VA(0x004c8740, 0x55)
void resourceManager::AddResource(class resource *) {}

VA(0x004c87a0, 0x8b)
void resourceManager::Expunge(void) {}

VA(0x004c8830, 0x4b)
class resource * resourceManager::Query(unsigned long int) { return 0; }

VA(0x004c8880, 0x1a)
int resourceManager::Main(struct tag_message &) { return 0; }

VA(0x004c88a0, 0xab)
int resourceManager::Open(int) { return 0; }

VA(0x004c8950, 0x88)
void resourceManager::RemoveResource(class resource *) {}

VA(0x004c89e0, 0xc8)
void resourceManager::Close(void) {}

VA(0x004c8ab0, 0x143)
int resourceManager::LoadAggregateHeader(char *) { return 0; }

VA(0x004c8c00, 0x11c)
void resourceManager::PointToFile(unsigned long int) {}

VA(0x004c8d20, 0xfa)
unsigned long int resourceManager::GetFileSize(unsigned long int) { return 0; }

VA(0x004c8e20, 0x52)
void resourceManager::SavePosition(void) {}

VA(0x004c8e80, 0x53)
void resourceManager::RestorePosition(void) {}

VA(0x004c8ee0, 0x81)
signed char resourceManager::ReadByte(void) { return 0; }

VA(0x004c8f70, 0x84)
short int resourceManager::ReadWord(void) { return 0; }

VA(0x004c9000, 0x84)
long int resourceManager::ReadLong(void) { return 0; }

VA(0x004c9090, 0xe3)
unsigned long int resourceManager::MakeId(char *, int) { return 0; }

VA(0x004c9180, 0x26)
void resourceManager::Read13(signed char *) {}

VA(0x004c91b0, 0xbd)
void resourceManager::ReadBlock(signed char *, unsigned long int) {}


// ===== vtable resourceManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x004c88a0, 0xab)  int resourceManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004c89e0, 0xc8)  void resourceManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x004c8880, 0x1a)  int resourceManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(resourceManager, 0x004eb9f0);
