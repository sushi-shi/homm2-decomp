// VC6 SP5 header visibility, first requirement and archive selection.
// Measured 2026-09-27, matcher-4 / matcher/native-link-recovery, working alone.
// Production source was not modified by these controls.
//
// Question: does the late retail Node destructor prove header ownership?
// Separate four facts: a visible definition, an inline expansion, emission of
// an out-of-line copy in a raw object, and selection/placement of that copy.
// Source declaration order is not a universal final-RVA ordering rule.
//
// Actual retail constraints, independently checked through incoming calls:
// - Node::~Node at RVA 0xcd050 has two direct callers, both in Purge at
//   instruction RVAs 0xcc7a4 and 0xcc853. Its body is 44 bytes.
// - The identical-looking helper at 0xccf70 is RefPtr<OutputStream>::~RefPtr,
//   called by Effects and Music unwind funclets at 0xe9437 and 0xe948b.
//   See ../Audiere-helper-identities/typed-unwind-ownership.cpp for typed
//   stack/member ownership and raw relocation proof. Swapping these names
//   would discard that evidence.
// - Effects public functions precede the stream helpers, ctype initialization
//   helpers, and Node; Music's static initialization then begins at 0xcd080.
// - Play expands node destruction whereas Purge calls the out-of-line body.
//   Therefore inspect both callers, not just the first call to Node.
//
// Small compiler control: real Audiere RefPtr, three-field Node, actual-shaped
// Purge, and CreateEffect with a null-stream cleanup path. Both TUs include
// <string>, so the real VC6 ctype initialization contributions are present.
// Nine source forms x /Ob0,/Ob1 = 18 arms, 36 compiles. Each arm is linked in
// both direct-object orders = 36 native links. Complete, no timeouts or omitted
// arms. All source products are written before compilation.
//
// Forms:
//   implicit-header: implicit Node destructor, class definition in Node.h;
//   empty-header: explicit empty in-class destructor;
//   inline-header: inline declaration and out-of-class body in Node.h;
//   inline-effects-only: inline body visible only in Effects;
//   ordinary-music: ordinary declaration, non-inline body in Music;
//   ordinary-effects: ordinary declaration, non-inline body in Effects;
//   inline-music-only: inline declaration, body visible only in Music;
//   music-first-use: Effects constructs but never deletes a Node;
//   pasted-definition: the same complete class text in both .cpp files.
//
// /Ob1 observations:
// - Across destructor declaration/body-owner forms, Purge's 354 raw bytes AND
//   ordered relocation records are identical. Its call does not distinguish
//   header ownership from an ordinary destructor supplied by another TU.
// - CreateEffect is 474 bytes with the visible inline/implicit definitions and
//   contains no Node-destructor call. It is 449 bytes with ordinary-music,
//   ordinary-effects, or inline-music-only, including a Node REL32 at +0x17f.
//   Thus both body visibility and inline eligibility matter in this fixture.
// - Header-visible definitions emit Node immediately after Purge: Effects
//   sections 5/6, Music sections 4/5. In either direct link order, the first
//   object's Node copy survives. Only Effects emits the copy when only Effects
//   sees the inline body; a call in the other TU alone cannot supply it.
// - Music-first-use emits no Node copy in Effects and one in Music.
// - Header versus pasted-definition produces identical executable section
//   bytes in each raw object. Some anonymous Effects EH symbol counters shift;
//   Music's complete records agree. Linked .text, .rdata and .data are identical
//   for both /Ob settings and both input orders. Physical header location is
//   consequently not identifiable from those linked sections. A shared header
//   is the natural developer organization, not a fact encoded by those bytes.
//
// Archive control: reuse implicit-header /Ob1 with a separate main that calls
// both Purges. Two caller orders x four ordinary library arrangements. Four
// compilations, four LIB outputs, eight native links; all complete. No forced
// root, /ORDER, altered object or corrected executable.
//
// Driver calls    Library arrangement       First backend / surviving Node
// E then M        combined, members E M      Music / Music
// E then M        combined, members M E      Music / Music
// M then E        combined, members E M      Effects / Effects
// M then E        combined, members M E      Effects / Effects
// either          E.lib then M.lib          Effects / Effects
// either          M.lib then E.lib          Music / Music
//
// Raw driver COFF explains why source-call order is insufficient:
// E-then-M: undefined symbol 18=Music, 19=Effects; call relocations at 4->19,
//          11->18 in section 3.
// M-then-E: undefined symbol 18=Effects, 19=Music; call relocations at 6->19,
//          11->18 in section 3.
// Combined-library extraction agrees with undefined symbol-table order in
// both controls. This does not establish LINK's universal worklist algorithm.
// Reversing archive member order does not change these results; separating
// the archives makes their command-line order decisive here.
//
// Crucial joint constraint: EVERY switch to Music's Node also moves Music's
// ordinary functions ahead of Effects. These plain archive changes cannot
// choose a late Music copy while retaining the retail ordinary-function order.
// Header ownership alone therefore does not solve the placement mismatch.
// The minimal source-first Node remains a private header type; no artificial
// destructor TU, template parameter, or library boundary is retained.
//
// Disposition: strong evidence for a visible inline-eligible implementation
// when reproducing the cleanup shape, consistent with a header definition.
// The remaining question is why a compatible build would omit, defer, or lose
// the early required copy while preserving the surrounding functions. None
// of these controls supplies that explanation. They do not exclude all other
// ownership graphs, compiler inputs, or legitimate build configurations.
// PoL 2.0 debug module boundaries remain version-specific clues; they are not
// direct TU ownership evidence for newly added or changed Buka 2.1 code.
//
// Reproduce from the worktree root inside nix develop .#build:
//   python3 docs/matching/source-first-ownership/header_probe.py
//   python3 docs/matching/source-first-ownership/archive_probe.py
// Artifacts: build/holistic-source-first/{header-probe,archive-probe}/.
// results.json records all raw executable sections/ordered relocations for
// header probes and selected MAP owners for archive probes. Each source,
// object, response file, image, MAP and tool log is retained locally.
// Additional audits: header-probe/header-vs-pasted-linked.json and
// archive-probe/driver-reference-order.json. Rechecked retail xrefs live in
// build/holistic-source-first/{early,late}-destructor-xrefs.log.

// Core definition, before choosing any destructor ownership arm:
// struct Node {
//     audiere::OutputStreamPtr stream;
//     sample* resource;
//     Node* next;
//     Node(sample* r, Node* n) { stream = NULL; resource = r; next = n; }
// };
// Node* CreateEffect(sample* resource, Node* next,
//                    audiere::OutputStream* stream) {
//     Node* node = new Node(resource, next);
//     node->stream = stream;
//     if (node->stream == NULL) { delete node; return NULL; }
//     return node;
// }
