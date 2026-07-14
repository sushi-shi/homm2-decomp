# Matcher experiment matrices

These TSV files are durable no-repeat sets for large exact-preserving searches. The original
matrices contain:

```text
whole-source-file SHA-256<TAB>focused function match percentage
```

Newer matrices add a third `outcome` column. Always follow the file's header; the outcome records
whether all pins held or names the raw pin/review that rejected the state.

Before replaying a source state, compare its hash here and confirm the canonical source and pinned
sibling hashes have not changed. A listed hash should not be rebuilt under the same compiler/header
state.

## BASE/Textntry AST searches

All three searches used only `scripts/permute_ast.py`, never the regex permuter, and required all 11
siblings to remain pinned:

- `textntry-ctor-ast-e4e96c0.tsv`: initial constructor pass, 562 unique hashes; SHA-256
  `150e0e735e04b738cb4cf3fe17cf2308e3a5f2ff878f898a65f4b60f65449e75`;
- `textntry-read-ast-ctor8aaf3.tsv`: `Read` pass from the improved constructor state, 356 unique
  hashes; SHA-256 `de0d4815b5c32b62e679891af36c71bb7865589814ea63dd112527786a705b0c`;
- `textntry-ctor-ast-8aaf3-deep.tsv`: constructor continuation, 819 unique hashes; SHA-256
  `0e5a6584c1c4786abf35867a9c1072070ced7586d13f64bd69ac67d0354c9f8a`.

The initial constructor pass found the retained 98.695656% source state. The `Read` pass and deep
constructor continuation found no further improvement.

## BASE/Textntry include-surface audit

`textntry-include-surface-8aaf3.tsv` (SHA-256
`d58c40bf716815c90a362b6f0f5fbd6a733105499dd25ea089ee57ad11e13f0a`) records the canonical state
plus eight previously untested declaration/include surfaces. The matrix includes every objdiff
function row and retail size; the two deleting-destructor aliases remain unscored because the
canonical standalone-destructor tradeoff emits one 0x1f base wrapper for the two retail 0x36
aliases. All scored siblings and sizes remained pinned. Canonical and the redundant-`va.h` removal
also produced the same aggregate emitted `.text` dump SHA-256,
`d68541c49e4bf533e079cc8410cc8d24d82b6ad72d3d3010be1fd16c74238d4f`.

The audit established that `BASE/Misc.h` is still required as the owner of `BaseAlloc`/`BaseFree`,
`SOURCE/KB.h` is still required as the owner of the manager globals, `glTimers`, and
`gbTextEntryEscaped`, and `SOURCE/X_GLOBAL.h` and `SOURCE/kbwin.h` own the remaining globals and
functions. Only the direct `va.h` include is redundant, and removing it is byte-neutral, so the
canonical include block is retained rather than creating a no-gain source hash.

## BASE/Textntry fresh SetupDisplayString structure pass

`textntry-setup-fresh-2e351c8.tsv` records the independent retail-structure pass from checkpoint
`2e351c8`. The retained second-loop guard and explicit zero-offset clear raised
`SetupDisplayString` from 95.000000% to 97.128380%, matched the retail `0x1be` size, and kept the
`0x130` frame and 8/8 relocations. The remaining code difference is the whole-function EBX/EBP
allocation swap between `this` and the cursor/second-loop flag lifetime.

The final whole-file source SHA-256 is
`fd22c7692d4e9293c47a13eb83b422b678371fe4ee19755d78b515c9b94fd5cb`; the unchanged
`include/BASE/textEntryWidget.h` SHA-256 is
`f5d08f5a98e96310182cf720733ccb4670b41bfcb24c44ec8ce36fcce86072a3`.

Every row pins the same sibling baseline: default constructor 100%, long constructor 98.695656%,
destructor 100%, `Read` 98.675500%, `Main` 99.852940%, and `Draw` 99.992424%; deleting-destructor
aliases remain unscored. The cursor snapshot, `register` snapshot, and loop-flag declaration-order
states are byte-identical at the new source structure and must not be repeated while the final
source/header hashes agree. No permutation tool was used.

## BASE/Textntry SetupDisplayString AST pass

`textntry-setup-ast-341e775.tsv` records a focused `scripts/permute_ast.py` pass from integrated
checkpoint `341e775` and source SHA-256
`fd22c7692d4e9293c47a13eb83b422b678371fe4ee19755d78b515c9b94fd5cb`. The pass exposed 18 legal
first-order variants and ran a deterministic 300-attempt walk with seed `341775`. Existing matrix
hashes were skipped before compilation, and unsafe inequality +/-1 rewrites were disabled.

The pass compiled 286 new unique whole-file hashes. Every candidate remained at 97.128380% and
kept all 11 objdiff symbols pinned, so the integrated source was restored unchanged. The matrix
SHA-256 is `ec3473c9c38668f263892a0a5868cd8280ec34c01d28c9359b95064be7ec8344`. Do not replay these
hashes while the source and header state agree. The regex permuter was never used.

## BASE/iconf2by manual reconstruction

`iconf2by-manual.tsv` contains 131 full-SHA manual source states spanning the 84.29% through
93.72654% reconstruction. Each row records the source hash, source-shape label, match, candidate
size/frame, relocation counts, and disposition. Its SHA-256 is
`d546bd7d633fa3dde7c268ae163961575b38251bd0f870079183fd1af4be0e92`.

