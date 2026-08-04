# A branch ternary parks its result in a temp slot BELOW `this`

**Trigger.** Two constant stores to the *same* frame slot, joined by a `jmp`, then a
load of that slot into the destination - and the slot sits **deeper than the `this`
spill**, i.e. it is a compiler temp, not a named local. That shape is a `?:` whose
arms are not `setcc`-foldable. Writing the same thing as an `if/else` pair of stores
straight into the destination lvalue produces the two stores but no temp, and the
frame comes out 4 bytes smaller.

(`od-constant-ternary-lowering` covers the *`setcc`-folded* constant ternary; this is
the branchy one, and its tell is the temp slot rather than the `dec`.)

## The divergence

`advManager::UpdateHeroLocator` (RVA 0xa491). Retail `sub esp,0x40` with the `this`
spill at `-0x3c`; ours `sub esp,0x3c`, same `this` slot, no `-0x40`.

```
retail                                          ours
----------------------------------------------- -----------------------------------------------
75 09              jne  <else>                  75 09              jne  <else>
c7 45 c0 99 00 00 00  movl $0x99, -0x40(%ebp)   c7 45 f8 99 00 00 00  movl $0x99, -0x8(%ebp)
eb 07              jmp  <join>                  eb 07              jmp  <join>
c7 45 c0 24 00 00 00  movl $0x24, -0x40(%ebp)   c7 45 f8 24 00 00 00  movl $0x24, -0x8(%ebp)
8b 55 c0           movl -0x40(%ebp), %edx
89 55 f0           movl %edx, -0x10(%ebp)
```

`-0x8`/`-0x10` is `msg.payload.widget.data.value` on each side; `-0x40` exists only in
retail, one slot past `this`.

## What made it match

```cpp
    if (whichHero == gpCurPlayer->m_currentHero && ... && !gbAllBlack) {   // ours
        message.payload.widget.data.value = LOCATOR_SELECTED_COLOR;
    } else {
        message.payload.widget.data.value = LOCATOR_NORMAL_COLOR;
    }
```
```cpp
    message.payload.widget.data.value =                                   // retail
        (whichHero == gpCurPlayer->m_currentHero && gpCurPlayer->m_currentHero != INVALID_HERO
         && !gbAllBlack)
            ? LOCATOR_SELECTED_COLOR
            : LOCATOR_NORMAL_COLOR;
```

`UpdateHeroLocator` 96.27% -> EXACT and `UpdateTownLocators` 95.71% -> EXACT (same
locator-colour idiom, temp at `-0x34` past `this` at `-0x30`). Because the temp lands
after `this`, it also pushes every later compiler temp - the `fild` conversion slots in
`UpdateTownLocators` moved `-0x34/-0x38` -> `-0x38/-0x3c` - so the whole tail of the
frame reads wrong until the ternary is restored.
