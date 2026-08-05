# od-slot-scope-groups

Companion to [od-slot-solve](od-slot-solve.md). That entry solves the slot order
*within one scope* by renaming. This one is the case a rename cannot reach: the
whole frame is right except that a run of locals sits **below** everything else,
or a local sits above a lower-bucket one. The lever is the declaration's
**scope**, not its name.

**Symptom.** Displacement-only divergence where the required stack order is not
bucket-monotone — `homm2 od-frames --slots --unit <TU> <symbol>` prints a
`scopes :` line with more than one group, or a `blocked :` pair that no
declaration order can satisfy.

## Rule (VC6 SP5, measured on this branch)

/Od lays the frame out in three stages:

1. **Scope groups, outermost first (shallowest first).** Each lexical block is
   its own group, and a nested block's group is allocated *below* the whole
   enclosing group. A `for`-init scope does **not** make a group — it merges
   into its enclosing block. So `for (i32 i = ...)` at function level is a
   function-level local, but the same declaration in a loop nested inside
   another loop body is one group deeper.
2. **Within a group**, `sort(key=(bucket(name), -declaration_index))` — the rule
   in od-slot-solve.
3. **`__fastcall`/`this` register arguments spill below every local**, in
   argument order, and are *not* hashed. They are the tail of the frame that the
   bucket order does not explain.

Probe (`homm2 audit od-oracle` reads it straight out of /Z7):

```c
int Probe(int n) {
    int zebra = n;                              /* bucket 3  */
    { int alpha = zebra; zebra += alpha; }      /* bucket 1, inner block */
    for (int loopy = 0; loopy < 4; loopy++)     /* bucket 10, for-init   */
        zebra += loopy;
    int omega = zebra;                          /* bucket 13 */
    return omega;
}
```

```
  -4  zebra   bucket 3      function scope, bucket sorted 3 < 10 < 13
  -8  loopy   bucket 10     for-init merged into function scope
 -12  omega   bucket 13
 -16  alpha   bucket 1      inner block: below the whole outer group
 -20  n       bucket 14     register argument: below every local
```

`alpha` has the *lowest* bucket of the four and still lands deepest; `n` has the
highest and is not sorted with them at all. Neither fact is expressible in the
single-scope model.

## Worked close — `swapManager::SwapArtifacts`

Source before (loop counter declared in the inner `for`-init, so it is one scope
deeper than `message` and `side`):

```c
    if (selectedArtifact == ARTIFACT_SPADE_NECROMANCY || ...) {
        tag_message message;
        message.type = MESSAGE_WIDGET;
        for (SwapManagerSide side = SWAP_SIDE_LEFT; side < SWAP_SIDE_COUNT; ++side) {
            for (i32 ssIndex = 0; ssIndex < SECONDARY_SKILL_WIDGET_COUNT; ++ssIndex) {
```

```
ours                                          retail
1362: c7 45 d4 00 02 00 00  mov [ebp-0x2c],0x200   12ae: c7 45 d0 ...  mov [ebp-0x30],0x200   ; message.type
1369: c7 45 f0 00 00 00 00  mov [ebp-0x10],0x0     12b5: c7 45 ec ...  mov [ebp-0x14],0x0     ; side  = SWAP_SIDE_LEFT
1385: c7 45 d0 00 00 00 00  mov [ebp-0x30],0x0     12d1: c7 45 f0 ...  mov [ebp-0x10],0x0     ; ssIndex = 0
```

Ours: `side -0x10`, `message -0x2c`, `ssIndex -0x30`.
Retail: `ssIndex -0x10`, `side -0x14`, `message -0x30`.

Buckets are `ssIndex 6`, `side 8`, `message 9`. Retail's order is exactly those
three bucket-sorted in **one** group, so retail declared the counter in the
`if` block, not in the inner `for`. No rename can produce it: our `ssIndex` was
in a deeper group, and a deeper group is always below the outer one whatever it
is called.

Fix — hoist the declaration one scope out, leaving the loop a plain `for`:

