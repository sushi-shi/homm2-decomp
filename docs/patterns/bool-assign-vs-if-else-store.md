# bool-assign-vs-if-else-store

**Trigger.** A boolean `&&`/`||` chain whose 0/1 result ends up in a named
local. Ours materialises the value in a compiler temporary and copies it to the
local; retail stores `1`/`0` straight into the local. The tell is a
`mov <temp>,1 / jmp / mov <temp>,0 / mov <reg>,<temp> / mov <local>,<reg>`
quintet where retail has only `mov <local>,1 / jmp / mov <local>,0` - plus one
extra 4-byte slot in the temporary region, which pushes the register-argument
spill (and every temp below it) four bytes deeper.

Measured on `advManager::QuickInfo` (SOURCE/ADVMGR), VC6 SP5 `/Od /Ob1`.

## Bytes

`blocked` is the local at `-0x444`; the ternary that consumes it follows.

```
ours                                                    retail
85 d2                test  %edx,%edx                    85 d2                test  %edx,%edx
74 0c                je    +0xc                         75 0c                jne   +0xc
c7 85 7c fb ff ff 00 00 00 00   movl $0,-0x484(%ebp)    c7 85 bc fb ff ff 01 00 00 00   movl $1,-0x444(%ebp)
eb 0a                jmp   +0xa                         eb 0a                jmp   +0xa
c7 85 7c fb ff ff 01 00 00 00   movl $1,-0x484(%ebp)    c7 85 bc fb ff ff 00 00 00 00   movl $0,-0x444(%ebp)
8b 85 7c fb ff ff    movl -0x484(%ebp),%eax
89 85 bc fb ff ff    movl %eax,-0x444(%ebp)
83 bd bc fb ff ff 00 cmpl $0,-0x444(%ebp)               83 bd bc fb ff ff 00 cmpl $0,-0x444(%ebp)
74 0c                je    +0xc                         74 0c                je    +0xc
```

Ours is 12 bytes longer here and the whole frame is 4 bytes bigger
(`sub esp,0x48c` vs `sub esp,0x488`), because `-0x484` is a temporary that
retail never allocates.

## Source

```c
/* ours - the assignment form materialises a temp */
blocked = (cell->m_objectIndex != MAPCELL_SPRITE_NONE
           && cell->m_objectTileset != TILESET_DUMMY)
          || cell->m_overlayIndex != MAPCELL_SPRITE_NONE
          || giGroundToTerrain[cell->m_terrainImageIndex] == TERRAIN_WATER;

/* retail - the branch form stores into the local directly */
if ((cell->m_objectIndex != MAPCELL_SPRITE_NONE
     && cell->m_objectTileset != TILESET_DUMMY)
    || cell->m_overlayIndex != MAPCELL_SPRITE_NONE
    || giGroundToTerrain[cell->m_terrainImageIndex] == TERRAIN_WATER) {
    blocked = 1;
} else {
    blocked = 0;
}
```

Note the polarity flip that comes with it: the assignment form falls through to
the `0` arm, the `if`/`else` form falls through to the `1` arm, so the last
term's `jcc` inverts as well.

## Third measurement: the same axis pointing the OTHER way

`highScoreManager::Open` (RVA 0x65169) writes the chain into a **byte class
member**, and there retail is the side that materialises the temp:

```
ours   if (a || b) m_showCampaignScores = 1;    retail  m_showCampaignScores = a || b;
       else        m_showCampaignScores = 0;
--------------------------------------------    --------------------------------------------
75 ..              jne  <then>                  74 ..              je   <one>
8b 45 e8           movl -0x18(%ebp), %eax        c7 45 e4 00 00 00 00  movl $0, -0x1c(%ebp)
c6 40 5e 01        movb $1, 0x5e(%eax)           eb ..              jmp  <join>
eb ..              jmp  <join>                   c7 45 e4 01 00 00 00  movl $1, -0x1c(%ebp)
8b 4d e8           movl -0x18(%ebp), %ecx        8b 45 e8           movl -0x18(%ebp), %eax
c6 41 5e 00        movb $0, 0x5e(%ecx)           8a 4d e4           movb -0x1c(%ebp), %cl
                                                 88 48 5e           movb %cl, 0x5e(%eax)
83 ec 10           subl $0x10, %esp   (frame)    83 ec 14           subl $0x14, %esp   (frame)
```

The 4-byte temp is retail's here, so the *assignment* form is the retail
spelling. Same reading rule as `QuickInfo`, opposite answer: whoever owns the
extra slot owns the assignment form. `highScoreManager::Open` 94.39% -> EXACT.

## Companions

- [bool-expression-return-temp](bool-expression-return-temp.md) is the same
  materialisation in a `return`; there retail *keeps* the temp, so the two
  entries point in opposite directions - read the frame, not the habit.
- [inline-accessor-return-temp](inline-accessor-return-temp.md) for the other
  common source of a "frame is 4 bytes too big" residual.
