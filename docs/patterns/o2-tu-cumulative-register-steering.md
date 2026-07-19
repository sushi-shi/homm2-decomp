# `/O2` TU-cumulative register steering

MSVC 4.2 `/O2` register allocation is sensitive to the source accumulated before a
function and to declarations brought in by shared headers. A source spelling that is
byte-exact in an older lane can therefore regress after that lane is fast-forwarded onto
the combined root. This is not automatically a compiler wall: first retune an equivalent
source shape in the affected function, then try audited exact-preserving variants in its
predecessors.

## Fingerprint: `bitmap::CopyTo`

`BASE/BITMAP` was exact in its original lane. After the IconEntry and FONT/BORDER state
was combined at `ac1b6b2`, a fast-forwarded build made `bitmap::CopyTo` score
**89.59155%**. The transient source had redundant aliases:

```cpp
bitmap *target = destination;
bitmap *source = this;
PollSound();
// ...
memcpy(target->m_pixels + destinationRowOffset + destinationX,
       source->m_pixels + sourceRowOffset + sourceX, width);
```

Retail is 203 bytes (`0xcb`). The alias form is 207 bytes (`0xcf`). The first structural
divergence is in the row-copy loop at function offset `+0x3c`:

```text
offset  retail (and final direct form)        mismatching alias form
+3c     8b 4c 24 10  mov ecx,[esp+10h]       8b 4c 24 10  mov ecx,[esp+10h]
+40     8b 74 24 18  mov esi,[esp+18h]       8b 74 24 24  mov esi,[esp+24h]
+44     8b 7c 24 1c  mov edi,[esp+1Ch]       8b 7c 24 18  mov edi,[esp+18h]
+48     90           nop                     81 c5 80020000 add ebp,280h
+49     8b 76 16     mov esi,[esi+16h]       8b 49 16     mov ecx,[ecx+16h]
+4c     8b 49 16     mov ecx,[ecx+16h]       03 ce        add ecx,esi
+4f     03 cd        add ecx,ebp             8b 77 16     mov esi,[edi+16h]
+51     03 f0        add esi,eax             03 f0        add esi,eax
+53     03 fe        add edi,esi             8b 7c 24 1c mov edi,[esp+1Ch]
+55     81 c5 80020000 add ebp,280h           03 fe        add edi,esi
+5b     8b 74 24 24 mov esi,[esp+24h]        8d b4 29 80fdffff
+5f     03 f1        add esi,ecx                           lea esi,[ecx+ebp-280h]
```

The alias form still has the right two external calls (`@PollSound@0`), but the extra
four bytes shift the second relocation from retail `+0xbd` to `+0xc1`. This is a real
code-generation regression, not a delinker-label artifact.

The exact resolving spelling removes only the redundant aliases and names the real
objects directly:

```cpp
PollSound();
// ...
memcpy(destination->m_pixels + destinationRowOffset + destinationX,
       m_pixels + sourceRowOffset + sourceX, width);
```

That spelling emits all 203 retail bytes exactly, including the `nop` at `+0x48`, the
loop branch, and both relocation offsets/targets. `bitmap::CopyToCareful`, which follows
it and legitimately retains its own `target`/`source` aliases, remained raw-exact. This
is the important distinction: source shape is TU-state dependent, and a fix should be
limited to the function that actually regressed.

## Reproduction and proof

Run the comparison after fast-forwarding the worker onto the same combined commit that
will receive it:

```sh
ninja build/objdiff/base/BASE/BITMAP.obj
homm2 sema disasm 0xd0430 --diff --lite
homm2 sema disasm 0xd0500 --diff --lite
homm2 relocs 0xd0430
llvm-objdump -d -r --x86-asm-syntax=intel \
  --disassemble-symbols='?CopyTo@bitmap@@QAEXPAV1@HHHHHH@Z' \
  build/delink/BASE/BITMAP.c.obj
llvm-objdump -d -r --x86-asm-syntax=intel \
  --disassemble-symbols='?CopyTo@bitmap@@QAEXPAV1@HHHHHH@Z' \
  build/objdiff/base/BASE/BITMAP.obj
```

Do not trust a score produced only in the older worker state. Recompile on the combined
root, compare raw bytes, and audit relocations. In this case the final checks report
`CopyTo` as 73 identical instructions and `CopyToCareful` as 41 identical instructions.

## Steering order

1. Fast-forward or reproduce the combined root/header state before tuning.
2. Rebuild the translation unit and find the first raw-byte divergence.
3. Verify earlier functions in source order remain raw-exact and their relocation
   offsets and targets agree. A fuzzy maximum is not enough for this invariant.
