# BASE `@early-stop` proof ledger

This ledger audits every current `@early-stop` marker under `src/BASE` from base checkpoint
`775f0d3`, plus the accepted WINMGR/shared-member state identified in the audit snapshot.
It is deliberately stricter than the prose beside the functions: a score or a claim that
instructions are "the same" is not proof.  The audit joined the current
`build/objdiff/report.json` (refreshed with `homm2 status`) to
`config/match_baseline.tsv`, then compared the function ranges directly in the candidate and
delinked COFF objects.  `Δ` below is the number of differing bytes after masking the union of
the two ranges' four-byte relocation payloads; `rel B/R` is the candidate/retail relocation-site
count.  A zero `Δ` is necessary but is certified as an artifact only after the external targets
and any local-table ranges are also accounted for.

The accepted categories are the two from `.claude/agents/matcher.md`:

- `proven-artifact`: raw code is exact after the documented relocation/local-table masking, and
  the residual is only delinker, relocation-name, COMDAT-alias, or empty-stub identity.
- `proven-soft-TU-cumulative`: logic, CFG, frame, every visible stack displacement, and
  relocations are exact; only an unsteerable commutative operand order remains after the full
  combined-root/predecessor checklist.
- `unresolved/not-a-wall`: anything else.  In particular, `/O2` register allocation,
  scheduling, predicates, intrinsics, SIB encodings, or a small score residual are not permanent
  walls.

No BASE marker presently meets the soft-TU-cumulative checklist.

## Audit snapshot

The counts and rows below are pinned to the following fully rebuilt state:

- Base Git checkpoint: `775f0d3`
- Accepted `src/BASE/WINMGR.cpp` SHA-256:
  `255975bbfae92f01fe9eb603abdeaa48996f937759f7bfb7678efee46ae25df5`
- Accepted `include/BASE/heroWindowManager.h` SHA-256:
  `68a628f236fb7603240d0b766df838fcd845e3318391c632473521675ee0ea61`
- Accepted `src/SOURCE/GAME.cpp` SHA-256:
  `35db1e6f05dae168c7315b4e6639429fa9fdc79fad963cfa2e6ddbbae7ed95f7`
- Accepted `src/SOURCE/SPELLS.cpp` SHA-256:
  `5c611ade00f62636c124da6741638e0286278dc8a388f477f5d219e11c3f0563`
- `build/objdiff/report.json` SHA-256:
  `6a346956e9f8535010d2249d21dc16eac958bb97f0c6640177894934d387524b`
- `config/match_baseline.tsv` SHA-256:
  `2c4db5795133477413bd2c7c50c6bf4985b0ba8b8de67ecbfddffe54613a7abb`
- Sorted `rg -n '@early-stop' src/BASE` inventory SHA-256:
  `2d27f20638aadd1a9f9d313c18ad8ecf8578bd686adfdddb079058d770064b5e`

Recompute all listed identities before reusing classifications after a root/header/source change.

## Summary

| Category | Marker count | Notes |
| :--- | ---: | :--- |
| `proven-artifact` | 9 | Seven ordinary function markers and two generated destructor aliases. |
| `proven-soft-TU-cumulative` | 0 | No marker has the required frame/slot, standalone, combined-root, and exact-predecessor proof bundle. |
| `unresolved/not-a-wall` | 60 | Includes five unproved deleting-destructor tradeoffs and two stale markers on exact functions. |
| **Total** | **69** | Exactly 62 ordinary rows plus seven generated-alias rows: one row per current source marker. |

Syntax/state defects found by the audit:

