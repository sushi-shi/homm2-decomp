# short-cast-around-constant-select

**Trigger.** A two-constant `?:` whose value is stored into an `i32` field, and
retail sign-extends the result through 16 bits first:

```
83 c0 64        addl   $0x64, %eax     ; the select's `add B` tail
0f bf c8        movswl %ax, %ecx       <-- ours does not emit this
89 8d 1c fe ff ff  movl %ecx, -0x1e4(%ebp)
```

A `movswl reg16,reg32` between the select and an `i32` store is a **narrowing
cast written in the source**, not an enum-width effect.

Measured on `highScoreManager::Update` (RVA 0x654ef), VC6 SP5 `/Od /Ob1`.

## Why it is not the enum's width

The same function makes two of these selects out of enumerators of the *same*
`H2_ENUM_BEGIN(HighScoreManagerConstant)` block:

```
data.value = m_showCampaignScores ? HIGH_SCORE_CAMPAIGN_TITLE_FRAME  /* 7 */
                                  : HIGH_SCORE_STANDARD_TITLE_FRAME  /* 6 */
f7 da  negl %edx ; 1b d2 sbbl %edx,%edx ; f7 da negl %edx ; 83 c2 06 addl $6,%edx
89 55 ...  movl %edx, <data.value>          <-- NO movswl

id = m_showCampaignScores ? HIGH_SCORE_CAMPAIGN_BUTTON  /* 0x93 */
                          : HIGH_SCORE_STANDARD_BUTTON  /* 0x64 */
f7 d8 negl %eax ; 1b c0 sbbl %eax,%eax ; 83 e0 2f andl $0x2f,%eax ; 83 c0 64 addl $0x64,%eax
0f bf c8  movswl %ax, %ecx                  <-- movswl
89 8d ..  movl %ecx, <id>
```

If VC6 were sizing the enum as `short`, both would narrow. Only one does, so
the cast is a source token.

## What made it match

```cpp
    messageValue.payload.widget.id = static_cast<i16>(
        m_showCampaignScores ? HIGH_SCORE_CAMPAIGN_BUTTON : HIGH_SCORE_STANDARD_BUTTON
    );
```

Two sites (the enable/disable pair). `highScoreManager::Update` 93.50% -> EXACT.

## Companions

- [od-constant-ternary-lowering](od-constant-ternary-lowering.md) and
  [neg-sbb-ternary-zero-compare](neg-sbb-ternary-zero-compare.md) read the
  arms and the condition off the `and`/`add` tail; this entry only adds the
  cast that may sit on top of it.
- [short-cast-of-int-sum](short-cast-of-int-sum.md) is the same `movsx` tell
  over an arithmetic expression rather than a select.
