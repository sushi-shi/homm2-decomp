// VC6 SP5 concrete destructor ownership, measured 2026-09-27.
// Worktree .claude/worktrees/matcher-2, branch matcher/bss-audiere; verified
// cwd/branch/HOMM2_DIR in the existing persistent Nix environment. Disposable
// overlays only; no production sources or matcher-4 files were changed.
//
// Parent: the integration-ready unsplit source with a genuinely resource-
// parameterized node and explicit concrete sample-constructor specialization.
// That source preserves all12 public functions/EH and emits S A N G R; root's
// ordinary native replay measured115 differing executable bytes. It is an
// intermediate source state, not executable closure.
//
// Hypothesis: ownership of N by an explicit concrete destructor specialization,
// rather than implicit primary-member instantiation, could enter a different
// deferred emission channel. Compiler RE identifies a specialization scanner,
// but does not prove this concrete member is routed through its post-$E drain.
// Earlier explicit RefPtr and nested-owner specialization probes use other
// structural parents. This product changes the actual destructor definition
// owner without extra state, calls, compiler inputs or altered public bodies.
//
// COMPLETE product: concrete destructor specialization declaration in header
// {absent, present} x specialization body {before public functions, after Purge
// before Find/Play, EOF}. Six of six compile successfully, without warnings.
// The declaration-present arms explicitly establish specialization ownership
// before uses. Declaration-absent late definitions are VC6 dialect diagnostics;
// their acceptance is not a claim of portable specialization-after-use rules.
//
// All six preserve all12 public sizes, raw non-relocation bytes, and complete
// ordered relocation sites/types/semantic target identities/raw addends. Play
// remains0x39c with40 relocations. Its actual handler and xdata sections retain
// raw bytes and ordered relocations, followed through the Play+6 handler target.
// N remains44bytes with zero relocations. No constructor body is added.
//
// Emission results, identical with/without the separate header declaration:
// - Before-public and after-Purge definitions: N is section5, immediately
//   after Purge and before Find/Play; S19,A20,AudioDevice21,G22,R23,cleanup24.
// - EOF definition: N18,S19,A20,AudioDevice21,G22,R23,cleanup24.
// Thus explicit specialization makes source definition position observable,
// but every measured arm places N earlier than the primary-definition parent.
// None reaches the required S A G R N tail. The lower states remain preserved.
// This rejects this complete concrete-member specialization product, not all
// possible source ownership or specialization scheduling.
//
// Artifacts: build/audiere-concrete-dtor-owner/
// probe.py, provenance.json, results.json, audit.py, audit.json, audit.log;
// six complete ordinary source/header/object/compiler-log sets. Sources retain
// canonical includes; compilation uses an include-directory overlay plus the
// current matcher-4 header tree. Anonymous EH counter names correspond only
// through exact section roles and offsets. No output corrections are applied.

// Existing parent class declares the primary destructor inline:
template<class Resource>
struct AudiereSampleListNode {
    audiere::OutputStreamPtr stream;
    Resource* sampleResource;
    AudiereSampleListNode<Resource>* next;
    H2_RETAIL_INLINE AudiereSampleListNode(
        Resource* resource, AudiereSampleListNode<Resource>* nextNode);
    H2_RETAIL_INLINE ~AudiereSampleListNode();
};
template<>
H2_RETAIL_INLINE AudiereSampleListNode<sample>::AudiereSampleListNode(
    sample* resource, AudiereSampleListNode<sample>* nextNode);

// Header-declaration arm adds this before the concrete typedef:
template<>
H2_RETAIL_INLINE AudiereSampleListNode<sample>::~AudiereSampleListNode();
typedef AudiereSampleListNode<sample> AudiereSampleNode;

// The real constructor implementation remains unchanged before Purge:
template<>
H2_RETAIL_INLINE AudiereSampleListNode<sample>::AudiereSampleListNode(
    sample* resource, AudiereSampleListNode<sample>* nextNode) {
    stream = NULL;
    sampleResource = resource;
    next = nextNode;
}

// Specialization replaces, rather than supplements, the primary EOF body.
// This definition occupies each of the three reviewed source boundaries:
template<>
H2_RETAIL_INLINE AudiereSampleListNode<sample>::~AudiereSampleListNode() {}
