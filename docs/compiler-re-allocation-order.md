# VC6 front-end allocation & emission order (C1XX.DLL RE)

Reverse-engineering of the pinned VC6 SP5 C++ front end to explain the object
topology residuals: deferred COMDAT emission order, and string-literal / data
cell allocation order.

Subject: `build/toolchain/msvc/bin/C1XX.DLL`, 1,206,323 bytes,
md5 `90b6789d3facaf549b002bf8285ca788`, PE32 i386, **image base `0x10400000`**
(`.text 0x10401000..0x104BDFFF`, `.rdata 0x104BE000..0x104D6BFF`,
`.data 0x104D7000..0x104EEBFF`). Companion `C2.DLL` md5
`4a52c159d7eae2f4c5063c49908d2383`, image base `0x10700000`.

All addresses below are **VC6 SP5 C1XX.DLL VAs at that base** unless marked
otherwise. Ghidra 12.0.4 headless was used; the project is scratch-local and
not retained, so every claim is stated with its address so it can be re-derived.

---

## 1. Prior art that already exists locally

Checked before doing any new work. Read this section before re-deriving anything.

| Where | What | Subject |
|---|---|---|
| `scripts/homm2/core/od_slots.py` | `/Od` stack-slot predictor; cites `C1XX.EXE FUN_0041d1a2`, `C2.EXE FUN_0044bb5e/FUN_0044c4df` | **MSVC 4.2** (cl 10.20), *not* VC6 — those addresses do not map here |
| `homm2-decomp/docs/od-stack-layout.md` | The origin doc, with raw 4.2 disassembly (`0x0041d312`–`0x0041d31a`) and the behavioral oracle | MSVC 4.2 |
| `homm3/homm3-decomp/docs/vc6/` | The largest body: `behavior-catalog.md` (entry **A14** "Emission order of deferred bodies"), `handle-order.md`, `il-format.md`, `c2-atlas.md`, `inliner.md`, `driver-passes.md` | **VC6** (RTM/SP3 hashes) |
| `homm3/homm3-decomp/build/re/vc6/c2-atlas.rep` | Ghidra project, 18 MB | **C2.DLL** |
| `homm3/homm3-decomp/evidence/vc6/c2-tu-map.tsv` | 2,484 C2 functions mapped to compiler source TUs by link-order interval (`coffemit.c 0x80ee1..0x82f0f`, `coff.c 0x801ee..0x80e14`) | C2.DLL |
| `gruntz/docs/compiler-data-layout.md` | `.data`/`.rdata` = declaration order (mechanism); `.bss` = end-of-TU hash walk, `check16 & 0x3FF`, LIFO chains. Cites `c1xx.dll` `FUN_1040e132`, `FUN_1040b2b6`, `FUN_10403ea6` at base `0x10400000`. **Explicit VC6 cross-check: "Order: identical. Alignment: different."** | **MSVC 5.0** c1xx.dll |
| `gruntz/docs/string-pooling.md` | `/Od` default = no pooling, one `$SG<n>` per *occurrence*; `/Gf` → content-named `??_C@` COMDATs; `/GF` → same in `.rdata` | MSVC 5.0 |
| `homm3` + `gruntz` | An **IL tap**: `/d1il<prefix>` captures the C1XX→C2 intermediate; `/d2il<prefix>` feeds it back | VC6 + VC5 |

**Nothing existed anywhere for C1XX.DLL / C1.DLL / CL.EXE** — no Ghidra, IDA,
r2 or Binary Ninja database. The `.idb` files under `build/link/` and
`survarium/` are MSVC *incremental-build* databases, not IDA.

Explicit gaps the prior art states it does **not** cover, and which this
document closes: `$SG<n>` allocation order; `??_G` emission order; `$E<n>` /
`.CRT$XCU` cell order within a TU.

> Provenance note: `/home/sheep/Projects/archive/crypto/winxpscodes/` contains a
> leaked MSVC source tree. The homm3 VC6 README forbids consulting it. It was
> **not** opened; everything here is from the shipped binaries and from probes.

---

## 2. New: C1XX / C2 internal module maps

`C2.DLL` retains its `__FILE__` assert strings — 48 paths under
`E:\8966\vc98\p2\src\{Common,P2,P2\x86}`, including `coff.c`, `coffemit.c`,
`emit.c`, `hash.c`, `p2symtab.c`, `reader.c`, `inline.c`, `dag.c`, `regasg.c`.
This confirms the build stamp **8966** and that C2 is the COFF writer (it also
owns the section-name table at file `0x9a384`: `.drectve .text .text$x .xdata$x
.debug$T .debug$P .debug$S .bss .data .const .xdata .rdata`).

`C1XX.DLL` strips the drive-qualified paths but keeps relative ones. Recovered
front-end module names: `p0gettok.c`, `p0io.c`, `p0macros.c`, `p0pragma.c`,
`p0prepro.c`, `cvserver.c`, `10010613.410\grammar.y`, `nbrowser.c`, `esu.c`,
`esumem.c`, `funcs.c`, `outdname.c`, `typesym.c`, **`toil.c`** ("to IL"),
`trees.c`, `main.c`, `pch.c`, `error.c`, `symtable.cpp`, `symtable.inl`,
`nheapall.c`, `Brepro.cxx`, `p1inl.inl`, `Token_IO.cpp`, `msc1.cpp`.

C1XX's release build asserts sparsely, so these give only 1–5 anchor functions
each — not the clean partition C2 has. Useful anchor: `toil.c` → `0x10415BCE`.

---

## 3. Question C — the hash function and table (SOLVED, on VC6 directly)

`FUN_10401213` is the **name-intern / lexeme-table** entry point. Every
identifier and every compiler-generated symbol name passes through it.
Decompiled:

```c
h = 0;
for (c = *name; c != 0; c = *++p)
    h = c + (h >> 4) + h * 4;            /* 32-bit wraparound */

bucket   = h & 0x7FF;                    /* 2048-bucket table, base DAT_104D7430 */
check16  = (h >> 16) ^ (h & 0xFFFF);     /* stored in the name node */
```

- **Table size: 2048 buckets** (`h & 0x7FF`), separately chained; a new node is
  bump-allocated 0x10 bytes from the arena at `DAT_104E2E10`/`DAT_104E2E14`, the
  text is copied into the `DAT_104ECC10` arena, and the node is **prepended** to
  its bucket.
