#pragma once
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 26 methods, 3 own-virtual, 0 static data.
#include "../_macros.h"
#include "../BASE/baseManager.h"
// forward declarations:
class heroWindow;
struct tag_message;

class townManager : public baseManager {
public:
    // --- constructors ---
    townManager(void);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void SetupExtraStuff(void);
    void ChangeTown(void);
    void SetupTown(void);
    void UnloadTown(void);
    void SetArmyCommand(int);
    void SetCommandAndText(struct tag_message &);
    void ShowText(char *);
    void DoCommand(int);
    void RedrawTownScreen(void);
    void SplitArmy(void);
    void ShiftQualChange(void);
    void ResetStrips(void);
    void Toggle(int);
    void DrawTown(int, int);
    int BuyBuild(int, int, int);
    void BuildObj(int);
    void SetupMage(class heroWindow *);
    int RecruitHero(int, int);
    void DoTavern(void);
    void SetupWell(class heroWindow *);
    void SetupThievesGuild(class heroWindow *, int);
    void SetupCastle(class heroWindow *, int);
};
