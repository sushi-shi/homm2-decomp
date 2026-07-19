# MSVC 4.2 translation-unit declaration state

MSVC 4.2 code generation is sensitive to more than the tokens in the function being
compiled. Header declarations, in-class bodies, string-pool entries, and the source shape of an
earlier function can change register/evaluation choices in a later function while the later
function's source hash, semantics, size, CFG, and relocation count remain unchanged.

This is why the structural phase must not grind every fuzzy residual. A declaration can be
ABI-neutral and still be **matching-active**. Conversely, one measured neutral spelling is not a
license to call that category neutral everywhere.

## Controlled experiment

Measurements were made at integrated BASE head `57b49b633587b3e78322434a69df87a87898d24c`.
Every variant changed one source surface, compiled with MSVC 4.2, recorded the normalized function
source hash, raw function bytes, size, relocation count and ordered-target digest, and then restored
the source. The raw evidence is in
[msvc42-tu-declaration-state.tsv](msvc42-tu-declaration-state.tsv). Reproduce the metrics with:

```sh
PYTHONPATH=scripts/archive python3 scripts/archive/tu_state_metrics.py \
  build/objdiff/base/BASE/FONT.obj --unit BASE/FONT
```

The profiles were:

- `/O2`: `/nologo /c /O2 /MT /Gr /G5 /QIfdiv` for `icon2bc`, `WIDGET`, and `BITMAP`.
- `/Od /Oi`: `/nologo /c /Od /MT /Gr /G5 /Ob1 /QIfdiv /Oi` for `FONT`.

`WIDGET` is `/O2`; it is not an `/Od` control. `FONT` supplies the true debug-build control.
Whole-object hashes are recorded only as diagnostics: paths, debug records, and local-label names
can change them when code bytes do not. Function `.text` hashes and explicit relocation ranges are
the proof surface.

## A real byte-level state flip

Adding an **unused** header-level `typedef enum` before `/Od /Oi` `FONT` left the source hash,
175-byte size, frame, CFG, and zero relocations of `font::GetCharacterWidth` unchanged, but changed
22 raw bytes. Removing only that declaration restored the baseline. The differing suffix is:

```text
empty guarded header                         same header + unused typedef enum
33 c0       xor eax,eax                      8b 45 fc    mov eax,[ebp-04]
8a 45 08    mov al,[ebp+08]                  8b 40 1c    mov eax,[eax+1c]
8b c8       mov ecx,eax                      8b 40 12    mov eax,[eax+12]
8d 04 40    lea eax,[eax+eax*2]              33 c9       xor ecx,ecx
8d 04 81    lea eax,[ecx+eax*4]              8a 4d 08    mov cl,[ebp+08]
8b 4d fc    mov ecx,[ebp-04]                 8b d1       mov edx,ecx
8b 49 1c    mov ecx,[ecx+1c]                 8d 0c 49    lea ecx,[ecx+ecx*2]
8b 49 12    mov ecx,[ecx+12]                 8d 0c 8a    lea ecx,[edx+ecx*4]
0f bf 44 08 04  movsx eax,[eax+ecx+04]       0f bf 44 08 04  movsx eax,[eax+ecx+04]
```

The two sides compute the same address. Only allocation/evaluation order changed. The same unused
enum changed 11, 3, and 2 raw bytes in three later FONT functions while four earlier functions
remained raw-identical. The effect is therefore neither local to a declaration nor uniformly
whole-TU: it is a cumulative state consumed at particular later lowering decisions.

## What was and was not neutral

- An empty guarded include changed `icon2bc`'s raw `/O2` text, but was raw-neutral in WIDGET and
  FONT. Removing broad `Misc.h` in favor of direct `IconEntry.h` produced the same `icon2bc` text
  state as the empty probe. Swapping adjacent `Misc.h`/`dimPalette.h` includes was neutral. Include
  **presence and transitive surface** matter; order matters only when it changes the parsed surface
  or state in that TU.
- An unused plain typedef alias was neutral relative to the same empty-header state in `icon2bc`.
  An unused `typedef enum` was not: `icon2bc` grew from 1368/89 to 1390/92 bytes/relocations;
  WIDGET `Main` grew from 752 to 756 bytes; FONT retained sizes/counts but changed code in four
  functions. Enum order/value changes were neutral in one decoder state, while adding one unused
  enumerator changed WIDGET `Main` again. Thus `typedef enum` is ABI-appropriate for named domains
  but not reliably byte-neutral.
- An unused one-member struct changed `icon2bc` from 1368 to 1372 bytes. The equivalent class was
  identical to the struct. Adding the repository `SIZE(...)` was identical because `SIZE` expands
  to nothing under MSVC 4.2. Replacing it with an active typedef-array sizeof assertion changed the
  decoder to 1302 bytes and 88 relocations. Keep `SIZE` MSVC-empty.
