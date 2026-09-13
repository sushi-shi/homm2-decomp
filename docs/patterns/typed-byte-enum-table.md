# A byte lookup can have an enum domain without enum-width storage

Measured in VC6 SP5 SOURCE/SPELLAI, RawEffectSpellInfluence RVA0x9716f.
The table maps fifteen ArmySpellInfluence values to SpellType values. A u8
owner formerly required a functional SpellType cast at the only consumer.

```cpp
// Before: definition and declaration use u8.
target->SpellCastWorkChance(SpellType(giSpellInfluenceToSpell[IDX(influence)]));

// After: shared owner has a semantic domain and the proven physical width.
H2_ENUM_STORAGE(SpellType, u8) giSpellInfluenceToSpell[KB_SPELL_INFLUENCE_MAP_COUNT];
target->SpellCastWorkChance(giSpellInfluenceToSpell[IDX(influence)]);
```

Use named SpellType initializers for all16 physical entries, including the
unused zero tail. Do not replace this declaration with a 32-bit enum array.
The existing strict-build wrapper converts to SpellType; target VC6 storage
remains u8. No new reinterpretation helper or compiler-state input is needed.

The semantic type difference emits no byte difference:

```text
offset  retail PE                  old u8 + cast / new byte enum (COFF)
+1b     33 c9  xor ecx,ecx          33 c9  xor ecx,ecx
+1d     8a 88 68 ba 4f 00           8a 88 00 00 00 00
        mov cl,[eax+0x4fba68]       mov cl,[eax+DIR32 giSpellInfluenceToSpell]
+23     51     push ecx             51     push ecx
+24     8b 4d 08 mov ecx,[ebp+8]    8b 4d 08 mov ecx,[ebp+8]
```

The following call resolves to army::SpellCastWorkChance at0x00420595.
The table operand at+0x1f has zero native addend and resolves to0x004fba68;
all16 bytes match the retail image and native KB data. Complete native
comparison across98 objects and1826 function instances also proves the shared
declaration did not perturb dependent code/data/relocations.

This recovers a credible domain, not the unknowable original declaration.
Review every consumer and representation boundary before applying it to
another table. Evidence and the separate four-cast product are in
[C60/C61](../reconstruction/C60-C61-S57-B77.md) and the fixed operand checker.

## Castle hexes: second measured owner

C63 applies the same domain/width recovery to iWallToHexCell and
iTowerToHexCell. The shared CombatCastleHex enum moves intact to combatTypes.h;
both four-entry definitions/externs become
`H2_ENUM_STORAGE(CombatCastleHex, u8)`. CatAttack's two functional casts are
removed; all array-index/comparison consumers use the standard IDX boundary.

```cpp
// Before
targetHex4 = CombatCastleHex(iWallToHexCell[wallIndex]);
// After, with the recovered byte-enum table owner
targetHex4 = iWallToHexCell[wallIndex];
```

CatAttack RVA0x28bb8, offsets relative to its entry:

```text
offset  retail PE                         old u8 + cast / new byte enum COFF
+363    33 d2 xor edx,edx                  33 d2 xor edx,edx
+365    8a 91 04 d4 4f 00                 8a 91 00 00 00 00
        mov dl,[ecx+0x4fd404]             mov dl,[ecx+DIR32 iWallToHexCell]
+36b    89 95 78 ff ff ff                 89 95 78 ff ff ff
        mov [ebp-0x88],edx                mov [ebp-0x88],edx
+39a    33 d2 xor edx,edx                  33 d2 xor edx,edx
+39c    8a 91 08 d4 4f 00                 8a 91 00 00 00 00
        mov dl,[ecx+0x4fd408]             mov dl,[ecx+DIR32 iTowerToHexCell]
+3a2    89 95 78 ff ff ff                 89 95 78 ff ff ff
        mov [ebp-0x88],edx                mov [ebp-0x88],edx
```

Both native operands have zero addends and the unchanged `PAEA` symbol
spelling. All8 bytes match retail at0x004fd404/408. Existing names are kept:
the wall-named table stores9/34/86/113 and the tower-named one22/47/73/100;
the enum's wall/tower names are not grounds for swapping the table contents.
Every consumer in CatAttack, ShotIsThroughWall and Earthquake was read.
All98 native objects and1826 emitted function instances, allocated sections
and ordered relocation identities are unchanged. See
[C62/C63](../reconstruction/C62-C63-S58-B78.md) for scope and residuals.
