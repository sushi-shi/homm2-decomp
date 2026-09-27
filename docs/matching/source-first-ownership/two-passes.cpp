// Two source-first ownership passes, measured 2026-09-27 with VC6 SP5.
// User direction: write plausible developer code before inspecting linked bytes;
// test multiple complete arrangements, including worse intermediate layouts.
// All 24 arrangements were written before compiling their respective pass.
// No standalone destructor TU, fabricated template owner, literal-backed static
// arrays, generated probe declarations, object edits or executable edits remain.
//
// Common parent:
// - All 47 Misc functions in one Misc.cpp; all includes precede implementation.
// - Remove 118 local static string copies introduced by the 204-byte checkpoint.
//   Restore direct literals/catalog references at their original read-only uses.
// - LogInt("IME", ...) uses the literal directly. gcCDTrackName points directly
//   to the track-name literal; there is no second named array for that string.
// - A concrete dimmerWidget : widget implements all methods in DIMMER.cpp.
//   There is no nested BaseWidget template or explicit instantiation request.
// - AudiereSampleNode and AudiereEffectsState are private types in
//   BASE/AudiereEffects.h. Only AudiereEffects.cpp includes that header.
//   Resource has one actual type, sample*, so no node template is introduced.
// - The obsolete DIMMERDestructor unit and its special archive split are removed.
//
// Pass 1: complete 3 x 2 product, six native full-game links.
// Node constructor/destructor organization:
//   members: in-class member-initializer constructor, implicit destructor;
//   body: in-class body assignments, implicit destructor;
//   tu: constructor and explicit empty destructor defined before public functions
//       in AudiereEffects.cpp, with ordinary declarations in the private header.
// DIMMER destructor: ordinary out-of-line definition / in-class empty definition.
//
// Pass 2: complete 3 x 3 x 2 product, eighteen native full-game links.
// Retest all three node organizations against all-TU DIMMER definitions,
// header-defined Main/Draw/destructor, or header-defined constructors. Read stays
// in DIMMER.cpp; no new TU is introduced. Both Audiere helpers have only same-TU
// callers, so test file-static and anonymous-namespace ownership independently.
// WINDOW.cpp is recompiled in every arm so header-owned emission is included.
//
// Flags: unchanged config/units.toml profiles through unit_flags(), including
// the normal BASE /Gy rule. Each native image uses the ordinary four-LINK
// historical mode, untouched raw objects, and the simplified archive graph.
// No state probes, partial products, timeouts or failed compilations occurred.
//
// All arms compiled and linked. None recovered retail layout. Fixed-file-offset
// differences below include changed operand addresses and contribution movement;
// they are not a count of independent incorrect source bytes or functions.
// Do not compare these broad shifts to the old 115-byte local residual as though
// only the old helper tail had changed. That result used the now-rejected source.
//
// Raw local observations:
// - All-TU concrete DIMMER retains seven ordinary method/wrapper body sizes, but
//   the scalar deleting wrapper still follows the first constructor.
// - members emits PlayAudiereSample at 906 bytes; body emits 924 (retail 924);
//   tu emits 851 plus a real 117-byte out-of-line node constructor.
// - body emits the 44-byte node destructor immediately after Purge, before Find,
//   rather than after the RefPtr and ctype helpers. It does not solve that order.
// - Moving complete constructors into the DIMMER header changes ownership in
//   WINDOW as expected; it does not produce the missing retail arrangement.
// These are diagnostic body-size/order observations, not ordered-relocation
// exactness claims. Final normalized comparison and relocation audit are recorded
// in integration.md after the retained source is regenerated.
//
// Retained source: body + ordinary DIMMER TU. These straightforward initializers
// were written before the comparison and preserve the known Play body size without
// the template specialization. Both experimental helper-linkage alternatives are
// preserved as diagnostics, not selected solely to perturb compiler state.
//
// Reproducer and complete literal source products (isolated matcher-4 worktree):
//   build/holistic-source-first/{run.py,run-pass2.py,manifest.json,pass2-manifest.json}
//   build/holistic-source-first/{results.json,pass2-results.json}
//   build/holistic-source-first/{variants,pass2}/<arm>/{src,include}/BASE/*
// Each arm retains raw objects, complete executable and MAP, compiler/link logs,
// section bodies and ordered relocation records in <unit>.json, and image.json.
// Both scripts restore the simple parent on exit; final source selection and a
// fresh normal build are separate from those disposable experiments.
//
// Native image results (differing bytes at fixed file offsets):
//   members-tu                               318457  sha256=44d556fdeb683d9a8573fc69a86aa453ac7dbfd4e8a9d0f044d5e96948e90196
//   members-header                           318472  sha256=978a90e4741d9ad5ecf6aad41be3fe581fc4c34bd300fc0ad94f1560dbdfdaab
//   body-tu                                  290183  sha256=8f3fef432648fd9fe6f8e9a654580c967bc71636c691fce9dc760e97ce60c9e4
//   body-header                              290199  sha256=4d7ffddf09ece419cbfd9f3cd738b4010445a87bd74044cd8ef68d21f1428380
//   tu-tu                                    317773  sha256=c9c19de65ccf794098f14ad278ba062997156ef269eb4e4df87498e58c2604af
//   tu-header                                317780  sha256=19d33b0de0353fbcad6cc6eca5be1e42c1fbc6747afdc6c0c4380b511cb27ec4
//   members-tu-static                        318816  sha256=953aeb4e92ce46f82164f08c2dbc8497381d5d0f9e1c9faa79ea8e6514ea064a
//   members-tu-namespace                     318457  sha256=44d556fdeb683d9a8573fc69a86aa453ac7dbfd4e8a9d0f044d5e96948e90196
//   members-forwarding_header-static         318849  sha256=8faa7c1d05d6eeb7462f3714d3498dcb620a9872bcd4f02bcdf5ec502d38a883
//   members-forwarding_header-namespace      318490  sha256=dead0e20d74a9d5d27f484a8d427bc1876dc42f0233721cd7a936fc050772c94
//   members-constructors_header-static       389450  sha256=f5f5d52c4ee32da89cc84f298b838fcd022f22686080e094d62f21c29fe7093d
//   members-constructors_header-namespace    389455  sha256=dbbdf77d369b09a96136b1aefb9e0fa7c6b040042867c213bb631b16e259d496
//   body-tu-static                           290531  sha256=58f33b2d33224255b710e9d3a413251786810eadb5e1ccb2da5ff9dfdc2ea40f
//   body-tu-namespace                        290183  sha256=8f3fef432648fd9fe6f8e9a654580c967bc71636c691fce9dc760e97ce60c9e4
//   body-forwarding_header-static            290548  sha256=0f7f569ab5972c14468c7ffc6612a406da20dbfbb075310353a3273228837659
//   body-forwarding_header-namespace         290200  sha256=5fab38610189275f63b2198264791f9aaef70145c14e3454597eb1f3392574f6
//   body-constructors_header-static          384062  sha256=bd49a400b518b316315f227a5664443d35fce32280b59d2a471534cd7132cdff
//   body-constructors_header-namespace       384085  sha256=9cf0ff1490bcf9fbb51268f37e25c6094befb05953876e69ec6ba7b723ab55cf
//   tu-tu-static                             317764  sha256=a321be61ed963219dbce9b7b697bec1e403ac30ab47a3e8b50519e91b9cc43ca
//   tu-tu-namespace                          317773  sha256=c9c19de65ccf794098f14ad278ba062997156ef269eb4e4df87498e58c2604af
//   tu-forwarding_header-static              317783  sha256=6fc7e79bfdaea1f7eb58dd9becbad54101dc94f8b0608fc7d9585fd4467c4730
//   tu-forwarding_header-namespace           317792  sha256=8a0a5883c5cd190b12ee5c43ac8043de2ee143c6720d0910c06e234936cc3aed
//   tu-constructors_header-static            385183  sha256=c69a9c2b447286f9547857d8d7e093b9d3fadc2602ae811cc1dea5b57978b7a7
//   tu-constructors_header-namespace         385171  sha256=8e729c55d0fffef8175a5d49c64a516b15dce3cb379f1bf4fe8380ca9aec1dc0

