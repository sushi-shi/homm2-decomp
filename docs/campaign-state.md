# Buka 2.1 campaign state (2026-08-03)

**1047 / 1727 BYTE-EXACT (60.6%); objdiff reports 999 (57.8%), 1008 banked,
fuzzy 88.10, matched-code-bytes 16.35%.**
Session start was 880 (51.0%) / fuzzy 87.13. Zero real regressions at every
checkpoint; `homm2 selftest` green (513) throughout.

The 48-function gap between byte-exact and objdiff is entirely reloc NAMES -
the delinker calls unclaimed data operands `const_*`/`$anon_str_*`, so
functions that agree byte-for-byte report 98-99.9%. Closing it is the data
campaign (task 20). Note it does NOT move the byte-exact count: those
functions are already matched, so data work cannot help reach a byte-exact
target.

## Control-flow detectors (2026-08-03) - run these FIRST on any residual

Three cheap byte-signature scans now rank the whole image. Each is a few
lines over the retail bytes vs ours; all three closed functions on first use.

**1. Guard clauses.** `jcc L; jmp end` (TWO jumps) is `if (...) return;`.
A single `jcc end` is `if (...) { ... }`. Count `jcc +2/eb` and `jcc +5/e9`
(plus the `0f 8x` near forms) on both sides and diff the counts.
Current queue: 39 fns missing 90 sites, 36 fns with 62 SPURIOUS guards.

**2. `&&` vs `||` guard.** The discriminator is where the SECOND
conditional jumps:

    if (A && B) { body }        -> both jcc target the END
    if (!A || !B) return;       -> first jcc targets a shared `jmp end`,
       body                        second jcc targets the BODY

**3. Ternary temp.** Retail materialising `c ? a : b` allocates a temp
BELOW the `this` spill (e.g. `this` at -0x18, temp at -0x1a) and stores
to the destination ONCE; an if/else stores twice. So *retail frame
LARGER than ours* is the tell. Queue: 53 fns >=80%, every one +4.

## RESOLVED: the /Od frame layout, and the `this`-slot counter

Settled with two probe TUs compiled by the real VC6 (scratchpad
`probe.cpp` / `probe2.cpp`; same names, different decl order, then same
decl order, different names). Result:

    [ebp-0x4] ... named locals, sorted by (bucket(name), -decl_index)
    ...       ... `this`, DEEPEST of the named group
    ...       ... compiler temps, BELOW `this`

`od_slots.slot_order` reproduces the named-local order on VC6 exactly -
it predicted both probes. `this` is always last of the named group; it
is NOT hashed (`bucket('this')`=11 is a red herring).

**So `this`'s slot is a COUNTER.** Retail spilling `this` at `[ebp-N]`
means retail has exactly `N/4 - 1` named locals. If we have more, the
extras are things retail never named - a ternary or an inlined call
result that lives in a compiler temp below `this`. That is a direct,
cheap detector, and it does not need any naming judgement.

`army::AttackTo` is the worked example. Retail spills `this` at -0xc =
two named locals; we had three. The third (`finishStanding`) is retail's
ternary argument:

    Walk(dir, pathIndex != 1 && stepCount < m_monster.speed ? 0 : 1, ...)

Dropping the variable took it 23 -> 14 diffs, and the last 14 were an
ordinary two-slot swap: `pathIndex` had to precede the step counter, so
`numSteps`(bucket 0) became `stepCount`(9), tying with `pathIndex`(9)
and letting the later declaration win the tie. EXACT.

(My earlier note here claimed retail contradicted od_slots and that this
blocked 74 solves. That was wrong on both counts - the model is right,
and the `this` position is a useful signal rather than an obstacle.)

### Caveat: initialised decls do not always follow bucket order

`combatManager::RightClick` has `i32 col = ...; i32 row = ...;` adjacent
at function scope. bucket(col)=0 < bucket(row)=2 predicts col shallower;
our own VC6 compile puts row shallower, and retail puts col shallower.
Renaming col -> column(5) made it worse (5 -> 7 diffs) and swapping the
declaration order made it worse again (5 -> 9, because it also reorders
the evaluation). So the probe-validated ordering rule holds for plain
declarations but something about initialised adjacent decls - or the
later function-scope `currentArmy` - is not captured. Measure before
trusting the model on initialised locals.

## Negative result: the dead-local class is EXHAUSTED

The decl-vs-use scan finds 168 unreferenced locals image-wide, but the
intersection with ours-larger frames is EMPTY. Every remaining one sits
in a function whose frame already matches - it is padding retail has too
(the `unusedXxx` names). The Netbios `scratch[]` pair was the last real
one. Do not re-run this expecting yield.

