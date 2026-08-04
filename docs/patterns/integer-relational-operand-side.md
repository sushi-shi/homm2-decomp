# integer-relational-operand-side

**Symptom.** One `cmp` whose operands are on the wrong sides, with the
conditional jump mirrored to compensate. Byte-wise it is a one-bit opcode
change (`39` vs `3b`) plus a different `jcc`, so fuzzy barely moves and the
row is easy to skim past.

    ours                                     retail
    ---------------------------------------- ----------------------------------------
    e8 00 00 00 00  call KBTickCount         e8 00 00 00 00  call KBTickCount
    3b 05 <glTimers>  cmp  eax, [glTimers]   39 05 <glTimers>  cmp  [glTimers], eax
    7e 1e             jle  <skip>            7d 1e             jge  <skip>

(`townManager::Main` 0xa6842 +0x11b, and seven more sites in SOURCE/TOWNMGR.)

## The rule

VC6 `/Od` materialises the operand that needs work into a register and leaves
the other one as the `cmp`'s **memory** operand — and the memory operand is
always the **left** source operand. So:

    source                        emitted
    ----------------------------  ------------------------------------
    KBTickCount() > glTimers[0]   cmp reg, mem   (3b /r)  + jle to skip
    glTimers[0] < KBTickCount()   cmp mem, reg   (39 /r)  + jge to skip

Both spellings evaluate the call first (it is the only operand that needs a
register), so evaluation ORDER is not the discriminator — only which side of
the relational the memory reference sits on. Read the answer straight off the
opcode:

- `39 /r` (`cmp r/m32, r32`, AT&T `cmpl %reg, mem`) -> the memory reference is
  the source's LEFT operand.
- `3b /r` (`cmp r32, r/m32`, AT&T `cmpl mem, %reg`) -> the memory reference is
  the source's RIGHT operand.

The same rule decides member-vs-member compares: whichever member is loaded
into the register last is the RIGHT operand.

    ours    mov edx,[eax+0xe6] ; cmp edx,[ecx+0xde]   m_pendingStrip == m_swapStrip
    retail  mov edx,[eax+0xde] ; cmp edx,[ecx+0xe6]   m_swapStrip == m_pendingStrip

**Closes.** `townManager::Main`, `SetupTown`, `SetArmyCommand`, `DoCommand`,
`SetupMage`, `MageGuildHandler`, `TavernHandler`, `SplitArmy`,
`SetupThievesGuild` — every one of them had at least one relational whose only
defect was this side choice.
