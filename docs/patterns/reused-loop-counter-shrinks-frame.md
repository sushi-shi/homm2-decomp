# A frame exactly one slot too big = two of our locals are one retail local

When the reconstruction's `sub esp,N` is 4 bytes larger than retail's and the
`this` spill sits 4 bytes deeper, the usual first guess (a phantom dead local)
is only one of two possibilities. The other is that retail reused **one** loop
counter for two loops that the reconstruction gave separate names — the 1996
house style of a single `int i` walking several unrelated loops.

The tell is a slot that carries two different loop bounds.

## Retail evidence — `combatManager::DrawFrame` (0x004395b8)

Frames:

```
ours    81 ec e0 00 00 00     sub esp,0xe0     this @ -0xc4
retail  81 ec dc 00 00 00     sub esp,0xdc     this @ -0xc0
```

Grepping retail for the loop-bound compares shows `-0xc` doing double duty:

```
1936: 83 7d f4 02           cmp DWORD PTR [ebp-0xc],0x2    <- side loop  (COMBAT_SIDE_COUNT)
1952: 83 7d d8 14           cmp DWORD PTR [ebp-0x28],0x14  <- army slot loop
1a6d: 83 7d f4 02           cmp DWORD PTR [ebp-0xc],0x2    <- second side loop
21c8: 83 7d f4 04           cmp DWORD PTR [ebp-0xc],0x4    <- draw-state loop (ARMY_DRAW_PHASE_COUNT)
2282: 83 7d f4 04           cmp DWORD PTR [ebp-0xc],0x4    <- second draw-state loop
1e21: 83 7d f8 09           cmp DWORD PTR [ebp-0x8],0x9    <- row loop, a separate local
```

Our object had `side5` at `-0xc` and `drawState5` at `-0x1c`: two locals, and
consequently one extra slot in the frame and every deeper slot (including
`this`, read 96 times) shifted by 4.

## What made it match

Merge them into one local and let `IDX()` supply the plain index where the
value is used as an array subscript. Under the retail flags
`H2_ENUM_STORAGE_STEPPED(name, storage)` is just `storage`, so the merged
declaration is codegen-identical to a plain `i32`, and the strict-enum clang
pass still type-checks:

```diff
-    i32 side5;
-    H2_ENUM_STORAGE_STEPPED(ArmyDrawState, i32) drawState5;
+    H2_ENUM_STORAGE_STEPPED(ArmyDrawState, i32) state;
...
-        for (side5 = 0; side5 < COMBAT_SIDE_COUNT; side5++) {
-            if (m_limitCreatureCount[side5][armyIndex2] > 0) {
+        for (state = 0; IDX(state) < COMBAT_SIDE_COUNT; state++) {
+            if (m_limitCreatureCount[IDX(state)][armyIndex7] > 0) {
...
-        for (drawState5 = ARMY_DRAW_BEHIND; drawState5 < ARMY_DRAW_PHASE_COUNT; drawState5++) {
+        for (state = ARMY_DRAW_BEHIND; state < ARMY_DRAW_PHASE_COUNT; state++) {
```

Frame `0xe0 -> 0xdc`, `this` back to `-0xc0`, and the function went
88.19% -> 99.5%+ in one step (EXACT after the remaining slot solve).

## How to spot it

1. `sub esp` differs by exactly one slot and every slot deeper than some point
   is shifted by the same amount.
2. Build the `base slot -> target slot` census (disassemble both, pair the
   instructions, collect single-`(%ebp)` operands). If **two** of our slots map
   to **one** retail slot with a high hit count on both, that slot is reused.
3. Confirm by grepping retail for the loop bounds: one slot compared against
   two unrelated limits is a reused counter, not a coincidence.