- **Key hashed: the plain NUL-terminated byte string handed to intern.** That is
  the source identifier for ordinary names, and the *generated* spelling for
  compiler names (`$S20`, `$E21`), and the *decorated* name for function-local
  and class statics.

This is byte-for-byte the same arithmetic and the same `check16` fold that
`scripts/homm2/core/od_slots.py` documents from MSVC 4.2 `C1XX.EXE
FUN_0041d1a2`. **The 4.2-derived hash is hereby confirmed on VC6 SP5 (8966).**
`od_slots.py`'s "hypothesis until re-validated" caveat can be narrowed: the hash
and the `check16` fold are proven identical; only the *consumer* (C2's 16-bucket
per-scope frame table) remains to be re-confirmed on VC6.

Note the two masks are different consumers of one hash: the **lexeme** table is
2048-wide (`h & 0x7FF`), the **global symbol scope** is 1024-wide
(`check16 & 0x3FF`, per gruntz on VC5), and the **`/Od` local frame** table is
16-wide (`check16 & 0xF`, per od_slots on 4.2).

### Compiler-generated name formats

| Format | VA | Generator | Counter |
|---|---|---|---|
| `$S%u` | `0x104BF238` | `FUN_10423548` | `DAT_104D7BB0` |
| `$E%u` | `0x104D0C50` | `FUN_1044B2E1` | `DAT_104D7BB0` (**same**) |
| `$N%u` | `0x104D1260` | at `0x10474463` | argument |

Both generators are `n = DAT_104D7BB0++; sprintf(buf, fmt, n); intern(buf);` —
one shared anonymous-symbol counter, which is why `_$S3` and `_$E1/_$E2`
interleave numerically in one object.

`$SG` and `??_C@` are **not** string constants anywhere in C1XX, C2, C1 or
MSOBJ10 — they are assembled character-by-character by the mangler
(`outdname.c`). The `$SG<n>` ordinal is the symbol's creation ordinal, which is
why substituting an older front end shifts every `$SG` by a constant (the
prior-art +451 observation) without changing any order.

---

## 4. Question A — the deferred-emission queue (SOLVED)

### 4.1 The data structure

`FUN_10427303` — **enqueue**, `__thiscall(queue, item, kind, flag, callback, ctx)`:

```c
if (kind == 10) kind = item->field_0x30;      /* kind carried by the item */
node = *(&DAT_104ECC10 + kind*8);             /* per-KIND bump arena      */
*(&DAT_104ECC10 + kind*8) = node + 5;         /* 5 dwords per node        */
if (*(&DAT_104ECC14 + kind*8) < node+5) grow(...);   /* FUN_1040BF3C */
node[0] = item;
node[1] = queue->head;      /* ---- PREPEND ---- */
node[2] = *ctx;
node[3] = flag;
node[4] = callback;
queue->head = node;
```

So: a singly-linked list of 5-word nodes, bump-allocated from a **per-kind
arena** indexed by `kind` at `&DAT_104ECC10 + kind*8` (value/limit pairs), and
**prepended** to the head. Queue objects are `{ctx @ +0, head @ +4}`:

| Queue object | Head |
|---|---|
| `0x104DA6D8` | `DAT_104DA6DC` |
| `0x104DA6D0` | `DAT_104DA6D4` |
| `0x104DA6C0` | `DAT_104DA6C4` |
| `0x104DA710` | `0x104DA714` |

There are **41 enqueue call sites** — every construct that can defer a body.

### 4.2 The driver

`FUN_1040ABD0` is the pending-work driver. Normal path: when queue A
(`DAT_104DA6DC`) is empty it pops **one** item from queue B (`DAT_104DA6D4`),
runs its callback, then **re-enqueues that item onto the `0x104DA710` queue**
with callback `0x1040ABB2`, bumps `DAT_104D7218`, and returns; the caller loops.
Prepend-into-B followed by pop-and-prepend-into-C is a **double reversal**, so
the net traversal is **FIFO in registration order** — which is exactly what the
probes show.

When all queues drain, `FUN_1044B373` runs the **dynamic initializers**:

```c
for (i = 0; i < DAT_104D7438->count; i++)      /* counted growable ARRAY   */
      ... emit initializer for element i ...   /* front-to-back = reg order */
FUN_1044B65F(DAT_104E6A88);                    /* emit the .CRT$XCU cell   */
```

`DAT_104D7438` is a counted array (u16 count at +0, element pointer at +8), not
a list — walked front-to-back, i.e. **registration (declaration) order**.

`FUN_1044B65F` then allocates the `.CRT$XCU` cell: it gets the segment
(`FUN_1044BB1D`), makes a `$S<n>` name (`FUN_10423548`), creates the symbol
(`FUN_1044B7A6`) and emits the pointer initializer. `#pragma init_seg` is
`DAT_104D7B9C` indexing a segment-name pointer table at **`0x104D0C58`**
(`.CRT$XCZ 0x104D0C6C`, `XCC 0x104D0C78`, `XCL 0x104D0C84`, `XCU 0x104D0C90`,
`XCA 0x104D0C9C`; the `compiler`/`lib`/`user` keywords at `0x104BF1EC`,
`0x104BF1E8`, `0x104BF1D8`).

### 4.3 The measured rule

Probes `g1.cpp` / `g2.cpp` (three template instantiations with virtual
destructors, used in permuted order):

> **Deferred COMDAT emission order is strictly FIRST-USE order.** Permuting the
> first-use order in the source permutes the emission order identically. The
> `??_7` vtable is emitted immediately before its `??_G`, as an adjacent pair,
> at the point of first use.

### 4.4 The consequence that explains the AudiereEffects residual — NEW

Because `FUN_1044B373` (dynamic initializers) runs **after** the pending-work
queues drain, anything emitted from the work list necessarily precedes every
`$E<n>` thunk. Retail `BASE/AudiereEffects` has `??1AudiereSampleNode` **after**
`_$E21`/`_$E20`. Under this mechanism that is only reachable one way: **the
destructor's first use in the TU is inside a dynamic initializer.**

Probe `k1.cpp`, compiled `/Od /MT /Gr /G5 /Ob1 /Gy /Gi- /GX`:

