# Retail empty stubs: prove before expanding

An empty body or `return 0` in reconstructed source is not necessarily a placeholder.
MSVC 4.2 emits several distinct legitimate stub forms depending on return type, member
calling convention, and argument byte count. Compare retail bytes and relocations before
inventing a body.

The following BASE targets were audited directly against retail. Sizes exclude alignment
padding; every function has **zero relocations** on both sides.

| TU | function | verified RVA | size | exact retail bytes | note |
| :-- | :-- | --: | --: | :-- | :-- |
| WIDGET | `widget::Close()` | `0xDDED0` | `0x1` | `c3` | canonical target retains the real name |
| soundmgr | `soundManager::Main(tag_message&)` | `0xCCA50` | `0x1a` | `55 8b ec 83 ec 04 53 56 57 89 4d fc 33 c0 e9 00 00 00 00 5f 5e 5b c9 c2 04 00` | `/Od` member `return 0` |
| soundmgr | `soundManager::StartSample(...)` | `0xCCA70` | `0x1a` | `55 8b ec 83 ec 04 53 56 57 89 4d fc 33 c0 e9 00 00 00 00 5f 5e 5b c9 c2 1c 00` | null pointer; callee pops 28 argument bytes |
| soundmgr | `soundManager::GetNumberCDDrives()` | `0xCDA80` | `0x16` | `55 8b ec 83 ec 04 53 56 57 89 4d fc e9 00 00 00 00 5f 5e 5b c9 c3` | `/Od` empty member body |
| WINMGR | `CreateFizzleTables()` | `0xCB1D0` | `0x1` | `c3` | canonical target retains the real name |
| WINMGR | `CreateColorTables()` | `0xCB610` | `0x1` | `c3` | canonical target retains the real name |
| WINMGR | `CreateColorLookupTables()` | `0xCB620` | `0x1` | `c3` | canonical target retains the real name |
| RESMGR | `resourceManager::GetMouse(char*)` | `0xC84B0` | `0x1a` | `55 8b ec 83 ec 04 53 56 57 89 4d fc 33 c0 e9 00 00 00 00 5f 5e 5b c9 c2 04 00` | null pointer return |
| RESMGR | `resourceManager::Main(tag_message&)` | `0xC8880` | `0x1a` | `55 8b ec 83 ec 04 53 56 57 89 4d fc 33 c0 e9 00 00 00 00 5f 5e 5b c9 c2 04 00` | `/Od` member `return 0` |
| INPUTMGR | `inputManager::Main(tag_message&)` | `0xCE2D0` | `0x5` | `33 c0 c2 04 00` | optimized `return 0` |
| INPUTMGR | `inputManager::SetMouseCoords(int,int)` | `0xCE450` | `0x3` | `c2 08 00` | empty body; callee pops 8 argument bytes |
| miscwin | `AutoInitSVGA()` | `0xD8730` | `0x4` | `66 33 c0 c3` | zeroes 16-bit return register |
| MOUSEMGR | `mouseManager::Main(tag_message&)` | `0xC94E0` | `0x5` | `33 c0 c2 04 00` | optimized `return 0` |
| Midi | `soundManager::MIDIPoll()` | `0xD4040` | `0x1` | `c3` | canonical target retains the real name |

For the one-byte cases, canonical delinking retains real PDB identities and every
legitimate same-RVA alias. The legacy `--coalesce-common-functions` flag reproduces old
synthetic byte-pattern names only for compatibility and is never used for HoMM2 target regeneration.
The only function byte is `c3`, followed by alignment `90` bytes.

Useful proof commands:

```sh
homm2 sema disasm 0xc84b0 --diff --lite
homm2 relocs 0xc84b0
llvm-nm -n build/delink/BASE/WIDGET.c.obj
llvm-objdump -d --x86-asm-syntax=intel \
  --start-address=0xb8 --stop-address=0xbc \
  build/delink/BASE/WIDGET.c.obj
```

Rule: keep the simplest source that emits the proven retail stub. Expand it only when
retail contains additional instructions, data accesses, calls, or relocations. Neighboring
tainted decompilation is not evidence that the stub itself needs a body.
