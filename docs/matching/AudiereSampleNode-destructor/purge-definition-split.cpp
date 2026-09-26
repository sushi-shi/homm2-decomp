// VC6 SP5 source-ownership branch, measured 2026-09-27.
// Worktree .claude/worktrees/matcher-2, branch matcher/bss-audiere; cwd,
// branch and HOMM2_DIR verified in the persistent Nix shell. All generated
// sources are disposable overlays. No production source or output edits.
//
// NEW FIRST-REQUIREMENT BOUNDARY:
// Purge is the only actual out-of-line node-destructor caller; Play needs its
// inline expansion. Earlier extern-template tests suppressed both in one TU.
// Split Purge into an owner using extern class instantiation, while Effects
// keeps the visible primary destructor definition without suppression.
// The real resource parameter remains Resource* in the node's stored field.
// The existing EffectsState becomes a shared header type and its real single
// object becomes externally linked, defined by the prefix owner. Its fields
// and 0x1c storage are unchanged. No fake root, reference or object is added.
//
// Complete first product: prefix contains {Purge, Purge+Find} x primary inline
// destructor body {header, Effects EOF}; four pairs / eight compiles, all pass.
// All twelve public functions retain size, every relocation-masked byte, and
// complete ordered relocation sites/types/semantic owners/raw addends. Both EH
// sections retain raw bytes and ordered semantic relocations. Prefix leaves N
// undefined; Effects emits NO N at all while Play remains 0x39c/40 relocations.
// Full-header prefixes each emit an extra ctype pair, so they are intermediate
// source states, not layout solutions. This result narrows the old claim that
// visible inline destruction necessarily forces an early out-of-line N: that
// was observed only with Purge's first requirement in the same TU.
//
// Constructor/body ownership descendant:
// - Declare the node constructor inline and move its body into Effects,
//   either before the remaining public functions or at EOF: both preserve
//   all12 public bodies/ordered relocations/EH in this new split context.
// - Music explicit class-instantiation owner, constructor {class-body,
//   declaration-only} x instantiation {before globals, EOF}: four compiles pass.
//   Class-body arms additionally emit a 234-byte constructor; declaration-only
//   arms emit only N and warn C4661 about the absent unused constructor body.
//   All four put N after Music methods, before Music's ctype pair, not at the
//   needed start of that owner. No Music body-exact claim is made by this test.
//
// The generic constructor still produces A then S (assignment, stream dtor).
// Concrete constructor specialization for the actually used sample type,
// body {before functions, EOF}, changes that order to retail S then A.
// Both arms again preserve all12 public bodies/ordered relocations/EH, emit
// no N in Effects, and create no extra out-of-line constructor body.
// The specialization declaration is consistently visible in shared headers.
//
// NARROW ABI INTERFACE / THREE REAL OWNERS:
// Audiere's <vector>/<string> dependencies belong exclusively to convenience
// functions SplitString/GetSupportedFileFormats/GetSupportedAudioDevices and
// their description records. A disposable ABI-only header retains all original
// class/interface definitions, enums, ABI declarations and other wrappers;
// it omits only those convenience definitions and their two standard includes.
// This is an input-header decomposition diagnostic, not a vendor binary edit.
// Purge needs only this interface and the real shared node/state declarations.
// A provider sees that same node declaration, the primary empty destructor
// definition, and explicit class instantiation, but no constructor definition.
//
// Final three raw objects:
//   special-narrow-prefix/prefix.obj: only Purge, size0x162, plus real state.
//   effects-special-ctor-before-functions/AudiereEffects.obj: other11 methods,
//       S,A,AudioDevice dtor,G,R,ctype cleanup; no N.
//   special-narrow-provider/provider.obj: only N, size0x2c, zero relocations.
// Narrow prefix/provider emit no ctype pair or .CRT$XCU contribution. Provider
// retains C4661 for the absent unused constructor definition even with explicit
// specialization declared; no constructor code or undefined call is emitted.
// final-audit.json proves all12 public bodies/ordered relocations and both EH
// sections versus the fresh unsplit control. N independently equals the raw
// 44-byte control body with zero relocations and is the only provider function.
//
// NATIVE LINKS (full executable, unmodified raw compiler objects):
// A suffix-direct control and branch follow the earlier native direct probe.
// Then all BASE library objects are supplied directly in their baseline MAP
// contribution order, giving an independently matched direct control/branch.
// All four images are linked in four historical LINK passes with a lane-owned
// PDB. Input snapshots, hashes, RSPs, MAPs and logs are retained. Root objects
// carried concurrent unrelated source regressions; absolute retail totals are
// therefore NOT a completion metric for these controls. Suffix-direct also
// loads suffix objects before still-archived BASE predecessors, a known global
// order change; the all-BASE-direct arm avoids that partial-direct asymmetry.
//
// All-BASE-direct native candidate relative to Purge (native VA4cc470):
//   +000 Purge; +170 Find; +1b0 Play; remaining public starts unchanged;
//   +830 S; +860 A; +8c0 G; +8f0 R; +910 N.
// These are the retail relative starts. The earlier AudioDevice definition
// wins normally and the helper-only provider supplies N at the correct tail.
// Candidate versus the lane-matched direct control changes202 .text bytes,
// one .data byte, zero .rdata/.rsrc bytes, and263 header bytes (466 total).
// C++ producer 0x000b2306 rises96 ->98. File size remains1208393.
// Absolute retail differences are239304 for direct control and239538 for this
// candidate because the root snapshot already has unrelated placement/body
// regressions. Those numbers are not evidence that the Audiere local branch
// regresses or closes the whole image. Stable-baseline replay is required.
//
// Remaining integration constraints:
// - Two additional C++ producers; no compensating owner consolidation proven.
// - Ordinary archive extraction need not reproduce explicit direct-object
//   order: only Effects references Purge, so its unresolved chain may extract
//   Effects first. No /INCLUDE placement root or binary correction is proposed.
// - Whole-image headers, absolute bytes and every selected relocation still
//   need closure on the stable root lane. Keep this as a measured branch.
//
// Artifacts: build/audiere-purge-split/
// probe.py, provider.py, narrow.py, constructor-specialization.py,
// special-narrow.py; each records full source/header/log/object products.
// audit.py, provider-audit.py, final-audit.py and corresponding JSON results.
// native.py, all-native.py, native-audit.py; native-inputs/ holds captured
// ordinary project inputs; all-native-results.json and native-audit.json retain
// complete comparisons. all-direct-{control,three-owner}/ contain native images.
// Every source product here completed; no wall-time truncated matrix.

