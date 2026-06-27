# HEROES2W.EXE is an unoptimized /Od /Gr debug build

**tags:** `toolchain:vc42` `topic:od` `topic:flags`

## symptom
The retail `.text` for a TU spills `this`/params to the stack and reloads them
on every use, emits `jmp` to the next instruction at statement boundaries,
always pushes ebx/esi/edi in the prologue, and never keeps a value live across
statements. That is textbook **/Od** (no optimization), not /O2.

## cause
PoL `HEROES2W.EXE` was shipped compiled **/Od** (which is also why it carries a
full CodeView NB09 stream). The configured `base` flag profile is therefore the
debug set, not an optimized one:
```
base = ["/nologo", "/c", "/Od", "/MT", "/Gr"]
```
`/Gr` makes the *default* calling convention `__fastcall` for free functions —
member functions stay `__thiscall` (ecx=this, callee cleanup `ret $N`), but any
free/CRT function you declare yourself will be mis-called unless it carries an
explicit convention.

## fix
- Match by literal translation: each C statement lowers independently, locals
  live on the stack, `this` is reloaded per access. Don't try to be clever; the
  optimizer isn't there to fight.
- For CRT helpers (`memcpy`, `_read`, `_write`, ...) include the real toolchain
  headers (`<string.h>`, `<io.h>`) so they carry `__cdecl` and aren't turned
  into fastcall by `/Gr`. Self-declaring them without `__cdecl` produces wrong
  (register) argument passing.
- `operator new`/`operator delete` stay `__cdecl` (`??2@YAPAXI@Z` /
  `??3@YAXPAX@Z`) regardless of `/Gr`.

## upside
/Od is *much* easier to match than /O2 — no scheduling or instruction-combining
to reproduce. The two real costs are (1) hash-ordered local slots
(see od-hash-slots.md) and (2) occasional regalloc/scheduling of member-base
pointer additions inside loops.
