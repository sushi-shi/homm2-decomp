# Exhaustive Breadth Audit

This report records the completed whole-repository function audit. The campaign
processed every live source-backed function that was not exact, in descending fuzzy
percentage order, with a hard cap of ten non-improving attempts. The machine-readable
per-function evidence remains authoritative in config/breadth_audit.json.

## Completion

At epoch cc7b2cff78090f65c374e818fede1f9ad8ce32ee0d5a55ec7d5ccd3ec4dc8628:

- 1,475 source-backed functions were in scope.
- 1,019 were exact and needed no current audit record.
- All 456 non-exact functions had current records; none remained queued or stale.
- The tracker retained 617 records across epochs. No record exceeded ten attempts and
  no (unit, function) key was duplicated.
- Current parked functions have complete semantics, CFG, frame/layout evidence, and
  reviewed relocation owner/addend tuples. Their residuals are compiler scheduling,
  register allocation, local-label identity, constant-pool identity, or proved inline
  continuation shape. A parked percentage is not treated as correctness proof.

The queue builder sorts pending functions by descending fuzzy percentage. The empty
build/breadth-audit-queue.tsv therefore proves that the closest-first pass reached the
end of the live non-exact set rather than stopping at an arbitrary cutoff.

## Correctness Bugs Fixed

The audit found and corrected these behavioral or storage defects:

| Area | Defect and correction |
| --- | --- |
| VIEW::HandleViewGeneral | Context help tested Left Shift (0x2) instead of right mouse (0x200). The retail input condition is restored. |
| PHILAI::DetermineTargetPosition | Human target retention used an anonymous 1.1f; retail uses AI_EVENT_HUMAN_VALUE_FACTOR (1.5f). The stray constant was removed and all 179 relocation tuples now agree. |
| ARMY::DamageEnemy | The alternate condition tested Curse, while retail intentionally repeats the Bless test. The shipped behavior is restored. |
| ARMY::SpellEffect | Creature animation used 75 ticks instead of the retail 275 ticks. |
| ARMY::Walk | Walking used a cached duration instead of m_frameInfo.walkDuration. |
| COMMAND::ResetMouse | Message modifiers were confused with the screen-X payload field. The X/Y payload slots and chained assignments are corrected. |
| RECRUIT::Main | Right-button quick view and its guard/shared-tail control flow were incomplete. |
| SETUP::SetupGameHandler | The right-button command test had incorrect nesting and fallthrough. |
| GAME::SetupOrigData | Sorceress Navigation and Warlock Scouting starting-skill initialization were wrong. |
| EVENTS::PlayerMonsterInteract | Monster-event awards were applied in the wrong order. |
| PHILAI::DoDimensionDoor | Path bytes were indexed with the wrong signedness. |
| ADVMGR::ShowRoute | Route-direction bytes were loaded unsigned instead of signed. |
| KB::NormalDialog | An invented resourceType >= 0 guard changed negative, non-sentinel resource behavior; the retail branch and value widths are restored. |
| KB::PopNetBox | Three control/stack locals were cyclically assigned and the keyboard byte view was mis-modeled. |
| PATH::ValidAttack | The wide-east facing branch was reversed. |
| dpnetwin::dpnet_term | A 104-byte frame buffer was passed where the retail protocol requests a 100-byte payload. |
| Monster runtime flags | Runtime monster flag bits were shifted to the wrong values; the retail bit assignments are restored. |
| Sample playback data | A raw overlay used incorrect field offsets. It was replaced by the real nested SamplePlaybackData layout at sample offsets 0x10..0x2f. |
| Global field references | Owner-only relocation masking had hidden wrong owner-relative offsets such as gConfig+0x1c versus retail gConfig+0x30. The layouts and references were corrected and are now checked by ordered addends. |

The audit also recovered numerous signed-short results, stack-slot identities, field
types, switch layouts, and expression orders. These changes make the reconstruction
structural or byte-exact but are not listed as runtime bugs unless the prior source
could produce different behavior. Every such function-level decision is recorded in
config/breadth_audit.json.

## Invalid Proofs Found

Four prior early-stop claims were not supported by current raw-byte evidence and were
demoted or corrected instead of being carried forward:

- PHILAI::EvaluateMonsterEvent
- advManager::InsertSound
- game::DoKnob
- fileRequester::Update

town::SetupThievesGuild was likewise retained as semantic/compiler-shape work after
non-jump differences disproved a jump-only claim. This is why the current audit treats
old markers only as leads and reproduces their byte and relocation evidence.

A later COFF audit also disproved the claimed 18-byte monAnimDrawFrame allocation.
Retail stores 16 bytes and its 18-phase readers intentionally obtain two trailing zeros
from the immediately following zero-valued word. The temporary 18-byte model did not
change the lookup bytes or address and therefore cannot explain the observed crash; that
crash remains unattributed rather than being recorded as a false fix.

## Tooling Defects Fixed

The campaign exposed infrastructure errors that could otherwise hide source defects or
invalidate correct reviews:

- Relocation masking compared owner identity without reliably comparing the
  owner-relative addend. Ordered relocation checks now make global+field_offset
  differences fatal even when instruction relocation bytes are masked.
- The first breadth epoch hashed raw COFF serialization. Harmless symbol/string-table
  reordering invalidated 367 reviews. The current logical COFF epoch hashes semantic
  section, byte, symbol, and relocation content instead.
- The link verifier classified zero-valued PE file-alignment tail bytes as initialized
  storage. BSS is now accepted at that boundary only with candidate MAP evidence.
- Required initialized extents were stale for gArtifactBaseRV (0x190 to 0x19c)
  and gNeutralBaseResourceValues (0x40 to 0x50). The audited extents now cover
  the complete objects.
- Generic aggregate headers obscured declaration ownership and amplified unrelated
  compiler-state changes. Message and config/type owners were made concrete before the
  final forced refresh.

## Remaining Non-Bugs

The remaining non-exact rows are not silently accepted mismatches. Each has bounded
attempt history and current relocation review. Typical residuals are equivalent /O2
register coloring, /Od commutative load order, compiler-local strings and jump-table
labels, or retail-only inline continuation jumps. The final relocation gate reviewed
1,499 functions and 37,090 ordered sites with zero structural review items.

The linked image has no unresolved symbols, all 249 required initialized-storage rows
are verified, and the storage audit reports zero mismatches. The 17 absent public-data
spellings are classified identities rather than missing storage: 15 CRT-private literals
and two legacy WING descriptor aliases. The MIDI-to-CD-stereo assertion was reproduced
in the retail executable and is therefore not a reconstruction regression.

## Validation Artifact

The final linked and installed executables are byte-identical:

- Build: build/link/HEROES2W.EXE
- Playable folder: /home/sheep/Projects/homm2/homm2-linked-game/HEROES2W.EXE
- SHA-256: 4a5990b05e3630141d3309479d67aa0cf12b1c3ae03b7cef309324d91555471b

The final validation ran 55 focused breadth/link tests, the full homm2 build, header,
global, vtable, relocation, early-stop, storage, resource, and link gates. The full build
completed in 7.1 seconds.
