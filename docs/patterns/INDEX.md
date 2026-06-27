# pattern index (grep by symptom token or tag)

> Full reference for `/Od` frame layout: **[../od-stack-layout.md](../od-stack-layout.md)**
> (the exact name hash, per-scope 16-bucket tables, multi-scope rules) — SOLVED,
> tools `od_slots.py` (predict/solve) + `od_oracle.py` (verify).

| pattern | tags | one-line |
| :------ | :--- | :------- |
| od-hash-slots | topic:regalloc topic:stack topic:od | /Od frame offsets follow MSVC name-hash order; hash SOLVED -> compute names (see od-stack-layout.md) |
| od-debug-build | toolchain:vc42 topic:od topic:flags | retail is /Od /Gr /G5 /Ob1; mostly-literal lowering, CRT must stay __cdecl |
| packed-records-and-bitfields | topic:layout cpp:bitfield cpp:pack | recover pack(1) structs + u8/u16 bitfields from stride & andb/andw forms |
| inline-accessors | topic:codegen cpp:inline flag:Ob1 | `jmp $+0` clusters = inlined in-class accessors (/Ob1); reconstruct getters, don't hand-inline |
| od-cell-access-and-block-jmps | topic:od cpp:array cpp:inline | 2D access needs an inline `Row(y)[x]` accessor (raw reassociates); block jmps SOLVED (see inline-accessors) |