- [`BITS.cpp:11`](../src/BASE/BITS.cpp#L11) uses invalid `// @early-stop ~44%:` syntax.  The
  marker line must be exactly `// @early-stop`, with no percentage.
- [`CheckChangeCursor`](../src/BASE/INPUTMGR.cpp#L452) and
  [`resourceManager::Close`](../src/BASE/RESMGR.cpp#L292) are live/retained 100% with `Δ=0`;
  their markers are stale and should be removed with the next source-owning change.
- `dd7973b` removed six now-exact sound-manager markers.  `487dc58` also made all three PALETTE
  functions and both TILESET functions exact; neither TU has a current marker, so none appears in
  the row ledger.  Rows are never retained merely to record a historical marker.
- `Textntry::Draw` is not raw-exact: candidate `[esp+esi+0x0e]` differs from retail
  `[esp+esi+0x0f]` at `+0x6d`.  `resourceManager::resourceManager` is also not a literal-only
  residual: candidate adds `0x14` while retail adds `0x62` at `+0x56`.  These examples are why
  marker prose is not accepted as proof.

## Retry codes

- **A1 — artifact revalidation only:** retry only if the function bytes, owning TU/header hash,
  delinker/report behavior, or relocation map changes.  Re-run the explicit-range raw and
  relocation audit; do not search predicate synonyms.
- **R0 — stale marker:** confirm the refreshed exact row and remove the marker.  No matching
  search is required.
- **R1 — structural work now:** start at the first raw divergence, reconstruct types/layouts,
  bodies, CFG, frames and real fields, then rebuild the TU in source order.  A low score or many
  differing bytes is not last-mile work.
- **R2 — aligned last mile:** first repair any real frame/slot/CFG/relocation mismatch shown by
  the raw audit.  Once structurally aligned and roughly 96--99%, use audited local source shapes,
  then exact-preserving predecessor/header/TU-state variants as documented in
  [`o2-tu-cumulative-register-steering.md`](patterns/o2-tu-cumulative-register-steering.md).
  The libclang AST permuter is allowed only in that range; never use the regex permuter.
- **R3 — modified vendor source:** defer Bzip grinding while higher-value BASE work remains.
  Retry from the closest known upstream Bzip source plus the developers' surrounding headers and
  modifications, and re-audit raw bytes.  The current comments do not convert real byte
  differences into delinker artifacts.
- **R4 — generated deleting alias:** retry after a destructor/body, class layout, COMDAT, or
  compiler-emission change.  Compare the emitted `??_G` body separately against each retail
  `??_E`/`??_G` copy; an exact standalone `??1` alone is not proof.

## Ordinary function markers

Unless a row explicitly says otherwise, the current audit did not separately prove frame/stack
slots.  The linked source note is retained only as tried-family history, not as proof.  Retained
and live scores are shown as `retained/live`.

| TU | RVA / function | Score | Classification | Current byte/reloc evidence | Tried record | Retry |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| `BITS` | `0x004d1594` `BitTest` | 47.6316/47.6316 | unresolved/not-a-wall | Δ=44; rel 0/0 | [source note](../src/BASE/BITS.cpp#L11) | R1 |
| `BORDER` | `0x004d22f0` `border::Main` | 99.3621/99.3621 | unresolved/not-a-wall | Δ=2 at `+0x7e,+0x81`; rel 9/9 | [source note](../src/BASE/BORDER.cpp#L82) | R2 |
| `BUTTON` | `0x004dd6d0` `button::Main` | 94.3705/94.3705 | unresolved/not-a-wall | Δ=823; rel 36/36 | [source note](../src/BASE/BUTTON.cpp#L95) | R1 |
| `BUTTON` | `0x004ddc70` `button::Select` | 89.0652/89.0652 | unresolved/not-a-wall | Δ=105; rel 6/6 | [source note](../src/BASE/BUTTON.cpp#L258) | R1 |
| `BUTTON` | `0x004ddd10` `button::Deselect` | 99.7778/99.7778 | unresolved/not-a-wall | Δ=2; rel 4/4 | [source note](../src/BASE/BUTTON.cpp#L286) | R2 |
| `Blur` | `0x004d28e0` `DoBlur` | 93.9030/93.9030 | unresolved/not-a-wall | Δ=1046; rel 43/43 | [source note](../src/BASE/Blur.cpp#L26) | R1 |
| `Bzip` | `0x004d4470` `minUInt32` | 100.0000/99.8095 | unresolved/not-a-wall | Δ=4; rel 0/0 | [source note](../src/BASE/Bzip.cpp#L238) | R3 |
| `Bzip` | `0x004d47a0` `arithDecodeSymbol` | 99.9730/99.9730 | unresolved/not-a-wall | Δ=2; rel 11/11 | [source note](../src/BASE/Bzip.cpp#L338) | R3 |
| `Bzip` | `0x004d4e90` `getMTFVal` | 99.3478/99.3478 | proven-artifact | Δ=0 over 0x153 bytes; rel 28/28; external calls agree, model addends and 12 table labels are delinked aliases | [source note](../src/BASE/Bzip.cpp#L528) | A1 |
| `Bzip` | `0x004d5930` `moveToFrontCodeAndSend` | 99.6452/99.6344 | unresolved/not-a-wall | Δ=228; rel 10/13 | [source note](../src/BASE/Bzip.cpp#L786) | R3 |
| `Bzip` | `0x004d5ac0` `getAndMoveToFrontDecode` | 99.6340/99.6340 | unresolved/not-a-wall | Δ=478; rel 18/28 | [source note](../src/BASE/Bzip.cpp#L835) | R3 |
| `Bzip` | `0x004d6000` `qsortFull` | 99.5181/99.5181 | unresolved/not-a-wall | Δ=827; rel 44/44 | [source note](../src/BASE/Bzip.cpp#L979) | R3 |
| `Bzip` | `0x004d6610` `shellTrivial` | 99.5556/99.5556 | unresolved/not-a-wall | Δ=36; rel 7/7 | [source note](../src/BASE/Bzip.cpp#L1064) | R3 |
| `Bzip` | `0x004d6f40` `getRLEpair` | 99.8913/99.8913 | unresolved/not-a-wall | Δ=10; rel 18/18 | [source note](../src/BASE/Bzip.cpp#L1275) | R3 |
| `Bzip` | `0x004d7290` `unRLEandDump` | 99.7273/99.7273 | unresolved/not-a-wall | Δ=30; rel 15/15 | [source note](../src/BASE/Bzip.cpp#L1357) | R3 |
| `Bzip` | `0x004d7420` `compressStream` | 99.9011/99.9011 | unresolved/not-a-wall | Δ=18; rel 79/79 | [source note](../src/BASE/Bzip.cpp#L1407) | R3 |
| `Bzip` | `0x004d7710` `uncompressStream` | 99.7484/99.7484 | unresolved/not-a-wall | Δ=39; rel 56/56 | [source note](../src/BASE/Bzip.cpp#L1476) | R3 |
| `Bzip` | `0x004d7cd0` `endsInBz` | 99.9524/99.9524 | unresolved/not-a-wall | Δ=2; rel 0/0 | [source note](../src/BASE/Bzip.cpp#L1694) | R3 |
| `Bzip` | `0x004d7d60` `compress` | 99.9178/99.9178 | unresolved/not-a-wall | Δ=6; rel 16/16 | [source note](../src/BASE/Bzip.cpp#L1708) | R3 |
| `Bzip` | `0x004d7e50` `uncompress` | 99.9405/99.9405 | unresolved/not-a-wall | Δ=5; rel 19/19 | [source note](../src/BASE/Bzip.cpp#L1733) | R3 |
| `Bzip` | `0x004d7f60` `EncodeData` | 99.9266/99.9266 | unresolved/not-a-wall | Δ=13; rel 28/28 | [source note](../src/BASE/Bzip.cpp#L1762) | R3 |
| `Bzip` | `0x004d8240` `DecodeData` | 99.9293/99.9293 | unresolved/not-a-wall | Δ=13; rel 26/26 | [source note](../src/BASE/Bzip.cpp#L1807) | R3 |
| `EXEC` | `0x004d1750` `executive::DoDialog` | 99.0000/99.0000 | unresolved/not-a-wall | Δ=12; rel 16/16 | [source note](../src/BASE/EXEC.cpp#L66) | R2 |
| `EXEC` | `0x004d1a90` `executive::MainLoop` | 99.8851/99.8851 | unresolved/not-a-wall | Δ=1 at `+0x7d`; rel 7/7 | [source note](../src/BASE/EXEC.cpp#L221) | R2 |
| `ICON` | `0x004c7b50` `icon::CombatClipDrawToBuffer` | 97.2049/97.2049 | unresolved/not-a-wall | Δ=395; rel 37/37 | [source note](../src/BASE/ICON.cpp#L60) | R2 |
| `ICONWDGT` | `0x004d0ad0` `iconWidget::iconWidget(id)` | 97.9444/97.9444 | unresolved/not-a-wall | Δ=18; rel 4/4 | [source note](../src/BASE/ICONWDGT.cpp#L32) | R2 |
| `ICONWDGT` | `0x004d0b40` `iconWidget::iconWidget(name)` | 98.1500/98.1500 | unresolved/not-a-wall | Δ=18; rel 6/6 | [source note](../src/BASE/ICONWDGT.cpp#L53) | R2 |
| `ICONWDGT` | `0x004d0cd0` `iconWidget::Main` | 98.7453/98.7453 | proven-artifact | Δ=0 over 0x291 bytes; rel 17/17; 12 external sites agree, five local dispatch/table labels differ only in owner identity | [source note](../src/BASE/ICONWDGT.cpp#L100) | A1 |
| `ICONWDGT` | `0x004d0f70` `iconWidget::Draw` | 97.8723/97.8723 | unresolved/not-a-wall | Δ=6; rel 4/4 | [source note](../src/BASE/ICONWDGT.cpp#L194) | R2 |
| `INPUTMGR` | `0x004cdb50` `KeyboardMessageHandler` | 99.7166/99.7166 | proven-artifact | Δ=0 over 0x308 bytes; rel 40/40; external sites agree, local tables and `gConfig+0x30` are delinked-owner aliases | [source note](../src/BASE/INPUTMGR.cpp#L19) | A1 |
| `INPUTMGR` | `0x004cde60` `MouseMessageHandler` | 99.4068/99.4068 | unresolved/not-a-wall | Δ=67; rel 59/55 | [source note](../src/BASE/INPUTMGR.cpp#L104) | R2 |
| `INPUTMGR` | `0x004ce480` `inputManager::AsciiConvert` | 98.7616/98.7616 | unresolved/not-a-wall | Δ=12; rel 23/23 | [source note](../src/BASE/INPUTMGR.cpp#L310) | R2 |
| `INPUTMGR` | `0x004ce990` `CheckChangeCursor` | 100.0000/100.0000 | unresolved/not-a-wall (stale exact) | Δ=0; rel 18/18 | [source note](../src/BASE/INPUTMGR.cpp#L452) | R0 |
| `INPUTMGR` | `0x004cea80` `inputManager::ForceMouseMove` | 97.4265/97.4265 | unresolved/not-a-wall | Δ=154; rel 11/11 | [source note](../src/BASE/INPUTMGR.cpp#L480) | R2 |
| `Icondf2b` | `0x004daa20` `FlipDimIconToBitmap` | 72.1084/71.6747 | unresolved/not-a-wall | Δ=258; rel 38/37 | [source note](../src/BASE/Icondf2b.cpp#L27) | R1 |
| `Iconm2b` | `0x004cfae0` `MonoIconToBitmap` | 81.5359/79.8785 | unresolved/not-a-wall | Δ=333; rel 36/38 | [source note](../src/BASE/Iconm2b.cpp#L24) | R1 |
| `Iconmf2b` | `0x004da800` `FlipMonoIconToBitmap` | 69.4671/69.4671 | unresolved/not-a-wall | Δ=292; rel 27/27 | [source note](../src/BASE/Iconmf2b.cpp#L24) | R1 |
| `MOUSEMGR` | `0x004c9350` `mouseManager::Open` | 93.3256/93.3256 | unresolved/not-a-wall | Δ=53; rel 4/3 | [source note](../src/BASE/MOUSEMGR.cpp#L61) | R1 |
| `MOUSEMGR` | `0x004c93f0` `mouseManager::Close` | 98.8461/98.8461 | unresolved/not-a-wall | Δ=14; rel 18/14 | [source note](../src/BASE/MOUSEMGR.cpp#L90) | R2 |
| `MOUSEMGR` | `0x004c9630` `mouseManager::SetPointer` | 94.3421/94.3421 | unresolved/not-a-wall | Δ=335; rel 72/70 | [source note](../src/BASE/MOUSEMGR.cpp#L168) | R1 |
| `MOUSEMGR` | `0x004c9a40` `mouseManager::NewUpdate` | 97.8916/97.8916 | unresolved/not-a-wall | Δ=278; rel 86/83 | [source note](../src/BASE/MOUSEMGR.cpp#L262) | R2 |
| `MOUSEMGR` | `0x004ca230` `mouseManager::SetColorMice` | 96.8381/96.8381 | unresolved/not-a-wall | Δ=183; rel 33/29 | [source note](../src/BASE/MOUSEMGR.cpp#L497) | R2 |
| `Midi` | `0x004d3ac0` `soundManager::MIDIPlay` | 99.9588/99.9588 | unresolved/not-a-wall | Δ=1 at `+0x167`; rel 79/66 | [source note](../src/BASE/Midi.cpp#L61) | R2 |
| `RESMGR` | `0x004c7fa0` `resourceManager::resourceManager` | 99.9836/99.9836 | unresolved/not-a-wall | Δ=1 at `+0x56` (`add edi,0x14` vs `0x62`); rel 3/3 | [source note](../src/BASE/RESMGR.cpp#L25) | R2 |
| `RESMGR` | `0x004c84d0` `resourceManager::GetFont` | 99.7755/99.7755 | unresolved/not-a-wall | Δ=11; rel 5/5 | [source note](../src/BASE/RESMGR.cpp#L153) | R2 |
| `RESMGR` | `0x004c8950` `resourceManager::RemoveResource` | 99.8837/99.8837 | unresolved/not-a-wall | Δ=1 at `+0x28`; rel 0/0 | [source note](../src/BASE/RESMGR.cpp#L271) | R2 |
| `RESMGR` | `0x004c89e0` `resourceManager::Close` | 100.0000/100.0000 | unresolved/not-a-wall (stale exact) | Δ=0; rel 4/4 | [source note](../src/BASE/RESMGR.cpp#L292) | R0 |
| `RESMGR` | `0x004c8ab0` `resourceManager::LoadAggregateHeader` | 99.9495/99.9495 | unresolved/not-a-wall | Δ=5; rel 15/15 | [source note](../src/BASE/RESMGR.cpp#L314) | R2 |
| `Ripple` | `0x004d26a0` `DoRipple` | 99.9355/99.9355 | unresolved/not-a-wall | Δ=1 at SIB byte `+0x156`; rel 13/13 | [source note](../src/BASE/Ripple.cpp#L17) | R2 |
| `TEXTWDGT` | `0x004d1060` `textWidget::textWidget()` | 97.3684/97.3684 | unresolved/not-a-wall | Δ=10; rel 2/2 | [source note](../src/BASE/TEXTWDGT.cpp#L14) | R2 |
| `TEXTWDGT` | `0x004d10f0` `textWidget::textWidget(...)` | 99.2727/99.2727 | unresolved/not-a-wall | Δ=8; rel 4/4 | [source note](../src/BASE/TEXTWDGT.cpp#L38) | R2 |
| `TEXTWDGT` | `0x004d1280` `textWidget::Main` | 99.6316/99.6316 | unresolved/not-a-wall | Δ=2; rel 6/6 | [source note](../src/BASE/TEXTWDGT.cpp#L88) | R2 |
| `Textntry` | `0x004d8b90` `textEntryWidget::Main` | 99.8529/99.8529 | unresolved/not-a-wall | Δ=1 at `+0x21`; rel 55/55; claimed 0x9a8 frame is not raw-exact | [Textntry matrices](matching-matrices/README.md#basetextntry-manual-tu-state-and-lifetime-pass) | R2 |
| `Textntry` | `0x004d9410` `textEntryWidget::Draw` | 99.9924/99.9924 | unresolved/not-a-wall | Δ=1 at `+0x6d`, stack displacement `0x0e` vs `0x0f`; rel 6/6 | [Textntry matrices](matching-matrices/README.md#basetextntry-manual-tu-state-and-lifetime-pass) | R2 |
| `WIDGET` | `0x004dde00` `widget::widget` | 96.8261/96.8261 | unresolved/not-a-wall | Δ=18; rel 1/1 | [source note](../src/BASE/WIDGET.cpp#L30) | R2 |
| `WIDGET` | `0x004dded0` `widget::Close` | unscored | proven-artifact | candidate named body and retail `empty_stub` are the same single `c3`, rel 0/0 | [empty-stub proof pattern](patterns/retail-empty-stubs.md) | A1 |
| `WIDGET` | `0x004ddee0` `widget::Main` | 99.5667/99.5667 | unresolved/not-a-wall | Δ=14; rel 17/17 | [source note](../src/BASE/WIDGET.cpp#L86) | R2 |
| `WINDOW` | `0x004cecd0` `heroWindow::heroWindow(resource)` | 99.9167/99.9167 | proven-artifact | Δ=0 over 0x521 bytes after 57 relocation payloads; 0x68 frame and EBP displacements exact; only folded dispatch/table identities remain | [WINDOW matrix](matching-matrices/README.md#basewindow-whole-tu-jump-table-revalidation) | A1 |
| `droplist` | `0x004dc2e0` `dropListWidget::Main` | 99.8745/99.8745 | proven-artifact | Δ=0 over 0x350 bytes; rel 22/22; external sites agree and seven dispatch/table sites are owner-label aliases | [source note](../src/BASE/droplist.cpp#L126) | A1 |
| `icon2bs` | `0x004d2f90` `IconToBitmapScale` | 76.3262/76.3262 | unresolved/not-a-wall | Δ=317; rel 4/4 | [source note](../src/BASE/icon2bs.cpp#L12) | R1 |
| `icon2by` | `0x004da270` `IconToBitmapYModify` | 91.5102/91.5102 | unresolved/not-a-wall | Δ=405; rel 129/130 | [manual matrix](matching-matrices/iconf2by-manual.tsv) | R1 |
| `soundmgr` | `0x004ccc80` `soundManager::ModifySample` | 99.8380/99.8380 | proven-artifact | Δ=0 over 0x202 bytes; rel 23/20; three candidate-only IAT relocation records correspond to fixed retail operands and local table labels account for the rest | [soundmgr raw audit](matching-matrices/README.md#basesoundmgr-whole-tu-raw-audit) | A1 |

## Generated deleting-destructor aliases and unscored symbols

These seven rows are kept separate because the marker applies to compiler-generated
`??_E`/`??_G` output, not to the next ordinary source function.  Retail has two same-address or
duplicate `??_E` rows for each affected CodeView alias, while VC4.2 may emit a strong `??_G` plus
a weak `??_E`.  A report score of zero or an absent row says nothing about the bytes.

| TU / marker | Generated RVA / symbol | Score state | Classification | Current proof or missing proof | Retry |
| :--- | :--- | :--- | :--- | :--- | :--- |
| [`BORDER:23`](../src/BASE/BORDER.cpp#L23) | `0x004d20e0` `border ??_E/??_G` | `??_E` retained 0, live unscored; `??_G` absent | unresolved/not-a-wall | Current emitted `??_G` is 0x1f with 2 relocs; each retail `??_E` is 0x4d with 7.  Masked comparison still has Δ=42.  The exact standalone `??1` and a hypothetical inline tradeoff do not satisfy permanent-artifact proof. | R4 |
| [`BUTTON:86`](../src/BASE/BUTTON.cpp#L86) | `0x004dd480` `button ??_E/??_G` | `??_E` retained 0, live unscored; `??_G` absent | unresolved/not-a-wall | Current `??_G` has 2 relocs versus 5 in each retail copy and Δ=28.  No emitted raw-exact deleting body is present. | R4 |
| [`ICON:35`](../src/BASE/ICON.cpp#L35) | `0x004c7a90` `icon ??_E/??_G` | `??_E` retained 0, live unscored; `??_G` absent | unresolved/not-a-wall | Current `??_G` has 2 relocs versus 5 in each retail copy and Δ=28. | R4 |
| [`ICONWDGT:23`](../src/BASE/ICONWDGT.cpp#L23) | `0x004d0a90` `iconWidget ??_E/??_G` | `??_E` retained 0, live unscored; `??_G` absent | unresolved/not-a-wall | Current `??_G` has 2 relocs versus 5 in each retail copy and Δ=28. | R4 |
| [`SAMPLE:72`](../src/BASE/SAMPLE.cpp#L72) | `0x004daef0` `sample ??_E/??_G` | generated alias unscored; standalone `sample::~sample` is 100/100 | proven-artifact | Candidate `??_G` and each retail `??_E` are 0x41 bytes, Δ=0, rel 5/5 with the same destructor/delete/vtable targets; see the [SAMPLE audit](matching-matrices/README.md#basesample-whole-tu-audit-and-constructor-shapes). | A1 |
| [`SAMPLE:98`](../src/BASE/SAMPLE.cpp#L98) | `0x004daff0` `MIDIWrap ??_E/??_G` | generated alias unscored; standalone `MIDIWrap::~MIDIWrap` is 100/100 | proven-artifact | Candidate `??_G` and each retail `??_E` are 0x3d bytes, Δ=0, rel 5/5 with the same destructor/delete/vtable targets; see the [SAMPLE audit](matching-matrices/README.md#basesample-whole-tu-audit-and-constructor-shapes). | A1 |
| [`TEXTWDGT:30`](../src/BASE/TEXTWDGT.cpp#L30) | `0x004d10a0` `textWidget ??_E/??_G` | `??_E` retained 0, live unscored; `??_G` absent | unresolved/not-a-wall | Current `??_G` has 2 relocs versus 7 in each retail copy and Δ=35. | R4 |

## Semantic-layout debt census

Do not cache a field count here; it becomes stale as soon as another TU recovers a member. Derive
the live inventory directly from the tree:

```sh
rg -n 'field_0x[0-9A-Fa-f]+' include/BASE
```

These declarations are structural debt, not padding evidence. Replace a field only when retail use
sites, CodeView, serialized data, or size/offset evidence identifies the real member. An enum name
does not justify changing proved storage width; known serialized domains should use a header-level
`typedef enum` with explicit values while preserving layout.

There are currently **zero** Ghidra-style `iVarN`/`uVarN` identifiers in `src/BASE` or
`include/BASE`.  That does not certify clean structure: the real-offset discrepancy in the
RESMGR constructor and the anonymous fields above show that semantic layout recovery is still
required.  If an `iVarN`, `uVarN`, or similar tainted identifier reappears, reconstruct that whole
function from retail evidence on a clean slate instead of editing the decompile incrementally.

## No-repeat and restart procedure

1. Run `homm2 status` first and rejoin the refreshed report to the retained baseline.  Remove R0
   markers that remain exact.  Never copy a percentage into an `@early-stop` marker.
2. Recount markers and require one ledger row per marker.  Treat source comments as search history
   only; recompute `Δ`, relocation sites/targets, frame size and visible EBP/ESP displacements.
3. For A1 rows, do no local synonym search unless their retry trigger fired.  For jump-table
   functions, use explicit object ranges, exclude documented table data only when necessary, and
   compare every external relocation before normalizing local owner labels.  The reusable proof
   method is in [`jump-tables.md`](jump-tables.md).
4. Put R1 rows back into the weighted queue and start at the first raw divergence.  Recover real
   types, layouts, fields, accessors, bodies and CFG before codegen steering.  Empty and `return 0`
   bodies are valid only after the calling convention, return bytes and zero relocations are
   proved as in [`retail-empty-stubs.md`](patterns/retail-empty-stubs.md).
5. For R2 rows, preserve exact siblings and already-correct target spans.  Try a small audited local
   family, then exact-preserving predecessor/header/TU variants under the combined root.  Record
   full source/header hashes, raw pins and rejection reasons in a matrix so the same family is not
   repeated.  A score increase that moves an already-correct byte is rejected.
6. Use `scripts/permute_ast.py` only once the function is structurally aligned at roughly 96--99%,
   and audit every retained mutation.  Do not use the regex permuter.  The current durable negative
   searches are indexed by [`matching-matrices/README.md`](matching-matrices/README.md) and
   [`matching-soft-defer-ledger.md`](matching-soft-defer-ledger.md).
7. For R3, prefer upstream Bzip structure as a secondary reference but keep developer modifications,
   headers and current retail bytes authoritative.  Do not spend a lane climbing those walls while
   unresolved first-party BASE pastures remain.
8. For R4, compare emitted compiler aliases directly; do not infer success from the exact ordinary
   destructor or from a 0/unscored report row.  Any retained change must preserve the standalone
   destructor and produce the required alias bytes/relocations simultaneously.
9. Before changing a classification, run the full build, raw/relocation audit and
   `git diff --check`.  Only `Δ=0` plus accounted relocation identities can become
   `proven-artifact`; only the complete TU-cumulative checklist can become `proven-soft-TU-cumulative`.
