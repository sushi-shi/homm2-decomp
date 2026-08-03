# paren-suppresses-fp-commute

**Symptom.** A single `float * float` whose operands are (indexed memory, frame
slot) comes out with `fld` and `fmul` on the wrong operands. Everything around it —
including the two integer address loads — is byte-identical, so it is not a slot,
type or evaluation-order problem:

    ours                                        retail
    8b 4d e8  movl -0x18(%ebp), %ecx            8b 4d e8  movl -0x18(%ebp), %ecx
    8b 55 f0  movl -0x10(%ebp), %edx            8b 55 f0  movl -0x10(%ebp), %edx
    d9 45 c4  flds  -0x3c(%ebp)      <<<        d9 04 8a  flds  (%edx,%ecx,4)   <<<
    d8 0c 8a  fmuls (%edx,%ecx,4)    <<<        d8 4d c4  fmuls -0x3c(%ebp)     <<<

`combatManager::RippleCreature` (0x9e51a), 6 bytes, the only residual in the
function.

**Not fixable by operand order.** A 10-arm exact-span matrix
(`homm2.permute.batch_source_variants`, one compile per arm, same TU state) proved
that `wave[waveIndex] * amplitude5`, `amplitude5 * wave[waveIndex]`,
`*(wave + waveIndex) * amplitude5`, `amplitude5 * *(wave + waveIndex)`,
`waveIndex[wave] * amplitude5`, the C-cast forms and the `*(gyModify + row_i)`
destination forms all score identically (99.1453%): VC6 commutes the pair itself
and the source operand order is byte-neutral here.

**Close — redundant parentheses on the indexed operand.** The one arm out of ten
that moved (99.4872%, and EXACT in the full build) wraps the subscript:

```cpp
gyModify[row_i] = static_cast<i8>((wave[waveIndex]) * amplitude5);
```

The parenthesised operand is no longer eligible for the commute, so it stays the
`fld` and the frame slot becomes the `fmul` memory operand — retail's shape.

| arm | fuzzy |
| :-- | --: |
| `(wave[waveIndex]) * amplitude5` | **99.4872** |
| all nine unparenthesised/reassociated/cast/destination spellings | 99.1453 |

`combatManager::RippleCreature` 99.66% -> EXACT.

**Caveat.** Measured once, on one site. The commute may be an at-state decision
(a token-stream change can reroll it), so before adopting the parentheses on a new
site, run the matrix — do not assume the parentheses are always the lever.
