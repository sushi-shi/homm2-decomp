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
| od-early-return-guard-clauses | topic:od topic:control-flow | **DEFAULT shape** — NWC devs used early returns EVERYWHERE; invert `if(cond){body}` to `if(!cond)return;body`, each `&&` its own guard |
| od-hash-slots | topic:regalloc topic:stack topic:od | /Od frame offsets follow MSVC name-hash order; hash SOLVED -> compute names. **fuzzy% HIDES slot misses — diff with (%ebp) on** |
| packed-records-and-bitfields | topic:layout cpp:bitfield cpp:pack | recover pack(1) structs + u8/u16 bitfields from stride & andb/andw forms |
| inline-accessors | topic:codegen cpp:inline flag:Ob1 | `jmp $+0` clusters = inlined in-class accessors (/Ob1); reconstruct getters, don't hand-inline |
| od-cell-access-and-block-jmps | topic:od cpp:array cpp:inline | 2D access needs an inline `Row(y)[x]` accessor (raw reassociates); block jmps SOLVED (see inline-accessors) |
| tu-cumulative-eval-order | topic:od topic:regalloc topic:tu-state | Diagnose cumulative operand order, then rebuild the combined TU and exhaust local/predecessor steering before a documented soft defer |
| o2-tu-cumulative-register-steering | topic:o2 topic:regalloc topic:tu-state | BITMAP `CopyTo`: combined state turned redundant aliases into 89.59%; direct object access restored all 203 bytes; use audited exact-preserving predecessor variants and consult the target no-repeat ledger |
| msvc42-global-reload-cse | toolchain:vc42 topic:o2 topic:regalloc topic:relocs topic:cse | Source references are not relocation occurrences; diagnose dominance, branch ownership, tail merging and live registers. `CycleColors` branch-local copies recovered exact 71/71 relocations |
| msvc42-tu-declaration-state | toolchain:vc42 topic:tu-state topic:o2 topic:od | Controlled BASE experiments: unused enums/classes/accessors, includes, exact predecessors, and `__FILE__` can change unrelated emitted code; structural-phase retest rules and raw-hash evidence |
| retail-empty-stubs | topic:stubs topic:proof topic:delinker | Empty/`return 0` source can be exact retail: prove raw bytes, calling-convention return, and zero relocations before expanding |
| short-local-coord-truncation | topic:o2 cpp:short topic:widget | /O2 widget parent-relative coord `(short)(win->off + this->pos)` stuck 30–90% -> use a `short` LOCAL, not inline `static_cast<short>` (local reproduces the (short)operand+operand truncation) |
| codeview-array-decayed-to-pointer | topic:globals topic:o2 topic:od | a global `T*` whose CodeView symbol SIZE>4 is really `T[]`; change decl+def to array -> indexed `g[i]` becomes direct `[g+i]`, byte-exact |
| struct-typed-member-modeling | topic:structs topic:o2 topic:layout | model a member as its real struct/array type (`T member[N]`) not a raw pad; struct-copy + by-value return + indexing then match byte-exact (event ring, aggEntry directory) |
