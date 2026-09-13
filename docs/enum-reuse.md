# Enum-value reuse review

This is a decomp change based on PR #59 at
`36ace4cae41910ac21388d6b50042a84855f667e`, following the value-to-members
approach used in [King's Field PR #3](https://github.com/sushi-shi/kings-field-decomp/pull/3).
Equal integers identify candidates; the owning field, index space, message
protocol, or execution path determines whether they are the same constant.

## Inventory and replay

Inside `nix develop .#build`, after `homm2 clangd` has prepared the compilation
database:

```sh
homm2 audit enums --duplicates
homm2 audit enums --value 0x200
homm2 audit enums --json > build/enums.json
homm2 audit enums --strict --tu SOURCE/ADVMGR --json > build/enums-strict.json
```

The tool uses the existing compilation database, target ABI, localization
support, and libclang. It evaluates aliases, expressions, implicit increments,
and signed/unsigned values rather than parsing integer spellings. Repeated
header declarations are deduplicated by source site and value while retaining
their TU contexts. JSON includes the `by_value` map, member locations and
source enum owners, including domain names erased by the retail H2 macros.
It never rewrites source automatically.

The starting retail census contains **7,882 members, 652 values, and 705 source
enum blocks**, across 96 C++ TUs (the other two build units are assembly).
The lexical inventory also detects inactive or unreferenced enum blocks. The
only block absent from the retail view is the strict-only `CursorSampleSet`
in ADVMGR; a strict scan evaluates its two members as 0 and 2. SDK declarations
and `Ints.h` compatibility machinery are outside the game-enum inventory.
SDK dialect diagnostics and the existing reviewed project parse exception are
reported explicitly; unreviewed project errors and fatal errors fail the scan.

## Consolidated domains

The [replacement ledger](enum-reuse.tsv) records 221 removed declarations,
their evaluated values, canonical owners and reasons. Five execution-owned
spell coefficients move into the shared spell header, for a **net reduction
of 216 declarations**. This is a focused semantic consolidation from a
whole-tree census, not a claim that every remaining equal-value group has
received a final semantic verdict.

| Domain | Evidence and retained representation |
| --- | --- |
| Message types, widget commands, executive commands | Per-screen aliases already belong to the same payload enum and pass unchanged between construction and dispatch. Use the shared command names directly. In particular, misleading local enable/disable or hover/click names no longer conceal the actual command sent. |
| Monster attributes | Recovered `m_monster.attributes` is the common owner. Width/flying/shooting/breath/dead/retaliation/undead/summoned aliases use the existing common flag names. Context-dependent status interpretations remain named separately. |
| Army and player capacities | Campaign/event/runtime groups, AI loops, dialogs, and serialization access the same five army slots or six player entries. Use the existing owner constants. The 20 usable combat slots remain distinct from the 21 physical storage slots. |
| Artifact and hex domains | The artifact UI and AI traverse the same 14 hero slots; spell/army/AI adjacency traverses the same six neighboring directions. These are not conflated with unrelated arrays of equal length. |
| Calendar | Campaign scoring, event scheduling and new-game code share the seven-day week and 28-day month. |
| Adventure and transport storage | View-cell extents/center and resource-array aliases use their existing owners; save transfer uses the shared filename and packet capacities. No viewport coordinate or transport behavior changes. |
| Spell execution and AI | AI reads the same per-power coefficients as the execution routines. Cure, Holy Word/Shout and Death Ripple/Wave acquire shared spell-header owners. Cold Ring and both fire spells use the coefficient of their shared `Fireball()` executor. Meteor scoring now names the Meteor coefficient rather than the numerically equal Elemental Storm coefficient. |

## Equal values deliberately not merged

- `CREATURE_SUMMONED_NONE` is a sentinel in the summon-state field, not an
  instruction to summon the numerically equal Peasant creature.
- Hero visit bits have different meanings for weekly and permanent-site
  consumers. Building slots likewise depend on faction/overlay context.
- `MONSTER_FLAGS_DEAD`, AI exclusion and rendering interpretations share bits
  but their contextual predicates remain explicit. The wake/bad-morale/full-AI
  quantity interpretations are not renamed as if they were interchangeable.
- `CombatSide`, `CombatResult` and `ArmyFacing` are distinct contracts: a result
  includes pending/draw, and attacker-facing direction is numerically reversed.
- Resource IDs, creature IDs, spell IDs, UI control IDs and artwork-frame IDs
  are separate index spaces even where their small numbers coincide.
- Equal individual spell damage coefficients do not make different spells
  one policy. Lightning, Meteor and Elemental Storm still have distinct names.
- Allocation/source-line evidence, packed payload sizes, independent screen
  dimensions, timeouts, campaign rewards and imported algorithms are not
  deduplicated merely because they share a number.

## Verification

Before editing, rebuild the starting revision and preserve its raw objects and
census. When changing source-owned data claims between branches, regenerate
the target with `homm2 redelink --force` before quoting match totals.

```sh
homm2 audit enums --json > build/enum-before.json
# Apply the reviewed source changes.
homm2 build
homm2 audit enums --verify-before build/enum-before.json --json > build/enum-after.json
python3 -m homm2.audit.object_equivalence build/enum-before-objects build/objdiff/base
homm2 audit enums --strict --json > build/enum-after-strict.json
homm2 selftest
homm2 build --no-match --en
```

The ledger verifier accounts for every starting and current active member,
including untouched members: aliases must disappear, canonical values must
agree, and an unexplained addition/deletion/value change fails. A partial TU
census cannot be used as a whole-tree reuse verification.

Validation of this batch:

- Russian matching build: 1,727/1,727 functions and 291,987/291,987 data bytes.
- All 98 raw objects preserve non-debug section bytes/layout, named symbols
  and all 42,433 ordered relocations against the freshly built starting source.
- The retail census has 7,666 remaining members; the ledger verifier accounts
  for every starting/current value, including all untouched declarations.
- The strict census parses all 96 C++ TUs without unreviewed project errors.
  Its 7,665 active members cover the checker-only cursor enum; conversely,
  `MapTriggerEncodingConstant` is retail-only. Four existing narrow signed
  enum values evaluate differently in the strict view (128/-128 and 255/-1);
  the tool preserves those context differences rather than merging them.
- The complete tool suite passes 988 tests, with seven existing skips.
- English ordinary build compiles and links; this is not gameplay validation.

The standalone `homm2.build.clang_cxx11` command fails in the pinned VC6 SDK's
pre-standard templates (95/96 TUs). Its result is not reported as a passing
modern compiler check. The enum scanner's strict view instead uses the existing
analysis configuration, exposes tolerated SDK diagnostics, and rejects
unreviewed project errors.

The retained source preserves SIZE checks, layout/storage widths, locale
logic, and retail behavior. Generated source/classic and portable descendants
are not edited independently in this change; propagation follows review and
integration into decomp.
