# case-guard-break-vs-else-arm

**Symptom.** One 4-byte displacement at the end of a `switch` case arm, nothing
else. Ours jumps to the case's *own* trailing `break`; retail jumps to the end
of the whole `switch`. The instruction is the same `e9` in both — only the
target differs, so the diff is exactly four bytes and no code shifts.

    ours   if (visited) { dlg(); } else { reward(); } break;
    -------------------------------------------------------------------------
    24c2   e9 52 38 00 00   jmp  0x5d19   <- switch end   (ours, wrong)
    retail
    24c2   e9 e8 00 00 00   jmp  0x25af   <- the case's own `break`, 0xed away

**The rule.** In a switch case written

    if (C) { A; break; }            <- guard form
    B;
    break;

the guard's `break` targets the *switch end*, and the case's trailing `break`
targets it too, so both are near jumps to the same far label. Written as

    if (C) { A; } else { B; }       <- else form
    break;

the then-arm instead jumps to the if/else JOIN, which sits immediately before
the case's single `break` — a near jump to a nearby label.

So a case arm whose `jmp` lands on a *local* address that is itself a
`jmp <switch end>` is the **else form**; a case arm that jumps straight to the
switch end is the **guard form**. Both compile to one `e9`; only the four
displacement bytes tell them apart, which is why fuzzy scoring hides it.

The same test applies to a nested `if` inside a case: at `DoEvent+0x2a9` retail
jumps to `0x5d19` (the switch end) where we jumped to `0x593` (the if/else
join) — retail wrote `if (!active) { dlg(); break; } else { ... }`, i.e. an
explicit `break` inside an arm that already has an `else`.

**Closes.** `advManager::DoEvent` — eight sites in one pass
(`MAP_OBJECT_XANADU`, `_FORT`, `_STANDING_STONES`, `_WITCH_DOCTOR_HUT`,
`_MERCENARY_CAMP`, `_GAZEBO`, `_WITCH_HUT` (two guards -> `else if`), plus the
sphinx and pyramid arms in the other direction).
