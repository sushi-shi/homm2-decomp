/*
mouseManager::Close, retail RVA 0x000c93f0: inline imported-API ownership.

This reviewed matrix tested a structural family not covered by the earlier
local function-pointer experiments:

  - direct DestroyIcon and DeleteObject calls;
  - an inline wrapper around DestroyIcon only;
  - an inline wrapper around DeleteObject only;
  - inline wrappers around both calls.

Every structure was compiled against the clean state and all 50 top-of-TU
declaration-forest states, for a complete 4 x 51 = 204-cell matrix:

    build/mouse-close-inline-wrapper-axes-20260727.json
    build/mouse-close-inline-wrapper-states-manifest-20260727.json
    build/match-variants/mouse-close-inline-wrapper-states-20260727

All 204 cells emitted the same 99.692310%, 237-byte target body with 18/18
object relocation entries. The wrappers inline away completely and cannot
change the imported-call register ownership. Generated helpers were discarded
and the direct source was restored.
*/
