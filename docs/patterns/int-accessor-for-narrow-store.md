# `movsx` on the source of a byte-to-byte store proves an `int`-returning accessor

**Trigger.** A store of one 1-byte member into another 1-byte member where retail
sign-extends the *source* first:

```
8b 0d ..            movl   gpCurPlayer, %ecx
0f be 51 02         movsbl 0x2(%ecx), %edx        <-- widened
8b 45 fc            movl   -0x4(%ebp), %eax
88 50 17            movb   %dl, 0x17(%eax)        <-- truncated back
```

VC6 `/Od` never widens for `char = char`; it emits `mov dl,[..] / mov [..],dl`. The
`movsx` means the right-hand side is an **`int` rvalue**, and the only construct that
produces an `int` rvalue from a 1-byte member without emitting an extra instruction is a
**function call whose return type is `int`**. Casts do not do it: `(int)`, `static_cast<int>`
and `(char)(int)` all fold back to a byte move under `/Od`.

Probed under the exact unit flags (`/nologo /c /Od /MT /Gr /G5 /Ob1 /Gi- /GX /DNO_STRICT`):

```cpp
struct P { char pad[2]; signed char cur;
           signed char CurrentHero(void) { return cur; }        // narrow accessor
           int         CurrentHeroI(void) { return cur; } };    // int accessor
struct T { char pad[0x17]; char occ; };
P* gp;
void f1(T* t) { t->occ = gp->CurrentHero();       }   // narrow accessor
void f2(T* t) { t->occ = gp->cur;                 }   // member read
void f3(T* t) { t->occ = gp->CurrentHeroI();      }   // int accessor
void g1(T* t) { t->occ = (int)gp->cur;            }   // C cast
void g2(T* t) { t->occ = static_cast<int>(gp->cur); } // static_cast
```

```
f1  narrow accessor          f2  member read              f3  int accessor  (== retail)
---------------------------- ---------------------------- ----------------------------
83 ec 08 subl $0x8,%esp      51       pushl %ecx          51       pushl %ecx
a1 ..    movl gp,%eax        8b 45 fc movl this,%eax      a1 ..    movl gp,%eax
8a 48 02 movb 0x2(%eax),%cl  8b 0d .. movl gp,%ecx        0f be 48 02 movsbl 0x2(%eax),%ecx
88 4d fc movb %cl,-0x4(%ebp) 8a 51 02 movb 0x2(%ecx),%dl  8b 55 fc movl this,%edx
8b 55 f8 movl this,%edx      88 50 17 movb %dl,0x17(%eax) 88 4a 17 movb %cl,0x17(%edx)
8a 45 fc movb -0x4(%ebp),%al
88 42 17 movb %al,0x17(%edx)
```

`g1`/`g2` are byte-for-byte identical to `f2`. Only `f3` reproduces retail: source
operand evaluated first, one `movsx`, no frame temp.

## What this proves about `playerData::CurrentHero()`

`include/SOURCE/playerData.h` currently declares

```cpp
    i8 CurrentHero(void) { return m_currentHero; }
```

Retail's `advManager::TownEvent` (RVA 0x42d86) and `philAI::TownEvent`
(PHILAI object offset 0xabf8) both contain the `f3` shape for
`town->m_occupyingHeroId = <current hero>`:

```
    abf8: a1 00 00 00 00       movl   0x0, %eax        ; gpCurPlayer
    abfd: 0f be 48 02          movsbl 0x2(%eax), %ecx
    ac01: 8b 55 f8             movl   -0x8(%ebp), %edx
    ac04: 88 4a 17             movb   %cl, 0x17(%edx)
```

Changing the accessor to `i32 CurrentHero(void)` and keeping the call spelling was
measured on the whole tree: **functions-exact 1336 -> 1339, fuzzy 91.88% -> 91.93%,
no function regressed.** `advManager::TownEvent` becomes EXACT with it; without it the
best available spelling is the direct member read `gpCurPlayer->m_currentHero`, which
still costs the operand order and one `movsx`/`mov` byte per site (94.62%).

Newly EXACT with the change: `advManager::SeedTo` (0x12533), `advManager` 0xeffa,
`advManager` 0x1246b. Improved: `DoAdvCommand` 87.67 -> 90.74, `Main` 81.13 -> 82.39,
`QuickInfo` 75.93 -> 77.05, `UpdBottomViewHero` 93.81 -> 95.11, `SummonBoat` 93.12 -> 96.75,
`philAI::TownEvent` 96.19 -> 99.85, `KB` 0x6d715 52.46 -> 56.99.

The header edit is **not** applied in this branch's SOURCE/EVENTS lane: `playerData.h`
is a shared type header outside a TU lane's edit set. Apply it centrally together with
reverting `advManager::TownEvent`'s two sites from `gpCurPlayer->m_currentHero` back to
`gpCurPlayer->CurrentHero()`.
