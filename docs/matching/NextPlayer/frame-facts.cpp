// game::NextPlayer (SOURCE/GAME, RVA 0x7bd99) — 2026-07-29 -> EXACT 100
//
// Pure frame facts, pinned by the delta-filtered slot analysis (the mirror-band
// recon dossier) plus one correction discovered on integration:
//   1. index declares before humansLeft (humanCount renamed, bucket 15, so the
//      pair walks index(-0x1c) then humansLeft(-0x20) as retail wants).
//   2. remotePlayer is NOT block-scoped: retail stores it at -0x18, i.e. it
//      walks between scratch and index — a FUNCTION-scope declaration
//      (bucket 4). The recon called -0x18 a phantom; the store rows
//      `mov [ebp-0x18], eax` disproved that and pinned the hoist.
// Zero differing rows, 114/114 relocations, retail frame 0x28.
//
// FlyTo (same session): the five pinned defect fixes + the machine-verified
// 25-name frame solution landed 98.73 -> 99.75 banked; residual is the fdiv
// attribution artifacts plus three at-state mirrors. CheckForCreatureUpgrades:
// agent-verified 12-slot rename solution + else flatten, 98.67 -> 99.55; one
// dead jmp (node-loop head) and a merge-add at-state pair remain.
