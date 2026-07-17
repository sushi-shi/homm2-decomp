#ifndef HOMM2_X_CDTEST_H
#define HOMM2_X_CDTEST_H

#include <Ints.h>
// Declarations of the free functions DEFINED in x_cdtest.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

struct sCDTest_Track_Data {
    u32l start;
    u32l end;
};

i32 CDTest_Init(void);
void CDTest_Cleanup(void);
i32 CDTest_VerifyCDQuick(void);
i32 CDTest_VerifyCDThorough(void);
i32 CDTest_VerifyTrack(i32);
i32 CDTest_GenerateTable(char*);

// ---- globals (declarations, RVA order) ----
extern struct sCDTest_Track_Data cdTestTrackData[43];

#endif // HOMM2_X_CDTEST_H
