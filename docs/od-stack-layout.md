# MSVC 4.2 `/Od` local stack-slot allocation — complete model

How the retail compiler (MSVC 4.2, `cl 10.20`, the build HEROES2W.EXE was made
with) decides the `ebp`-relative offset of every local in an unoptimized (`/Od`)
function. Reverse-engineered from the toolchain itself — the C++ front end
**C1XX.EXE** and back end **C2.EXE** — and verified byte-exact against the real
compiler (see §8). This is the single dominant cost in `/Od` matching: instruction
selection is otherwise very literal, but every local references a frame offset, and
those offsets are a function of the **identifier spelling**, not declaration order
or type.

Tools: **`scripts/od_slots.py`** (predict a layout / solve names for a target layout,
no compiler in the loop) and **`scripts/od_oracle.py`** (ground-truth: compile a probe
and read the real offsets).

---

## TL;DR

* A local's slot is chosen by a **hash of its name**. Renaming a local moves its slot;
  re-ordering the declarations does **not** (except to break within-bucket ties).
* Per **lexical scope**, locals are placed in a fresh **16-bucket** hash table. The
  frame layout walks bucket `0..15`; each bucket's chain is newest-first (LIFO).
* **Within one scope:** `slot order = sort locals by (bucket ascending, declaration
  index DESCENDING)`. Slot `-0x4` is the hash-earliest.
* `bucket = key & 0xF`, where `key = (H ^ (H>>16)) & 0xFFFF` and `H` is the rolling
  identifier hash in §1.
* **Across scopes:** offsets are continuous, laid out shallow→deep — a scope's *own*
  locals first, then nested child blocks in source order. **No slot reuse** at `/Od`.

---

## 1. The identifier hash — C1XX.EXE `FUN_0041d1a2`

The C++ lexer hashes each identifier *as it scans the characters*, gated by the
character-class table at `DAT_004b41c0` (bit `0x10` = identifier character):

```c
unsigned h = 0;
for (char *p = ident; (idclass[(unsigned char)*p] & 0x10); p++)
    h = (h >> 4) + h*4 + (unsigned char)*p;     // 32-bit
unsigned key = (h ^ (h >> 16)) & 0xffff;        // 16-bit, stored in the name record (+0x8)
```

Raw assembly of the per-character step (`0x0041d312`–`0x0041d31a`):

```
MOV  EDX, EBP            ; EDX = h
SHR  EDX, 0x4            ; EDX = h >> 4
LEA  EDX, [EDX + EBP*4]  ; EDX = (h>>4) + h*4
LEA  EBP, [EDX + EAX]    ; h   = (h>>4) + h*4 + char     (EAX = current char)
```

and the finalization fold (`0x0041d1f3`–`0x0041d206`):

```
MOV  EAX, EBP           ; EAX = h
SHR  EAX, 0x10          ; EAX = h >> 16
XOR  EAX, EBP           ; EAX = h ^ (h>>16)
AND  EAX, 0xffff        ; key = (h ^ (h>>16)) & 0xffff   -> name record +0x8
```

Notes:
* The `>>4` term recirculates high bits into the low nibble — this is why simple
  rolling-hash guesses fit *short* names but fail on long ones.
* The `^(h>>16)` fold only changes the low nibble once `h >= 2^16` (~5+ characters),
  so for ≤4-char names `bucket == (raw rolling hash) & 0xF`.
* For a **single character** `c`: `h = c`, fold is a no-op, so `bucket = c & 0xF`.
  That makes the inverse closed-form (see §7).

## 2. The symbol table — C2.EXE

The back end keeps one such table **per lexical scope**:

* `FUN_0044bb5e` — allocates the table for a scope: `alloc(0x40)` = 16 pointers,
  zeroes 16 entries.
* `FUN_0044c4df` — **insert**: `bucket = (sym->key @ +0x24) & 0xF`; the new node is
  pushed at the **head** of the bucket chain (LIFO / reverse-insertion).
* `FUN_0044c4bc` — lookup by the same key.

The front-end `key` (§1) rides through the IL into the symbol record; C2 buckets on
`key & 0xF`. The frame layout walks bucket `0..15`, each chain newest-first, handing
out `-0x4, -0x8, -0xc, …`.

## 3. Single-scope ordering

For locals all in one scope:

```
slot order (slot -0x4 first) = sort names by (bucket ascending, declaration_index DESCENDING)
```

`this` and compiler temporaries are inserted last → they take the deepest slots of
their scope (consistent with LIFO).

Example — `int i; int buf; int j;` (declaration order):

| name | bucket | slot |
|------|-------:|-----:|
| `buf`| 2 | -0x4 |
| `i`  | 9 | -0x8 |
| `j`  | 10| -0xc |

`buf` (bucket 2) wins `-0x4` even though `i` was declared first. Reordering the
declarations changes nothing; only renaming does.

## 4. Multi-scope layout

### 4.1 VC4.2 uses *old* for-scope
`for (int k …)` declares `k` in the **enclosing block**, not the loop body. So two
`for(int i…)` in the same block is a redefinition error (`C2374`). Retail VC4.2 code
therefore reuses one counter or wraps each loop in its own `{}`.

### 4.2 No slot reuse
`/Od` never coalesces stack slots. Sibling scopes whose lifetimes don't overlap still
get **distinct** slots. The same name in two scopes = two slots.

### 4.3 Each scope is an independent 16-bucket table
Bucket numbering **resets per scope** — it does not carry across. (Measured: a
bucket-0 local in a *nested* block lands deep in the frame, not at `-0x4`, because its
low bucket only ranks it within its own scope.)

