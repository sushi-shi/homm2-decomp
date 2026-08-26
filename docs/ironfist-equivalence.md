# Ironfist equivalence audit

The portable `ironfist` branch is a port of Project Ironfist's game changes,
not a byte-identical rebuild of an Ironfist Windows executable. It keeps the
recovered Heroes II Gold 2.1 engine and expresses Ironfist's changes through
recovered fields, full function integrations, and narrow hooks.

This distinction matters because Project Ironfist has two different useful
reference points:

1. The shipped `ironfist-install-1.3.0.exe` contains an `ironfist.exe` built on
   2019-01-01. Its best repository-correlated source snapshot is
   `7afcf919e27cd7aeb6257a9ca7e6ad2642c860e3`: that commit changes
   `OnBattleMeleeAttackComplete` and rebuilds the installer together. The
   installer blob is subsequently renamed without a source change. This is
   strong correlation, but it is not cryptographic proof that every source
   file in that commit produced the executable.
2. This branch's declared reference is the later upstream commit
   `314932011ed5308efb9f35cecc62e8ca638a7375` from 2024-11-28. It contains
   substantial work added after the shipped 1.3.0 executable.

Therefore, “exact to Ironfist” must always name which target it means.

## Verdict

| Question | Result |
|---|---|
| Is this the shipped Ironfist 1.3.0 binary expressed portably? | No. Its scripting surface is a strict superset derived from later source. |
| Does it expose the complete pinned-2024 Lua surface? | Yes: all names, handler bindings, result arities, callback names, and static numeric constant values match. |
| Are the pinned-2024 game semantics fully proven equivalent? | No. All callback sites were reviewed, but the preserved Gold base and the spell-chance implementation are known differences. |
| Is the original Windows map editor ported? | No. This branch builds the game only. |
| Are the resources the shipped 1.3.0 payload? | No. They are rebuilt from the pinned-2024 upstream source. The generated aggregate matches the reviewed pinned-source packer output byte for byte. |

The branch is consequently best described as **the latest Ironfist feature set
ported onto recovered Gold 2.1**, not as a source-level reproduction of the
1.3.0 binary.

## Binary evidence

The audited repository copy of `installer/ironfist-install-1.3.0.exe` has
SHA-256
`bc0b9f471e831e6b9699b744dd31c6d0b71ccb10fe97b41d608fbccfff7575e5`.
Its extracted `ironfist.exe` has SHA-256
`8c5cef2f2a4de71dc81538a053736eb5b41064bb8c53468ab71bf1f9cf6b0ef4`
and PE timestamp 2019-01-01 08:29:28 UTC.

Every Lua registration, callback, and static constant registration requires a
NUL-terminated name in the executable. Scanning that binary gives an exact
surface match to the release-correlated source snapshot:

| Surface | 1.3.0 binary | Release-correlated source | Pinned-2024 source/current port |
|---|---:|---:|---:|
| Lua functions | 105 | 105 | 129 |
| callback names | 12 | 12 | 23 |
| static constant names | 353 | 353 | 374 |

The current port adds exactly the 24 Lua functions, 11 callbacks, and 21
static constants present in the later source. The executable scan finds none
of those names in the shipped binary.

For the 105 Lua functions shared with 1.3.0, all registration-to-handler
bindings match and all 352 numeric constant values shared with 1.3.0 match.
One observable result-arity difference remains by design: release-era
`AdvancedMessageBox` returns no Lua value, while pinned-2024 upstream and this
port return one Boolean value.

## Pinned-source interface evidence

`tools/ironfist_interface_audit.py` reads files from the exact Git object at
the requested revision, so checkout filters or unrelated working-tree changes
cannot alter its reference input. Against the declared 2024 pin it verifies:

- 129 Lua-visible function names;
- 129 implementation handler names;
- all 129 name-to-handler registrations;
- every result-count value returned by all 129 handlers;
- 23 callback names;
- 374 static constant names, including the one nil sentinel; and
- all 373 statically numeric constant values.

The static comparison does not cover the dynamic `ARTIFACT_*` names generated
from `artifacts.xml`, and matching an interface does not prove the behavior
behind it. Those limits are reported by the tool rather than hidden behind an
“exact” summary.

The Lua implementation review also found deliberate invalid-input hardening in
the port: index validation, safe stored-ID checks, a nil result for a town with
no visiting hero, spell-index validation, and bounded hero/town name copying.
These changes preserve valid-input behavior but replace upstream out-of-bounds
access or unterminated writes with defined behavior. They are not bug-for-bug
equivalent for invalid scripts.

## Callback-placement audit

The original mod either wrapped a retained assembly body with `*_orig` or
replaced the whole function. The portable port instead integrates the delta
into the recovered Gold function. Equivalence therefore depends on call order
and on which part of the host function a Boolean callback suppresses.

