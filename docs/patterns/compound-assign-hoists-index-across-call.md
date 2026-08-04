# A store index computed into `esi` BEFORE a helper call means `a[expr] *= x`, not `a[expr] = (T)(a[expr] * x)`

**Trigger.** An array element with a *computed* subscript is scaled by a `double`,
so the store needs `__ftol`. Two spellings differ only in where the destination
subscript is evaluated:

- `a[expr] = static_cast<i32>(a[expr] * d);` — subscript recomputed AFTER the
  `call __ftol`, in a scratch register (`edx`).
- `a[expr] *= d;` — subscript recomputed BEFORE the call, into the callee-saved
  `esi` so it survives it.

The `esi` (or `edi`) in a `/Od` body that otherwise never needs a callee-saved
register is the tell.

Measured on `combatManager::AICheckRetreat` (RVA 0x15a60).

## The divergence

```
ours   force[opp] = (i32)(force[opp] * MOD);      retail   force[opp] *= MOD;
--------------------------------------------      --------------------------------------------
8b 95 b8 fe ff ff  movl -0x148(%ebp), %edx        8b 95 b8 fe ff ff  movl -0x148(%ebp), %edx
b8 01 00 00 00     movl $0x1, %eax                b8 01 00 00 00     movl $0x1, %eax
2b 82 af f2 00 00  subl 0xf2af(%edx), %eax        2b 82 af f2 00 00  subl 0xf2af(%edx), %eax
db 84 85 f8 fe..   fildl -0x108(%ebp,%eax,4)      db 84 85 f8 fe..   fildl -0x108(%ebp,%eax,4)
dc 0d ........     fmull <MOD>                    dc 0d ........     fmull <MOD>
e8 ........        calll __ftol                   8b 8d b8 fe ff ff  movl -0x148(%ebp), %ecx
8b 8d b8 fe ff ff  movl -0x148(%ebp), %ecx        be 01 00 00 00     movl $0x1, %esi
ba 01 00 00 00     movl $0x1, %edx                2b b1 af f2 00 00  subl 0xf2af(%ecx), %esi
2b 91 af f2 00 00  subl 0xf2af(%ecx), %edx        e8 ........        calll __ftol
89 84 95 f8 fe..   movl %eax,-0x108(%ebp,%edx,4)  89 84 b5 f8 fe..   movl %eax,-0x108(%ebp,%esi,4)
```

Same instruction multiset, same two subscript computations, only the position of
the second one relative to the call differs — and with it the register class.

Note the *other* site in the same function, `force[side] = (i32)(force[side] * MOD)`
with a plain local subscript, keeps the "after the call" order in BOTH builds
(`0x445: movl -0x13c(%ebp), %edx` follows `0x440: call __ftol`). Removing the
`static_cast<i32>` and letting the conversion be implicit at the assignment does
**not** move the subscript either; only the compound-assignment form does.

## What made it match

```cpp
    force[IDX(OppositeCombatSide(m_currentSide))] *= COMBAT_AI_TOWN_STRENGTH_MODIFIER;
```

`combatManager::AICheckRetreat` 94.24% -> EXACT (with the slot solve and the
`fild`/`fdivrp` divisor).
