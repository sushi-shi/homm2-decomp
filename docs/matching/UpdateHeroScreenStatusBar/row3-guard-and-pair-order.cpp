// UpdateHeroScreenStatusBar (SOURCE/HERO, RVA 0x6e0be) — 2026-07-29, OPEN
//
// Matrix: build/uhssb-axes.json -> build/source-variant-batch/uhssb
// (4 arms x {clean + 8 forest states}, complete). No integration:
//   row3_guard and_else: 1899 bytes (worse than or_goto 1894; retail 1880) —
//     the else emits an extra join jmp. REJECTED.
//   creature_pair swap: byte-identical (canonicalized). REJECTED.
//
// Retail's row-3 guard branches far to default_hero_text on BOTH terms with
// no goto stub (jl X; jge X) — neither the || goto nor the &&/else spelling
// reproduces it here; suspect jump threading tied to another construct, plus
// three unidentified base-only stubs (+14 bytes). Needs a full INSERT-row
// enumeration and --blocks --dot pass before the next matrix.