## Codegen laws proven this session

Each is byte-verified and closed at least one function. They are the levers
to try FIRST on any residual, in rough order of yield:

1. **Misplaced body.** A function far too long usually contains code that
   belongs in a callee retail actually calls. Read retail's reloc list for
   the call targets. (MOUSEMGR pointer trio, SetColorMice −272 bytes,
   NewUpdate, GetCharacterWidth.)
2. **Wrongly-`inline` linkage.** A caller that swells by its callee's size,
   or a claimed span missing from our object, means an `inline` that should
   not be. (bitmap::GrabBitmap, bitmap::DrawToBuffer.)
3. **`__cdecl` in a `/Gr` build.** Retail prologue reads its first arg from
   `[ebp+8]` with NO ecx spill, and callers `push`/`add esp,4`. Fix both the
   declaration and the definition; `source_symbols` re-mangles `YI`→`YA` and
   `redelink` re-pairs. (wingraph palette trio, netwin entry points.)
   *Caveat:* the spill can use a long-form displacement — scan ≥16 bytes
   past the prologue before concluding (MAKEFILEID false positive).
4. **Loop shape / fusion.** Retail favours one fused indexed loop where a
   reconstruction has several, and plain `for` over running-offset
   `do/while`. (mouseManager ctor, bitmap::CopyTo/CopyToCareful.)
5. **Flat addressing.** `*(base + x + W * y)`, computed-term-first; the
   compound-assign RMW emits ASYMMETRIC shapes from one spelling.
   `fullMap::GetCell` is `&Column(x)[y * width]`. (SetVisibility,
   MakeAllWaterVisible, SummonElemental, VaporizeCreature, 24-site sweep.)
6. **Frame temps.** A `+4`/`+8` frame with no dead locals is a materialised
   temp: a named temp retail folds into its use site, or a ternary retail
   spells `if/else`. NOT reliable in reverse — see the negative result below.
7. **Scope grouping.** Inner-block locals slot BELOW the whole outer group;
   hoisting a decl to fn top moves it between groups.
8. **Slot buckets.** sort by `(bucket(name), −decl_index)`, `align4(size)`.
   Arrays are NOT privileged — a buffer sits deeper than a scalar only if
   its bucket is higher (`font::font`: `fname`(0) → `name`(13)).
   Enum-typed locals defeat the model outright (SetupAdjacencyArray).

## Negative results (do not re-derive)

- `while (1)` vs `for (;;)` is a REAL /Od distinction - `while (1)` emits
  `mov eax,1; test eax,eax; je <end>` at the loop top, `for (;;)` does not -
  but it is PER SITE. `nb_arm_recv` needed `while (1)` (-10 -> exact);
  `Icon2b::IconToBitmap` got worse with it (+6 -> +25). Read the loop head
  in the retail bytes before changing either way.

- The ternary→`if/else` lever does **not** generalise by inspection.
  `army::CheckLuck` needs it; `ShowSpellCastFailure` is unaffected (168
  bytes either way). The 16 ternary rows in the frame queue must each be
  measured.
- `RESOURCE_NAME_CAPACITY` is not the MOUSEMGR SetPointer lever: 16→20
  fixes `SetPointer(char*)`'s frame and breaks `SetPointer(i32)`'s.
- `DrawAdventureBorder`'s sibling rename does not transfer (3 probes).
- `TurnToStone`'s `++m_limitCreatureCount[side][index]`: flat-`+` and
  commuted-bracket spellings both canonicalise identically to ours.

## Permuter workflow (confirmed working on this branch, 2026-08-03)

```
homm2 permute <src.cpp> <rva> --min-depth 1 --max-depth 2 \
    --families identifier_rename --rename-identifier <local> ... \
    --limit 80 -o /tmp/<fn>.json --run --top 8 \
    --allow-external-diagnostic 'template parameter redefines default argument' \
    --allow-external-diagnostic "template specialization requires 'template<>'" \
    --allow-external-diagnostic 'no member named' \
    --allow-external-diagnostic 'invalid operands to binary expression'
```

All four allowances are REQUIRED - without them the run dies on the MSVC
STL headers under clang. `--min-depth 0 --max-depth 0` generates the
baseline only; depth must be >= 1 to apply a mutation. Note the engine
generates its own suffixed rename candidates (`fooAmount`, `fooIndex`,
`fooCount`...) and ignores `--rename-candidate` values.