The current source is restored to
`f33ab84fe5e3b9205e40ae09de314bced82063fe587bddee30646c73d44bfea0`. The first normalized
register divergence is in the prologue: candidate `+0x5c` loads `shear` into EBP, while retail
loads it into ESI and then loads `clipW` into EBP at `+0x60`. The later missing `gFYClipR` reload is
now recovered by the retained first-use scratch declaration order with `gFYClipR` last. That order
raises the match to 93.72654%; the branch-local fill snapshot keeps the fill `gFYRun` load after
the vertical checks, matching retail's source lifetime. CodeView has no symbol
subsection for this TU, so neither stack-local nor optimized-register names exist; the matrix—not
guessed local names—is the no-repeat oracle. No permutation tool was used.

The integrated-root `4d381e1` continuation added 18 combined-TU states. Direct, commuted,
`register`, and top-level-`const` width/shear/fill spellings were byte-neutral; retail-order edge
publication remained 92.544235% and did not free EBP; restoring the owner/Misc declaration surface
fell to 84.28954% with 141/144 relocations; and an unsigned global run fell to 89.83914% with no
frame and 141/144 relocations. Keep the current narrow headers and declaration order pinned, and do
not recombine these axes before a newly retained predecessor/shared-header state exists.

## BASE/Textntry fresh lifetime and semantic-name pass

`textntry-fresh-lifetimes-97c1152.tsv` records the independent pass from checkpoint `97c1152`.
It did not use either permutation tool. The decisive retail-evidenced lifetime was that the same
32-bit `shifted` flag controls both display-adjustment loops. Reusing that flag makes VC4.2
coalesce its non-overlapping lifetime with the unsigned-short cursor in EBX, assigns `this` to EBP,
and produces a raw-exact `SetupDisplayString`: size `0x1be`, frame `0x130`, and 8/8 matching
relocation targets.

The matrix also records why reusing the cursor itself is structurally wrong: it reaches 99.73% but
emits `xor/test bx` where retail proves a separate 32-bit flag with `xor/test ebx`. Narrowing the
separate flag's lexical scope and adding dead initializers both reproduce the old 97.128380% state.
One constructor-local snapshot perturbed the later exact Setup function to 99.93%, directly proving
that exact-preserving predecessor/TU state must be audited after every earlier source change.

The retained source/header SHA-256 values are `ac8dd08884961ba6d73e8a312391cdeecac71ba132f5badef5f79201a63b34d8`
and `8207f088751e59f5a467c7a7e582c4870d82c3fb4e060262d02d5e3a07a17517`.

## BASE/Textntry post-exact-Setup constructor and Read AST passes

The exact Setup lifetime changed the combined TU state, authorizing one fresh pass over each of the
two remaining Textntry residuals from checkpoint `2aeff38`. Both used only
`scripts/permute_ast.py`; the regex permuter was never used, unsafe inequality rewrites were
disabled, and all previously listed Textntry hashes were skipped before compilation.

`textntry-ctor-ast-ac8dd.tsv` contains 286 new unique whole-file hashes from seed `28482744` and 20
first-order variants. It has SHA-256
`aae422221e02fcdce50fbf103aa918624b0e01b5c97f62287e6061772a9bf873`. Thirty-eight hashes reached
98.695656%, but the final score-max representative `4363f3b...` failed the required raw target
review: it moved `m_color` (`+0x28`) ahead of the other constant stores. That made the score rise
while regressing an already-correct span, so it is explicitly marked `REJECT_RAW_STORE_ORDER` and
the canonical `ac8dd088...` source was restored.

`textntry-read-ast-ac8dd.tsv` contains 303 new unique hashes from seed `2848920` and 23 first-order
variants. It has SHA-256
`6399eacb8b875aefe21feb91aee2033692dca88a71f02f78ca76047b69613bdc`. Of these, 240 kept every
pin and 63 are marked `PIN_BYTES_MOVED` for the `Main` disassembly fingerprint/placement. No
pin-clean state exceeded 98.675500%.

The byte pins covered default constructor/destructor, `Main`, `Draw`, and the now-exact Setup; the
`Read` walk also pinned the long constructor. The accepted canonical source/header hashes remain
`ac8dd08884961ba6d73e8a312391cdeecac71ba132f5badef5f79201a63b34d8` and
`8207f088751e59f5a467c7a7e582c4870d82c3fb4e060262d02d5e3a07a17517`.

## BASE/Textntry manual TU-state and lifetime pass

`textntry-manual-tu-a4fa3a0.tsv` records 35 new unique manual states from integrated checkpoint
`a4fa3a0`. Its SHA-256 is
`eba42858cfad92679dfe1736d6ab31656064d1029f5f6bae88a177f6984cf2ec`. Neither permutation tool was
used. Each row records the full source SHA, focused scores, raw target outcome, sibling outcome, and
disposition; every state was checked against all older Textntry matrices before compilation.

The pass covers exact-neutral default-constructor, long-constructor, and destructor spellings;
constructor parameter/member lifetime aliases; width and storage-class variants of `Read`'s
`enabled`; three progressively factored duplicated-arm CFGs; and two combined exact-predecessor
bundles. No state was retained.

The important new boundary is hash `9704ae26...`: `short enabled` emits retail's desired
`cmp edi,3` before the `m_height` load, but changes the following constant from retail
`mov eax,1` to candidate `mov ax,1`. Hash `9fe21062...` proves that `enabled | 0` is optimized back
to the same wrong-width instruction. Short-to-int snapshots add `movsx`; using the short in the
final predicate adds an extra sign-extend/subtract/compare sequence. The duplicated-arm family
leaves real extra branches and must not be repeated.

The canonical source/header hashes remain `ac8dd08884961ba6d73e8a312391cdeecac71ba132f5badef5f79201a63b34d8`
and `8207f088751e59f5a467c7a7e582c4870d82c3fb4e060262d02d5e3a07a17517`.
