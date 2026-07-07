# Class inheritance — recovered from the binary (NOT CodeView)

CodeView carries **no type stream** (`sstGlobalTypes` is empty — see `docs/codeview-contents.md`),
so it has **no base-class records**. For **single** inheritance the MSVC mangling doesn't encode the
base either (only *multiple* inheritance names bases, via `??_7C@@6B<Base>@@@`; all 30 of our vtables
are single). So — like the class *fields* (recovered from disassembly via Ghidra) — the **inheritance
hierarchy must be recovered from the binary**.

## Method (`scripts/recover_inheritance.py`)

Every derived ctor calls its base ctor on `this` **before** it stores its own vtable; a member
subobject's ctor is called on `this + offset`. So disassembling each `??0Class@@` ctor and tracking
`ecx` symbolically (`ecx` enters = `this` = offset 0) at each `??0X@@` call distinguishes:

- `ecx == this+0`  → **X is a base** (base subobject at offset 0)
- `ecx == this+N>0` → **X is a member subobject** at offset N (composition, *not* inheritance)

This is decisive where the naive "first ctor call = base" heuristic gives false positives — e.g.
`hero`/`town` call `armyGroup::armyGroup` on `this+0x65`/`this+0x8` (a *member* armyGroup), and
`bankBox` stores `heroWindow`'s ctor result into `this->0x8` (a *member* pointer). Cross-check with the
`SIZE()` asserts: a derived class can't be smaller than its base (this is what caught `townObject`
0x18 vs `border` 0x2a — not a base).

## Recovered hierarchy

```
resource (abstract)   ← bitmap, font, icon, palette, sample, tileset, MIDIWrap
baseManager           ← advManager, combatManager, fileRequester, heroWindowManager,
                        highScoreManager, inputManager, mouseManager, recruitUnit,
                        resourceManager, soundManager, swapManager, townManager
widget                ← border, button, dimmerWidget, dropListWidget, iconWidget,
                        listBoxWidget, textWidget ← textEntryWidget
roots (no base): army, armyGroup, baseManager, executive, ExpCampaign, fullMap, game, hero,
        heroWindow, hexcell, philAI, resource, searchArray, town, townObject, bankBox, strip, widget
```

## Composition (member subobjects, NOT inheritance)

These *contain* an object at a fixed offset — model as a member, not a base:

| class | member | offset |
| :--- | :--- | :--- |
| `hero` | `armyGroup` | +0x65 |
| `town` | `armyGroup` | +0x8 |
| `combatManager` | `hero` | +0x328f |
| `combatManager` | `hexcell` | (in ctor) |
| `bankBox` | `heroWindow *` | +0x8 (stores ctor result) |

`heroWindow` constructs a `border` into a **local** (temporary), not at `this` — so it does *not*
derive from `border` despite the ctor calling `border::border`.