```c
        tag_message message;
        i32 ssIndex;
        message.type = MESSAGE_WIDGET;
        for (SwapManagerSide side = SWAP_SIDE_LEFT; side < SWAP_SIDE_COUNT; ++side) {
            for (ssIndex = 0; ssIndex < SECONDARY_SKILL_WIDGET_COUNT; ++ssIndex) {
```

Exact. The same edit closed `swapManager::Open` (its `i` had to move up to
function scope, declared after `message` so the bucket-9 tie breaks the right
way).

## Why this matters more than renaming

The layout algorithm is identical in MSVC 4.2 and VC6 - see
[od-frame-compiler-invariance](od-frame-compiler-invariance.md) for the two-compiler
measurement. The consequence for this entry: a frame difference is always a SOURCE
difference, and of the source facts that can cause one, scope and declaration order
carry no naming claim while a rename does. Reach for the block first.

`recruitUnit`'s two constructors are the clean demonstration. Both retail frames
are 0x24 with `this` at -0x24 and the same items — one `int[7]`, one `int` — and
the 2.0 source is character-identical to ours:

```
2.0 retail    resource @-0x04   costs @-0x20
Buka retail   costs    @-0x1c   resource @-0x20
```

`bucket(resource)=3 < bucket(costs)=12`, so no declaration order can put the
array first, and an earlier session "fixed" it by renaming `resource`->`n`
(bucket 14). Byte-correct, but an invention. Scoping the cost search instead
reproduces Buka's frame with the 2.0 identifier intact:

```c
    GetMonsterCost(m_creatureType, costs);
    m_goldCost = costs[GOLD_RESOURCE];
    {
        i32 resource;
        for (resource = 0; resource < RESOURCE_COUNT; ++resource) { ... }
        ...
    }
```

Both constructors exact, and the rename is gone. `advManager::SystemOptions` is
the same story at function scale: retail wraps everything after the two saved
config reads in a block holding `heroContextLocked` and `sampleIndex`. When a
frame will not come out and the only escape is a rename, try the block first.

## Reading it off the compiler instead of guessing

`homm2 audit od-oracle --unit <TU> [symbol]` names every slot in a real frame by
compiling the configured TU with `/Z7` and reading its CodeView records. `/Z7`
adds records without changing /Od codegen, so the offsets are the shipped ones.

VC6 emits **CV5** records with 16-bit type indices. These numbers are the whole
reason the oracle previously returned an empty table without failing — it was
reading the MSVC 4.2 (CV4) layout:

| record | VC6 | layout |
| :-- | :-- | :-- |
| `S_BPREL32` | `0x1006` | `off(i32) typind(u32) namelen(u8) name` |
| `S_LPROC32` | `0x100a` | `off(u32)` at `body[28]`, `namelen(u8)` at `body[35]`, name at `body[36]` |
| `S_GPROC32` | `0x100b` | same shape |
| `S_END` | `0x0006` | empty |

Three further traps. An object carries **one `.debug$S` section per COMDAT**, so
all of them must be read. The procedure record names the function **unmangled**
(`combatManager::ViewBallista`) — the join back to the mangled COFF symbol has to
go through the relocation that patches the record's own `off` field. And a
lexical block emits `S_BLOCK32 = 0x0207` — note the CV4-style number, in the same
stream as the CV5 `0x100b` procedures — followed by **its own `S_END`**: reading
that as the end of the procedure silently drops every local after the first
block, which is exactly the inner-scope shape this entry is about. Track the
nesting depth.

## Measured scope facts

```
                                          slots, shallowest first
for (int loopy ..) ...; for (int alpha ..) ...   alpha(1) total(7) loopy(10)
int loopy; int alpha; for (loopy ..) ...         alpha(1) total(7) loopy(10)
for (..) { int loopy; } for (..) { int alpha; }  total(7) i(9) j(10) | loopy | alpha
```

A `for`-init declaration **merges into the enclosing block** — it is
indistinguishable from hoisting the same declaration to the top of that block, so
`for (i32 i = ...)` is not a scope lever. A loop **body** `{ }` is, and two
sibling bodies get **two** groups laid out one after the other (`loopy` at -16,
`alpha` at -20 — sibling scopes do not share storage). So a retail order that is
bucket-monotone within runs, but restarts between them, is counting real braces.
