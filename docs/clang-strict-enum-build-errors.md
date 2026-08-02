# Clang strict-enum build audit

## Build modes

The repository has two explicit Clang modes:

- `ClangMode.RETAIL_ANALYSIS` uses C++98, the closest Clang language mode to the
  pre-standard retail MSVC compiler. `VA`, `DATA`, compiler-generated-data, and
  matching AST tools use this view so their types and mangled names describe the
  retail candidate objects.
- `ClangMode.STRICT` uses C++20. Clangd and the whole-tree Clang check use this
  view so reconstructed enum domains are strongly typed.

`H2_STRICT_ENUMS` is derived from the language mode. The former
`-DHOMM2_STRICT_ENUM_TYPES` switch is no longer used. This preserves the retail
MSVC ABI while giving modern builds typed parameters, returns, fields, locals,
and mangled names.

Run the strict check with:

```sh
nix develop .#build --command homm2 clangd
nix develop .#build --command \
  python -m homm2.build.clang_cxx11 --errors-only
```

The final audit checks 94 translation units. `BASE/TILE.cpp` remains excluded
because its MSVC naked inline assembly cannot be parsed by Clang. The final
result is 94 units, zero failed units, and 1,416 non-fatal warnings.

## Original 41 hard errors

Locations are the coordinates reported by the exhaustive audit before repairs.
Repeated locations represent distinct diagnostics.

| Area | Locations | Errors | Cause and type-safe repair |
|---|---|---:|---|
| Sample format flags | `src/BASE/SAMPLE.cpp:59` | 1 | `SampleAudioFormat` is a flag domain. Combine flags within that domain; the strict form uses `\|=`, while retail retains the matched `+=` spelling. |
| Widget flags | `src/BASE/WIDGET.cpp:101`, `:103`, `:139`, `:140`, `:142` | 5 | A raw message payload was mixed with `WidgetFlag`. Convert once at the message boundary and keep subsequent operations in the flag domain while retaining the proven retail storage expression. |
| Cursor sample set | `src/SOURCE/ADVMGR.cpp:1222`, `:10168` | 2 | `ConfigWalkSpeed` and the cursor sample-set choice were conflated with `i32`. Model the sample-set choice as its own enum and convert from walk speed explicitly. |
| Opera-mode condition | `src/SOURCE/ADVMGR.cpp:1269` twice | 2 | A scoped `ConfigOperaMode` was used as a boolean. Compare it with `CONFIG_OPERA_DISABLED`. |
| Army swap | `src/SOURCE/ARMYGRP.cpp:228` | 1 | A `CreatureType` was routed through an untyped temporary. Use a same-type swap whose strict temporary type is deduced while retail retains the matched swap shape. |
| Combat-side indexing | `src/SOURCE/CMBTMGR.cpp:1147`, `:1179`, `:1190` | 3 | `CombatSide` was used as a raw array index. Keep the loop variable typed and use `IDX` only at the array boundary. |
| Packed site decoding | `src/SOURCE/EVENTS.cpp:4029`, `:4324`, `:7152`, `:7252`; `src/SOURCE/PHILAI.cpp:6150` | 5 | Encoded metadata was masked after being treated as an enum. Decode the masked raw field into `GenericSiteType` or `RecruitSiteType`; retail retains the original assignment followed by `&=`. |
| Sentinel comparisons | `src/SOURCE/GAME.cpp:5618`; `src/SOURCE/HERO.cpp:957`; `src/SOURCE/REQUEST.cpp:655`; `src/SOURCE/SPELLS.cpp:710`, `:733` | 5 | Typed values were compared with integer zero. Add named `*_VALID_BEGIN` domain boundaries and compare within the domain. |
| Hero event flags | `src/SOURCE/HERO.cpp:712`, `:716`, `:720`, `:724`, `:728`, `:732`, `:736`, `:740`, `:744`, `:748`, `:752`, `:756` | 12 | Flags were removed by integer subtraction. Use a same-domain clear operation; strict mode uses bit removal and retail retains the matched guarded subtraction. |
| Effective secondary-skill level | `src/SOURCE/HERO.cpp:2042`, `:2043` | 2 | `HeroSkillLevel` ends at 3, but necromancy bonuses can produce an effective value up to 9. Keep the stored level typed and make the computed quantity numeric. |
| Overview frame index | `src/SOURCE/Overview.cpp:342` | 1 | `OverviewType` was added to an unrelated layout enum. Convert the domain value at the icon-frame boundary. |
| Overview skill selection | `src/SOURCE/Overview.cpp:1520`, `:1521` | 2 | `HeroSecondarySkill` was erased to `i32` and immediately passed back to a typed API. Retain the returned domain in strict mode and the shared retail local in MSVC mode. |
| **Total** |  | **41** |  |

