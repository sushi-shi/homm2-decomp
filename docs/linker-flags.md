# Build flags (HEROES2W.EXE)

The retail PoL HEROES2W.EXE is a **DEBUG build** (it ships a CodeView NB09 stream).

- **`/Od`** - unoptimized. Verified from retail disasm: full `ebp` frames
  (`push ebp; mov ebp,esp; sub esp,N; push ebx/esi/edi`), `this`/locals stored to the
  stack and immediately reloaded, every intermediate value round-tripped through a stack
  slot. `/Od /MT /Gr` reproduces this shape byte-for-byte.
- **`/Gr`** - `__fastcall` is the default calling convention for free functions
  (458 mangle `@@YI` vs 6 `@@YA` cdecl / 6 `@@YG` stdcall). Member functions are
  `__thiscall` as usual (`@@QAE`).
- **`/MT`** - static multithreaded CRT (`LIBCMT.lib`, from `C:\MSDEV\LIB\`).
- `/GX` (C++ EH) only where a TU uses try/catch (the `eh` flag profile).

Implication: matching is MUCH easier than an /O2 decomp - each statement lowers
mechanically; there is no regalloc/scheduling/inlining to fight.
