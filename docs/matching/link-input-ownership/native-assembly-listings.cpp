// VC6 assembler-output ownership probe, 2026-09-26.
// Hypothesis: a native CL /FA -> MASM build path might emit the generated
// destructor contributions in a different order from CL's direct COFF writer.
// No source, listing, object, library or executable correction was applied.
//
// Complete measured product:
//   two TUs: BASE/DIMMER and BASE/AudiereEffects
//   production unit_flags plus /FA /Fa<disposable-output.asm>
//   each untouched listing passed to pinned MASM 6.11 in COFF and OMF modes
// Artifacts/reproducers:
//   build/native-assembly-order/{probe,assemble}.py
//   build/native-assembly-order/{results,assembler-results}.json
//   build/native-assembly-order/*.asm and per-invocation logs
// Both CL invocations succeeded. All four MASM invocations terminated.
//
// The emitted PROC order already matches the undesired direct-COFF order:
// DIMMER ctor(), deleting wrapper, ctor(args), Read, Main, Draw, ordinary dtor.
// AudiereEffects public bodies, node dtor, three RefPtr helpers, $E21, $E20.
// Thus the textual output does not expose a postponed-helper emission path.
//
// Both unmodified DIMMER listings fail MASM with A2006: the vector deleting
// destructor referenced by the vtable is undefined in the listing. The direct
// COFF object represents it with weak fallback metadata, not a listing body.
// Both AudiereEffects assemblies fail A2071 (initializer magnitude) and A1010
// (unmatched PROC nesting) at the long decorated PlayAudiereSample procedure.
// LISTING.INC is the unmodified vendor header from the pinned toolchain.
//
// Disposition: this direct, unmodified listing/assembler pipeline supplies no
// replacement link input. No repair aliases, added declarations, shortened
// names, edited segments, or hand-assembled copies are justified by the test.
// No final-link improvement is claimed, and no production change is retained.
