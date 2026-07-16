# Reviewed relocation spellings

A linked PE preserves the relocated 32-bit value, not the original COFF symbol and
implicit addend. The same value may be represented as `earlier_symbol + positive`,
`later_symbol + negative`, or many other equivalent pairs. Vostok's ordinary
last-preceding-symbol rule is therefore only a fallback representation, not evidence
of the original source owner.

Canonical target generation accepts reviewed disambiguation rows from
`config/delink_reloc_aliases.tsv`:

```text
function_rva  target_rva  site_rva  owner  addend  occurrences  provenance
```

The normal key is the containing retail function plus the encoded target RVA, with
`site_rva=*`. When multiple relocations in that function encode the same address but
need different COFF spellings, `site_rva` is the absolute retail RVA of the four-byte
relocation field. An exact-site row overrides a matching wildcard row and must have
one occurrence. Vostok checks that every numeric site lies inside the containing
function, that the named owner exists, `owner RVA + addend == target RVA` with 32-bit
wrapping, and that each selected row's configured occurrence count closes. Reviewed
aliases take precedence over data-manifest ownership and nearest-symbol selection.

The same manifest can resolve ambiguous code aliases for decoded `REL32` calls and
jumps. For a code target, Vostok requires a zero addend and requires the selected name
to be one of the public symbols at that exact target RVA. Do not use this to compensate
for a wrong CRT source declaration: for example, VC4.2's `_write` and legacy `write`
declarations naturally emit different C++ COFF spellings. Reconstruct the declaration
that emits retail's spelling instead of forcing the target to match the candidate.

These rows are reconstruction evidence. They are added whenever owner/addend identity
is certain; fuzzy percentage is not an acceptance criterion. Exact same-site code can
provide evidence, but semantic array indexing, TU ownership, serialization layout, or
other independent evidence can also prove a row below 100%.

`python3 -m homm2.build.gen_reloc_aliases` writes candidate-derived proposals to
`build/gen/reloc_alias_proposals.tsv` and unresolved groups to
`build/gen/reloc_alias_diagnostics.json`. Proposals never enter canonical targets
automatically. Multiple candidate spellings for one function/address are inherently
ambiguous after instruction movement and remain diagnostics until independently
resolved.