4. Simplify redundant aliases, temporaries, casts, and reassociated expressions in the
   affected function. Retain only value-preserving changes that improve raw agreement.
5. At roughly 96-97% or better, the libclang AST permuter may explore audited operand
   order and independent-statement order. Never use the regex permuter for this work.
6. If the residual is still register coloring, try audited, exact-preserving AST variants
   in preceding functions. Every predecessor must remain raw-exact with identical
   relocations; then rebuild and measure the downstream function.
7. Retest the retained variant on the combined root and run the full build.

Pin the target's already-correct raw spans as well as its exact siblings. A target candidate can
increase fuzzy percentage by fixing one later span while regressing an earlier instruction order.
For example, a post-Setup Textntry constructor pass recovered its prior 98.695656% maximum only by
moving the `m_color` store ahead of two stores that already matched retail. Raw review rejected it
and restored the lower-scoring canonical source, whose only residual remained the intended delayed
`m_iconFrame` store. Therefore a score maximum is a search hint, never an acceptance criterion.

The predecessor technique is especially useful for a downstream listbox function: an
equivalent operand or statement order in an already-exact predecessor can move the
compiler's cumulative register state without changing that predecessor's emitted bytes.
Apply one audited variant at a time, prove the predecessor is still exact, and keep it
only when the listbox residual improves without introducing a new byte or relocation
mismatch elsewhere.

## What an exhausted search actually means

Never record an unqualified "all `/O2` variants exhausted" conclusion. `BASE/BITMAP`
demonstrated that a target which appeared exhausted in one worker state could become raw-exact
after an earlier source shape changed under the combined translation-unit state. Exhaustion is
therefore a statement about one reproducible state tuple, not about the function forever:

- combined-root commit and compiler flags/toolchain;
- target source hash and include order;
- every included shared-header hash that can alter declarations or compiler state;
- source and object hashes of all preceding functions in the same translation unit;
- target candidate/retail sizes, relocation counts and targets, and first unmasked divergence.

Keep one target-specific TSV for each materially new state tuple. Give every attempted variant a
single row containing the exact source-shape axis, fuzzy score, candidate size, relocation counts,
raw/object hashes when retained, and a disposition such as `retained`, `rejected`, or
`rejected_byte_identical`. Use `NA` rather than inventing a hash for a transient artifact that was
not retained. The accompanying source note should summarize only the current checkpoint and link
to the TSV; historical measurements belong in the matrix or no-repeat ledger.

When revisiting a target, compare the tuple before compiling old ideas. If it agrees, do not replay
the matrix. If it differs, start again at the first raw divergence and retest only axes plausibly
affected by that change. A shared-header or predecessor change is a valid trigger; a stale fuzzy
score or a desire to try more predicate synonyms is not.

For a target with no function predecessor, the same method is limited to real exact-preserving TU
inputs: existing include order, declaration order justified by owned retail data, and shared-header
shape. Do not invent dummy declarations, fake globals, or labels to steer the compiler. For a
target with predecessors, mutate one predecessor axis at a time and retain it only if its complete
code bytes and relocation identities remain exact. In either case, rebuild through the target on
the combined root before measuring it.

This bookkeeping is what makes a negative result reusable: it prevents identical searches at an
unchanged checkpoint while leaving an explicit, evidence-based trigger for a later combined-TU
retest. It must not be converted into `@early-stop` unless the residual independently satisfies the
project's byte-proven artifact rules.

## Rejection criteria

Reject a candidate when any of the following is true:

- it changes values, overflow behavior, evaluation side effects, or object lifetimes;
- an exact predecessor changes even one raw byte or relocation;
- it improves fuzzy percentage while worsening CFG, frame, calls, or relocation targets;
- it improves fuzzy percentage while regressing any earlier raw-exact span in the target;
- it matches only in the stale worker state and regresses on the combined root;
- it depends on a regex mutation or an unaudited AST mutation;
- it perturbs an already-exact sibling such as `CopyToCareful` to rescue the target.

Do not declare an `/O2` register-allocation wall until the combined-root retest,
source-order predecessor audit, local source-shape simplification, and exact-preserving
predecessor steering have all been attempted and documented.

Related: [tu-cumulative-eval-order.md](tu-cumulative-eval-order.md),
[inline-accessors.md](inline-accessors.md).

Target-specific negative search history belongs in the reviewed
[matching matrices](../matching-matrices/README.md). Treat those measurements as historical
evidence and reproduce the relevant state before drawing a current conclusion.
