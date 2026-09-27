// Function-boundary inline-policy audit, measured 2026-09-27 with pinned VC6.
// Worktree matcher-2, branch matcher/bss-audiere; cwd and HOMM2_DIR verified
// in the existing persistent Nix shell. Disposable overlays only.
// No production source, compiler binary, link input, or root-worktree edits.
//
// Parent is build/audiere-unsplit-integration/candidate: actual Resource*
// node, concrete inline constructor specialization, primary inline EOF dtor.
// Fresh control's complete semantic section records match the previously
// traced 115-byte parent. Raw records differ only in anonymous COFF names;
// identity normalization uses their physical section and offset. This is not
// a new executable comparison or a claim the remaining115 bytes are closed.
//
// Actual delete lowering in that control:
// - Purge is0x162; REL32 operands at+0x65 and+0x114 target the real Node dtor.
//   The surrounding scalar-delete body tests pointer/flags and calls delete.
// - Play is0x39c,40 relocations. Its delete cleanup expands member release
//   and clears stream inline; there is no Node-dtor relocation.
// - The standalone Node dtor is0x2c with no relocations; its body is exact.
// Thus a selective boundary in inline policy was a concrete untested axis,
// unlike a global /Ob change which affects unrelated accessors and cleanup.
//
// COMPLETE product: {inline_depth(0), inline_depth(1), inline_depth(2),
// auto_inline(off)} x {Purge only, Play only}, plus fresh ordinary control.
// All nine compiles succeed, no diagnostics, timeout or truncated product.
// Each policy is placed immediately before that function's VA annotation;
// inline_depth() or auto_inline(on) follows immediately before the next
// function. No source body, type, field, call, or declaration changes.
//
// Results compare all12 public functions: exact size, all non-relocation
// bytes, and complete ordered sites/types/target identities/raw addends.
// Play's actual handler relocation identifies its .text$x/.xdata$x owners;
// raw bytes and ordered EH relocations are compared, including added owners.
//
// arm             public exact   Purge/Play sizes     Play EH  helper order
// ordinary          12/12         162/39c              exact    S A N G R
// depth0 Purge      10/12         120/385              exact    S A N G R
// depth0 Play       11/12         162/2ab              differs  S A N G R
// depth1 Purge      11/12         162/385              exact    S A N G R
// depth1 Play       12/12         162/39c              exact    S A N G R
// depth2 Purge      12/12         162/39c              exact    S A N G R
// depth2 Play       12/12         162/39c              exact    S A N G R
// auto-off Purge    12/12         162/39c              exact    S A N G R
// auto-off Play     12/12         162/39c              exact    S A N G R
// Sizes are hexadecimal. Order projects OutputStream dtor/assignment, Node,
// guarded ctype initializer and registration. Other helpers are audited too.
// Every arm retains the exact0x2c Node body and no Node relocations.
// All five non-control arms with12/12 also have identical RAW section records
// to the fresh control, including helper bytes/order, symbols and relocs.
//
// Important nonlocal observation: depth1 around Purge preserves Purge exactly
// but changes the later Play to0x385/41 relocs, adding a Node call at+0x2a6.
// The first masked divergence is Play+0x58 (branch distance), first ordered
// relocation divergence is record32. Depth1 applied only around Play leaves
// Play unchanged. This is consistent with a shared delete-helper lowering
// decision established at its earlier use. It is not proof of the internal
// metadata field or a general rule for all VC6 functions. The restoration
// directive therefore does not imply effects are confined to one function.
//
// depth0 Purge emits a46-byte scalar deleting dtor in section5 and a16-byte
// OutputStream pointer accessor; Purge first differs at+5 and stops calling
// Node directly. It also gives the same regressed Play as depth1 Purge.
// depth0 Play emits the234-byte Node ctor,46-byte deleting dtor and extra
// pointer/bool accessors; its frame diverges at+0x1a, and Play EH differs.
// These lower states are preserved, not discarded from the artifact matrix.
// None provides a later Node owner or changes N/G/R relative order.
//
// Prior ownership coverage reviewed alongside these observations:
// - specialized-constructor-boundaries.cpp already covers primary dtor body
//   before public funcs / after Purge / EOF x local / extern class before
//   Purge / extern class after Purge / explicit class instantiation. The
//   three local boundaries are exact but keep N before G; all extern arms
//   lose Play's required inline cleanup. Explicit instantiation adds ctor/EH.
// - concrete-destructor-ownership.cpp covers concrete specialization with
//   or without forward specialization declaration at all three boundaries.
// - member-template-id.cpp measured both bare and full-template-id member
//   extern-dtor spellings crossed with both primary spellings: all four
//   member-only declarations fail C2631 at the extern declaration itself.
//   This is dialect rejection of those spellings, not every possible syntax.
// - forceinline-extern-product.cpp crosses inline/__forceinline with whole
//   class suppression and header/EOF body; forceinline does not restore Play.
//   That earlier product uses the generic constructor parent. It is not a
//   measured forceinline x extern product on the concrete-ctor115 parent.
//   No observed change in suppression mechanism or required caller shape
//   justifies silently presenting that unmeasured cross as covered.
//
// Scoped conclusion: the new ordinary policy boundary is measured, but no
// arm delays N. Pure body visibility and the tested member-extern syntax are
// separately covered as listed. No current result supplies a new ownership
// mechanism that preserves public/EH code and moves N past ctype init.
// No pragma or generated overlay is retained. This is not a universal
// impossibility proof; lower states remain available under build/.
//
// Reproducer/artifacts: build/audiere-inline-policy/
// probe.py, audit.py, provenance.json, results.json, audit.json, audit.log,
// summary.json, full-object-audit.json, and each full source/header/log/object.
// Run probe.py then audit.py in the verified lane Nix shell. Flags are the
// unchanged unit_flags(BASE/AudiereEffects), with canonical header overlays.

// Reviewed source product: each policy opening occupies one of the two
// indicated boundaries, followed by the corresponding closing directive.
// The actual function bodies are copied byte-for-byte from the115 parent.
//
// #pragma inline_depth(0)    // mutually exclusive with1,2,auto_inline(off)
// VA(0x004cc740, 0x162)
// void PurgeFinishedAudiereSamples(void) { /* unchanged actual body */ }
// #pragma inline_depth()
// VA(0x004cc8b0, 0x3a)
// ...
//
// #pragma auto_inline(off)   // mutually exclusive with inline_depth(0/1/2)
// VA(0x004cc8f0, 0x39c)
// void PlayAudiereSample(...) { /* unchanged actual body */ }
// #pragma auto_inline(on)
// VA(0x004ccc90, 0x3c)
// ...