// Node alternatives (actual complete products are retained at the paths above):
// struct AudiereSampleNode {
//     audiere::OutputStreamPtr stream;
//     sample* sampleResource;
//     AudiereSampleNode* next;
//
//     // members
//     AudiereSampleNode(sample* resource, AudiereSampleNode* nextNode)
//         : sampleResource(resource), next(nextNode) {}
//
//     // body
//     AudiereSampleNode(sample* resource, AudiereSampleNode* nextNode) {
//         stream = NULL;
//         sampleResource = resource;
//         next = nextNode;
//     }
//
//     // tu: declarations here; definitions in AudiereEffects.cpp, using the
//     // same member-initializer constructor and an empty ordinary destructor.
//     AudiereSampleNode(sample* resource, AudiereSampleNode* nextNode);
//     ~AudiereSampleNode();
// };
//
// Header-forwarding DIMMER alternative (ordinary concrete class):
// MessageDispatchResult Main(tag_message& message) { return widget::Main(message); }
// void Draw() { Dim(); }
// ~dimmerWidget() {}
//
// Header-constructor DIMMER alternative:
// dimmerWidget() : widget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {}
// dimmerWidget(i16 x, i16 y, i16 w, i16 h, i16 id, i16 kind)
//     : widget(x, y, w, h, id, kind) {}
//
// The experiments change only real ownership, linkage and initialization.
// No fabricated source is retained to recover the old producer count or metrics.
