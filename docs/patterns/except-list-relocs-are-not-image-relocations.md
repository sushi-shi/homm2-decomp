# `/GX` frames add three `__except_list` relocations that retail's site list never has

Measured on this reconstruction's `/GX` objects against
`config/retail/absolute_relocations.tsv`.

## Mechanism

A `/GX` function that registers an EH frame reads, installs and removes
`fs:[0]` through the absolute symbol `__except_list`:

```
64 a1 00 00 00 00       mov eax, fs:[__except_list]   ; read
64 89 25 00 00 00 00    mov fs:[__except_list], esp   ; install
64 89 0d 00 00 00 00    mov fs:[__except_list], ecx   ; uninstall
```

Each is a COFF DIR32 relocation in the object, but the operand is an offset
into the `fs` segment, not an image address, so the linked image has no base
relocation and the retail DIR32 site manifest has no row for it.

## Use

Expect every `/GX` function with a frame to show exactly three more DIR32
sites in ours than in retail. On same-length functions a relocation-count
census found 72 mismatches, all `/GX`, all exactly +3. Skip `__except_list`
before counting, as `homm2.audit.reloc_donation`, `homm2.audit.census` and
`homm2.compare.canonicalize_relocs` do; a +3 is not a missing or extra data
reference.
