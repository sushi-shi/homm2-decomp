/*
GetDataEntry (RVA 0x000c6ac0, retail size 902)
Gold/Buka 2.1 layout arithmetic and null-branch reconstruction, 2026-08-05.

The original PoL reconstruction already had the retail 24-block CFG, size,
and ordered 59/59 relocations, but scored 99.102040%.  Its two disassembly
islands corresponded to algebraically equivalent source that remained visible
in the exact Gold/Buka reconstruction.

First matrix, 2 x 51 = 102/102 complete:

  PoL folded:  rows = (height + 15) / 45;
               entryY = rows * 45 + 95 - cancelOffset;

  Gold staged: height += 40;
               rows = (height - 25) / 45;
               entryY = (rows + 1) * 45 + 50 - cancelOffset;

The staged form is semantically identical and raised the clean object to
99.918365%, leaving only four register-publication instructions different.
All 50 state probes emitted the same result, proving a structural improvement.

Second matrix, 2 local-declaration structures x 2 initial-text structures x
51 states = 204/204 complete.  The Gold/Buka null branch closed the remaining
island in the clean state:

  if (initialText != NULL)
      strcpy(entryText, initialText);
  else
      strcpy(entryText, defaultText);

Both scoped-current and Gold top-declaration ownerships were exact with that
branch.  The smaller scoped-current declaration structure was retained.  The
result is clean 100.000000%, size 902, exact 24/24 CFG, and ordered 59/59
relocations.

Artifacts:
  build/getdataentry-row-structure-axes.json
  build/getdataentry-row-structure-manifest.json
  build/source-variant-batch/getdataentry-row-structure/results.json
  build/getdataentry-local-ownership-axes.json
  build/getdataentry-local-ownership-manifest.json
  build/source-variant-batch/getdataentry-local-ownership/results.json

Disposition: retain the staged layout arithmetic, its named constants, and
the branch-owned copies.  No generated probe or unused Gold local is retained.
*/

#if 0
height += ROW_TOP_MARGIN;
rows = (height - ROW_ROUNDING_BIAS) / ROW_HEIGHT;
entryY = (rows + 1) * ROW_HEIGHT + ENTRY_BASE_Y - cancelOffset;

if (initialText != NULL)
    strcpy(entryText, initialText);
else
    strcpy(entryText, defaultText);
#endif
