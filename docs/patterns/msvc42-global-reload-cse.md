# MSVC 4.2 global reloads, CSE, and branch ownership

## Symptom

The candidate uses every correct global or callee, and every relocation target identity agrees,
but the relocation occurrence counts differ. Typical forms are:

- retail reloads a global while the candidate carries its earlier value in a register;
- the candidate reloads a global twice while retail retains one load;
- retail emits the same store in two control-flow arms while the candidate tail-merges them;
- a real source store exists, but VC4.2 deletes it before a later publication.

Source references do not map one-for-one to COFF relocations. A relocation exists only when the
emitted instruction directly addresses the symbol. Register reuse, CSE, dead-store elimination,
tail merging, and inlining can remove or move that occurrence.

Tags: `toolchain:vc42 topic:o2 topic:regalloc topic:relocs topic:cse topic:control-flow`.

## Resolved example: `CycleColors`

The incorrect source selected a palette pointer in the COMBAT/ALTERNATE arms, then performed one
common three-byte copy after the join:

```cpp
signed char *src;
if (giCycleType == WINDOW_COLOR_CYCLE_COMBAT) {
    src = combatSource;
} else {
    if (giCycleType != WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE)
        goto updatePalette;
    src = alternateSource;
}
memcpy(gCyclePal + 66, src, 3);
```

VC4.2 also routed the DEFAULT arm through that common copy. The DEFAULT restore therefore had no
separate `gCyclePal+66` store relocation: candidate 70 occurrences versus retail 71. The relevant
candidate-before code was:

```text
; DEFAULT has restored savedColor in EAX
+29d  e9 92 00 00 00       jmp  +334       ; joins the common store

; COMBAT and ALTERNATE also join here with their source in EAX
+334  66 8b 10             mov  dx,[eax]
+337  b9 42 00 00 00       mov  ecx,gCyclePal+66
       DIR32 gCyclePal+66                         ; the only store occurrence
+33c  66 89 11             mov  [ecx],dx
+33f  8a 40 02             mov  al,[eax+2]
+342  88 41 02             mov  [ecx+2],al
```

Retail instead owns the DEFAULT restore in the DEFAULT arm and merges only the two combat copies.
The reconstructed source expresses that ownership directly:

```cpp
if (giCycleType == WINDOW_COLOR_CYCLE_DEFAULT) {
    // rotate and restore savedColor
    memcpy(gCyclePal + WINDOW_DEFAULT_CYCLE_SOURCE_OFFSET, savedColor,
           WINDOW_PALETTE_COLOR_BYTES);
    goto updatePalette;
}

if (giCycleType == WINDOW_COLOR_CYCLE_COMBAT) {
    memcpy(gCyclePal + WINDOW_DEFAULT_CYCLE_SOURCE_OFFSET, combatSource,
           WINDOW_PALETTE_COLOR_BYTES);
} else {
    if (giCycleType != WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE)
        goto updatePalette;
    memcpy(gCyclePal + WINDOW_DEFAULT_CYCLE_SOURCE_OFFSET, alternateSource,
           WINDOW_PALETTE_COLOR_BYTES);
}
```

Candidate and retail then emit the same control-flow/store structure. The two relocation sites are
the DEFAULT-owned store at `+0x2a1` and the combat-join store at `+0x34a`:

```text
candidate and retail (relocation operands normalized)

; DEFAULT restore -- separate owner
+29d  66 8b 10             mov  dx,[eax]
+2a0  b9 42 00 00 00       mov  ecx,gCyclePal+66
       DIR32 gCyclePal+66
+2a5  66 89 11             mov  [ecx],dx
+2a8  8a 40 02             mov  al,[eax+2]
+2ab  88 41 02             mov  [ecx+2],al
+2ae  e9 a4 00 00 00       jmp  updatePalette

; COMBAT and ALTERNATE merge with source in EDX
+346  66 8b 0a             mov  cx,[edx]
+349  b8 42 00 00 00       mov  eax,gCyclePal+66
       DIR32 gCyclePal+66
+34e  66 89 08             mov  [eax],cx
+351  8a 52 02             mov  dl,[edx+2]
+354  88 50 02             mov  [eax+2],dl
```

This raised `CycleColors` from 95.33% to 99.83%, changed relocations from 70/71 to exact
71/71, and produced 261/261 instructions through the CodeView procedure end. The remaining code
residual is only an ECX/ESI allocation swap in an unrelated loop.

## What is predictable

These are strong diagnostics, not a complete allocator model:

1. **Dominating load with no invalidation:** if one load dominates later uses, its value remains
   available, and register pressure is low, VC4.2 often reuses it. Expect fewer relocations.
2. **Control-flow ownership:** identical accesses after a join are candidates for CSE/tail merging.
   Accesses used inside genuinely distinct arms are more likely to remain distinct. Repair the CFG
   and expression owner before changing spellings.
3. **Dead publications:** a store overwritten before any observable use can disappear even when the
   source assignment is real. Do not add `volatile` merely to retain it.
4. **Calls and clobbers:** EAX/ECX/EDX values normally do not survive a call; EBX/ESI/EDI/EBP can.
   A call can therefore create a natural reload, but a value held in a callee-saved register may
   remain live across it.
5. **Possible alias writes:** an unknown call or pointer write can invalidate a cached memory value.
   A file-static/global whose address does not escape is easier for the compiler to retain.
6. **Register pressure and live ranges:** extra real locals, wider scopes, entry/type roots, and
   expression order change which values are carried, spilled, or reloaded.
7. **TU/declaration state:** VC4.2 `/O2` allocation also depends on preceding source and reachable
   declarations. The same function source may change after an exact predecessor or header changes.

No single rule predicts the final choice. In particular, `const`, reference aliases, or a source
reference count do not prove a retail reload pattern.

## Reconstruction workflow

1. Audit target identities first. A count match with the wrong target is not progress.
2. List relocation occurrences in instruction order and assign each to its semantic CFG owner.
3. Start at the first missing/excess site. Determine whether retail reloads, retains, duplicates,
   tail-merges, or publishes a value in a different arm.
4. Repair real CFG, type, expression ownership, and lifetime structure. Rebuild and check the
   surrounding instructions—not just the new count.
5. Reject dummy reads, `volatile` steering, aliases, inert expressions, and artificial calls.
6. At 96-97% or better, use only `scripts/permute_ast.py` for value-preserving last-mile variants.
7. Retest on the combined root. If local structure is already correct, try only audited
   exact-preserving predecessor/header/TU-state variants.

Current contrast cases are documented at their functions:

- `Misc::BlitBitmapToScreen`: candidate carries one `gpMouseManager` load across horizontal and
  vertical predicates; retail reloads it. The CFG/frame are aligned, but no real invalidation or
  ownership boundary has yet been recovered.
- `FlipIconToBitmapColorTable`: candidate reloads `gFCY` for both vertical clauses; retail retains
  one load in ECX.
- `IconToBitmapYModify`: candidate retains `gYMX` in ESI where retail reloads it.

Those residuals are triggers for structural research, not permission to manufacture relocations.

Related: [O2 TU-cumulative register steering](o2-tu-cumulative-register-steering.md),
[TU-cumulative evaluation order](tu-cumulative-eval-order.md).
