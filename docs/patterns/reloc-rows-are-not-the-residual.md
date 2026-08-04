# reloc-rows-are-not-the-residual

**Symptom.** A near-exact function's diff is dominated by relocation rows -
retail names `const_00123e50` / `$anon_str_<hash>_7` where we name the real
global, or retail carries `const_00118be1` where we carry `owner + 0x1`. It
looks like the whole residual, and it is not: those rows never scored.

**Rule (measured, this tree).** `build/objdiff/objdiff.json` sets
`"functionRelocDiffs": "none"`. objdiff compares the masked instruction text
only, so a differing relocation symbol *or addend* contributes exactly zero to
`fuzzy_match_percent` and to `functions-exact`. Naming the owner with a source
`DATA()` claim is an audit fix, not a score fix.

**Evidence.** `advManager::ComboDraw` (0xf30a). `homm2 sema disasm --diff`
before the claim was 1256 lines, 570 of them relocation rows:

```
-reloc DIR32	?bComboDraw@@3PAY0BC@CA          (ours)
+reloc DIR32	const_00123e50                   (retail, 57 sites)
+reloc DIR32	const_00123e4f                   (= owner - 1)
```

Adding `DATA(0x00523e50) i8 bComboDraw[...]` collapsed that to 90 lines and
moved the score by nothing at all: 99.30763% before, 99.30763% after, with
`diff` over all 2,473 per-function scores reporting zero changed rows.

**What the residual actually was.** Read the rows through the objdiff-cli
truth channel (`objdiff-cli diff -1 <target> -2 <base> <symbol> -o -
--format json`) and drop every row whose two sides differ only in a symbol
name. Twenty-one rows remained, all real bytes:

```
row  target                             base
502  movsx edx, byte [eax+ecx+bComboDraw]   movsx edx, byte [ecx+eax+bComboDraw]
1051 cmp [giDeferObjDrawX], eax             cmp eax, [giDeferObjDrawX]
1062 mov eax, [giDeferObjDrawY]             mov eax, [ebp+0xc]
1063 add eax, 0x1                           add eax, [ebp-0x14]
1064 mov ecx, [ebp+0xc]                     mov ecx, [giDeferObjDrawY]
1065 add ecx, [ebp-0x14]                    add ecx, 0x1
1273 mov eax, [ebp-0x10]                    mov eax, [giLimitUpdMaxX]
1274 cmp eax, [giLimitUpdMaxX]              cmp eax, [ebp-0x10]
1275 jle short ...                          jge short ...
```

Seven source edits closed it - the SIB order came from
`bComboDraw[0][drawY + drawX * COMBO_GRID_CELLS]` -> `bComboDraw[drawX][drawY]`
([flat-map-index-pointer-form](flat-map-index-pointer-form.md)), and the rest
were relational operand sides
([integer-relational-operand-side](integer-relational-operand-side.md)):
`giDeferObjDrawX == originX + drawX`, `giDeferObjDrawY + 1 == originY + drawY`,
`drawX > giLimitUpdMaxX`, `drawY < giLimitUpdMinY`, `drawY > giLimitUpdMaxY`,
`giLimitUpdMinX > giLimitUpdMaxX || giLimitUpdMinY > giLimitUpdMaxY`. 99.31%
-> EXACT.

The same mirage hid a one-row residual in `game::ShowMoraleInfo` behind 29
`cMoraleInfo + N` rows, and a six-slot frame permutation in
`townObject::townObject` behind five `sBuildingInfo + N` rows.

**Diagnostic.** Before planning work on a "relocation residual", strip the
symbol tokens from both sides of every differing row. If the rows collapse to
nothing, the function is score-blocked elsewhere; if a `+0xN` addend or a
register order survives the strip, that is the real byte difference. The
relocation identity is still worth claiming - `assert_relocs` reads it, and its
`FAKE call/ref` census fell from 1,141 functions to 117 across this campaign -
but claim it for the audit, and look elsewhere for the percentage.
