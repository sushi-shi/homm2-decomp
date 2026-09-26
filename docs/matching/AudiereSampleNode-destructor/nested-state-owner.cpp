// VC6 SP5 nested-owner structural probe, 2026-09-26.
// Verified worktree: .claude/worktrees/matcher-2, matcher/bss-audiere;
// HOMM2_DIR resolves to that worktree in the persistent Nix shell.
// Existing dirty work was preserved; no production source/header was changed.
//
// Motivation: the node is used only by AudiereEffects and its sole lifetime
// and list owner is gAudiereEffects. The current global helper type and the
// existing effects-state record are reconstruction claims. A nested node
// owned by that state record is a legitimate structural alternative that
// preserves layout and semantics. Prior private-type and name censuses did
// not exercise nested-class member ownership/deferred-definition scheduling.
//
// Complete structural matrix and artifacts:
//   build/audiere-nested-owner/probe.py
//   build/audiere-nested-owner/results.json
//   build/audiere-nested-owner/{control,nested_eof,nested_class_body,
//                              nested_implicit}/
// Each arm directory retains full source/header, compiler log, raw object,
// and raw llvm-objdump symbol/relocation report (coff.txt). results.json
// records all function bytes, sizes, sections, hashes and ordered relocation
// sites/types/targets/addends. All four arms compiled using run_compile and
// unit_flags(BASE/AudiereEffects), including production /Gy; no truncation.
//
// Reviewed structural shape, replacing the former global node/state records:
//
// struct AudiereEffectsState {
//     struct SampleNode {
//         audiere::OutputStreamPtr stream;
//         class sample* sampleResource;
//         SampleNode* next;
//         SampleNode(class sample* resource, SampleNode* nextNode) {
//             stream = NULL;
//             sampleResource = resource;
//             next = nextNode;
//         }
//         inline ~SampleNode();
//     };
//     void* buffer;
//     i32 frameCount;
//     i32 channelCount;
//     i32 sampleRate;
//     audiere::SampleFormat sampleFormat;
//     SampleNode* sampleList;
//     i32 sampleIterationDepth;
// };
// typedef AudiereEffectsState::SampleNode AudiereSampleNode;
//
// EOF-definition arm, after all public functions:
// inline AudiereEffectsState::SampleNode::~SampleNode() {}
//
// Class-body arm replaces the declaration with ~SampleNode() {} and omits
// the EOF definition. Implicit arm omits both the declaration and definition.
// All public function bodies and the constructor body are otherwise unchanged.
// The state declaration is placed in the owner header; its DATA definition
// stays in AudiereEffects.cpp with the same storage and initializer.
//
// Measured order:
// control/nested EOF: public functions through sec17; node dtor sec18;
//                    RefPtr helpers sec19..21; $E21 sec22; $E20 sec23.
// nested class-body: node dtor sec5, immediately after Purge; public functions
//                    resume at sec6; helpers/$E pair remain sec19..23.
// nested implicit:   same as nested class-body.
//
// All twelve public function payloads and all helper bodies remain byte-exact
// against the clean control after masking relocation operands. The node dtor
// remains0x2c and Play remains0x39c. Complete ordered relocation differences:
// - the reviewed change to the node's qualified C++ name, including Find's
//   return-type decoration;
// - Play's +6 DIR32 EH-table local label: $L55809 -> $L55810 (EOF) or
//   $L55815 (class-body/implicit). Sites, types and raw addends are unchanged.
// No relocation is added or removed. None of these arms moves the destructor
// after the guarded-static $E pair, which is the required native placement.
//
// Current baseline dossier independently remains exact:19 destructor
// instructions,3/3 blocks,0/0 relocations, and aligned od-frame evidence.
//
// Disposition: REJECT for the final-link placement objective. Nested module
// ownership is semantically credible but provides no new deferred-emission
// ordering in these measured forms. The caller fingerprints do not justify
// retaining a source change with no placement benefit. No state probes, fake
// functions, padding, COFF changes, or output edits were retained. This is a
// bounded negative for these nested ownership forms, not a universal claim
// about every possible original-source or original-build explanation.
