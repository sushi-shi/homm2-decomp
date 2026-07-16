# Anonymous data symbol normalization

MSVC 4.2 gives compiler-private data symbols counter-based names such as
`$SG30360`, `$T42489`, and `name$S31565`. The counters depend on compiler state
and are not stable identities. Comparing those names directly makes equivalent
data and code relocations look different.

`scripts/homm2/build/canonicalize_data_symbols.py` rewrites those names in
disposable COFF copies under `build/objdiff/normalized/`. It processes each
object independently and uses no source, manifest, retail RVA, or paired-object
information. `objdiff.json` points at the normalized copies. Compilation,
linking, disassembly, and hard gates continue to use the original objects in
`build/objdiff/base/` and `build/delink/`.

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
relocation to a semantic local label (`type == 0`, storage class 6) is rewritten
to the unambiguous containing external function. The encoded addend is adjusted
so the resolved section offset is unchanged. This also covers MSVC labels such
as `$normalEvent` that do not use the `$L<number>` spelling.

The rewrite does not apply to `REL32` branches, cross-function references,
ambiguous function owners, or static function symbols (`type == 0x20`, storage
class 3). Static callback/function-pointer initializers therefore retain their
original identity. A wrong switch destination remains a different
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