```cpp
template<int N> struct R { int v; virtual ~R() { v = 0; } };
void u1() { R<1>* q = new R<1>(); delete q; }
R<2>* gp = new R<2>();                  // first use of R<2> is a dynamic init
```

Resulting section order:

```
[4] .text   ?u1@@YIXXZ
[5] .rdata  ??_7?$R@$00@@6B@          <- R<1>, first-used in u1
[6] .text   ??_G?$R@$00@@UAEPAXI@Z
[7] .text   _$E2                       <- dynamic-init thunks, DESCENDING
[8] .text   _$E1
[9] .rdata  ??_7?$R@$01@@6B@          <- R<2>, first-used in the initializer
[10] .text  ??_G?$R@$01@@UAEPAXI@Z
```

This reproduces the retail shape exactly, including the **descending** thunk
order (`_$E2` before `_$E1`; retail has `_$E21` before `_$E20`).

**Actionable**: the retail `AudiereEffects.cpp` most likely had a file-scope
object whose *dynamic initialization* was the first thing in the TU to require
`AudiereSampleNode::~AudiereSampleNode`, with no earlier use. The prior-art
matrices covered include order, ownership forms, `__forceinline`, template
specialization/instantiation, and cross-TU first-definer — but never
"first use inside a dynamic initializer". That axis is untested and is the one
the mechanism predicts.

### 4.5 The phase-routing rule (pre-`$E` vs post-`$E`)

The main session independently observed that emission has two sweeps: our
`AudiereEffects.obj` runs ordinary functions, then deferred inlines, then
`$E21`/`$E20`, and then a *post-`$E`* item (`?id@?$ctype@G@std@@$E`) that was
generated **during** `$E` processing. The mechanism above explains it exactly,
and it is not a flag:

> **An item is emitted in the phase in which it first becomes required.** The
> pending-work queues (`FUN_10427303` / `FUN_1040ABD0`) are drained *before*
> `FUN_1044B373` walks the dynamic-initializer array. Anything first required
> while the queues are being drained lands **pre-`$E`**. Anything first required
> *while the initializers themselves are being emitted* is enqueued after that
> drain and therefore flushed **post-`$E`**.

There is no routing bit and no per-item priority field — the 5-word queue node
(`item, next, ctx, flag, callback`) carries no ordering key. Routing is purely
"when did you first get queued", which is why probe `k1` moves a COMDAT across
the `$E` boundary purely by moving its first use into a dynamic initializer.

This also matches the coordinator's point 3 (renaming classes does not perturb
any order): the queues are an insertion-ordered list and a counted array. **No
hash walk is involved in question A at all** — the only hash tables in this path
are the lexeme table (§3) and the sym→segment map (§5.1), and neither is
iterated to produce emission order.

### 4.6 `??_7` and `??_G` are an atomic, eager pair

Probes `m1.cpp` / `m2.cpp` (`/Gy`), construction and destruction deliberately
split across different functions:

```cpp
template<int N> struct R { int v; virtual ~R() { v = 0; } };
R<1>* gp;
void ctor_site() { gp = new R<1>(); }
void middle()    { gp->v = 1; }
void dtor_site() { delete gp; }      /* m2: gp->~R();  — no delete at all */
```

Both give `ctor_site`, **`??_7` immediately followed by `??_G`**, then `middle`,
then `dtor_site`. The deleting destructor is emitted with the vtable at the
first *vtable* requirement, **even in `m2` where no deleting destructor is ever
needed**. So `??_G` generation is unconditional and eager, and it cannot be
separated from `??_7` by any arrangement of use sites.

For retail `BASE/DIMMER` (`… Draw, ??_G, ??1, $E19, $E18`) this means the
vtable's first requirement was *not* either constructor. Within a single TU
under this front end there is no source arrangement that keeps both
constructors emitted early while deferring the pair — consistent with all
eleven `dimmer-dtor-shapes` arms and the six declaration orders the campaign
already measured, and with the fact that only suppressing every constructor
moved the pair. The DIMMER placement is therefore **not** reachable by
declaration-level source shaping; the residual is a genuine unrecovered
first-requirement difference, not a spelling.

---

## 5. Question B — string-literal / data cell allocation (SOLVED)

### 5.1 The machinery

| Function | Role |
|---|---|
| `FUN_1044BB1D(name, class, align)` | get-or-create **segment**; miss → ordinal `DAT_104D7BA0++`, create via `FUN_1044BB7A` |
| `FUN_1044B757(table, name)` | segment lookup in `DAT_104D7534` |
| `FUN_1044B7A6(name, type, seg)` | create a **static data symbol** in a segment (`FUN_10447EDC` + `FUN_1044B810`) |
| `FUN_1044B810(sym, seg)` | if `sym->field_0x20 & 0xFFFFFF == 0`, assign ordinal `DAT_104D7BA0++`; record sym→seg in the 31-bucket map `DAT_104D7B3C` (created `FUN_10413FCB(0x1f,0,3,2,0,1)`, insert `FUN_104143F8`) |

`DAT_104D7BA0` is **one monotonic ordinal counter shared by segments and
cells**. The sym→seg association is a map, so it carries no order; the final
byte offsets are assigned by C2 in **record-arrival order** — the VC5 mechanism
in `gruntz/docs/compiler-data-layout.md`, unchanged on VC6. The arrival order is
therefore decided by `toil.c` when C1XX streams the records.

### 5.2 The measured rule

Probes `p1`, `e1`–`e3`, `f1`–`f3`, all `/Od /MT /Gr /G5 /Ob1 /Gi- /GX`, **no
`/Gf`** (the profile `BASE/Misc` and `SOURCE/REQUEST` actually use):

> Within a TU, cells are emitted in **three groups**, in this order, and the
> lexical position of the definitions does **not** move an item between groups:
>
> 1. file-scope variables (declaration order) — uninitialized ones in `.bss`,
>    initialized ones in `.data`;
> 2. string-literal cells referenced by **file-scope initializers**, in the
>    declaration order of those initializers;
> 3. function-local statics, then all **function-body** literal cells, in
>    lexical order.

