// Measured 2026-09-27, root decomp-gold-2.1-buka, pinned VC6 SP5.
// Parent: extern-template-ownership.cpp's real resource-parameterized node.
// Extern class instantiation removes the out-of-line node destructor from
// AudiereEffects, allowing an adjacent owner, but also suppresses its expansion
// in Play. Previous forceinline matrices retained /Ob1. This product tests
// whether the ordinary automatic-inlining policy changes that interaction.
//
// Complete four-arm matrix:
//   /Ob1 or /Ob2 x ordinary local or extern class instantiation.
// Every arm retains the meaningful Resource* field, visible inline destructor
// declaration, and EOF primary-template empty destructor definition. No added
// helper, fake statement, output correction, or source padding is involved.
// Only the single /Ob argument differs within each matched pair. Worktree,
// branch and HOMM2_DIR were verified in the same persistent Nix build shell.
//
// All four compile successfully without timeout/truncation. Whole-object audit
// finds each /Ob2 arm identical to its /Ob1 counterpart in every section's raw
// bytes/name/characteristics, every symbol's name/section/value/type, and every
// ordered relocation's section/site/type/target name. This covers EH sections,
// data and initializer topology as well as the twelve public method bodies.
// Timestamp and file/section auxiliary records are not comparison predicates.
//
// Both local arms:
//   Purge 0x162; Play 0x39c; node destructor section21, size0x2c;
//   _$E21 section22, size0x27; _$E20 section23, size0x12.
// Both extern arms:
//   Purge 0x162; Play 0x385; no local node-destructor definition;
//   _$E21 section21, size0x27; _$E20 section22, size0x12.
// As in the independently audited parent, extern ownership replaces Play's
// inline destruction with the extra out-of-line call. /Ob2 does not repair it.
//
// Disposition: rejected. No source/build flag retained, no full link necessary
// because Play's changed body is already a retail contradiction. This rejects
// the interaction of automatic inlining with the measured extern-template
// ownership; it is not a universal source or compiler-state impossibility claim.
//
// Artifacts: build/audiere-extern-auto-inline/
//   probe.py, probe.log, results.json, audit.py, audit.json
//   {Ob1,Ob2}-{local,extern}/{AudiereEffects.cpp,soundBackends.h,
//                           AudiereEffects.obj,compile.log}
// Reproducer shares the retained real template source and compile/inspection
// setup from build/link/audiere-extern-template/probe.py.

// Ownership alternative (after the actual template owner declaration):
// extern template struct AudiereSampleListNode<sample>;
// Both arms keep the EOF definition:
// template<class Resource>
// H2_RETAIL_INLINE AudiereSampleListNode<Resource>::~AudiereSampleListNode() {}
