# Anonymous data symbol normalization

MSVC 4.2 gives compiler-private data symbols counter-based names such as
`$SG30360`, `$T42489`, and `name$S31565`. The counters depend on compiler state
and are not stable identities. Comparing those names directly makes equivalent
data and code relocations look different.

`scripts/homm2/build/canonicalize_data_symbols.py` rewrites those names in
disposable COFF copies under `build/objdiff/normalized/`. This anonymous-data
pass processes each object independently and uses no source, manifest, retail
RVA, or paired-object information. `objdiff.json` points at the normalized
copies. Compilation, linking, disassembly, and hard gates continue to use the
original objects in `build/objdiff/base/` and `build/delink/`.

## Paired relocation identities

The synthetic delink target can spell a relocation as the nearest known public
symbol plus an addend even when the retail address has another exact public
identity. Those names are supplied by the reconstruction pipeline, so such a
spelling is not retail evidence and must not create a comparison residual.

Before anonymous-data normalization,
`scripts/homm2/build/canonicalize_relocs.py` makes a disposable paired target
under `build/objdiff/paired/`. At the same function-relative relocation site it
accepts the candidate spelling only when the CodeView public owner RVA plus the
candidate addend equals the raw retail operand address exactly. It then adds a
separate undefined COFF symbol and redirects only that relocation. It never
renames a shared target symbol globally, and it does not require a prior match
score to authorize the address proof.

The same pass handles `REL32` aliases such as retail `_open` versus candidate
`__open`. It rewrites only when both names resolve to one unique public RVA and
the encoded COFF addends are equal. The raw linker inputs retain their original
names; only the disposable comparison target receives the candidate spelling.
Thus neither a compiler-private counter nor an alternate external alias is
itself matching evidence: resolved identity, addend, payload, occurrence, and
topology are the evidence.

The paired target also receives a local `$fnpad@<offset>` boundary symbol after
each function whose delinked `.text` span runs past the reviewed retail size in
`build/gen/symbol_names.csv` and whose remainder is one to fifteen bytes of
pure `0x90`/`0xCC` linker alignment fill. The delinked blob attributes that
fill to the preceding provisional span even though padding is not part of any
function. Objdiff strips such fill by itself only when the tail is code; a
function that ends in embedded switch-table data would otherwise keep foreign
fill inside its compared extent and could never observe an exact score. The
boundary symbol restores the reviewed size as the comparison extent without
deleting, masking, or reordering any byte, so a candidate that is genuinely
longer than retail stops pairing its overflow against linker fill. A span
remainder containing any other byte value, sixteen or more fill bytes, or a
function without a reviewed size claim is left untouched.

Anonymous definitions use these forms:

```text
$anon_str_<sha256-of-bytes-through-NUL>_<occurrence>
$anon_f32_<raw-32-bit-value>_<occurrence>
$anon_f64_<raw-64-bit-value>_<occurrence>
$anon_data_<sha256-of-normalized-record>_<occurrence>
```

Named statics retain their semantic prefix while replacing only the volatile
`$S<number>` suffix. Occurrences distinguish identical writable allocations and
are assigned in section-table and section-offset order within an identical
canonical identity. Redirecting a relocation from one identical allocation to
another therefore remains visible.

## Classification

- `$SG<number>` is a string when its conservative allocation span contains a
  NUL and only zero alignment bytes follow it. The hash includes the NUL and
  excludes alignment padding.
- A `$T<number>` span of exactly four bytes is `f32`.
- A `$T<number>` span of exactly eight bytes with a nonzero upper dword is
  `f64`.
- A `$T<number>` that could be either a padded `f32` or a zero-valued `f64`
  remains `anon_data`. This is an explicit lack of type evidence, not skipped
  comparison: its bytes and relocation topology remain in the normalized
  record.
- Other anonymous records hash their storage class, span, relocation-masked
  payload, and sorted relocation site/type/width/signed-addend/target identity.

The classification intentionally does not inspect instructions. A code change
can remove the only typed x87 use while leaving the same constant allocation;
using that instruction as type evidence would create a false data mismatch.

## Embedded switch tables

MSVC also emits switch and event-dispatch tables inside `.text`. Their `DIR32`
relocations commonly name compiler-local code labels such as `$L36759`, while
the delinked retail object represents the same destination as the containing
function plus a function-relative addend. Those are two COFF spellings of the
same table value.

In the disposable comparison copies, a same-function `.text` `DIR32`
relocation to a semantic local label (`type == 0`, storage class 6) or through
another function plus an interior addend is rewritten to the unambiguous
containing function, including a uniquely owned static function. The encoded addend is adjusted so the
resolved section offset is unchanged. This also covers MSVC labels such as
`$normalEvent` that do not use the `$L<number>` spelling.

The rewrite does not apply to `REL32` branches, cross-function references, or
ambiguous function owners. Static callback/function-pointer initializers that
target another function therefore retain their original identity. A wrong switch destination remains a different
function-relative addend and remains visible to objdiff.

## Safety properties

The command refuses overlapping input, output, and sidecar paths. After every
rewrite it reparses the output and proves that section metadata, relocation
site/type/order, symbol indices and metadata, and auxiliary records are
unchanged. Only primary symbol names, the COFF string table, and the paired
symbol-index/addend fields of authorized switch-table relocations may differ.
For every such pair, both the original and normalized relocation must resolve
to the identical section offset. Every other payload and relocation byte must
remain unchanged. Same-offset aliases, dependency cycles, unsupported
relocation forms, and hash or name collisions fail closed.

Each normalized object has a readable `.symbols.tsv` sidecar. A corpus census
can be regenerated with:

```sh
python3 -m homm2.build.canonicalize_data_symbols \
  --summary-root build/objdiff/base \
  --summary-root build/delink \
  --summary-output build/gen/data_symbol_canonicalization_summary.json
```
