# pattern index (grep by symptom token or tag)

> Full reference for `/Od` frame layout: **[../od-stack-layout.md](../od-stack-layout.md)**
> (the exact name hash, per-scope 16-bucket tables, multi-scope rules) — SOLVED,
> tools `od_slots.py` (predict/solve) + `od_oracle.py` (verify).

| pattern | tags | one-line |
| :------ | :--- | :------- |
| od-hash-slots | topic:regalloc topic:stack topic:od | /Od frame offsets follow MSVC name-hash order; hash SOLVED -> compute names (see od-stack-layout.md) |
| od-debug-build | toolchain:vc42 topic:od topic:flags | retail is /Od /Gr; literal lowering, CRT must stay __cdecl |
| packed-records-and-bitfields | topic:layout cpp:bitfield cpp:pack | recover pack(1) structs + u8/u16 bitfields from stride & andb/andw forms |
| od-cell-access-and-block-jmps | topic:od cpp:array topic:wall | `(cells+w*y)[x]` matches 2D access; leading/block jmp-to-next is an open wall |