`f1.cpp` (`.bss`): `gA, gB, gC`, then `$SG271, $SG278` (the two global-init
literals), then `$SG269, $SG276` (the two function-body literals).
`f2.cpp` (`.data`): `p1, p2`, then `PPPP, QQQQ`, then `AAAA, BBBB`.

### 5.3 A correction to the recorded prior art

`docs/matching/…` records this as *"global-initializer literal cells are
batched/reserved BEFORE function-level literal cells"*, implying the global's
cell is reserved first. **The numbering proves the opposite.** In `e2.cpp` the
global is defined *last*:

```cpp
void InitializeFiles() { char* a = ""; }
void Open()            { char* b = ""; }
void Main()            { char* c = ""; }
char* cFRDummy = "";
```
```
.bss  +0x00 $SG278   <- cFRDummy's literal: HIGHEST ordinal, allocated LAST
      +0x04 $SG268
      +0x08 $SG272
      +0x0c $SG276
```

`$SG` numbering is strictly **lexical/allocation** order; placement is a
separate **emission-phase grouping**. The global's literal is reserved last and
emitted first. This matters because it rules out any "reserve the cell earlier /
later" source trick: allocation order is already what retail wants, and it is
the *phase* assignment that differs.

---

## 6. Is there ANY reachable compiler state that flips A or B? — **No**

### 6.1 The undocumented front-end option surface

`/d1<text>` pass-through to C1XX is **proven**: `/d1ZZZbogusZZZ` gives
`fatal error C1048: unknown option 'Z' in '-ZZZbogusZZZ'`. So `/d1X` reaches the
front end as `-X`.

C1XX's **complete** option surface is a table of 12-byte records
`{ const char* name; void* target; u32 kind; }` at file `0xCEB8C..0xCF078`
(VA `0x104CF78C..0x104CFC78`), **103 records**, decoded in full. `kind 0x0101` =
set byte to 1, `0x0501` = clear, `0x2201/0x2401/0x2601` = takes a
string/number/list argument.

Every option is a documented CL flag except these 15, which are internal:

| Option | Target global | Note |
|---|---|---|
| `-BnoWorkList` | `0x104E69E9` | names the deferred work list — see below |
| `-BforceInst` | `0x104D8106` | force template instantiation |
| `-Bshow_incl` | `0x104E69EA` | |
| `-Bdlp` | `0x104D8105` | |
| `-BMOVE` | `0x104D8824` | |
| `-BNOPPCH` | `0x104D80C0` | clears |
| `-Binl` | `0x104E6080` | |
| `-noexpinl` | `0x104D8174` | |
| `-cbstring` | `0x104D7D50` | adjacent to `-Gf 0x104D7D4C` / `-GF 0x104D7D4E` |
| `-C9IL` | `0x104D8088` | |
| `-NoEHForNew` | `0x104D808E` | |
| `-SplitPdbs` / `-SplitPdbs-` | `0x104D808D` | |
| `-noBool` | `0x104D7D6B` | |
| `-int2enum` | `0x104E69E8` | |
| `-debugBreak%` | `0x104E7864` | |
| `-il$` | `0x104D8144` | the IL tap the homm3/gruntz work uses |

**No option in the table targets any global read by the emission driver
`FUN_1040ABD0`, by the enqueue `FUN_10427303`, by the dynamic-initializer walk
`FUN_1044B373`, or by the segment/cell allocators `FUN_1044BB1D` /
`FUN_1044B7A6` / `FUN_1044B810`.** The whole ordering path is unparameterised.

`DAT_104E69FC`, the flag that sends `FUN_1040ABD0` and `FUN_1044B373` down their
alternate early-out paths, looked like a mode bit but is not: its only writers
are `FUN_1048A1B6`, `FUN_1048A318` and `FUN_1043C4AE`, and it is read by
`FUN_1040BF94` (the `error.c` anchor). It is the **fatal-error / abort-in-progress
flag** — the alternate path drains the queues *without* emitting.

`-BnoWorkList` (`DAT_104E69E9`) is read at exactly **two** sites, `0x104422C5`
and `0x10449450`, and both have the same guard:

```asm
MOV AL,[0x104E2DC9]   ; secondary-work-list mode
TEST AL,AL
JZ   skip             ; inactive in our configuration -> skip anyway
MOV AL,[0x104E69E9]   ; -BnoWorkList
TEST AL,AL
JNZ  skip
...                   ; walk a symbol list at [EDI+0x228], chain [EDI+4]
```

So it suppresses a *secondary* list that only runs when `DAT_104E2DC9` is set
(the `/Gm` / `/FR` / PCH-era state cluster, both already swept by the campaign
with no topology change). With `DAT_104E2DC9` clear the option is inert — which
is exactly what the behavioural sweep measured. **`-BnoWorkList` is not the
deferred-emission knob.**

### Environment and registry surface

`CL.EXE` reads the two documented environment variables `CL` and `_CL_`
(strings at file `0x6003` and `0x5FFC`) plus `INCLUDE`. These only *prepend or
append command-line text*, so they reach the same 103-record table and add no
state beyond it. `C1XX.DLL` calls `getenv` but reads no registry: there are no
`RegOpenKey`/`RegQueryValue` imports and no `Software\Microsoft` path strings in
C1XX or C2. There is no hidden configuration channel.

### 6.2 The sweep

Every boolean option above was applied via `/d1` to all four residual TUs
(`BASE/AudiereEffects`, `SOURCE/REQUEST`, `BASE/Misc`, `BASE/DIMMER`) on top of
their real flag profiles, comparing a full section-topology fingerprint
(ordered section names + sizes + ordered symbol names per section):

| Option | Result |
|---|---|
| **`-BnoWorkList`** | **accepted, SAME on all four** — a no-op here |
| `-cbstring` | SAME on all four |
| `-Bshow_incl`, `-SplitPdbs`, `-C9IL`, `-BMOVE`, `-noexpinl`, `-BNOPPCH`, `-EHc`, `-Binl`, `-GFR`, `-GFR-`, `-int2enum`, `-ZBe`, `-Brepro` | SAME on all four |
| `-NoEHForNew` | **DIFFERENT** on ae/req/misc — but only code sizes (`.text 2041→2028`, `.text$x 39→28`, `.xdata$x 56→48`); **section order unchanged**. A codegen change, not an ordering lever |
| `-BforceInst`, `-noBool`, `-Bdlp`, `-SplitPdbs-`, `-FAT` | compile failure (semantic or path-mode changes) |

