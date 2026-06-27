# pattern index (grep by symptom token or tag)

| pattern | tags | one-line |
| :------ | :--- | :------- |
| od-hash-slots | topic:regalloc topic:stack topic:od | /Od frame offsets follow MSVC name-hash order; tune local *names* (not order) to match |
| od-debug-build | toolchain:vc42 topic:od topic:flags | retail is /Od /Gr; literal lowering, CRT must stay __cdecl |
| packed-records-and-bitfields | topic:layout cpp:bitfield cpp:pack | recover pack(1) structs + u8/u16 bitfields from stride & andb/andw forms |
| od-cell-access-and-block-jmps | topic:od cpp:array topic:wall | `(cells+w*y)[x]` matches 2D access; leading/block jmp-to-next is an open wall |
