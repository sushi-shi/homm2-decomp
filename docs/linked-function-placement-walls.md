# Linked function placement walls

Object-level function closure and final linked RVA closure are independent.
Every configured project function can have exact bytes and relocations while
Microsoft LINK selects an identical COMDAT from another owner or places an exact
contribution at a different address. The final-link report therefore joins
functions semantically first and records address placement as a separate result.

At the natural-link checkpoint, all 1,727 project functions have a unique
semantic candidate identity. Of those, 1,715 land at the retail RVA and 12 are
displaced. None are missing or ambiguous. The 12 are confined to three units:

| Unit | Count | Bounded cause |
| --- | ---: | --- |
| `BASE/AudiereEffects` | 5 | One implicit destructor COMDAT is emitted early, shifting later contributions; identical template COMDATs may be selected from earlier owners. |
| `BASE/AudiereMusic` | 2 | LINK selects identical `RefPtr<OutputStream>` template COMDATs from the earlier AudiereEffects owner instead of the retail-local copies. |
| `BASE/DIMMER` | 5 | The scalar deleting destructor COMDAT is emitted after the first constructor rather than after `Draw`, shifting four ordinary functions by 48 bytes. |

These counts include ordinary source functions and compiler-generated functions.
The final report's `project_summary` is the authoritative reproducible census;
the broader inventory also contains CRT helpers, thunks, and funclets whose
placement is a separate runtime/link-input campaign.

## Semantic counter-function matching

Raw `_$E<n>` suffixes are per-compilation counters, not identities. The final-link
audit removes the already named `VA_COMPGEN` functions, then pairs the remaining
counter functions in normalized per-TU section order. A pair is accepted only if
its relocation-masked body bytes and ordered relocation site/type shape agree.
Any count or signature mismatch rejects the entire TU pairing.

This maps all 197 volatile helpers in 95 TUs. Eight pairs are genuinely
renumbered: six in `AudiereMusic`, one in `PATH`, and one in `PHILAI`. Treating
their literal suffixes as stable would manufacture missing and ambiguous results;
the semantic pairing leaves neither.

## `TILE` and `BITS`

The retail Rich census identifies both units as OMF inputs converted by VC6.
Their C++ analysis mirrors emitted the right bytes but the wrong COFF topology:
`TileToBitmap` was a 16-byte-aligned COMDAT and the three BITS functions were
separate one-byte-aligned COMDATs. Reconstructing the functions as ordinary OMF
assembly contributions makes VC6 perform the evidenced conversion. `TileToBitmap`
and all three BITS functions then land at their exact retail RVAs with exact linked
bytes and relocations. No alignment, producer-id, padding, or post-link adapter is
retained.

## Audiere ownership and folding

`AudiereSampleNode::~AudiereSampleNode` is byte-exact as an implicit destructor,
but VC6 emits its COMDAT immediately after `PurgeFinishedAudiereSamples`. Retail
places it after the TU's `RefPtr` and counter helpers. The early 48-byte
contribution shifts eleven ordinary functions, one `RefPtr` assignment, and two
counter helpers by 48 bytes.

An explicit inline class-body destructor is section-identical and does not move
the contribution. An out-of-line destructor moves later, but still precedes five
retail-predecessor helpers and changes the previously exact
`PlayAudiereSample` body from 0x39c to 0x385 bytes. The authentic implicit form is
retained. The measured source-shape ledger is
`docs/matching/AudiereSampleNode-destructor/ownership-forms.cpp`.

Three other Audiere template functions are selected from earlier identical
COMDAT definitions: one `RefPtr<AudioDevice>` destructor attributed to
AudiereEffects, and the `RefPtr<OutputStream>` destructor and assignment attributed
to AudiereMusic. Their semantic implementations are exact; the selected-owner
RVA differs from retail. No duplicate body or linker-order directive is retained
solely to force selection.

## DIMMER deleting destructor

Retail orders the DIMMER contributions as both constructors, `Read`, `Main`,
`Draw`, the scalar deleting destructor, then the ordinary destructor. The
candidate emits the exact deleting-destructor COMDAT immediately after the first
constructor. Four subsequent source functions are therefore 48 bytes late, while
the deleting destructor itself is 256 bytes early.

The bounded VC6 matrix covered all six declaration orders of the destructor,
`Draw`, and `Main`; inline and out-of-line destructor ownership forms; `/Gi`,
debug, exception, and `/Ob0`/`/Ob1`/`/Ob2` flag variants; individual and combined
inline constructor ownership; and the only other using TU. Declaration order and
valid project-state arms retained the early COMDAT. Inlining both constructors was
the only later orbit, but it suppresses two live retail functions. The other TU
emits no alternate destructor/vtable copy. The unusual retail placement is also
not a family rule: fourteen other BASE scalar deleting destructors reproduce the
ordinary constructor-adjacent position.

