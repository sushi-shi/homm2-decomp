# Buka helper and localization propagation

The upstream chain is merged decomp #57 (`3c3bc7f44`) and #58 (`e744880d8`),
then generated source (`77247c79f`) and Russian UTF-8 classic (`90d674be9`).
The portable helper patch is cherry-picked from the intermediate, generated
#57 source tree (`f37f21a7e`), separating shared C++ changes from Buka's static
localization compiler and VC6 build tools.

## Preserved portable behavior

- Shared widget/message operations, domain predicates, formulas, accessors,
  default arguments, drawing wrappers, and allocation/copy helpers propagate.
- Map-cell helpers use the port's packed-tileset accessors. Combat deadlines use
  `platform::Ticks()`. Existing layout assertions remain intact.
- Runtime UTF-8 formatting, plural messages, Unicode case conversion, and text
  buffer bounds remain unchanged. No Russian byte literals replace runtime IDs.
- Checked map/save/resource I/O remains checked. The legacy raw `read`/`write`
  macros do not replace those APIs.
- SDL coordinate conversion, portable networking, video handling, configuration
  files, and tick-wrap-safe control flow remain in their platform owners.
  Removed Win32 transport modules and their queue macros are not resurrected.
- The portable CMake/Nix build remains portable; source-branch VC6/Windows-only
  build scripts and locale compiler mirrors do not propagate into it.

## English catalog reconciliation

All 2,404 shared English IDs match #58 exactly: 423 direct messages and 1,981
legacy-table entries. No English correction remains to apply to those IDs.
The other 91 Buka IDs represent branch-specific fragments/signatures; importing
them over the port's existing whole-message/plural interfaces would change the
runtime contract. The existing English registry, table initializers, and PO
catalogs are therefore retained.

## Verification

Master's native Debug build and all 22 CTest checks pass. `common_helpers` covers
creature classifications, clip and widget boundaries, ordered geometry/message
writes, and packed-map-cell shadow/object semantics. Existing source-policy,
localization, map layout, binary I/O, platform, and presentation checks remain
enabled. No gameplay/GUI validation is claimed by this propagation.

The helper commit is carried onward by cherry-pick through `ironfist` and
`ironfist-master`, preserving each branch's extension behavior.

Ironfist's resolved cherry-pick passes its native Debug build and all 21 CTest
checks, including the existing Ironfist hook contract. Its extended creature-table
capacities, combat effects, hooks, and owned control flow are preserved; only
matching helper expressions and calls are substituted inside those functions.
Its dialog/sound wrappers and widget-message constructor use the same shared
helpers and default arguments.
