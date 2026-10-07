# `/Gf` emits a function's literals in reverse

Measured with the pinned VC6 SP5 compiler, 2026-10-07.

## Signature

Two literals of one function sit in source order in retail `.data`
(`editbtns.icn` before `clearManager`, `overlay.icn` before `lineManager`),
but the build has them swapped.

## Mechanism

Under `/Gf` each literal is its own COMDAT `.data` section. Within one
function VC6 creates those sections in reverse order of use
(`Open() { GetIcon("AAA1"); strcpy(m_name, "BBB2"); }` emits `BBB2`'s section
first); across functions they follow function order. Without `/Gf` the
literals are `$SG` cells in the object's `.data` in source order. Function
bytes are identical either way.

## Use

Forward order inside a function is retail evidence that the unit compiled
without `/Gf` (`base_nogf`), unless the unit's retail block also proves
pooling (one cell for a literal used twice). The editor's own objects all
compile without `/Gf`.