| Callback contract | Current status |
|---|---|
| `OnMapStart` and the first `OnNewDay` | Equivalent after the adventure manager is added for a new game. |
| recurring `OnNewDay` | Equivalent at the end of `game::PerDay`. |
| `OnMapVictory` / `OnMapLoss` | Equivalent. The recovered `CheckEndGame` performs the new-game guard before its integrated hook, whereas upstream needed the guard in its post-body wrapper. |
| `OnHeroMove` | Equivalent after the normal `MoveHero` body. |
| `OnLocationVisit` | Equivalent after correction: a true result skips the location handler but retains redraw, ambient-audio, and end-game processing. |
| `OnCastleConquered` | Equivalent after every normal `ClaimTown` exit. |
| `OnArtifactGive` / `OnArtifactTake` | Equivalent after the artifact-stat operation, including its no-artifact path. |
| `OnTownOpen` | Equivalent after the normal open/fade, followed by Ironfist's post-callback town-music refresh. |
| `OnUnitRecruit` | Equivalent before construction of the recruit window. |
| `OnCalcMobility` | Equivalent after each underlying mobility path, with the same minimum of one. |
| `OnCalcLuck` / `OnCalcMorale` | Equivalent after the base calculation, with the same post-callback clamp to `[-3, 3]`. |
| `OnCalcManaCost` | Equivalent after the base mana-cost calculation. |
| `OnCalcSpellChance` | Callback invocation and `[0, 1]` clamp are equivalent; its input is not fully equivalent because the underlying chance calculation differs as described below. |
| `OnBattleMeleeAttack` / `OnBattleMeleeAttackComplete` | Equivalent, including the self target for an invalid initial target, the null Hydra completion target, and no completion callback for retaliation. |
| `OnMonsterInteract` | Equivalent for both human and computer monster interactions. |
| `GetTooltipText` | Equivalent after correction: it runs before visibility/default/artifact handling and a nonempty result owns the tooltip. |
| `OnShowLuckInfo` / `OnShowMoraleInfo` | Equivalent after correction: script text is appended before deciding whether to add `none`. |
| `OnBattleStart` | Equivalent after normal combat initialization and before Pandora Box processing. |

`tools/test_ironfist_hook_contract.py` protects the integration-sensitive order
for map reset, location skipping, tooltip override, morale text, town music,
and the integrated end-game guard.

## Corrections found by this audit

The audit found several real errors introduced by the third, integration-hook
style rather than by SDL or by the recovered Gold code:

- New-map state was reset after loading the map script, erasing top-level
  vision, chase, and building-ban changes. It is now reset before script load;
  AI army sharing is restored after the base map body at the same point as
  upstream.
- A true `OnLocationVisit` result returned from `DoEvent`, skipping the normal
  redraw/audio/end-game tail. It now skips only the event handler.
- `GetTooltipText` ran after base tooltip construction and could not override
  artifact quick views. It now runs before visibility and artifact fallback,
  and a nonempty result follows the upstream custom-tooltip path.
- `OnShowMoraleInfo` ran after the `none` fallback. It now contributes text
  before that decision.
- `OnTownOpen` omitted the upstream post-callback music refresh.
- `SetTownName` returned no value rather than the assigned name.
- `LOCATION_SHIPYARD`, which upstream defines internally but does not register
  in Lua, was incorrectly exported by the port and has been removed.

## Known semantic differences

### Gold 2.1 versus PoL 2.0

Ironfist replaces functions in a PoL-derived executable. This branch retains
recovered Gold 2.1 bodies and applies the Ironfist-authored delta to them.
Version-specific bug fixes, data tables, expansion behavior, and ordinary
Heroes II control flow therefore remain Gold behavior unless Ironfist
explicitly changes them. Matching Ironfist's authored delta does not make the
entire game binary behavior-identical.

### Spell chance

Pinned upstream fully replaces `army::SpellCastWorkChance`. Its helper checks
whether a creature is both a Dwarf and a Battle Dwarf simultaneously, making
the intended 75% resistance unreachable. The function also contains several
unparenthesized `&&`/`||` conditions whose compiled behavior follows C++
precedence rather than the apparent indentation.

The portable branch retains the recovered Gold spell eligibility logic,
extends it for Ironfist spells, and uses the normal Dwarf-or-Battle-Dwarf test.
Consequently `OnCalcSpellChance` may receive a different initial value. Copying
the upstream condition would improve bug-for-bug identity while reintroducing
an apparent upstream defect; this remains an explicit policy decision rather
than an unnoticed mismatch.

### Platform substitutions

Registry preferences, Win32 dialog reporting, CD audio, middleware playback,
temporary script files, and fixed Windows paths are implemented through the
portable platform layer. These substitutions target the same game-visible
result, not Windows API call identity. The Windows-only editor is outside the
branch's scope.

## Resource evidence

The resource payload is built from the exact 2024 source pin. A fresh
`nix build .#ironfist-resource-payload` produces 32 files. Its generated
`DATA/ironfist.agg` is 8,536,086 bytes with SHA-256
`2952e91a5d6c38216d8c805346f4941e7527ff31ba05c0d7a1161e90f56a5599`,
the reviewed byte-for-byte upstream-packer result enforced by the builder.
The remaining XML, map, campaign, Lua, and music files are copied from the
pinned source with only the documented Cyborg music filename adaptation.

## Reproducing the audit

Against the declared latest-source target:

```sh
python3 tools/ironfist_interface_audit.py \
  --upstream /path/to/project-ironfist
```

Against the shipped 1.3.0 surface after extracting `ironfist.exe` from the
installer:

```sh
python3 tools/ironfist_interface_audit.py \
  --upstream /path/to/project-ironfist \
  --revision 7afcf919e27cd7aeb6257a9ca7e6ad2642c860e3 \
  --binary /path/to/extracted/ironfist.exe
```

The second command intentionally reports the later port's additional API and
the release-era `AdvancedMessageBox` result difference while proving that the
binary contains all names expected by its correlated source snapshot.

Build and regression checks:

```sh
nix develop -c cmake --build build
nix develop -c ctest --test-dir build --output-on-failure
nix build .#ironfist-resource-payload
```
