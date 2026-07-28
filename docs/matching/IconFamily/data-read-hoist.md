# Icon-family m_data read hoist

MSVC 4.2 kills field-load CSE across a store to a file-scope static: reading
`srcIcon->m_data` after publishing `s_entry` forces a second `[ecx+0x12]`
load, while retail's single load proves the read preceded the store. The
Iconm2b matrix measured the hoist directly (three shapes, clean plus 40
top-of-TU forest states, complete 123-arm product):

- `build/iconm2b-data-hoist-axes-20260728.json`
- `build/iconm2b-data-hoist-manifest-20260728.json`
- `build/source-variant-batch/iconm2b-data-hoist/`

`u8* data = srcIcon->m_data;` before the `s_entry` store reached 85.0663%
(exact 31/31 topology) against 81.3812% for the post-store read; the
first/between placements are byte-identical.

Propagated per TU with a fresh 64-state census each (hash-scoped MAX):

| TU | before | after |
|---|---:|---:|
| Iconm2b | 81.38 | 85.07 |
| Icon2b | 76.43 | 79.60 |
| iconf2bc | 82.87 | 84.25 |
| Iconmf2b | 79.60 | 81.25 |
| Iconf2b | 85.92 | 86.38 (clean) |
| Icondf2b | 73.73 | 73.46 (kept: same parent as flip-mono) |
| icon2bc | 82.70 | 81.58 — REVERTED |
| Icond2b | 79.25 | 78.86 — REVERTED |

icon2bc and Icond2b regressed under the hoist and keep their original
post-store spelling; their retail entry blocks show a different setup dialect
(icon2bc loads a static before the entry computation). Dialect differences
stay explicit per the family convention.
