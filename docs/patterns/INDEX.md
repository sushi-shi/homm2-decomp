# pattern index (grep by symptom token or tag)

> **Every pattern doc MUST show real asm and the resolution.** Include the actual
> byte-level disassembly of the diff — **retail vs ours, side by side** (from
> `llvm-objdump -d --disassemble-symbols=<mangled> build/delink/... vs build/objdiff/...`)
> — and **what made it match in the end** (the exact source spelling / flag / structural
> change, or for a "reverse pattern" the non-local trigger). Prose alone is not enough.

> Full reference for `/Od` frame layout: **[../od-stack-layout.md](../od-stack-layout.md)**
> (the exact name hash, per-scope 16-bucket tables, multi-scope rules) — SOLVED,
> tools `od_slots.py` (predict/solve) + `od_oracle.py` (verify).
>
> ⚠️ **objdiff fuzzy% hides wrong frame slots** (partial credit for displacement
> diffs) — a 97% function can have every local mis-slotted. Always re-diff with
> `(%ebp)` offsets VISIBLE. See `od-hash-slots.md`.

| pattern | tags | one-line |
| :------ | :--- | :------- |
| od-hash-slots | topic:regalloc topic:stack topic:od | /Od frame offsets follow MSVC name-hash order; hash SOLVED -> compute names. **fuzzy% HIDES slot misses — diff with (%ebp) on** |
| od-debug-build | toolchain:vc42 topic:od topic:flags | retail is /Od /Gr /G5 /Ob1; mostly-literal lowering, CRT must stay __cdecl |
| packed-records-and-bitfields | topic:layout cpp:bitfield cpp:pack | recover pack(1) structs + u8/u16 bitfields from stride & andb/andw forms |
| inline-accessors | topic:codegen cpp:inline flag:Ob1 | `jmp $+0` clusters = inlined in-class accessors (/Ob1); reconstruct getters, don't hand-inline |
| od-cell-access-and-block-jmps | topic:od cpp:array cpp:inline | 2D access needs an inline `Row(y)[x]` accessor (raw reassociates); block jmps SOLVED (see inline-accessors) |
| tu-cumulative-eval-order | topic:od topic:early-stop reverse-pattern | **REVERSE pattern (early-stop signal):** commutative operand load-order that's NOT source-steerable — resolves as sibling TU functions land. Confirm slots+logic, park soft. SIB-index variant IS fixable via `i[(T*)p]` |
