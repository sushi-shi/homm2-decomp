/*
ValidAttack unsigned-facing storage-domain matrix

Target: SOURCE/PATH army::ValidAttack, RVA 0x000be217.

Retail lowers the two wide-attack facing choices with unsigned carry/borrow:

    cmp facing, 1
    sbb eax, eax
    and eax, 5

and:

    cmp facing, 1
    mov eax, 3
    adc eax, -1

The enum-typed comparison emitted signed setcc sequences and made two blocks
larger.  The reviewed two-arm axis compared the enum expression with an
explicit conversion of the proven i32 storage domain:

    m_facing < ARMY_FACING_RIGHT

    static_cast<u32>(m_facing) < static_cast<u32>(ARMY_FACING_RIGHT)

Both occurrences were changed together.  The complete product crossed these
two source arms with the clean state and 50 declaration-forest states.

Artifacts:

    build/valid-attack-facing-axes.json
    build/valid-attack-facing-manifest.json
    build/source-variant-batch/valid-attack-facing/results.json

All 102 candidates completed.  The unsigned-storage arm is exact in the clean
state and in every reported forest state: 100% bytes, retail size 627, exact
46-block CFG, and 11/11 ordered relocations.

Disposition: integrated.
*/
