#ifndef HOMM2_SOURCE_EXPCAMPAIGN_H
#define HOMM2_SOURCE_EXPCAMPAIGN_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 36 methods, 0 own-virtual, 0 static data.
#include "../_macros.h"
// forward declarations:
struct tag_message;

class ExpCampaign {
public:
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
    static int MessageHandler(struct tag_message &);
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
#endif // HOMM2_SOURCE_EXPCAMPAIGN_H
