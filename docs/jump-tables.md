# Switch jump-tables cap switch/dispatch functions below 100% (delinker artifact)

**Symptom.** A function with a `switch` that MSVC lowered to a jump table (message dispatchers
`*::Main`, `*Handler`, ctors with a widget-type switch, …) reconstructs **byte-for-byte correct** yet
objdiff caps it a hair below 100% — the matcher parks it `@early-stop` with a "jump-table" reason.
Two matchers hit this independently on `heroWindow::heroWindow(int,int,char*)` (99.92%) and
`listBoxWidget::Main` (95.57%). It is **not** a reconstruction defect; it is how the delinked *target*
represents the table.

## Mechanism (byte-level)

MSVC emits the jump table as its own compiler-internal `$L` symbol, so the dispatch reads the table
at **disp 0** (the `$L` reloc supplies the address). Example — `heroWindow`, our compiled obj:

```
61f: ff 24 85 00 00 00 00     jmp  dword ptr [4*eax + 0x0]
        00000622: IMAGE_REL_I386_DIR32  $L2210          <- table is its own symbol, disp 0
626: <$L2210>:  dd $L2033, $L2036, $L2039, $L2042, $L2045   <- 5 entries, each a case-label symbol
```

CodeView records **no** `$L` label (it is compiler-internal), so `vostok-delinker` folds the table
into the owning function and names the dispatch `func+K`:

```
624: ff 24 85 d7 04 00 00     jmp  dword ptr [4*eax + 0x4d7]
        00000627: IMAGE_REL_I386_DIR32  ??0heroWindow@@QAE@HHPAD@Z   <- func + 0x4d7, disp 0x4d7
```

objdiff **masks the reloc symbol** ($L2210 vs heroWindow) but **not the displacement** (0x0 vs 0x4d7),
so that single `jmp` counts as a mismatch. Every other byte of heroWindow (all 320 instructions) is
identical → 99.92%.

### Tail vs mid tables

- **Mid-function table** (e.g. heroWindow): case bodies / epilogue follow the table, so *both* our
  compile and CodeView give the function the **same extent** (the table lives inside it). The only
  residual is the one `jmp` disp → ~99.9%.
- **Tail table** (e.g. `listBoxWidget::Main`, table at func+0x350, func end 0x368): MSVC ends the
  function at the code (0x350, a `retl` + a `mov edi,edi` align-nop) and puts `$L` *after* it, so our
  obj's Main is **0x350**; but CodeView's recorded size is **0x368** (code+table), so the delinked
  target Main includes the 0x18 table bytes. objdiff compares 0x350 vs 0x368 → the folded table is
  extra → 95.57%.

## Recovering the tables — `scripts/homm2/build/detect_jump_tables.py`

Ghidra does not surface these tables, so we recover them ourselves. The delinker already relocates
the dispatch and every table entry as `func+<off>` (DIR32), so a jump-table is a run of 4-byte-spaced
DIR32 **self-relocs** starting at the dispatch's disp `K`. The detector scans `build/delink/**/*.c.obj`
for `ff 24 <SIB base=101> <disp32>`, then counts that run to get the true entry count (robust to
mid-function and multi-table functions). Output `build/gen/jump_tables.csv`:

```
func_rva,func_name,table_rva,table_size
0xcecd0,??0heroWindow@@QAE@HHPAD@Z,0xcf1a7,0x14      # 5 entries, mid-function
0xdb520,?Main@listBoxWidget@@...,0xdb870,0x18        # 6 entries, TAIL
```

239 tables total (6 at a function tail, 233 mid-function). Run from repo root:
`python3 -m homm2.build.detect_jump_tables`.

## Why the fix is NOT in the synthetic PDB (tried, reverted)

Attempts to make the delinker reference the table at disp 0 by feeding `synth_pdb.py` a symbol at each
`table_rva`:

1. **`S_LDATA32` at the table address** — the delinker **drops** data/label symbols that land in
   `.text`; they never appear in the delinked obj.
2. **`S_GPROC32` (emit the table as a "function")** — this *works* for the reloc: the delinked
   dispatch becomes `jmp [4*eax + $Ttable]` at disp 0, matching MSVC. BUT it makes the delinker
   **slice the table into its own obj chunk and shrink the owning function**, and objdiff scores
   against the **CodeView-annotated function size** (0x368 for Main, which includes the table), so the
   shrink makes the function *shorter* than the compared extent → score got **worse** (Main
   95.57 → 94.48).
3. **Mid-function tables can't be split at all** — code follows the table, so shrinking would orphan
   it.

## The fix (for `vostok-delinker`, later)

The delinker should mirror MSVC: for each entry in `jump_tables.csv`, emit a **nested label** at
`table_rva` and reference the dispatch (and, ideally, the table entries) through it at **disp 0**,
**without changing the owning function's extent** (the function keeps its CodeView size; the label is
just an interior reference point, exactly as MSVC's `$L` is scl-6 *inside* the function). That makes
the delinked `jmp` byte-identical to our compile for every switch function in one shot.

Alternative (objdiff-side): compare the base obj's **actual symbol extent** rather than the CodeView
size, so a tail table our compile splits off doesn't count as "missing" — but this only helps tail
tables, not the mid-function `jmp`-disp residual, so the delinker fix is preferred.

`detect_jump_tables.py` + `build/gen/jump_tables.csv` are ready to drive either.
