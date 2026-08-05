# narrow-compound-assignment

**Symptom.** One hunk, same instruction count, where ours promotes a `char`/`i8`
(or `i16`) to 32 bits and retail does the arithmetic in the field's own width:

```
ours                                          retail
movsx eax, byte ptr [edx + 0x45]          |   mov  al, byte ptr [edx + 0x45]
sub   eax, 0x1                            |   sub  al, 0x1
mov   byte ptr [ecx + 0x45], al           |   mov  byte ptr [ecx + 0x45], al
```

The store is 8-bit on both sides, so the FIELD width is already right — only the
expression is wrong.

**Rule.** For a narrow lvalue `c`, MSVC keeps the operation narrow for `c--`,
`c++` and `c op= x` (it may read only the low byte of a 32-bit `x`, because the
result is truncated anyway), but promotes for `c = c op x`, which forces
`movsx` + a 32-bit op + a truncating store. The two spellings are semantically
identical here and byte-distinguishable.

**Fix.** Write the compound form.

| site | was | now |
| :-- | :-- | :-- |
| `hero::ApplyBattleWinTemps` (mermaid arm) | `m_luck = m_luck - 1;` | `m_luck--;` |
| `IsCDDrive` | `gText[0] = gText[0] + driveIndex;` | `gText[0] += driveIndex;` |

Both closed EXACT. `IsCDDrive` is the `c op= int` case: `driveIndex` stays `i32`,
and retail's `add al, byte ptr [ebp - 0x4]` is the compiler reading the spilled
argument's low byte — it is NOT evidence that the parameter is a `char`, so do
not narrow the signature to chase it.

**Do not sweep this blind.** For an `int` lvalue the two spellings are identical
code, so rewriting `x = x + y` tree-wide is pure churn and re-keys ledger rows.
Drive it from the byte evidence: the tell is a `movsx`/`mov r8` mismatch on the
same operand. `homm2 od-frames`-style scanning is not needed - grep the residual
hunks for one side carrying `movsx` where the other carries a byte/word `mov`.

**Related.** A different-looking member of the same family is a local whose
declared width is wrong: `movsx edx, word ptr [eax + 0x56]` into a 4-byte slot
against retail's `mov cx, word ptr [eax + 0x56]` into a 2-byte slot means the
LOCAL should be `i16` (open: `dropListWidget::DrawDropStuff`).
