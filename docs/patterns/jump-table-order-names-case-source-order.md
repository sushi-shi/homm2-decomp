# jump-table-order-names-case-source-order

**Symptom.** A big `switch` whose case bodies are individually byte-correct, but
whole bodies sit at the wrong addresses: the diff shows one long `insert` of
retail code followed by an equally long `delete` of ours, with identical
instructions in both.

**The evidence is the dispatch table, not the bodies.** `/Od` emits case bodies
in **source order** and the jump table in **case-constant order**, so reading
the table backwards recovers the developers' `case` ordering:

    retail  jmpl *0x5f9d(,%edx,4)        (index = (metadata & 0xf) - 1, 0..6)
    ------------------------------------------------------------------------
      case 0 -> 0x46d2      PICKUP           bodies sorted by ADDRESS:
      case 1 -> 0x4d16      (default)          0x462d  WISDOM       (case 3)
      case 2 -> 0x4750      GOLD               0x4681  LEADERSHIP   (case 4)
      case 3 -> 0x462d      WISDOM             0x46d2  PICKUP       (case 0)
      case 4 -> 0x4681      LEADERSHIP         0x4750  GOLD         (case 2)
      case 5 -> 0x4872      RESOURCE_3         0x4872  RESOURCE_3   (case 5)
      case 6 -> 0x4ac4      RESOURCE_5         0x4ac4  RESOURCE_5   (case 6)

    => source order is WISDOM, LEADERSHIP, PICKUP, GOLD, RESOURCE_3, RESOURCE_5
       (ours had PICKUP, GOLD, WISDOM, LEADERSHIP, RESOURCE_3, RESOURCE_5)

Read the table straight out of the delinked `.text` bytes at the displacement
in the `jmpl *0xNNNN(,%reg,4)` — each entry is the absolute body address, and
its relocation addend is the body's offset inside the function.

Distinct from [jump-table-entry-swap-names-case-labels](jump-table-entry-swap-names-case-labels.md):
there exactly two table entries swap and the bodies stay put (wrong `case`
constants); here the entries are correct and the BODIES move (wrong source
order of the `case` labels).

A `goto` label shared by several arms moves with them: after reordering, the
label lands in whichever case *falls into* it (no `jmp` of its own), and every
other case reaches it with an explicit `goto`.

**Closes.** `advManager::DoEvent`, `MAP_OBJECT_ARTIFACT`'s inner
`switch (metadata & ARTIFACT_EVENT_MODE_MASK)` — ~500 aligned instructions
recovered by reordering six `case` blocks; cross-checked against the already
exact `advManager::DoAIEvent`, whose copy of the same switch is in retail's
order.