No tested authentic source or compiler-state arm emits the complete retail order.
The source therefore retains the semantically correct form; no `/ORDER` file,
synthetic root, padding function, or vtable patch is used.

## Project EH funclets

The stripped final MAP contains no names for the 311 reviewed unwind funclets, so
an exact-name join incorrectly reported every one missing. All funclet code comes
from 75 `.text$x` sections emitted by 50 reconstructed project TUs; no `LIBCMT`
member contributes to this band. The candidate band begins at RVA `0xe7f50`, while
retail begins at `0xe7f60`, a uniform −16 candidate displacement.

Every relocation-masked candidate COFF section signature occurs in the 5,811-byte
linked band. The input sections carry 445 REL32 and 134 DIR32 relocations. Comparing
candidate bytes with retail shifted by 16 yields 5,341 identical bytes and 470
differing bytes in exactly 445 runs. Every run decodes as one `call` or `jump` REL32
operand; no opcode, stack displacement, immediate, or other body byte differs.

Of those calls/jumps, 443 resolve to the same absolute target. The other two call
candidate `0x4ccfa0` where retail calls `0x4ccf70`; both destinations are the same
48-byte `RefPtr<OutputStream>` destructor body, byte-for-byte, selected at different
Audiere COMDAT positions. The funclet bodies and semantic targets are therefore
closed, while all 311 raw funclet RVAs remain displaced by −16. The final-link JSON
retains the section census, every exceptional target pair, and the raw byte result.

## Import thunks

Decorated linker names are not stable enough to identify every import thunk. In
particular, retail records `RtlUnwind@KERNEL32` at RVA `0xe7c1a`, while the current
MAP exposes `_RtlUnwind@16` at RVA `0xe7c7e`. Both are the canonical six-byte
`FF 25 <IAT slot>` form, and both IAT slots resolve to
`kernel32.dll!RtlUnwind`.

The audit now decodes the IAT operand and pairs thunks only when the resulting DLL
plus imported name-or-ordinal identity is unique. This maps all 23 reviewed import
thunks without a spelling exception. Twenty-two form one contiguous import-library
band displaced by −8 bytes. `RtlUnwind` is displaced by +100 bytes in the later
runtime-library band. The six WinG MAP rows omit the optional function flag, so the
proof deliberately relies on executable thunk shape and IAT identity rather than
that presentation detail. No reviewed thunk is missing or ambiguous.

## Static runtime bodies

The 414-row CRT inventory cannot use raw decorated-name placement as its primary
oracle. Archive members contain repeated masked bodies, ABI spellings differ, and
LINK may select an identical implementation from a different member or runtime
archive. A name-first join originally reported 36 exact RVAs, 330 displaced rows,
46 missing rows, and two ambiguous rows; it also silently paired several stale
analysis labels with the wrong same-named body.

The final-link audit now compares every reviewed CRT body against every linked
`LIBCMT`/`LIBCPMT` function start after masking reviewed DIR32 fields and aligned
REL32 operands. A group is accepted only when the complete retail and candidate
multiplicities pair monotonically. An unbalanced collision additionally requires
one unique reviewed archive member or exact symbol name. Raw MAP names never
override the body group.

This produces three evidence tiers:

- All 343 rows already identified as `masked-exact` against the pinned archive
  map to a candidate address; none is missing or ambiguous.
- Of the 71 rows previously supported only by sandwich, callgraph, prefix, order,
  or linked-byte evidence, 52 form complete linked-body groups. These are bounded
  shape/placement results because their complete ordered relocation identities
  have not yet been audited against the archive objects.
- Nineteen rows have no complete linked candidate group and remain explicit
  bounded residuals: five internal floating-point bodies, ten cleanup fragments,
  two dispatch fragments, one exception-filter entry, and one locale body.

The semantic pass also corrected durable stale labels exposed by unique body
groups, including `__heap_alloc`, `___FrameUnwindToState`, `terminate`,
`_inconsistency`, `_realloc`, `__msize`, `_calloc`, the `___crtGet*` helpers,
and `___crtCompareStringA`. These are inventory corrections, not synthetic linker
aliases. The JSON retains every nontrivial candidate set and all nineteen residuals.

## Closure rule

These are bounded raw-placement walls, not byte-exact executable closure. They may
be revisited if authentic object ownership, compiler flags, or earlier link inputs
are recovered and naturally alter LINK's state. Until then, the report must retain
both facts: all project function identities and object implementations are closed,
and 27 final RVAs remain non-exact. Import resolution-history is tracked separately
in `docs/patterns/idata-thunk-order-is-resolution-history.md`.
