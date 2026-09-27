# Native incremental linking retains thunks and spare space

Measured on 2026-09-27 in `matcher-3` (`matcher/bss-dimmer`), with cwd,
branch, and `HOMM2_DIR` verified. Earlier DIMMER dossiers cover compiler `/Gm`
and `/Gi` history; no corresponding native LINK `.ilk` history measurement was
found in those dossiers. This probe uses untouched compiler output and stock
LINK 6.00, with no source or image corrections.

The seed is one ordinary source file defining `first`, `second`, and `entry`.
`entry` returns `first() + second()`. The final source adds the ordinary function
`helper` before those definitions. All functions use explicit C linkage and
`__cdecl`; the compiler uses production DIMMER flags, including `/Od` and `/Gy`.
Both versions are compiled to the same `history/probe.obj` path for the incremental
sequence. Source/object/EXE/ILK snapshots and logs are retained at each stage.

All five links use `/DEBUG /OPT:NOREF /NODEFAULTLIB /ENTRY:entry
/SUBSYSTEM:CONSOLE`, with the same pinned linker. `/INCREMENTAL` changes as shown.
The history stages retain the same output, MAP, and PDB paths. A separate fresh
full link provides the final-source layout control.

| Stage | Incremental | Function bodies in MAP order | `.text` virtual size | ILK size |
|---|---|---|---:|---:|
| Fresh final-source control | No | helper `401000`, first `401010`, second `401020`, entry `401030` | 69 | absent |
| Seed | Yes | first `401030`, second `401040`, entry `401050` | 4198 | 9972 |
| Final source added | Yes | helper `401030`, first `401040`, second `401050`, entry `401060` | 4198 | 10844 |
| Unchanged final source relink | Yes | same as preceding stage | 4198 | 10844 |
| History switched to full link | No | same as fresh final-source control | 69 | absent |

The MAP does **not** label the jump slots with `ILT`; absence of that string
would miss the actual table. Direct disassembly of the updated incremental
image shows:

```text
00401005  e9 56 00 00 00  jmp 00401060  ; entry slot
0040100a  e9 31 00 00 00  jmp 00401040  ; first slot
0040100f  e9 3c 00 00 00  jmp 00401050  ; second slot
00401014  e9 17 00 00 00  jmp 00401030  ; newly added helper slot
...
00401064  e8 a1 ff ff ff  call 0040100a
0040106b  e8 9f ff ff ff  call 0040100f
```

The incremental entrypoint is RVA `1005`, pointing at the entry jump slot.
`.rdata` grows from 28 to 289 bytes and a 12-byte `.reloc` section appears.
Large `CC`-filled spare space remains in `.text`. The added helper gets a new
jump slot, but its **body** is placed before the old functions according to the
updated object's order; the old body addresses shift. This tested history does
not retain a late helper body behind the existing body batch.

Switching the same history to `/INCREMENTAL:NO` removes the ILK and restores the
complete `.text` section record (name, virtual size, RVA, raw size, and every raw
byte) to the fresh final-source control. It does not preserve a special final
body order after removing incremental machinery. Whole EXE equality is not
claimed: independent PDB paths/history and timestamps differ in this probe.

A direct check of retail gives entrypoint RVA `d853e`, beginning
`55 8b ec 6a ff 68 48 aa 4e 00 68 a4 e0 4d 00 64`, and a relocation directory
of `(0,0)`. The tested incremental artifacts have conspicuous features absent
from that retail entry/relocation evidence. This is a bounded negative result
for the tested ordinary history, not a universal claim about every LINK option.
No production flag or source change is retained.

Artifacts: matcher-3's `build/link/incremental-layout-audit/`:

- `run.py`, `run.log`, `results.json`, `retail-entry.json`
- `seed.cpp`, `final.cpp`, and per-stage untouched OBJ/EXE/MAP/ILK snapshots
- Five linker logs and compile logs
- `added-disasm.txt` and `full-disasm.txt`

The current production driver explicitly uses `/INCREMENTAL:NO`; its existing
four-link PDB history should not be confused with ILK incremental code placement.
