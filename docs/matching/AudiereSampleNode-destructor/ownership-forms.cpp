// VC6-measured source-shape ledger for AudiereSampleNode::~AudiereSampleNode.
// Target RVA: 0x004ccfdc, size 0x2c. The clean implicit destructor is byte-exact,
// but its /Gy COMDAT is emitted immediately after PurgeFinishedAudiereSamples.
// Retail emits it after the TU's RefPtr helpers and $E21/$E20 static helpers.
//
// Compiler artifact used for both arms:
//   build/objdiff/base/BASE/AudiereEffects.obj
// The ordinary build path was compiled and inspected with llvm-objdump/llvm-readobj;
// the source and artifact were restored to the exact clean state after measurement.

// Arm 1: explicit inline class-body destructor.
//
// struct AudiereSampleNode {
//     ...
//     ~AudiereSampleNode() {}
// };
//
// Result: byte- and section-identical to the clean implicit destructor. The order
// remains Purge -> AudiereSampleNode dtor -> Find -> Play -> ... . Rejected because
// it cannot affect the first linked divergence.

// Arm 2: declaration in soundBackends.h, definition after the last public function.
//
// struct AudiereSampleNode {
//     ...
//     ~AudiereSampleNode();
// };
// AudiereSampleNode::~AudiereSampleNode() {}
//
// Result: the destructor moves after AudiereSampleIterationActive, but still precedes
// RefPtr<OutputStream>::~RefPtr, RefPtr<OutputStream>::operator=,
// RefPtr<AudioDevice>::~RefPtr, $E21, and $E20. Retail needs it after all five.
// Worse, PlayAudiereSample shrinks from the exact retail 0x39c bytes to 0x385 because
// VC6 exposes/inlines the explicit definition. Rejected as a semantic-codegen
// contradiction, not merely a lower fuzzy score.

// Disposition: retain the authentic implicit destructor and classify its final-link
// COMDAT position as an original object/compiler ownership wall. No explicit dtor,
// source padding, /ORDER directive, or synthetic linker root is retained.
