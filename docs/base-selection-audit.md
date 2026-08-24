# Canonical base selection audit

Status: **provisional; behavior gate not passed** (2026-08-24).

## Target branches

```text
decomp-2.0      -> clean-2.0 -----------+
                       |                |
                       +-> classic-2.0  |
                           (terminal)   +-> master -> ironfist
                                        |
decomp-2.1-buka -> clean-2.1-buka -----+
                       |
                       +-> classic-2.1-buka
                           (terminal)
```

- `decomp-*`: binary-matching evidence branches.
- `clean-*`: generated source without matching machinery; inputs to `master`.
- `classic-*`: terminal readable snapshots of each retail source model; never
  merged into `master`.
- `master`: Buka structure by default, English default locale, explicit
  edition/locale/platform policies, cross-platform implementation.
- `ironfist`: downstream gameplay project.

## Decision gates

| Gate | Evidence | State |
|---|---|---|
| Buka source reproduces retail code/data | 1727/1727 functions; 291995/291995 data bytes | PASS |
| Buka final executable reproducible | strict four-section / whole-file SHA-256 audit | PASS |
| Source/tool pipeline | `homm2 selftest`: 786 tests | PASS |
| Gameplay regression suite | no automated gameplay tests in this tree | MISSING |
| Buka behavior is defect-free | confirmed retail defects below | FAIL |
| 2.0 -> Buka behavior provenance | 76 entries still `2.1?`/`unclassified` | OPEN |

Exact matching proves faithful reconstruction, not correct gameplay. It also
does not prove unused fields, private names, or behavior hidden by equal retail
constants.

## Delta inventory

Detailed ledgers:

- Logic/version behavior: `docs/version-changes.md`.
- Matching-only source transfers: `docs/cross-version-spellings.md`.
- Structural/compiler comparison: PoL
  `docs/cross-version-source-structure-report.md`.

Bullet-level provenance census of `docs/version-changes.md`:

| Class | Entries |
|---|---:|
| Buka or Buka-specific compiler/product behavior | 89 |
| Confirmed upstream Gold 2.1 | 4 |
| `2.1?` / unclassified between Gold and Buka | 76 |
| Build-only | 1 |
| Matching-only | 1 |
| Bookkeeping/non-delta | 7 |
| Total | 178 |

| Delta family | Default `master` policy |
|---|---|
| Compiler, `/Od`, `/GX`, allocation/assert state | discard as build provenance |
| Guards, loops, scopes, accessors, types | take Buka unless contradicted by edition semantics |
| Gold 2.1 gameplay fixes/features | take after behavior tests |
| PoL expansion rules/content | edition policy; do not silently erase |
| Russian text, CP1251 grammar/input | locale module; English default |
| Buka registry, OGG/CD, Audiere/Miles plumbing | platform/backend module or replace |
| Serialized layouts and save/network formats | compatibility layer; never choose by convenience |

## Buka defect and risk register

| ID | Status | Risk | Evidence / master action |
|---|---|---|---|
| BUKA-001 | confirmed retail defect | high | `CampaignHandler` writes `m_campaignMapEnabled[scenario][side]`; the declared and all other accesses are `[side][scenario]`, so later scenarios write out of bounds. Correct in `master`; preserve in `classic-2.1-buka`. |
| BUKA-002 | confirmed inherited defect | high | `font::LineWidth` never advances past `\n`; fix and test multiline input in `master`. |
| BUKA-003 | confirmed retail defect family | medium | Five flipped icon decoders discard partially clipped solid/shadow runs. Correct the clean/master implementation; retain exact shape only in decomp/classic. See `docs/icon-format.md` F1. |
| BUKA-004 | latent unsafe API | medium | `IconToBitmapScale` ignores clip arguments except at scale 32. Current callers request no clipping; enforce bounds in `master`. See `docs/icon-format.md` F3. |
| BUKA-005 | suspect regression | medium | `PlaySmacker` removed the speed-test playback but still judges `smksum`, which may describe zero or a previous playback. Classify against English Gold and add a first-run test. |
| BUKA-006 | confirmed localization defect | low | Major and Minor Scroll share the Russian `Minor Scroll` label; fix Buka locale data. |
| BUKA-007 | confirmed localization defect | low | Genie-half combat text pairs singular/plural noun and verb forms incorrectly; fix Buka locale data. |
| BUKA-008 | confirmed process-lifetime leak | low | `SetupNetworkGame2` loads `DPLAYX.DLL` without `FreeLibrary`; replace with the cross-platform capability check. |
| MODEL-001 | confirmed source-model defect | medium | `IconEntry::flags : 5` cannot represent the documented value 32. Correct after an all-includer VC6 A/B audit; exact bytes do not settle this unused field. |

## Later behavior that appears corrective

These still require edition policy and tests; they are not matching backports:

| Site | PoL behavior | Later behavior |
|---|---|---|
| `army::DamageEnemy` | tests Bless twice | second test is Curse |
| `ValidHex` | accepts 0..125 | bounds the 117-cell combat grid |
| artifact transfer | permits part of ultimate range | excludes ultimate range and Golden Goose |
| `combatManager::DoVictory` | omits Ghost capacity | omits Skeleton capacity for raised Skeleton merge |
| `SeedCombatPosition` | tests speed | tests shooter shots |
| `SetupNewRumour` | may name artifact `-1` | falls back to ordinary rumour |
| `DoEvent` obelisk access | direct metadata index | 1-based metadata minus one |

## Approval conditions for `master`

1. Classify every high-risk gameplay/save/network delta against English Gold
   2.1; localization-only and compiler-only rows may remain provenance-only.
2. Add differential tests for combat bounds, artifact transfer, necromancy,
   luck, diplomacy fee, campaign switching, AI spell valuation, and pathing.
3. Add smoke coverage for campaign switching, save/load, network setup, audio
   backend selection, video first-run, text wrapping, and clipped sprites.
4. Fix confirmed defects only after the exact clean/classic snapshots fork;
   never rewrite the retail record.

Current decision: use Buka as the **source-structure authority**, but do not
adopt Buka retail behavior wholesale as `master` until these gates pass.
