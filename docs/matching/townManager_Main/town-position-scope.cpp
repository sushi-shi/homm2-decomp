// Rejected structural variants for the town-navigation jump.
//
// Hoisting townPosition to function scope and removing the case-local scope
// retained exact size/relocations but scored 99.421890%, below the retained
// 99.426865% structure. Moving break inside the local scope was byte-identical.
// Both are recorded as explored branches; neither is retained.
// Matrices:
//   build/match-variants/townmain-town-position-scope-compile-20260727
//   build/match-variants/townmain-town-position-break-scope-compile-20260727

i32 townPosition;
townPosition = gpGame->TownIDToTownPos(gpCurPlayer, m_town->m_id);
// The remainder is the same navigation arithmetic and ChangeTown call.
