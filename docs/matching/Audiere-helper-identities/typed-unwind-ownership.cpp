// Typed Audiere helper-identity audit, 2026-09-26.
// Read-only production audit in matcher-2, branch matcher/bss-audiere;
// pwd, branch and persistent-shell HOMM2_DIR independently verified.
// No reconstructed code, configuration, object, or output was changed.
//
// Conclusion: three body-only inventory identities are wrong. Retail caller
// addresses and native COFF relocation identities require these replacements:
//
// 0xccf70,44,??1?$RefPtr@VOutputStream@audiere@@@audiere@@QAE@XZ,BASE/AudiereEffects,BASE/AudiereEffects,reviewed-callgraph
// 0xcda50,44,??1?$RefPtr@VSampleSource@audiere@@@audiere@@QAE@XZ,BASE/AudiereMusic,BASE/AudiereMusic,reviewed-callgraph
// 0xcda80,81,??4?$RefPtr@VSampleSource@audiere@@@audiere@@QAEAAV01@PAVSampleSource@1@@Z,BASE/AudiereMusic,BASE/AudiereMusic,reviewed-callgraph
//
// Keep 0xb6b00 as RefPtr<AudioDevice>::~RefPtr,0xccfa0 as
// RefPtr<OutputStream>::operator=, and 0xcd050 as AudiereSampleNode::~Node.
// Identical helper bodies alone cannot choose among these identities.
//
// Exact edit artifact (not applied by this lane):
//   build/audiere-dtor-identity/expected-config.diff
//   build/audiere-dtor-identity/expected-config-rows.csv
// Reproducible invocation artifacts:
//   python3 build/audiere-dtor-identity/raw-call-census.py
//   python3 build/audiere-dtor-identity/coff-call-identity.py
// Results:
//   build/audiere-dtor-identity/raw-calls.json
//   build/audiere-dtor-identity/coff-call-identity.json
//   build/audiere-dtor-identity/*-play*.asm
//   build/audiere-dtor-identity/*-funclet*.asm
//
// Authoritative retail image SHA256:
// bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a
// Native raw candidate object SHA256s:
// AudiereEffects f027940a8c45b24580efc1d41b67c8bfc45c3d458c103faee0213f8eebe03172
// AudiereMusic   3352af1c4171aa9023d79450995a3cb7cb56ac3316dc5ad44eb562cc859fc21a
//
// Raw incoming-call census scans EVERY byte position for E8/E9 rel32 operands
// in EVERY executable PE section and decodes destinations numerically. It
// does not use the suspect function names or their inferred owning TU. Each
// listed relevant instruction is confirmed by the enclosing disassembly.
// It separately reads every reviewed DIR32 site in config/retail/absolute_relocations.tsv;
// none points at any of these six helper destinations. No direct JMP enters
// them. Indirect computed calls are not discoverable from this static census.
//
// Complete direct incoming CALLs, instruction RVA / operand RVA:
// target 0xccf70: 0xe9437 / 0xe9438; 0xe948b / 0xe948c.
// target 0xcda50: 0xe947f / 0xe9480.
// target 0xcda80: 0xcd7b0 / 0xcd7b1.
// Controls:
// target 0xb6b00: 0xe8f76 / 0xe8f77; 0xe9423 / 0xe9424;
//              0xe9453 / 0xe9454; 0xe9473 / 0xe9474.
// target 0xccfa0: 0xcca0b / 0xcca0c; 0xcca19 / 0xcca1a; 0xcd829 / 0xcd82a.
// target 0xcd050: 0xcc7a4 / 0xcc7a5; 0xcc853 / 0xcc854.
//
// Proof for 0xccf70 (OutputStream destructor):
// Retail unwind at 0xe9434:
//   8b4de0         MOV ECX,[EBP-20h]
//   e8343bfeff     CALL 0xccf70 (opcode 0xe9437)
//   c3             RET
// Native AudiereEffects section 7 .text$x, relocation+0x18:
//   IMAGE_REL_I386_REL32 -> ??1?$RefPtr@VOutputStream@audiere@@@audiere@@QAE@XZ
// The raw addend is0. Enclosing funclet is section+0x14 ($L55746).
// In PlayAudiereSample, +0xed pushes sizeof node 0xc; +0xef calls operator new;
// +0xf7 stores returned node pointer in[EBP-20h]. The node's first member is
// OutputStreamPtr stream. Constructor code passes exactly this pointer in ECX
// to OutputStream operator= at+0x11b, establishes EH state 2 at+0x120, then
// initializes the trivial sample/next pointer members. An incomplete node
// construction unwinds the constructed stream member, not an AudioDevice.
//
// Independent cross-TU retail unwind at 0xe9485:
//   8d8d8cfeffff   LEA ECX,[EBP-174h]
//   e8e03afeff     CALL 0xccf70 (opcode 0xe948b)
//   c3             RET
// Native AudiereMusic section 22 .text$x, relocation+0x1c:
//   IMAGE_REL_I386_REL32 -> same OutputStream destructor, raw addend0.
// Its enclosing funclet begins section+0x15 ($L56786). The source local is
// audiere::OutputStreamPtr stream = device->openStream(source.get());
// PlayAudiereMusic +0x192 initializes[EBP-174h], +0x1a3 takes its address,
// +0x1a9 calls OutputStream operator= (native relocation+0x1aa), and +0x1ae
// establishes EH state 2. The normal exit also unreferences this exact local.
//
// AudioDevice is separately distinguishable despite its identical dtor bytes:
// corresponding candidate unwind sites use LEA ECX,[EBP+8], the by-value
// AudioDevicePtr argument. Raw retail calls from those funclets target 0xb6b00,
// not 0xccf70. The false duplicate AudioDevice name is not a retail folding
// or alias fact; it is a mistaken body-only reconstruction identity.
//
// Proof for 0xcda50 (SampleSource destructor):
// Retail unwind at 0xe9479:
//   8d8d90feffff   LEA ECX,[EBP-170h]
//   e8cc45feff     CALL 0xcda50 (opcode 0xe947f)
//   c3             RET
// Native AudiereMusic section 22 .text$x, relocation+0x10:
//   IMAGE_REL_I386_REL32 -> ??1?$RefPtr@VSampleSource@audiere@@@audiere@@QAE@XZ
// raw addend0; enclosing funclet section+9 ($L56785).
// This is the source local initialized by audiere::OpenSampleSource(filename),
// immediately before the distinct OutputStream local described above.
//
// Proof for 0xcda80 (SampleSource assignment):
// PlayAudiereMusic +0x10d calls imported AdrOpenSampleSource; +0x119 zeros
// [EBP-170h]; +0x12a takes its address in ECX. The CALL at+0x130 is retail
// opcode RVA 0xcd7b0, operand 0xcd7b1, bytes e8cb020000, target 0xcda80.
// Native .text section 21 relocation+0x131 explicitly names
//   ??4?$RefPtr@VSampleSource@audiere@@@audiere@@QAEAAV01@PAVSampleSource@1@@Z
// with raw addend0. It stores the returned SampleSource pointer in its owning
// RefPtr. By comparison, the subsequent stream initialization at+0x1a9 calls
// retail 0xccfa0 and has the distinct OutputStream operator= identity.
//
// Disposition: replace precisely these three canonical claims and regenerate
// targets. Evidence category reviewed-callgraph records the stronger typed
// caller/relocation proof rather than implying body equality chose the name.
// This fixes metadata ownership; it does not itself move any native section
// or claim closure of the remaining Audiere/DIMMER executable differences.
//
// Root integration validation (three rows retained as reviewed-callgraph):
//   build/audiere-owner-integration/{redelink,build,link,audit}.log
//   build/audiere-owner-integration/{effects,music}-relocs.log
//   build/audiere-owner-integration/typed-target-relocs.asm
// Regenerated target cleanup relocations now carry the identities above.
// Focused caller reviews: PlayAudiereSample40/40 and PlayAudiereMusic36/36
// relocations, no missing owners. Full build:1727/1727 exact functions and
//291995/291995 data bytes. The native executable remains SHA256
// c0eacb837c0d8bdec918962df5bce11270d05247908f4be34badd4015255a1b8,
// still500 bytes from retail. The corrected placement report has10 displaced
// project functions instead of12: the two former Music discrepancies were
// wrong semantic identities, while the real Effects/DIMMER layout remains.