- Adding an unused member-function declaration changed both `/O2` icon2bc and `/Od /Oi` FONT.
  Supplying the in-class body changed further later functions even though the accessor was never
  called. Inline-accessor declaration/body recovery is therefore a legitimate combined-TU retest
  trigger, not merely a local call-site change.
- Renaming used `/Od` FONT local `pos` (bucket 14) to `p` (bucket 0) changed 28 bytes only in
  `DrawStringExecute`; size and 13 relocations stayed fixed. Renaming parameter `s` to `text`
  changed neither normalized source hash nor any code byte. Local names are an intentional stack
  layout input; parameter names are normally presentation-only.
- Copying all six parameters to locals in the exact WIDGET constructor left all 90 constructor
  bytes and its relocation exact, but changed later `Main` from 752 to 756 bytes (473 unequal raw
  positions under a naive aligned comparison) with its own source hash and 17 relocation count
  unchanged. Exact predecessor bytes do not imply neutral predecessor compiler state.

## Enums: declaration state versus ABI and promotions

Keep three questions separate:

1. **Declaration state:** merely parsing an unused enum can perturb later code, as above.
2. **Constant expression use:** replacing `-1` with an equal enum enumerator in a WIDGET constructor
   changed that constructor's normalized source hash but no emitted function bytes relative to the
   same enum-declaration state. Named constants can be locally code-neutral.
3. **Storage and promotion:** changing packed `short widget::m_kind` to `WidgetKind` was not neutral.
   MSVC first rejected the implicit `short` assignment; with an explicit conversion, constructor,
   `Main`, and `Dim` sizes/bytes changed because the enum occupies four bytes and moves following
   fields. Changing `m_flags` from signed to unsigned short preserved width and offsets but still
   changed only `Main` (752 to 756 bytes) through integer promotions.

Use header-level typedef enums for known numeric domains, but preserve proven packed storage types.
Do not type a serialized `short` field as an enum merely to name its constants.

## Allocation/assert wrappers and provenance

With identical explicit original arguments, replacing
`H2_ALLOC(size, originalFile, originalLine)` by direct
`BaseAlloc(size, originalFile, originalLine)` changed the function source hash but no code byte or
relocation target. The macro is a transparent source abstraction.

Using `H2_ALLOC(size, __FILE__, __LINE__)` was different. Existing `#line` recovery made
`__LINE__` equal the original 21, so the constructor's 83 code bytes stayed identical, but
`__FILE__` named the worktree path rather than the retail filename. Its string relocation target
changed, and two raw bytes in the later unchanged `bitmap::CopyToCareful` also changed through TU
state. Use explicit original filename/line arguments; `__FILE__` is not a harmless convenience in
a matching build.

## Structural-phase rules

During body/type recovery, integrate a function when all of these are credible and reviewed:

1. complete semantics and CFG;
2. real field/type/layout and packed storage widths;
3. frame and `/Od` slots, including `od_slots.py` evidence;
4. inline accessor boundaries when retail shows them;
5. external relocation target sequence and count;
6. a durable `@match-note` for the first remaining compiler-shape divergence and the few obvious
   spellings already tried.

Do not exhaust permutations just because fuzzy is 96-99%. The experiments above show that later
header/type/accessor/predecessor recovery can move that residual without editing the function.
The regex permuter is not part of this workflow. Use the libclang AST permuter only in the global
last-mile phase and audit every retained mutation.

Retest a previously exhausted matrix once, on the integrated combined TU, only after a material
state trigger:

- a transitive included header's token/declaration surface changed;
- include presence/order changed the actual parsed surface;
- an enum/class/struct/typedef, active assertion, or inline declaration/body changed;
- an earlier function's normalized source hash changed, even if its raw bytes stayed exact;
- a macro expansion introduced/removed a literal, filename, line value, declaration, or call shape;
- compiler flags/profile changed.

Do not reopen a matrix for comments, formatting, or an unrelated file with no include/predecessor
path. A trigger justifies one fresh combined-state measurement; it does not justify replaying all
local predicate synonyms.

Reproduce every result from the current objects after a header, predecessor, or compiler-state
change. Historical scores are not a correctness ledger; raw bytes and relocations remain decisive.

Related: [tu-cumulative-eval-order.md](tu-cumulative-eval-order.md),
[o2-tu-cumulative-register-steering.md](o2-tu-cumulative-register-steering.md),
[od-hash-slots.md](od-hash-slots.md), [inline-accessors.md](inline-accessors.md).