### 4.4 A scope pools *all* its own locals
A scope collects **every** local declared directly in it — whether before *or after*
an intervening nested block — into one bucket table and sorts them together. Textual
position relative to a nested block does not split them; only scope membership does.
Nested child blocks are laid out **below** the parent's own locals.

### 4.5 The layout algorithm

```
offset = 0
def layout(scope):
    # 1. this scope's OWN locals (declared directly in it, in declaration order),
    #    placed by the single-scope rule (§3)
    for name in slot_order(scope.own_locals):
        offset -= sizeof(name)
        assign name -> offset
    # 2. then nested child scopes, in source order (depth-first)
    for child in scope.children_source_order:
        layout(child)

layout(function_top_scope)
```

So `bucket` only ever orders locals **within a single scope**; everything else is
fixed by scope structure, which you control directly with `{}` placement
(name-independent).

## 5. Worked examples (all measured with `scripts/od_oracle.py`)

```c
// reused counter (one local, one slot)
int probe(){ int i,x=1,y=2; for(i..){} for(i..){} ... }
// -> x@-0x4 (b8), y@-0x8 (b9), i@-0xc (b9)   [y,i share b9; reverse-decl -> y above i]

// two loops, each its own block  ->  two distinct i slots (no reuse)
int probe(){ int x=1; { for(int i..) } { for(int i..) } ... }
// -> x@-0x4, {i@-0x8}, {i@-0xc}

// independent per-scope tables (bucket resets)
int probe(){ int s=1; { int i,x; } { int j,p; } ... }
// -> s@-0x4, { x@-0x8(b8), i@-0xc(b9) }, { p@-0x10(b0!), j@-0x14(b10) }
//    p is bucket 0 yet sits at -0x10, deeper than block1 -> per-scope numbering

// scope pools locals declared before AND after a nested block
int probe(){ int s=1; { int n; {int x;} int p; } ... }   // n before block, p after
// -> s@-0x4, { p@-0x8(b0), n@-0xc(b14), { x@-0x10 } }
//    p (declared last, after the block) is shallowest in its scope by bucket
```

## 6. Edge cases / gotchas

* **Mixed local sizes.** The *order* of slots is type-independent (bucket only depends
  on the name). The *byte offsets* depend on each local's size (`double`, structs,
  arrays). `od_slots.slot_order()` gives the exact order; `predict_offsets(sizes=…)`
  needs the real sizes for exact offsets.
* **Methods (`thiscall`).** `this` and temporaries are inserted into the function's top
  scope last → deepest there. Predict the named locals' order with the tool; verify
  `this`/temp placement against the disassembly per function.
* **Reserved/unused slots.** A declared-but-unreferenced local still gets a slot at
  `/Od`. If retail reserves a slot you can't explain, the original had an unused local —
  add a throwaway to reproduce it.
* **Keyword / alternative-token names.** Names like `or`, `and`, `xor`, `new`, `int`
  can't be used as identifiers — avoid them in name searches.

## 7. Tooling

`scripts/od_slots.py` (pure functions, no compiler):
* `bucket(name)`, `key16(name)`, `ident_hash(name)` — the hash.
* `slot_order(decl_names)` / `predict_offsets(decl_names, sizes=None)` — predict a
  single scope's layout.
* `BUCKETS` / `by_bucket(words)` — the name catalog binned by bucket.
* `solve_layout(slots)` — given per-slot candidate names (roles / lists / pins) in
  stack order, returns the cheapest **distinct** assignment that reproduces the order,
  plus the declaration order to use and any fallbacks.
* `solve(n)` — canonical guaranteed solution: single chars (`chr(0x70|k)` hits bucket
  `k` — the closed-form inverse, since `bucket(char) == char & 0xF`).

**Multi-scope** is intentionally *not* a single generic call — the scope tree is
awkward to express and easy to get wrong. Instead, apply `slot_order` per scope and
concatenate shallow→deep per §4.5; cross-scope order you control with braces.

`scripts/od_oracle.py` — ground truth. Compiles a one-function probe with
`/Od /MT /Gr /Z7` and reads each local's real offset from the `S_BPREL32` CodeView
records in the `.obj` (`/Z7` adds debug records but does **not** change `/Od`
codegen, so the offsets are identical to the real build). Use it to verify any
prediction.

## 8. How this was reverse-engineered (methodology)

1. **Read C2's local-table code** → confirmed `bucket = (sym->key) & 0xF`, 16 buckets,
   LIFO chains, per-scope tables.
2. The position-dependent hash was **not** a standalone routine in C2 → it's front-end,
   passed in the IL.
3. Followed C1XX's lexer **input cursor** `DAT_004c0064` to the identifier scanner
   `FUN_0041d1a2`, then read the raw assembly for the exact arithmetic + the
   `^(h>>16)` fold.
4. **Verified** against the real compiler via the oracle: 146/146 single-name buckets,
   40/40 random multi-name frame layouts, and the scope cases in §5.

Ghidra (headless) project of `C2.EXE` / `C1XX.EXE` and the RE scripts live under
`build/re/` (gitignored scratch).

## 9. Reference addresses

| what | binary | address |
|------|--------|---------|
| identifier lexer + hash | C1XX.EXE | `FUN_0041d1a2` |
| char-class table | C1XX.EXE | `DAT_004b41c0` (bit `0x10` = id char) |
| lexer input cursor | C1XX.EXE | `DAT_004c0064` |
| per-scope table alloc | C2.EXE | `FUN_0044bb5e` (`alloc(0x40)`, 16 ptrs) |
| symbol insert (LIFO) | C2.EXE | `FUN_0044c4df` (`bucket = key&0xF`) |
| symbol lookup | C2.EXE | `FUN_0044c4bc` |
| symbol hash field | C2.EXE | record `+0x24` |
