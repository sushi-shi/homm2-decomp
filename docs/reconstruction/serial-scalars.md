# S22 / C15: serial receive scalars

`read_byte` now declares one `u8 value`, passes `&value` to the one-byte
`com_rcv` request, and returns that byte only after a one-byte result. Its
former four-element array had three unobserved elements (S22).

`com_rcv` now declares the `DWORD bytesRead` that `ReadFile` actually writes,
passes `&bytesRead`, and explicitly narrows the return to the existing `i16`
ABI. This replaces an `i16[2]` object and a `reinterpret_cast<LPDWORD>` (C15).
Both words were written by the API; the second word was not an extra count.
The explicit return conversion preserves the low-word result for every DWORD,
not merely the current caller's request of one byte.

Each structural change had its own complete two-arm array/scalar matrix:

| Target | Matrix time | Bytes | Blocks | Native whole-TU proof |
| --- | ---: | ---: | ---: | --- |
| `read_byte`, RVA `0x7321d` | 2.14 s | 51 | 4 | all 18 emitted functions and allocated sections identical |
| `com_rcv`, RVA `0x32f87` | 1.58 s | 193 | 12 | all 14 emitted functions and allocated sections identical |

Both arms have the same bytes and topology; the obsolete private array-size
constants were then removed without shifting source line numbers. The modem
matrix certifies its only ordered call target. The comwin matrix rejects both
its baseline and scalar arm for private string identities; the independent
scoped retail checker instead checks each string's bytes at its actual retail
destination and passes all nine ordered target operands.

Both final functions pass `check-function.py` against hash-verified genuine
retail: exact instruction bytes/size, complete DIR32 sites and ordered
target identities/addends. Reproduce with `PYTHONPATH=scripts python3
docs/reconstruction/check-function.py SOURCE/Modem 0x7321d`, then
`SOURCE/comwin 0x32f87`. Native proof uses `compare-objects.py` with
`build/s22-modem-before.obj` or `build/c15-comwin-before.obj` and the matching
`build/objdiff/base/SOURCE/` object.

The [scalar-output pattern](../patterns/serial-api-scalar-storage.md) records
the actual byte-level evidence. Matrices and logs are `build/s22-*` and
`build/c15-*`; source arms are preserved under `docs/matching/read_byte/` and
`docs/matching/com_rcv/`. `homm2 build` and all 877 selftests pass.

No serial allocation, error handling, queue ownership or Win32 call sequence
was changed. Those are separate B39/B40 contract reviews.
