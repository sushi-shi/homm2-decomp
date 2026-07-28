# TODO

## Segment-aware `DATA` topology ratchet

Verify that the hard build checks require compiler-emitted and linked `DATA(...)`
owners to preserve the reviewed order within each independently ordered data
segment or contribution. Source declaration order is intentionally semantic:
MSVC 4.2 emits function-local statics in name-hash order. The check must include
function-local private statics, not only file-scope globals, and must not compare
owners that belong to different segments.

Add regression coverage for a private-static rename that changes candidate COFF
order and report the owning TU, symbols, segment, and offending physical pair if
the generated model or final link no longer preserves reviewed topology.
