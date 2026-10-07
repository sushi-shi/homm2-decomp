# Version lineage

This branch reconstructs Buka's release of Heroes of Might and Magic II Gold
2.1. It was seeded from the Price of Loyalty 2.0 reconstruction
(`decomp-pol-2.0`), so the differences below are those observed between the
pinned retail images. Build details are in [builds](../builds.md).

```text
PoL 2.0 -> Gold 2.1 (NWC) -> Buka Gold 2.1
VC4.2      not targeted      VC6 SP5, mostly /Od
HEROES2W.EXE HEROES2W.EXE    HMM2PL.exe (game), EDT2PL.exe (editor)
```

| Page | Contents |
| --- | --- |
| [Gold 2.1 Buka](gold-2.1-buka.md) | every functional change from PoL 2.0, classified [2.1], [Buka] or [unclassified] |
| [Cross-version spellings](cross-version-spellings.md) | source spellings that differ from PoL 2.0, and whether they port back |
| [Cross-version slot layout](cross-version-slot-layout.md) | why `/Od` stack slots move between 2.0 and 2.1 |
| [Base selection](base-selection-audit.md) | which reconstruction the downstream branches start from |

The PoL-to-Buka address claims harvested from the earlier attempt are in
[`buka-va-queue.tsv`](../../config/retail/versions/buka-va-queue.tsv);
`python3 -m homm2.audit.cross_version` uses them only to cross-check its
pairing; its report and the other version audits write to `build/versions/`.