// Shared actual template owner (constructor implementation is Effects-owned):
template<class Resource>
struct AudiereSampleListNode {
    audiere::OutputStreamPtr stream;
    Resource* sampleResource;
    AudiereSampleListNode<Resource>* next;
    H2_RETAIL_INLINE AudiereSampleListNode(
        Resource* resource, AudiereSampleListNode<Resource>* nextNode);
    H2_RETAIL_INLINE ~AudiereSampleListNode();
};
template<> H2_RETAIL_INLINE AudiereSampleListNode<sample>::AudiereSampleListNode(
    sample* resource, AudiereSampleListNode<sample>* nextNode);
typedef AudiereSampleListNode<sample> AudiereSampleNode;
template<class Resource>
H2_RETAIL_INLINE AudiereSampleListNode<Resource>::~AudiereSampleListNode() {}

// Effects source owns this real concrete constructor body:
template<> H2_RETAIL_INLINE AudiereSampleListNode<sample>::AudiereSampleListNode(
    sample* resource, AudiereSampleListNode<sample>* nextNode) {
    stream = NULL;
    sampleResource = resource;
    next = nextNode;
}

// Prefix owner: extern template struct AudiereSampleListNode<sample>;
// Provider owner: template struct AudiereSampleListNode<sample>;
// Public caller bodies stay identical; shared state remains one real object.
