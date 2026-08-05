# Native final-link pipeline

The final executable is assembled by the period Microsoft tools, not by Python emulation.
The normal `ninja link` graph has explicit preparation, native-tool, and audit boundaries:

1. `link-order` derives the 95 reconstructed contributions from retail NB09 `sstModule`
   evidence.
2. Python makes disposable final-link copies. It removes reconstruction-only `/EXPORT`
   directives and projects reviewed writable-section topology without changing the raw objects
   used by objdiff.
3. Python writes `build/link/basewin.rsp` for the 44 retail `basewin.lib` members. Native VC 4.2
   `LIB.EXE @build/link/basewin.rsp` creates `build/link/basewin.lib`. Member arguments are
   reversed because this librarian prepends inputs; the resulting archive scan order follows
   the retail contribution order.
4. Python writes the complete `build/link/HEROES2W.rsp`. Native pinned VC 4.0
   `LINK.EXE @build/link/HEROES2W.rsp` (version 3.00.5270) creates the executable and map.
5. Python reads the existing outputs and audits bytes, sections, imports, resources, static
   public RVAs, storage classes, contribution sizes, and unresolved symbols. It does not relink
   during this step.

The native response currently places system and middleware import archives before the 51 direct
game objects and `basewin.lib`, followed by `OLDNAMES.LIB` and the retail-ordered derived
`LIBCMT.LIB`. LINK 3.00 does not support `/LIBPATH`, so response entries name the selected
archives explicitly. The retained flags are the retail-evidenced PE and CodeView settings:
`/MACHINE:IX86`, `/DEBUG:NOTMAPPED,MINIMAL`, `/DEBUGTYPE:CV`, `/PDB:NONE`,
`/BASE:0x400000`, `/ALIGN:0x1000`, `/SUBSYSTEM:WINDOWS,4.0`, the retail stack and heap values,
`/INCREMENTAL:NO`, and `/OPT:NOREF`.

## Why preparation is still necessary

The remaining preprocessing is input reconstruction, not a replacement linker:

- Eleven candidate objects contain source-retention `/EXPORT` directives that the retail final
  link did not receive.
- Candidate COFF objects do not naturally reproduce all reviewed retail writable-section
  topology. The final-link copies rebuild 127 such sections.
- The selected CRT members require the retail `.data` member order. The derived runtime archive
  renames 159 sections in 143 members while retaining the pinned VC 4.0 member code and the
  retail `testfdiv.obj` literal identities.
- VC 4.2 `LIB /DEF` cannot express every combination of decorated local symbol and DLL lookup
  name used by the WinG, Miles, and Smacker imports. The generator therefore creates native
  archives from committed definitions, then minimally patches those archive records. LINK still
  performs member extraction and emits the final import table.
- `/INCLUDE` roots are needed to recover the proven LINK 3.00 vendor-member extraction order.
  The BASE-archive sequence produces exact vendor ABI and exact intra-DLL IAT order.

Normalized objdiff objects and Vostok-delinked objects never enter this graph.

## 2.1 comparison

The Gold 2.1/Buka project supplied one important structural clue: it invokes `LIB.EXE` and
`LINK.EXE` directly with response-file-like argument graphs, and treats BASE as an archive rather
than flattening its members into direct objects. Reusing that archive boundary materially helps
2.0. Its complete ordering is not transferable: 2.1 places source objects before libraries and
repeats a library group around BASE.

Isolating the 2.1-style objects-first placement in 2.0 worsened entry drift from `+0x100` to
`+0x190`, reduced `.rdata` equality from 95.954241% to 95.452009%, reduced exact static public
RVAs from 1213 to 1210, and broke vendor IAT order. The doubled 2.1 library graph also changed
vendor/descriptor ordering. These results reject the later ordering for 2.0 while retaining its
BASE archive clue.

## Measured link experiments

All percentages compare loaded section bytes against the retail executable. Exact static publics
count unique candidate symbols at their retail RVA.

| Input or ordering | Entry delta | `.rdata` equal | `.data` equal | Exact static publics | Result |
| :--- | ---: | ---: | ---: | ---: | :--- |
| Flattened BASE objects, projected game and CRT inputs | `+0xf70` | 95.452009% | 99.865501% | 1206 | superseded |
| Native BASE archive, initial runtime order | `+0x100` | 95.954241% | 99.849964% | 1209 | structural improvement |
| Native BASE archive, `OLDNAMES` before `LIBCMT` | `+0x100` | 95.954241% | 99.849964% | 1213 | retained maximum |
| 2.1-inspired objects first | `+0x190` | 95.452009% | 99.849521% | 1210 | rejected |
| Raw game objects, projected CRT | not competitive | 75.146% | 26.318% | 128 | rejects removing game projection |
| Projected game objects, raw CRT | unchanged `.rdata` | 95.452% | 98.631% | 1103 | rejects removing CRT projection |

Adding `/NODEFAULTLIB` with the SDK archives and omitting explicit `/ALIGN` were byte-neutral.
Generated system import archives can reproduce every retail DLL name, ordinal, and hint set, but
their natural LINK 3.00 member order leaves intra-system IAT order different and does not improve
the loaded-byte maximum. Forcing all 171 imports or grouping forced roots makes the result worse,
so the SDK system archives remain active. `OLDNAMES` before `LIBCMT` is retained because it raises
exact static public placement by four and agrees with the retail `sstLibraries` order.

The current strict link has no unresolved symbols. Vendor ABI and IAT order, ADVAPI ABI, complete
DLL descriptor order, resources, all 1447 comparable static storage classes, 102 selected CRT
writable literals, and 249 required initialized objects match retail. The first remaining text
layout divergence is a 16-byte shortfall in `SOURCE/DRAWING`; the first data-order divergence is
`__nhandle`/`__crtheap`. The full Windows import table still differs in system hint/member order.
These are the next evidence-bearing boundaries; previously rejected flag and layout changes do
not improve the maximum.
