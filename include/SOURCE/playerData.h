#ifndef HOMM2_SOURCE_PLAYERDATA_H
#define HOMM2_SOURCE_PLAYERDATA_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 6 methods, 0 own-virtual, 0 static data.
#include <va.h>

#pragma pack(push, 1)  // recovered layout is byte-packed
class playerData {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    char _pad_0x0[0x1];
    char   field_0x1;  // +0x01
    char _pad_0x2[0x42];
    char   field_0x44;  // +0x44
    // size >= 0x45 (accessed lower bound; no ctor/stride oracle)
    // --- methods ---
    void Write(int);
    void Read(int);
    int NextHero(int);
    int HasMobileHero(void);
    int BuildingsOwned(int, int, int);
    int NumOfGivenArtifact(int);
};
#pragma pack(pop)
#endif // HOMM2_SOURCE_PLAYERDATA_H