First use, on `townManager::SetArmyCommand` (6 disp diffs, whose chain
solver returned the identity): 8 rename arms all produced byte-identical
output at 99.6197%. That DISPROVES the rename hypothesis mechanically -
the residual is not a local-name problem. Same conclusion the identity
result implied, now measured rather than inferred.

### What the permuter reports that hand-probing does not

Its per-arm line carries `relocs A/B`. A MISMATCH there (ours/retail) is a
structural defect no rename can touch, and it is invisible to the masked
byte compare because the masker zeroes reloc sites on both sides.
`advManager::UpdBottomViewResMsg` reports `relocs 44/41` - we emit three
relocations retail does not, so its 13 "displacement" diffs are really a
call/global-reference difference. Check the reloc counts before spending
a slot solve on any function.

(That run also found a +0.024% rename arm using engine-generated names
like `iconWidth6Index` / `textY19Size`. Rejected under the prime source
rule: it does not close the function, and there is no natural name at the
required bucket for the textY local - the width slot wants bucket 0
(`wide` fits), the textY slot wants bucket 1 and nothing clean lands there.)

### DIR32 count census - RESOLVED, false alarm

Ran a reloc-count check across every same-length function: 72 mismatches,
every one exactly +3. Dumped the three unmatched site offsets for
`UpdBottomViewResMsg` and they are the **SEH registration triple**:

```
@0x0c  64 a1 00000000      mov eax, fs:[0]      ; __except_list read
@0x14  64 89 25 00000000   mov fs:[0], esp      ; install handler
@0x37c 64 89 0d 00000000   mov fs:[0], ecx      ; uninstall
```

Those are genuine COFF relocations against `__except_list` in our object,
but they are NOT image relocations - the operand is a segment-relative
`fs:` offset, so `config/delink_relocs.tsv` correctly omits them. All 72
rows are /GX functions and the +3 is the same three sites every time.

`homm2.audit.reloc_donation` already skips `__except_list`
(reloc_donation.py:203); my ad-hoc census simply failed to replicate that
rule. **The byte-proof harness was never wrong here** - retract the
earlier note claiming it has a reloc-count blind spot. `docs/
dir32-count-mismatches.tsv` is kept only as the worked example.

The permuter's `relocs A/B` line is still worth reading, but for /GX
functions expect ours to exceed retail by exactly 3.

## Work queues (artifacts, not prose)

| queue | file | rows | state |
|---|---|---|---|
| frame divergences | `docs/frame-overshoot-queue.tsv` | 231 | 155 ours-larger, 76 smaller |
| slot permutations | `scratchpad/slot_perms.py` (regenerate) | ~110 | 2–3 movers mostly drained; 4+ want the permuter |
| cross-version spellings | `docs/cross-version-spellings.md` | ~60 OPEN | verification deferred until 2.1 is complete |
| version changes | `docs/version-changes.md` | — | 9 value/structure dev-changes recorded |

## Task state

- **#16 slot solves** — tractable 2–3-mover band largely drained. Remaining
  rows need the permuter's `identifier_rename` axis, or have a hidden slot
  our model cannot see (`CongratsWait`, `Connect` both want a local we do
  not have).
- **#17 len-diff classes** — census done (578 fns); the bulk reclassified
  into #18 because the dominant cause is frame/shape, not spelling.
- **#18 body-diff** — MOUSEMGR 16/19 (was 8/19). Open there: `NewUpdate`
  (+20, needs the member-vs-recompute walk), `SetPointer` pair (−6/−18,
  retail LONGER so we are missing code). `ProcessCombatMsg` needs
  jump-table target mapping. Plus the 231-row frame queue.
- **#19 unpaired** — 11-TU STL include gap FIXED (+22 exact); audio stub
  indexes reconciled. Open: AudiereSampleNode COMDAT triplication, netwin's
  13 statics, and four TUs whose static-object COUNT differs (PHILAI,
  PATH, X_CAMPGN, AudiereMusic).
- **#20 data campaign** — GATED at ≥1500/1727 exact (user directive; it only
  pays once code matching is near done). Prep verified: `va.h` has no
  `DATA`/`VTBL` macros (2.0's shapes are the reference), the clang VarDecl
  inventory already parses `DATA(0x...)`, and all 10,421 current data rows
  are `reloc-manifest-target` placeholders.
- **#21 housekeeping** — funclet attribution, dead constants, and the
  force-push (`git push --force-with-lease origin decomp-gold-2.1-buka`)
  still awaiting the user.
