// VC6 SP5 concrete-constructor parent, measured 2026-09-27.
// Worktree .claude/worktrees/matcher-2, branch matcher/bss-audiere; verified
// cwd, branch and HOMM2_DIR in one persistent Nix shell. Disposable overlays
// only; production sources and exact three-owner artifacts are unchanged.
//
// Parent: real generic node stores OutputStreamPtr, Resource*, and next node.
// Its inline constructor is declared in the shared header, with an explicit
// specialization declaration for the used sample type. The real specialization
// body precedes public functions. EffectsState remains the original private
// object; all twelve public bodies are copied unchanged. Unlike previous
// unsplit generic-constructor parents, this concrete specialization schedules
// OutputStream destructor before assignment.
//
// COMPLETE boundary product (12/12 successful compiles):
// Primary inline destructor body {before public functions, after Purge before
// Find/Play, EOF} x ownership {ordinary local instantiation, extern class before
// Purge, extern class after Purge, explicit class instantiation after body}.
// No wall-time truncation. All three body boundaries are identical within an
// ownership arm, including extern suppression after Purge.
//
// - Local: all12 public sizes, bytes and ordered semantic relocations exact;
//   Play remains0x39c. Both Play EH sections exact. Helper projection S A N G R
//   (stream dtor, assignment, node dtor, ctype guard, registration). AudioDevice
//   dtor lies between A and N in the object but an earlier owner wins in the
//   native control. This is a new preserved intermediate order, not closure:
//   retail requires S A G R N. No additional producer or constructor output.
// - Extern class: N undefined;11/12 public functions exact. Play shrinks to
//   0x385 with41 relocations. Play EH remains exact. Both suppression positions
//   show the same whole-TU effect; declaration position does not bound it.
// - Explicit class instantiation: all12 public bodies/relocations and Play EH
//   exact, but additionally emits a234-byte concrete constructor and its own
//   19-byte .text$x plus40-byte .xdata$x. Node still precedes ctype pair.
//   Audit follows Play's actual +6 handler relocation to its EH owner rather
//   than selecting the first section named .text$x (the added constructor).
//
// COMPLETE requested destructor-kind descendant (3/3 compiles):
// The same concrete constructor parent x {primary destructor at EOF, explicit
// empty inline class-body destructor, implicit destructor/no declaration}.
// Every arm preserves all12 public sizes/bytes/ordered semantic relocations
// and both Play EH sections. Primary and class-body arms project S A N G R.
// The implicit destructor emits N immediately after Purge, before Find/Play,
// in section5 rather than tail section21. It moves the real helper earlier.
// These results apply to this concrete constructor parent, not all possible
// template ownership structures. No producer reduction closes the tail here.
//
// NATIVE lower-state check: ordinary raw unsplit object replaces only Effects
// in the previously captured all-BASE-direct control inputs. Four historical
// LINK passes, lane-owned PDB. No mutable root input refresh. Candidate versus
// that matched control changes134 .text bytes and zero bytes in the headers,
// .rdata, .data and resources. File size1208393 and producer count unchanged.
// Relative to Purge, helpers are S+830,A+860,N+8c0,G+8f0,R+920. Comparison with
// the preserved exact-local three-owner branch differs in the remaining helper
// arrangement; this is a lower state, not executable closure. Root independently
// replayed the three-owner branch on the stable baseline and found all nonheader
// bytes exact, with only producer-count/header changes remaining.
// Native RSP, MAP, executable and four logs: boundary artifacts native/;
// native-results.json retains all changed offsets and section comparisons.
//
// Artifacts:
// build/audiere-special-ctor-boundaries/{probe.py,results.json,provenance.json,
// audit.py,audit.json}; twelve complete source/header/object/log sets.
// build/audiere-special-ctor-boundaries/{destructor-kinds.py,audit-dtor-kinds.py}
// build/audiere-special-ctor-dtor-kinds/{results.json,audit.json}; all three
// complete source/header/object/log sets. Every lower state is preserved.
// Symbol correspondences identify the real template node destructor and Find
// specialization; anonymous EH labels match by proven Play section role and
// offset. No output byte editing or COFF layout correction occurs.

template<class Resource>
struct AudiereSampleListNode {
    audiere::OutputStreamPtr stream;
    Resource* sampleResource;
    AudiereSampleListNode<Resource>* next;
    H2_RETAIL_INLINE AudiereSampleListNode(
        Resource* resource, AudiereSampleListNode<Resource>* nextNode);
    H2_RETAIL_INLINE ~AudiereSampleListNode();
    // Class-body arm replaces declaration with the empty body.
    // Implicit arm removes the declaration entirely.
};
template<> H2_RETAIL_INLINE AudiereSampleListNode<sample>::AudiereSampleListNode(
    sample* resource, AudiereSampleListNode<sample>* nextNode);
typedef AudiereSampleListNode<sample> AudiereSampleNode;

template<> H2_RETAIL_INLINE AudiereSampleListNode<sample>::AudiereSampleListNode(
    sample* resource, AudiereSampleListNode<sample>* nextNode) {
    stream = NULL;
    sampleResource = resource;
    next = nextNode;
}
// Primary definition position is one complete product axis:
template<class Resource>
H2_RETAIL_INLINE AudiereSampleListNode<Resource>::~AudiereSampleListNode() {}
// Whole-class ownership axes:
// extern template struct AudiereSampleListNode<sample>;
// template struct AudiereSampleListNode<sample>;
