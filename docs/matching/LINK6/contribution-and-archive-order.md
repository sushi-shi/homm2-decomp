# LINK 6 contribution and archive decisions

This investigation analyzes the actual pinned linker with Ghidra 12.0.4 and
checks its archive-order interpretation against native compilations and links.
It does not modify LINK, compiler objects, or the resulting images.

Input: `build/toolchain/msvc/bin/LINK.EXE`, SHA-256
`9672e578fdfaa43bdb8e9c16071682988665cb90bba2904bf02f6a5576d8ffbc`.
Addresses below are virtual addresses at the linker's image base `0x00400000`.
Function names and structure-field meanings are reconstruction claims, not
recovered debug symbols. Artifacts live in `build/link/link6-re/`:
`analyze.py`, `query.py`, `project/link6.gpr`, per-address decompilations,
and `native-probe.py` with all eight linked probe arms.

## Observed implementation

- `0x00462500` processes COFF section headers in increasing section order,
  calling `0x004611a0` for each. It also runs the COFF symbol walk.
- `0x0040e330` derives an output section name by truncating at `$`.
  `0x0040e450` finds or creates a group keyed by the complete input section
  name and inserts new groups in lexical order.
- `0x0040e5a0` creates a contribution and appends it to the complete-name
  group's tail in the nonincremental path: the previous tail's `+0x20`
  next pointer receives the new contribution; group `+0x20` becomes the new
  tail. This is not a per-function alphabetical sort within ordinary `.text`.
- `0x004627e0` handles COMDAT selection. An already-selected `Any` definition
  is retained when another `Any` definition arrives. Selection modes,
  associative parents, checksum comparisons and largest-definition replacement
  have separate paths. This finding does not claim every COMDAT mode is
  first-definition selection.
- `0x00464890` changes external-symbol definedness and maintains the unresolved
  linked list. `0x00464970` initializes its iterator; `0x00464990` advances it.
- `0x00460560` traverses unresolved externals, looks them up in an archive,
  loads the defining member and immediately calls `0x00462500`. After extraction
  it adjusts the unresolved-list traversal to account for resolved and newly
  introduced symbols. Thus archive table/member order alone is not an object
  arrival schedule.
- `0x00461b20` recognizes the static absolute `@comp.id` symbol and stores its
  value in the module at `+0x30`. This directly verifies where compiler-producer
  metadata enters the linker's module model; the Rich-header writer itself
  has not been traced in this investigation.

## Native extraction check

Four small diagnostic C++ sources were compiled by the pinned CL with
`/Od /Gy /Zl`. Providers define `a()` and `b()`; the two clients call them in
opposite orders. Each client was linked against both provider orders as an
archive and as direct objects: a complete 2 x 2 x 2 product, eight successful
links, with `/NODEFAULTLIB /INCREMENTAL:NO /OPT:NOREF` and an explicit entry.
These disposable probe sources are not reconstructed game source.

| Input kind | Provider input order | Client call order | Client undefined symbols | Linked provider order |
| --- | --- | --- | --- | --- |
| Archive | a,b | a,b | b,a | b,a |
| Archive | b,a | a,b | b,a | b,a |
| Archive | a,b | b,a | a,b | a,b |
| Archive | b,a | b,a | a,b | a,b |
| Direct objects | a,b | either | either | a,b |
| Direct objects | b,a | either | either | b,a |

The direct and archive results support the decompiled arrival model. They do
not establish that all possible source changes preserve symbol-table order,
that all archive dependency graphs behave like this two-provider probe, or
that the remaining Audiere destructor is impossible to place naturally.
Actual source ownership, duplicate-template ownership and archive boundaries
remain viable inputs. Adding specially named sections solely to dictate the
missing placement would be steering, not the source recovery established here.

## Combined source/build checkpoint

Separately from these diagnostic probes, the merged Misc source with actual
initialized local string arrays was linked together with the measured nested
DIMMER template split through stock archives and LINK's historical passes.
The exact candidate inputs and response file are preserved under
`build/link/ownership-combinations/semantic-exact/`; its Misc input is
`matcher-1/build/misc-prefix-string-storage/local-named-semantic/Misc.obj`
(relative to `.claude/worktrees/`). No correction pass was applied.

The resulting executable has 1,208,393 bytes and SHA-256
`518d0b1416739bfe32f515b7719dfdf4436ebd39c3e103a4ff60a4444241a4cc`.
Against retail, 204 bytes differ: 203 in `.text`, one in `.data`; headers,
section layout, `.rdata` and `.rsrc` match. The full 464-byte DIMMER region,
its 12-byte vtable and the 3,928-byte Misc data interval match.

Against the preceding 500-byte-residual executable (SHA-256
`c0eacb837c0d8bdec918962df5bce11270d05247908f4be34badd4015255a1b8`),
296 differing file offsets are repaired and no new differing offsets appear.
This is a measured experimental candidate, not a claim of integrated production
closure. Source annotations, template-type metadata and native archive setup
must be carried through the ordinary build before integration is complete.
