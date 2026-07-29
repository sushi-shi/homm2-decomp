// PopNetBox (SOURCE/KB, RVA 0x9d4a6) — 2026-07-29 matrix, OPEN at 99.07
//
// Matrix: build/pnb-axes.json -> build/source-variant-batch/pnb.
// Residual: retail compares the keyboard byte at byte width
// (`cmp byte [ebp-0xf0],0x20 / jb`) at the two printable-range checks; ours
// zero-extends into eax first (xor/mov al/cmp eax). Measured rejections at
// every probe state: dropping the u32 casts, u8 casts on the constants, and
// char literals (worse) — all still emit the int-width promotion; the
// u32_cast spelling scores best of the four.
//
// UNTRIED: a `u8`-typed local holding the constant, a different lvalue for
// the key byte (char local copy), or whether the narrow is a peephole that
// only fires under other TU states (add to the KB island batch).