### 6.3 Pragmas

- `#pragma bss_seg(".bss")` before the global — **no effect** (global literal
  still first).
- `#pragma data_seg(".data")` around the global — **does** change topology: the
  global-init literal is forced into `.data` and `.bss` creation is deferred
  until after `.text`, leaving the three function cells at `.bss+0/4/8`. It
  removes the cell from `.bss` rather than reordering within it, so it does not
  produce retail's six-cell `.bss` with the global's cell last. (Consistent with
  the campaign's earlier `data_seg` rejection, commit `17e21f46`.)

### 6.4 Verdict

**No reachable compiler state — documented flag, undocumented `-d1` option,
environment variable, registry value, or pragma — flips A to tail emission or B
to lexical order.**

In particular, the attractive hypothesis of *"a single mode bit switching
'process global initializers eagerly at parse' vs 'defer to codegen'"* is
**disproved as a compiler option**: no entry in the 103-record table targets any
global on the emission path, and the eager/deferred split is not a branch on a
mode byte but the structural consequence of *which phase creates the record*.
The same applies to a global "generate-on-demand" mode for §4.6 — `??_G`
generation has no predicate at all; it is unconditional.

Both orders are structural properties of the front end:

- **A** is fixed by the *first-use* order of the referencing code, because the
  work queues are traversed in net-FIFO registration order and the dynamic
  initializers are a separate array walk that always runs last.
- **B** is fixed by the *emission phase* in which a cell is created, because
  C2 assigns offsets in record-arrival order and `toil.c` streams file-scope
  variables, then file-scope-initializer literals, then function-body literals.

The only lever on either is **which source construct first requires the item** —
which is a property of the original source, not of the compiler invocation.

---

## 7. Reproduce

Ghidra 12.0.4 headless (no r2/rizin in the shell):

```sh
GH=$(dirname $(readlink -f $(which ghidra)))/../lib/ghidra/support/analyzeHeadless
$GH <projdir> c1xx -import build/toolchain/msvc/bin/C1XX.DLL \
    -processor x86:LE:32:default -cspec windows      # ~280 s
```

`analyzeHeadless` in this build has no Jython; query the saved project through
PyGhidra instead (`GhidraProject.openProject` + `project.openProgram`, and wrap
every Java iterator with `hasNext()/next()` — bare `for` over them yields
nothing and silently looks like "no xrefs").

Probes were compiled with `scripts/homm2/build/cc_wrap.py` under
`nix develop .#build`; the COFF topology dumper and the option sweep are small
standalone scripts (section table + symbol table walk, no external deps).

---

## 8. Open / next

1. **Test the dynamic-initializer first-use axis on `BASE/AudiereEffects`** —
   §4.4/§4.5 predict it is the only same-TU shape that puts
   `??1AudiereSampleNode` after `_$E21`/`_$E20`, and probe `k1` reproduces the
   retail shape *including* the descending `$E` order. This is the one
   untested axis in the whole campaign and it follows directly from the
   mechanism. Concretely: a file-scope object in `AudiereEffects.cpp` whose
   **dynamic initializer** is the first construct in the TU to require the node
   destructor.
2. `SOURCE/REQUEST` and `BASE/Misc` get no such opening: §5.2 puts the
   file-scope-initializer literal in phase 2 for *every* spelling measured
   (including the dynamic, ternary/comma and folded `"" + 0`, `&""[0]` forms the
   main session tried). To reach retail the literal must be created in phase 3,
   which means it must be first required by *function-body* codegen — i.e. the
   original `cFRDummy` / track-name cell was not initialized from a file-scope
   initializer at all in the retail source.
3. The `-il` tap (`/d1il<prefix>` capture, `/d2il<prefix>` feed — confirmed
   present here as option record `-il$` → `0x104D8144`) would let the
   record-arrival order be read directly rather than inferred from the object.
   Note gruntz `build/il-probe/REPORT.md` §5 records that the injected-`-il`
   path itself perturbs COMDAT emission order on large TUs — unexplained, and
   worth resolving before trusting a tapped order.
4. C2's `coffemit.c` (`0x80EE1..0x82F0F` in `C2.DLL`, 24 functions, already
   anchored in homm3's `c2-tu-map.tsv`) is the place to confirm the
   record-arrival→offset assignment directly.
5. `DAT_104E2DC9` — the predicate that gates the secondary work list
   `-BnoWorkList` suppresses — is the one piece of front-end state on the
   emission path whose setter was not chased. It is adjacent to `DAT_104E2DC8`,
   which `FUN_1044B373` tests on the dynamic-init path. Worth 20 minutes if
   anyone wants to close the "is there a second work list that could reorder"
   question completely; the behavioural sweep already says it is inactive under
   every flag combination the campaign uses.

---

## 9. Follow-ups (round 2)

### 9.1 `DAT_104E2DC9` / `DAT_104E2DC8` — chased, and RULED OUT

**Sole writer of both: `FUN_10445981`** (a `main.c` module function, the
per-compilation setup called first by `FUN_10445601`), at `0x10445AA9` and
`0x10445AB8`. The condition is now named:

```asm
10445a84  MOV DL,[0x104D8098]   ; -Yu   target  (USE a PCH)
10445a8f  MOV AL,[0x104D80A0]   ; -Yc   target  (CREATE a PCH)
10445a94  CMP AL,BL             ; BL == 0
10445a96  JNZ skip              ;   -Yc set   -> skip
10445a9c  CMP DL,BL
10445a9e  JZ  skip              ;   -Yu clear -> skip
10445aa9  MOV [0x104E2DC9],1    ; secondary work list ON
10445ab8  MOV [0x104E2DC8],AL   ; = 0 on this path
```

> `DAT_104E2DC9` is set **exactly when `/Yu` is on and `/Yc` is off** — i.e. the
> PCH-*consuming* compile, precisely the MSDEV IDE-era build mode. No option
> targets `0x104E2DC8`/`0x104E2DC9` directly; they are pure derived state.
> `DAT_104E2DC8` is set to **0** on that same path, so `FUN_1044B373`'s
> `DAT_104E2DC8 != 0` branch is *off* whenever the secondary list is *on* —
> they are alternatives, not a pair.

**Drain order:** `FUN_10442285` (the `-BnoWorkList`-guarded site) is called from
`FUN_10445601`, the end-of-compilation routine. `FUN_1040ABD0` (deferred drain +
`$E`) has no caller in `FUN_10445601`; its only entry is the `FUN_1040ABB2`
trampoline used as a queue callback, i.e. it is driven from inside the emit
loop. **So the secondary list does drain after the `$E` walk** — the structural
prerequisite the coordinator hypothesised is real.

**But it carries no code.** Measured directly: a real `/Yu` PCH-consuming build
of `src/BASE/AudiereEffects.cpp` (PCH created in a separate `/Yc` step, then
`/FIpchhdr.h /Yupchhdr.h /Fp…`) yields the **same 11 sections in the same order
with the same sizes and the same function/COMDAT symbols** —
`??1AudiereSampleNode` is still the first COMDAT. The *only* difference in the
whole object is two anonymous EH-table ordinals in `.xdata$x`
(`$T55767/$T55771` → `$T55765/$T55769`), a uniform −2 shift because the PCH
consumed two values of the shared anonymous-symbol counter — which is itself an
independent confirmation of §3's single-counter finding, and is exactly the
class of difference §5.3 shows carries no ordering information.

Consistent with what `FUN_10442285` actually walks: it
is handed `DAT_104D7534` (the *segment* table) and iterates a per-segment list
at `[EDI+0x228]` testing `[EDI+0x28]`, then checks `-C9IL` — PCH-deferred
symbol/debug records, not function bodies.

**Verdict: the secondary work list cannot explain any of the three residuals.**
No IDE-era build state reorders code emission.

### 9.2 The registration rule for a not-yet-seen inline body

Probes `n1`–`n5`, a deliberate model of the AudiereEffects TU (fn1 deletes
through a pointer, three more functions using other inlines whose bodies were
visible from the start, a file-scope dynamically-initialized object, and the
destructor's body only at EOF):

| Probe | Body form | Object pos | Result (`base_gx`) |
|---|---|---|---|
| `n3` | defined **in-class** | after fns | `??1Node` emitted at **first use** (right after Purge) |
| `n1` | out-of-line `inline` at EOF | before body | `Purge…Stop, _$E2, _$E1`, then `??1Node` **last** |
| `n2` | out-of-line `inline` at EOF | after body | same — `??1Node` **last** |
| `n4` | `n1` + `inline` keyword *in-class too* | after fns | same — `??1Node` **last** |
| `n5` | `n4` + object defined **first** | before fns | `_$E2,_$E1, Purge…Stop`, then `??1Node` **last** |

> **Rule.** For a member function whose body has **not yet been seen** at the
> call site, nothing can be enqueued there; the out-of-line copy is emitted at
> its **definition** point. A body defined at EOF therefore lands after
> everything defined earlier — including the `$E` thunks. A body defined
> **in-class** is available at the call site and is enqueued there, so it is
> emitted in first-use order. The `inline` keyword on the in-class *declaration*
> is irrelevant (`n4` == `n1`).

Critically, **`Purge` is byte-identical across `n1`/`n2`/`n3`** (0x4d, 3 relocs,
`REL32 -> ??1Node@@QAE@XZ` at `+0x1e`): this axis is free on the caller side, so
it cannot cost anything already matched.

**New rule discovered in `n5`:** the `$E` thunk **body** is emitted at the
*source position of the initialized object's definition*, not at end of TU
(`n5` puts `_$E2`/`_$E1` at `.text+0` because the object precedes the
functions). Only the `.CRT$XCU` cell is created by the end-of-TU
`FUN_1044B373` walk. This refines §4.2.

### 9.3 The gap that remains for AudiereEffects — a sharp constraint

`src/BASE/AudiereEffects.cpp:203` **already** uses the EOF out-of-line form
(`inline AudiereSampleNode::~AudiereSampleNode() {}`, declared
`inline ~AudiereSampleNode();` at `include/BASE/soundBackends.h:21`), so §9.2's
axis is already satisfied — yet ours still emits it as the *first* COMDAT. The
residual is therefore **not** the definition-site axis. Retail's delinked
ground truth is one contiguous `.text` run:

```
…12 functions… , ??1RefPtr<AudioDevice> @0x7d0 , ??4RefPtr<OutputStream> @0x7fc ,
_$E21 @0x850 , _$E20 @0x878 , ??1AudiereSampleNode @0x88c
```

Three differences from ours, each now a precise statement about the retail source:

1. Retail never emits `??1RefPtr<OutputStream>` at all — we do. So in retail no
   `RefPtr<OutputStream>` was ever destroyed by value in this TU.
2. Retail's helper order is `RefPtr<AudioDevice>::~RefPtr` then
   `RefPtr<OutputStream>::operator=`; ours is the reverse plus an extra. By §4.3
   that is a first-use ordering statement about the twelve functions.
3. `??1AudiereSampleNode` sits **after** both `$E` thunks, so by §4.5 its first
   requirement came *during* dynamic-initializer emission — i.e. from the
   object's atexit destructor, not from `PurgeFinishedAudiereSamples`.

**Unresolved tension worth flagging:** by §9.2's `n5` result the `$E` thunk
bodies follow the object's source position, and retail's are late (`0x850`),
which implies the dynamically-initialized object was defined *after* all twelve
functions — but retail's only `.bss` symbol is `_gAudiereEffects` at `.bss+0`,
and by §5.2 `.bss` order is declaration order, which implies it was defined
*first*. Both cannot hold for a single object. The most likely resolution is
that retail's `$E21`/`$E20` belong to a **different, late-defined** file-scope
object than `gAudiereEffects` (one whose storage is not in this TU's `.bss` —
e.g. a `RefPtr`/`AudioDevicePtr`-typed static, which would also explain
difference 1). That is the next thing to test, and it is a source-shape
question, not a compiler question.

---

## 10. Follow-ups (round 3) — the AudiereEffects tail as a constraint problem

### 10.1 Reading (c) is DEAD — the `$E` pair's owner, pinned by relocation

The delinked retail object's tail relocations settle it:

```
_$E21 @0x850   +0x856 DIR32 -> ??_B?1???id@?$ctype@G@std@@$D@@9@51   (local-static guard)
               +0x863 DIR32 -> ??_B?1???id@?$ctype@G@std@@$D@@9@51
               +0x86c DIR32 -> ??_B?1???id@?$ctype@G@std@@$D@@9@51
               +0x871 REL32 -> _$E20
_$E20 @0x878   +0x87c DIR32 -> ?id@?$ctype@G@std@@$E
               +0x881 REL32 -> _atexit
.CRT$XCU       +0x000 DIR32 -> _$E21
```

> **Retail's `$E21`/`$E20` are the `std::ctype<G>::id` guarded-local-static
> machinery** — `$E21` is the guarded initializer (tests/sets the `??_B…@51`
> guard, calls `$E20`), `$E20` registers the facet's destructor with `_atexit`.
> The `?1?` in the guard's decoration marks a **function-local** static.

There is no late-defined `RefPtr`-typed file-scope object. My §9.3 reading (c)
is refuted, and with it the §9.3 "unresolved tension" — retail's `$E` position
has nothing to do with `gAudiereEffects`, so there is no conflict with
`_gAudiereEffects` sitting at `.bss+0`. The `$E` pair's position is set by where
the **ctype accessor containing that local static** was emitted.

Also confirmed from the same dump: Purge's two delete sites (`+0x65`, `+0x114`)
are `REL32 -> ??1AudiereSampleNode@@QAE@XZ`, the **tail** body at `0x88c`.

### 10.2 The retail tail is reachable — proven

Probes `q1`, `q2`, `q3` (`/Gy`), modelling the TU as: a delete site, a function
using a helper's dtor + `operator=`, a **function-local guarded static**, and
the node destructor defined **out-of-line at EOF**:

```
[3] Purge                      <- delete site
[4] Play                       <- calls ??1Ref / operator=
[5] .text$x  [6] .xdata$x      <- EH funclets
[7] ??1Ref              0x13   <- FIRST dtor body
[8] ?id@Facet (guarded) 0x34   <- accessor holding the local static
[9] _$E2 (atexit)       0x0f
[10] Use
[11] ??1Node            0x13   <- SECOND dtor body, LAST
```

That is retail's `[0x2c dtor][0x54 assign][$E21][$E20][0x2c dtor]` shape: **two
dtor bodies straddling the guarded-static `$E` machinery, the EOF-defined one
last.** So the target topology is *reachable* under the pinned toolchain.

**Option (b) is unnecessary.** `q2` (node class templated, `typedef` preserving
the spelling, dtor instantiated in the end-of-TU walk) and `q3` (plain class,
dtor defined out-of-line at EOF) produce **byte-identical topology**. Templating
buys nothing; §9.2's definition-point rule alone does the work.

**Option (a) is unnecessary too** — no inlining-at-the-delete-site trick is
needed; the delete sites keep their `REL32` call to the tail body (§9.2 already
showed the caller bytes are invariant across all these forms).

### 10.3 The one remaining gap, stated exactly

In `q1`/`q2`/`q3` the guarded-static accessor is defined **mid-file, before** the
destructor's EOF definition. In our real `AudiereEffects.cpp` the ctype
requirement lands **last** (section 11, the 5-byte `?id@?$ctype@G@std@@$E`),
*after* the line-203 definition — which is exactly why our node destructor
emits early instead of last. Everything else already matches.

> **The required source shape is therefore:** keep
> `AudiereSampleNode::~AudiereSampleNode` defined out-of-line at EOF (as it
> already is), **and arrange for the `std::ctype<G>::id` accessor to be required
> earlier in the TU than line 203** — i.e. between `PlayAudiereSample`'s
> `RefPtr::operator=` use and the destructor's definition point.

What triggers that requirement in our build is `<locale>` machinery reached
through the audiere headers; it currently instantiates at the very end. The
open question is a *header/instantiation-timing* question, not a compiler one:
find what in retail's TU touched the `ctype<G>` facet mid-file. Note retail's
ctype machinery is `0x28 + 0x14` bytes (a separate guarded init **and** an
atexit thunk) while ours is a single 5-byte thunk — so retail's `ctype<G>`
instantiation is not merely repositioned but *differently shaped*, which is a
strong hint that the trigger is a real use, not an incidental one.

### 10.4 DIMMER — the recipe does NOT transfer; the pair stays eager

Probes `d1` (virtual dtor defined out-of-line at EOF, after both ctors and
Read/Main/Draw) and `d2` (dtor defined first):

```
d1: ??0Widget() , ??_7 , ??_G , ??0Widget(int) , Read , Main , Draw , ??1Widget
d2: ??1Widget   , ??_7 , ??_G , ??0Widget()    , ??0Widget(int) , Read , Main , Draw
retail: ??0dimmerWidget() , ??0dimmerWidget(FFFFFF) , Read , Main , Draw , ??_G , ??1 , $E19 , $E18
```

Moving the destructor to EOF **does not** move the pair: in `d1` `??_7`/`??_G`
still emit immediately after the *first constructor*, confirming §4.6 — the
pair is eager and rides the first vtable requirement, which a constructor always
is. Retail's `??_G` sits after `Draw`, so in retail the constructors were **not**
the first vtable requirement.

I could not construct any state in which a constructor references `??_7`
without triggering pair emission, and there is no compiler flag for it (§6).
**DIMMER therefore remains unexplained** — the AudiereEffects recipe does not
transfer, and I am not going to guess a mechanism for it. The honest next step
is to check whether retail's `dimmerWidget` constructors are genuinely the
first vtable requirement in that TU, or whether some earlier-emitted construct
(a placement-new, a array-ctor helper, or a base-class subobject ctor) held
that role.

---

## 11. The post-`$E` channel — architecture, and the verdict

### 11.1 There IS a second drain, and here is the exact loop

`FUN_1040ABD0`, once queues A/B/C are empty:

```c
if ((DAT_104D7438 && *DAT_104D7438) || (DAT_104D743C && *DAT_104D743C) || DAT_104E9428) {
    FUN_1044B373();                 /* member-init / dynamic-init walk  => $E21/$E20 */
    clear DAT_104D7438;
    if (DAT_104D743C) return FUN_1045B299();   /* same tail: clears 743C, then recurses */
}
DAT_104E9428 = 0;
if (FUN_1040AE53()) { FUN_10423EFC(); FUN_10425299(); }   /* pending-body path   */
else if (!FUN_1040AE89()) return 0;                       /* nothing left: DONE  */
return FUN_1040ABD0();                                     /* RECURSE = 2nd drain */
```

`FUN_1045B299` is literally this same tail duplicated. So the shape is a
**fixpoint loop**: drain → member-init walk → re-scan → recurse → drain again.
The post-`$E` emission slot the coordinator inferred from the object layout is
real and is this recursion.

### 11.2 What routes an item into it

Two feeders, both re-scanned after `FUN_1044B373`:

- **`FUN_1040AE53`** — walks the symbol/segment table (`DAT_104D7410`,
  `DAT_104D7534`) with the iterator `FUN_1041E52C`/`FUN_1041E53D`, selecting on
  `sym->[0x32]` kind ∈ {0, 0xB…0x10}. True ⇒ driver runs `FUN_10423EFC` +
  `FUN_10425299`, then recurses.
- **`FUN_1040AE89`** — the **template-specialization scanner**. Guarded by
  `DAT_104D7610 && DAT_104D7610[2]`. It walks a two-level list `DAT_104D75BC`
  (outer chained `[2]`, inner chained `[2]`) and enqueues each inner item onto
  **queue C** via `FUN_10427303(&DAT_104DA6C0, item[1], 0, 1, 0, &DAT_104DA710)`
  when:

  ```
  (item->[2] & 4) == 0                       /* not already emitted */
  && (  (item->[0x1c] & 3) != 0
     || (item->[0x25] & 2) != 0
     || (owner->[0x28]->[0x3e] & 0x10) != 0
     || DAT_104D8106 != 0 )                  /* DAT_104D8106 == -BforceInst */
  ```

> **The post-`$E` channel is queue C, fed by `FUN_1040AE89`, and the routing key
> is "a pending TEMPLATE SPECIALIZATION not yet emitted whose needed-flag is
> set".** `-BforceInst` makes that predicate unconditional — but `FUN_1040AE89`
> is also scanned on every earlier iteration, so it *front-loads* instantiation
> rather than deferring it. That is exactly measurement 1: the template arm
> lands at s21, before the `$E`s.

### 11.3 Can the member-init synthesis attach to a source position? No

`?id@…$D` member-static initializers are registered into `DAT_104D7438` and are
emitted **only** by `FUN_1044B373`, which by construction runs when the queues
are exhausted. There is no source-position path for that form. (Contrast §9.2's
`n5`: a *file-scope object's* dynamic initializer body **does** follow source
position — a different registration path. The `$D` member form does not.) This
independently confirms measurements 3 and 4: nothing reachable moves the pair.

### 11.4 Does the post-item helper re-enter the instantiation machinery? Yes — but it drags only itself

Probe `r1` (user template specialization first required by a template
static-data-member's initializer) produces the retail-shaped tail:

```
[4] Touch   [5] Purge
[6] _$E6                  0x27   guarded init
[7] _$E4                  0x12   atexit registration
[8] ?id@?$Facet@H@@$E     0x2c   <- POST-$E item, and a substantial body
```

So the post-`$E` slot really can hold a 0x2c body. **But its payload is the
guarded-init/atexit-helper shape** (`558bec33c0a0…83e00185c07519…`), not a
destructor — and `??1User<int>` itself was *not* dragged out with it. The
facet-dtor-thunk chain emits the helper, not the user body.

### 11.5 Verdict: NONE — Audiere joins DIMMER as an honest wall

Retail's `0x88c` body is **REL32-called by Purge** (`+0x65`, `+0x114` — address
truth, not a delinker name claim), so it is a body that *user code calls*. The
architecture offers exactly one post-`$E` route, and it admits only pending
template specializations flagged during the walk, or the member-static's own
`$E` helper. Neither can be a user-called destructor whose only requirement site
is a `delete` in function #1 — that requirement is satisfied in the very first
drain, long before `FUN_1044B373`.

**There is no event sequence in this front end that produces
`[drain][id-init $Es][user dtor COMDAT REL32-called from fn1]`.** Combined with
§11.3 (the `$E`s cannot move early) both directions are closed, and the five
measurement sets bound the source axis. I am calling it: **AudiereEffects is a
wall, on the same footing as DIMMER (§10.4).**

### 11.6 One structural avenue I did NOT close — contribution granularity

Reading our own object rather than the delinked one surfaces something the
ordering analysis cannot explain, and it may be the real story:

```
ours  : [.text sec4 : 12 fns , _$E21 , _$E20][s7 ??1][s8 ??1][s9 ??4][s10 ??1][s11 ?id$E 0x5]
retail: 12 fns , ??1(0x7d0) , ??4(0x7fc) , $E21(0x850) , $E20(0x878) , ??1(0x88c)
```

**Our `_$E21`/`_$E20` are appended to the main `.text` contribution; retail's sit
between COMDATs**, which requires them to be in their own contribution. That is a
*granularity* difference (which section the thunks are emitted into), not a
queue-ordering difference — and no amount of source reshaping inside the TU can
change it, because §11.3 fixes when they are emitted, not where.

Note also all three of our 0x2c bodies share one payload
(`558bec51894dfc8b45fc8338…`). The pinned `LINK.EXE` **does** support COMDAT
folding — `OPT:ICF` (file `0x8860`) and `NOICF` (`0x7bf2`) are both in its
option table — so identical-payload COMDATs can collapse to a single body that
several call sites and slots share. That would dissolve the ordering problem
without any source change, because the object's tail COMDAT *count and order*
would stop mattering; only the post-fold image layout would.

This is **not a new axis for the campaign**: `scripts/homm2/build/link_exe.py`
already links with `/OPT:NOREF` (which suppresses REF and hence ICF), and
`scripts/homm2/build/exact_link/orchestrate.py:505` already has a conditional
`/OPT:ICF` prefix. So the folding path is present and was evidently settled
against. Whether the retail image is a NOREF or an ICF link is a linker-level
question the coordinator already owns — but it is the one reading under which
the §11.5 wall does not need to be climbed at all, so it is worth re-testing
with the payload identity above as the specific thing to look for.
