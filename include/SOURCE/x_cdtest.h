#ifndef HOMM2_X_CDTEST_H
#define HOMM2_X_CDTEST_H
// Declarations of the free functions DEFINED in x_cdtest.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

struct sCDTest_Track_Data {
    unsigned long start;
    unsigned long end;
};

int CDTest_Init(void);
void CDTest_Cleanup(void);
int CDTest_VerifyCDQuick(void);
int CDTest_VerifyCDThorough(void);
int CDTest_VerifyTrack(int);
int CDTest_GenerateTable(char *);

// ---- globals (declarations, RVA order) ----
extern struct sCDTest_Track_Data cdTestTrackData[43];

#endif // HOMM2_X_CDTEST_H