## Current-master integration errors

Current `master` added 30 further hard errors while the audit branch was in
progress:

| Area | Locations | Errors | Cause and accepted repair |
|---|---|---:|---|
| Resource sentinel | `src/SOURCE/ADVMGR.cpp:5898`, `:5925` | 2 | `ResourceType` storage was compared with integer zero. Add `RES_VALID_BEGIN` and compare within the resource domain. |
| Hero portrait frame | `src/SOURCE/Overview.cpp:460` | 1 | A ternary selected either a numeric town frame or typed `HeroPortrait`, then passed the result to the generic `i16` frame parameter. Use `IDX` only on the portrait branch at that UI boundary. |
| Town building widget cases | `src/SOURCE/TOWNMGR.cpp:1293`–`:1304`, `:1324`, `:1416`, `:1499`, `:1514`, `:1530`, `:1564`, `:1593`, `:1670`, `:1688`–`:1694` | 27 | Raw widget IDs were switched against `BuildingSlotType` constants. Switch once to the private `TownManagerWidgetId` domain and use its building-widget aliases. This avoids the `case IDX(...)` antipattern and documents the deliberate numeric mapping between building slots and UI controls. |
| **Total** |  | **30** |  |

The exhaustive archived policy also promoted one existing warning at
`src/SOURCE/Overview.cpp:626`: arithmetic combines
`OverviewDwellingIndex` with `OverviewTownRowLayout`. It is not a hard error in
the active Clang build. A future repair can convert the dwelling index at the
frame-layout boundary with `IDX`, subject to the normal byte and relocation
check.

## Safety notes

Strict casts do not validate raw runtime input. In particular,
`message.payload.widget.data.value` can still contain unknown `WidgetFlag` bits
or narrow when stored as `i16`. This is pre-existing retail behavior. A future
behavioral change should define the complete valid mask and an invalid-message
policy; this audit intentionally does not invent one.

The repairs do not add unconstrained enum/integer operators, implicit
enum-to-integer conversion, or cross-domain comparisons. Packed widths and the
retail ABI remain unchanged.

## Matching and build verification

The final isolated-worktree verification completed:

- clean compilation and normalization of all 95 MSVC units;
- all delink manifests and source-private symbol checks;
- 1,499 functions and 37,090 ordered relocation sites in the relocation-field
  audit, with zero structural review items;
- the final `HMM2PL.exe` link and link audit;
- the strict hook: 94 units and zero failures.

Focused current-master results:

| Function | Live match | Ordered relocations |
|---|---:|---:|
| `advManager::UpdBottomViewResMsg` | 100% | 44/44 |
| `game::SetupDynamicStuff` | 100% | 340/340 |
| `game::ProcessIconSelect` | 100% | 55/55 |
| `townManager::Main` | 99.44677% (unchanged) | 215/215; existing unmatched relocation identities remain |
| `combatManager::CastSpell` | 99.585434% (unchanged) | 270/270; existing unmatched relocation identities remain |

The repository hook regenerates clangd configuration and runs the strict mode
before each commit. See `.githooks/README.md` for worktree-local activation.
