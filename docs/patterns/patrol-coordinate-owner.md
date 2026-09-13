# Unsigned patrol coordinates replace duplicate hero views

The retail loader writes this hero region from `mapHeroExtra::hasPatrol` and
`patrolRadius`; the AI uses the first two bytes as a spatial anchor, not as a
boat identity. Recovering one real owner eliminates a misleading union and
two casts without altering the object layout.

```cpp
// Before: two overlapping three-byte records in a union.
u8 m_boatId; char m_boatDestY; i8 m_boatTravelRange;
i8 m_patrolX; i8 m_patrolY; i8 m_patrolRadius;
// Consumer: abs(y - static_cast<u8>(gpCurAIHero->m_boatDestY))

// After: one actual patrol anchor and signed radius.
u8 m_patrolX;
u8 m_patrolY;
i8 m_patrolRadius;
// Consumer: abs(y - gpCurAIHero->m_patrolY)
```

`DetermineTargetPosition` at RVA `0x81c03`, function-relative offsets:

| Offset | Retail bytes | Before and after bytes | Operation |
| --- | --- | --- | --- |
| `+0x7bd` | `33 c0` | `33 c0` | zero EAX |
| `+0x7bf` | `8a 42 2a` | `8a 42 2a` | unsigned Y byte into AL |
| `+0x7d5` | `8b 15 …` | `8b 15 …` | same `gpCurAIHero` owner |
| `+0x7db` | `0f be 42 2b` | `0f be 42 2b` | signed radius into EAX |
| `+0xcff` | `33 d2` | `33 d2` | zero EDX |
| `+0xd01` | `8a 51 2a` | `8a 51 2a` | fallback destination Y |

The byte zero-extension was already explicit in retail; putting that domain
on the actual field makes the compensating casts unnecessary. The radius's
sign-extension is a separate obligation and is preserved. The game loader's
narrow coordinate writes and decrement also remain native-identical.

This is an evidence-backed field/layout recovery, not a source respelling
to select a compiler state. All98 units/1826 emitted functions and allocated
sections match before/after, and the four affected bodies have complete
independent retail evidence including their private numeric constants.
See [U10](../reconstruction/U10.md) for provenance and verification scope.
