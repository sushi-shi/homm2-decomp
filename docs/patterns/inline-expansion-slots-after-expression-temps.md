# Inline-expansion slots come after ALL expression temps, never in body order

**Trigger.** Every named local and the `this`/`__fastcall` spill agree, the
instruction stream is identical, and yet one contiguous band of `-0xN(%ebp)`
displacements is a **rotation** of retail's: the same set of slots, the same
number of slots, only a different assignment of variables to them. `objdiff`
scores this in the high nines and `homm2 sema disasm --diff` renders it clean
(it prints `-0xN(%ebp)` as `<addr>`), so it is only visible in a raw
`llvm-objdump -d` comparison with displacements left in.

## The `/Od` frame has five regions, not four

    -0x4 ...   named locals, function scope, by (bucket(name), -decl index)
    ...        named locals, inner blocks, after ALL function-scope ones
    ...        front-end EXPRESSION temps      <- created while parsing exprs
    ...        front-end INLINE-EXPANSION slots <- one set per /Ob1 call site
    ...        `this` / __fastcall register-argument spill
    deepest    back-end temps

The two front-end regions are what this document adds. Both are dword-granular
and both sit *above* the register spill, so they read like one pool - but they
are filled in two separate passes, and **the expression-temp pass completes
before the first inline-expansion slot is handed out**. Inside each pass the
order is body order (shallowest = created first).

The consequence that costs time: an inline expansion at the *top* of a function
still slots **below** an expression temp created near the *bottom*. Body
position alone does not order the band.

## Measured on `advManager::DoEvent` (`SOURCE/EVENTS`, RVA 0x3b640, 24521 bytes)

`DoEvent` has exactly ten front-end slots. Three CP1251 case folds (body
0x2b97, 0x48de, 0x4b34), one `new heroWindow` / `delete` statement pair (body
0x3b98 .. 0x3c90, the function's only `??2@YAPAXI@Z`/`??3@YAXPAX@Z`), and four
expansions of the same `gpGame + 0xb3e` map accessor (body 0x3f93, 0x3fe8,
0x4112, 0x4163).

With the folds written as block-scope `char` locals they landed in the
named-local region and the band was off by a four-slot rotation - 34 bytes,
each a single displacement byte, out of 24521:

| slot | folds as block-scope locals (99.995%) | retail = folds as inline-callee locals |
| :-- | :-- | :-- |
| `-0x314` | `resourceInitial` (named) | new-expression value temp |
| `-0x318` | `artifactInitial3` (named) | `operator new` raw-pointer temp |
| `-0x31c` | `artifactInitial5` (named) | `delete` operand temp |
| `-0x320` | new-expression value temp | fold local, expansion @0x2b97 |
| `-0x324` | `operator new` raw-pointer temp | map-accessor temp @0x3f93 |
| `-0x328` | `delete` operand temp | map-accessor temp @0x3fe8 |
| `-0x32c` | map-accessor temp @0x3f93 | map-accessor temp @0x4112 |
| `-0x330` | map-accessor temp @0x3fe8 | map-accessor temp @0x4163 |
| `-0x334` | map-accessor temp @0x4112 | fold local, expansion @0x48de |
| `-0x338` | map-accessor temp @0x4163 | fold local, expansion @0x4b34 |
| `-0x33c` | `this` | `this` |

Read retail's column top to bottom: the three `new`/`delete` temps come first
even though the *earliest* front-end slot user in the body is the fold at
0x2b97. That is the two-pass rule. Everything after the first fold is plain
body order: fold(0x2b97), accessor(0x3f93), accessor(0x3fe8), accessor(0x4112),
accessor(0x4163), fold(0x48de), fold(0x4b34).

The byte-level residual (one representative site of each class):

```
ours (block-scope char locals)             retail
------------------------------------------ ------------------------------------------
2b97 88 85 ec fc ff ff  movb %al,-0x314    2b97 88 85 e0 fc ff ff  movb %al,-0x320
2bf7 8a 85 ec fc ff ff  movb -0x314,%al    2bf7 8a 85 e0 fc ff ff  movb -0x320,%al
3b98 89 85 dc fc ff ff  movl %eax,-0x324   3b98 89 85 e8 fc ff ff  movl %eax,-0x318
3bda 89 95 e0 fc ff ff  movl %edx,-0x320   3bda 89 95 ec fc ff ff  movl %edx,-0x314
3c90 89 8d d8 fc ff ff  movl %ecx,-0x328   3c90 89 8d e4 fc ff ff  movl %ecx,-0x31c
3f93 89 85 d4 fc ff ff  movl %eax,-0x32c   3f93 89 85 dc fc ff ff  movl %eax,-0x324
48de 88 8d e8 fc ff ff  movb %cl,-0x318    48de 88 8d cc fc ff ff  movb %cl,-0x334
```

Every other byte of the 24521 already agreed, including the whole surrounding
fold chain (`andl $0xff` zero-extends, `cmpl $0xc0/$0xdf/$0xa8`, the `movb
$-0x48` for `ё`) - only the displacement differed.

## What closed it

Move the value out of the named-local region and into the inline-expansion
region: one file-static `inline` per TU, with a **named local** inside it (an
early-`return` body allocates no slot at all - see
`inline-callee-local-holds-branchy-value.md`), called once per site.

```cpp
namespace {
    inline char ToLowerCp1251(u8 letter) {
        char smallLetter;

        if (letter >= 'A' && letter <= 'Z')
            smallLetter = letter + CP1251_CASE_STEP;
        else if (letter >= CP1251_CAPITAL_A && letter <= CP1251_CAPITAL_YA)
            smallLetter = letter + CP1251_CASE_STEP;
        else if (letter == CP1251_CAPITAL_YO)
            smallLetter = CP1251_SMALL_YO;
        else
            smallLetter = letter;
        return smallLetter;
    }
}
...
sphinxAnswer_a[0] = ToLowerCp1251(sphinxAnswer_a[0]);   /* x3 */
```

`advManager::DoEvent` 99.995% -> **EXACT** (0/24521 bytes differing), and
`SOURCE/EVENTS` 43/44 -> 44/44. The parameter costs no slot: the argument is a
simple lvalue, so every comparison re-reads `-0x64(%ebp)` exactly as retail
does, and the emitted instruction stream is unchanged from the open-coded form.

## Diagnosis rule

1. Confirm the residual is slot-only: same instruction count, same opcodes,
   only `-0xN(%ebp)` low bytes differ. Diff `llvm-objdump -d` output directly;
   `homm2 sema disasm --diff` masks displacements and shows this as clean.
2. Read `homm2 sema frames` to find where the named-local region ends and the
   spill sits. Slots strictly between them are front-end slots.
3. Classify each front-end slot by what writes it: expression temps come from
   `new`/`delete`, `?:` joins, call results and switch selectors; expansion
   slots come from inlined accessors' return values and inlined callees' own
   locals.
4. If retail's order is "all expression temps, then all expansion slots in body
   order" while ours interleaves a named local, the named local is really an
   inlined callee's local. Convert the site to an inline function; do **not**
   grind the local's name (`od_slots` cannot reach this region at all).

## Counter-evidence: this is a per-site choice, not a blanket rewrite

`UpdateTradingPost` (`SOURCE/tradpost`) is exact with the same CP1251 fold
**open-coded twice into function-scope locals** `chr1`/`chr2`, and
`SetupRecruitWin` (`SOURCE/RECRUIT`) is exact with one such local. Both forms
are in retail. Let the frame band decide which one a site used.
