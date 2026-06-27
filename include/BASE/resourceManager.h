#pragma once
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 31 methods, 3 own-virtual, 0 static data.
#include "../_macros.h"
#include "baseManager.h"
// forward declarations:
class MIDIWrap;
class bitmap;
class font;
class icon;
class mouse;
class palette;
class resource;
class sample;
class tileset;
struct tag_message;

class resourceManager : public baseManager {
public:
    // --- constructors ---
    resourceManager(void);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void GetBackdrop(char *, class bitmap *, int);
    void GetBackdropAtLoc(char *, class bitmap *, int, int, int);
    class palette * GetPalette(char *);
    class bitmap * GetBitmap(char *);
    class icon * GetIcon(char *);
    class icon * GetIcon(unsigned long int);
    class tileset * GetTileset(char *);
    class mouse * GetMouse(char *);
    class font * GetFont(char *);
    class sample * GetSample(char *);
    class MIDIWrap * GetMIDIWrap(char *);
    void Dispose(class resource *);
    void AddResource(class resource *);
    void Expunge(void);
    class resource * Query(unsigned long int);
    void RemoveResource(class resource *);
    int LoadAggregateHeader(char *);
    void PointToFile(unsigned long int);
    unsigned long int GetFileSize(unsigned long int);
    void SavePosition(void);
    void RestorePosition(void);
    signed char ReadByte(void);
    short int ReadWord(void);
    long int ReadLong(void);
    unsigned long int MakeId(char *, int);
    void Read13(signed char *);
    void ReadBlock(signed char *, unsigned long int);
};
