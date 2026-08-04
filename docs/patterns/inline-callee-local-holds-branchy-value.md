# A branchy value in a slot BETWEEN the temps and the register spill is an inlined callee's local

**Trigger.** One frame slot is written by an if/else (or `||`) chain and read
once, and it sits **deeper than every compiler temp but shallower than the
`this`/`__fastcall` spill**. No named local can land there (named locals, inner
blocks included, come first) and no compiler temp can either (temps come after
the spill). Under `/Ob1` that region belongs to the locals of *inlined callees*,
one set per call site.

## The frame really has four regions

```
  -0x4 ...   named locals, by (bucket, -declaration index), scope group by group
  ...        compiler temporaries - materialised `?:` and `||` values, call
             results, switch selectors, in creation order
  ...        INLINED-CALLEE LOCALS, one set per /Ob1 call site
  deepest    __fastcall / `this` register-argument spill
```

Measured on VC6 SP5 `/Od /MT /Gr /G5 /Ob1` with `homm2 sema frames` plus
`build/scratch-m1/probe_or*.cpp` (a `__fastcall` free function with two nested
switches and one call whose argument is a three-term `||`):

| argument spelling | frame | where the 0/1 value lands |
| :-- | :-- | :-- |
| `f(a\|\|b\|\|c)` inlined into the call | `0x14` | last temp, **after** the spill |
| `i32 z = a\|\|b\|\|c; f(z);` (named local) | `0x18` | temp after the spill **plus** a named slot |
| `inline i32 g(){ i32 z = a\|\|b\|\|c; return z; } f(g());` | `0x18` | still a temp - the initializer needs one |
| `inline i32 g(){ if (a\|\|b\|\|c) return 1; return 0; } f(g());` | `0x14` | inline local, but stores **1 before 0** |
| `inline i32 g(){ i32 z; if (!(a\|\|b\|\|c)) z=0; else z=1; return z; } f(g());` | `0x14` | inline local, stores **0 before 1** |

Only the last row reproduces retail's store order for a `||`-shaped select.

## Case 1 - `combatManager`-style CP1251 upcase (`game::ViewArmy` 0x55529, `townManager::SetupThievesGuild` 0xab344)

Retail writes the folded letter into a byte slot one step above `this` and then
copies it out; our named local wrote straight into the destination.

```
ours  (char upperFirst; ... ; armyName0[0] = upperFirst;)     retail
------------------------------------------------------------ ------------------------------------------------------------
8b 4d b0     movl -0x50(%ebp), %ecx   ; armyName0[0]          8b 4d b0     movl -0x50(%ebp), %ecx
...                                                           ...
88 55 b0     movb %dl, -0x50(%ebp)    ; store into the array  88 95 14 ff ff ff  movb %dl, -0xec(%ebp)   ; inline local
                                                              8a 95 14 ff ff ff  movb -0xec(%ebp), %dl
                                                              88 55 b0           movb %dl, -0x50(%ebp)
8d 45 b0     leal -0x50(%ebp), %eax                           8d 45 b0     leal -0x50(%ebp), %eax
```

`this` is at `-0xec` in ours and `-0xf0` in retail; every temp between `-0xa4`
and `-0xe8` is identical, so the extra slot is *below* the temps.

What closed it (identical edit in both TUs, a file-static inline per TU):

```cpp
inline char ToUpperCp1251(u8 letter) {
    char capital;

    if (letter >= 'a' && letter <= 'z')
        capital = letter - CP1251_CASE_STEP;
    else if (letter >= CP1251_SMALL_A && letter <= CP1251_SMALL_YA)
        capital = letter - CP1251_CASE_STEP;
    else if (letter == CP1251_SMALL_YO)
        capital = CP1251_CAPITAL_YO;
    else
        capital = letter;
    return capital;
}
...
armyName0[0] = ToUpperCp1251(armyName0[0]);   /* game::ViewArmy        */
gText[0]     = ToUpperCp1251(gText[0]);       /* townManager::SetupWell-style site */
```

The parameter costs no slot: the argument is a simple lvalue, so MSVC
substitutes it and every comparison re-reads `-0x50(%ebp)` exactly as retail
does. `SetupThievesGuild` 99.94% -> EXACT, `ViewArmy` 98.68% -> EXACT.

## Case 2 - a `||` predicate (`CastleHandler` 0x25230)

```
ours  (the || inlined as the call argument)   retail
--------------------------------------------- ---------------------------------------------
c7 45 c4 00000000  movl $0x0, -0x3c(%ebp)     c7 45 e0 00000000  movl $0x0, -0x20(%ebp)
eb 07              jmp <join>                 eb 07              jmp <join>
c7 45 c4 01000000  movl $0x1, -0x3c(%ebp)     c7 45 e0 01000000  movl $0x1, -0x20(%ebp)
8b 4d c4           movl -0x3c(%ebp), %ecx     8b 4d e0           movl -0x20(%ebp), %ecx
51                 pushl %ecx                 51                 pushl %ecx
```

`message` (the `__fastcall` spill) is at `-0x20` in ours and `-0x24` in retail,
so retail's `-0x20` is again the inline region. Closed with a file-static

```cpp
inline i32 CannotRecruitHero(void) {
    i32 cannot;

    if (!(gpTownManager->m_recruitResult != 0
          || gpCurPlayer->m_resources[IDX(RES_GOLD)] < gHeroGoldCost
          || gpCurPlayer->m_heroCount >= PLAYER_HERO_CAPACITY
          || gpTownManager->m_town->m_occupyingHeroId != -1))
        cannot = 0;
    else
        cannot = 1;
    return cannot;
}
```

and `gpTownManager->RecruitHero(heroChoiceIndex, CannotRecruitHero());`.
`CastleHandler` 99.94% -> EXACT.

## Diagnosis rule

Read the `this`/param spill depth first. If retail's spill is deeper than ours
by exactly the size of one value that our source materialises inline, the value
belongs to an inline callee, not to a named local and not to a temp. `?:`
spellings and inner-block declarations both fail here: the first lands after the
spill (`ternary-temp-after-this`), the second lands before every temp
(`compiler-temp-vs-named-local`, case 2).
