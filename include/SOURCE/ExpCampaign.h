#ifndef HOMM2_SOURCE_EXPCAMPAIGN_H
#define HOMM2_SOURCE_EXPCAMPAIGN_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 36 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class ExpCampaign {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    int    field_0x0;  // +0x00
    int    field_0x4;  // +0x04
    int    field_0x8;  // +0x08
    char   field_0xc;  // +0x0c
    char   field_0xd;  // +0x0d
    char   field_0xe;  // +0x0e
    char   field_0xf;  // +0x0f
    char   field_0x10;  // +0x10
    char   field_0x11;  // +0x11
    char   field_0x12;  // +0x12
    char   field_0x13;  // +0x13
    char _pad_0x14[0x2];
    char   field_0x16;  // +0x16
    char _pad_0x17[0x3];
    char   field_0x1a;  // +0x1a
    char _pad_0x1b[0x1];
    short  field_0x1c;  // +0x1c
    char _pad_0x1e[0xe];
    char   field_0x2c;  // +0x2c
    char   field_0x2d;  // +0x2d
    char   field_0x2e;  // +0x2e
    char   field_0x2f;  // +0x2f
    char   field_0x30;  // +0x30
    char   field_0x31;  // +0x31
    char   field_0x32;  // +0x32
    char   field_0x33;  // +0x33
    char   field_0x34;  // +0x34
    char   field_0x35;  // +0x35
    char   field_0x36;  // +0x36
    char _pad_0x37[0xc];
    int    field_0x43;  // +0x43
    int    field_0x47;  // +0x47
    int    field_0x4b;  // +0x4b
    // size >= 0x4f (accessed lower bound; no ctor/stride oracle)
    // --- constructors ---
    ExpCampaign(void);
    ~ExpCampaign();
    // --- methods ---
    void ResetMapChoices(void);
    void ResetMapsPlayed(void);
    void ResetAwards(void);
    void ResetBonusChoices(void);
    void GrantAward(int);
    void RemoveAward(int);
    signed char HasAward(int);
    void SetMapWasPlayed(void);
    void InitNewCampaign(int);
    void InitMap(void);
    void ShowInfo(int, int);
    void UpdateInfo(int);
    int HandleVictory(void);
    void HandleVictory1(void);
    void HandleVictory2(void);
    void HandleVictory3(void);
    void HandleVictory4(void);
    void ReplaySmacker(void);
    void ReplaySmacker1(void);
    void ReplaySmacker2(void);
    void ReplaySmacker3(void);
    void ReplaySmacker4(void);
    unsigned char IsCompleted(void);
    signed char IsThisMapCompleted(void);
private:
    static int MessageHandler(struct tag_message &);   // ?...@ExpCampaign@@CIH... (private static)
public:
    void Autosave(void);
    int Choose(void);
    short int Days(void);
    int CampaignID(void);
    char * JosephName(void);
    char * IvanName(void);
    signed char IsSpecialGoldenBow(int, int);
    signed char IsSpecialUA(void);
    signed char IsSpecialLossCondition(int);
};
#pragma pack(pop)
#endif // HOMM2_SOURCE_EXPCAMPAIGN_H
