# Signed value cast prevents `/Od` memory RMW folding

**Tags:** od, cast, flags, read-modify-write, vc42
**Seen in:** SOURCE/HERO `hero::ApplyBattleWinTemps`

VC 4.2 folds a plain unsigned flag-field assignment into an in-memory subtraction:

| ours before | retail (and ours after) |
| :-- | :-- |
| `8b 45 fc` `mov eax, [ebp-4]` | `8b 45 fc` `mov eax, [ebp-4]` |
| `83 a8 e3 00 00 00 20` `sub dword ptr [eax+0xe3], 0x20` | `8b 80 e3 00 00 00` `mov eax, [eax+0xe3]` |
|  | `83 e8 20` `sub eax, 0x20` |
|  | `8b 4d fc` `mov ecx, [ebp-4]` |
|  | `89 81 e3 00 00 00` `mov [ecx+0xe3], eax` |

The plain form was:

```cpp
m_eventFlags = m_eventFlags - HERO_EVENT_GRAVEYARD;
```

Materializing the field as a signed value before subtraction prevents that RMW fold:

```cpp
m_eventFlags = static_cast<i32>(m_eventFlags) - HERO_EVENT_GRAVEYARD;
```

The same shape was present in all twelve event-flag clears. Applying the cast to all
twelve changed `ApplyBattleWinTemps` from 470 bytes and 69.397260% to the exact retail
596 bytes: 26/26 blocks, all instructions, and 0/0 relocations match.

This is not a general request to cast flag arithmetic. Use it only when retail repeatedly
shows a separate value load, arithmetic instruction, and store while the otherwise
credible `/Od` source folds those operations into memory RMW. A complete 120-state
unchanged-source census produced only the folded state here, and a bounded expression
matrix isolated the signed value cast as the transition.
