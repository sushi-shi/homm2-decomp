// VC6 SP5 ownership probe, 2026-09-26.
// Worktree: .claude/worktrees/matcher-2, branch matcher/bss-audiere.
// Production source and headers were not modified by this experiment.
//
// Hypothesis: a real resource-parameterized list node could retain visible
// inline bodies in AudiereEffects while extern-template instantiation delegates
// its out-of-line destructor to the following AudiereMusic object. This differs
// from prior plain-destructor ownership probes, which hid the inline body.
//
// Artifact/reproducer:
//   build/link/audiere-extern-template/probe.py
//   build/link/audiere-extern-template/late-probe.py
//   build/link/audiere-extern-template/results.json
//   build/link/audiere-extern-template/results-late.json
//   build/link/audiere-extern-template/{clean,extern}-coff.txt
//   build/link/audiere-extern-template/{clean,extern}.asm
// Every arm used unit_flags(BASE/AudiereEffects), including production /Gy,
// and run_compile under the verified assigned worktree's Nix environment.
// The complete ten-arm diagnostic finished; no wall-time truncation occurred.
// Per-function records retain raw section sizes, relocation-masked SHA256s,
// and complete ordered relocation sites/types/names/raw addends.
//
// Source family (the template parameter has a real role in the resource type):
//
// template<class Resource>
// struct AudiereSampleListNode {
//     audiere::OutputStreamPtr stream;
//     Resource* sampleResource;
//     AudiereSampleListNode<Resource>* next;
//     AudiereSampleListNode(Resource* resource,
//                          AudiereSampleListNode<Resource>* nextNode) {
//         stream = NULL;
//         sampleResource = resource;
//         next = nextNode;
//     }
//     inline ~AudiereSampleListNode();
// };
// typedef AudiereSampleListNode<sample> AudiereSampleNode;
//
// template<class Resource>
// inline AudiereSampleListNode<Resource>::~AudiereSampleListNode() {}
//
// Suppression arms add either:
//   extern template struct AudiereSampleListNode<sample>;
// or the attempted member-specific declaration:
//   extern template AudiereSampleListNode<sample>::~AudiereSampleListNode();
//
// First matrix: clean control; unsuppressed template with EOF definition;
// extern-class and extern-destructor forms, each with definition at EOF or
// in the header. The member-specific syntax was rejected with C2631 in both
// positions. It is not evidence that every member-instantiation spelling is
// unsupported. All class-suppression arms compiled successfully.
//
// Second matrix: extern-class declaration after all twelve public functions,
// before or after the EOF definition, crossed with the body residing in the
// header or at EOF. All four arms compiled. This explicitly tests whether
// suppression can happen after the caller has already seen an inline body.
//
// Measurements:
// - Clean: node dtor sec18; OutputStream RefPtr dtor sec19; operator= sec20;
//   AudioDevice RefPtr dtor sec21; $E21 sec22; $E20 sec23.
// - Unsuppressed template: the exact 0x2c node dtor moves to sec21, immediately
//   before $E21/$E20. All twelve public function byte payloads remain equal
//   after relocation masking. Ordered relocation differences are the intended
//   reconstructed node/Find type names plus an anonymous EH-table ordinal.
// - Every compiling extern-class arm removes the node-dtor definition from
//   AudiereEffects. Purge remains 0x162 bytes, with its two destructor calls
//   at +0x65 and +0x114 targeting the now undefined template destructor.
// - Every extern-class arm changes Play from retail 0x39c to 0x385. Its raw
//   relocation count grows from 40 to 41: the inlined stream destruction is
//   replaced by a REL32 call to the node destructor at +0x2a6. Subsequent
//   operator delete moves from +0x2d4 to +0x2bb. This repeats the prior plain
//   cross-TU ownership contradiction despite the still-visible inline body.
// - Extern suppression at EOF, even after the body, does not preserve inlining.
//
// Fresh baseline dossier: sema reports identical destructor asm, 3/3 exact
// blocks; od-frames reports aligned=1; relocs reports 0 base/0 target.
// Raw-byte SHA256 confirms all three clean destructor bodies are identical:
//   4e59698146271d05d36728df5a7be5e31f6434e6058318c493991d1014fff150
// Their COFF auxiliary section checksums differ; those checksums must not be
// interpreted as raw-byte equality evidence.
//
// Identity/ownership follow-up: retail itself retains this identical body at
// both RVA 0xccf70 and 0xcd050. The former is called by EH funclets 0xe9434
// and 0xe9485; the latter by Purge. Thus anonymous-body equality does not
// collapse the retail destinations into one ownership claim. Relabeling the
// helpers cannot change those actual CALL targets. Whole-body ICF alone
// cannot explain two separately retained equal bodies. Selective eligibility
// would need independently recovered source or build evidence.
//
// The old non-/Gy contribution-granularity discussion also does not apply to
// this production compile: the guarded init and atexit helpers already occupy
// distinct sec22/sec23 COMDATs. The remaining measured issue is their order,
// not a missing ability to split them from the main text contribution.
//
// Disposition: REJECTED. Extern class instantiation genuinely transfers
// out-of-line ownership, but also changes the proven inline deletion in Play.
// A second-TU instantiation cannot repair that caller contradiction, so no
// AudiereMusic edit/compile was warranted. No source or build change retained.
// This closes these specific ownership forms, not all possible source forms.
// A next structural hypothesis needs evidence for independently retaining the
// caller's inlining while suppressing this one out-of-line definition; neither
// ordinary inline ownership nor VC6 extern-class suppression supplies it.
