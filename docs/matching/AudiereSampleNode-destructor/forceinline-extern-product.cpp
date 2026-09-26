// VC6 SP5 explicit-inlining / ownership product, 2026-09-26.
// Verified worktree .claude/worktrees/matcher-2, branch matcher/bss-audiere;
// persistent Nix shell HOMM2_DIR resolves to that worktree.
// Existing dirty work was preserved. No production source/header was edited.
//
// Hypothesis: __forceinline might retain Play's inline node destruction even
// when extern-template class instantiation suppresses its out-of-line body.
// Earlier forceinline measurements did not cross that ownership suppression.
//
// Complete independent Cartesian dimensions (8 compiling arms):
// - destructor declaration AND definition: inline / __forceinline;
// - extern template struct AudiereSampleListNode<sample>: absent / present;
// - inline destructor definition: header / EOF.
//
// Artifacts:
//   build/audiere-force-extern/probe.py
//   build/audiere-force-extern/results.json
//   build/audiere-force-extern/comparison.json
//   build/audiere-force-extern/{local,extern}.asm
// Each arm retains its complete source/header, compiler log and raw object.
// The probe reuses the reviewed resource-parameterized node reconstruction
// from extern-template-ownership.cpp; Resource* is the actual sampleResource
// type, not a dummy template parameter. Production unit_flags supplies all
// flags, including /Od /Ob1 /Gy, via run_compile. No matrix was truncated.
//
// Reviewed source shape (INLINE is the independent inline/__forceinline axis):
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
//     INLINE ~AudiereSampleListNode();
// };
// typedef AudiereSampleListNode<sample> AudiereSampleNode;
//
// template<class Resource>
// INLINE AudiereSampleListNode<Resource>::~AudiereSampleListNode() {}
//
// Suppressed arms additionally declare in the header:
// extern template struct AudiereSampleListNode<sample>;
// The definition above is placed either before that header declaration or at
// EOF after all twelve public functions. No caller or constructor body changes.
//
// Results, equal for both inline spellings and both definition locations:
// Unsuppressed: node destructor defined in sec21, size0x2c; $E21/$E20 follow
//               in sec22/sec23. Play0x39c,40 relocations, no node-dtor call.
// Suppressed:   node destructor is undefined (COFF section0); $E21/$E20 in
//               sec21/sec22. Play0x385,41 relocations, new node-dtor REL32
//               at+0x2a6. This is the known retail caller contradiction.
// All arms:     Purge0x162, with node-dtor REL32 calls at+0x65 and+0x114;
//               suppression changes only whether that target has a definition.
//
// comparison.json confirms complete function records are identical for each
// inline/__forceinline pair: every size, section number, relocation-masked
// byte hash, and complete ordered relocation site/type/name/addend array.
// Thus the new keyword has no observable effect in this entire tested product,
// not merely no effect on the printed target size or a fuzzy score.
//
// Disposition: REJECT. __forceinline does not override this VC6 extern-class
// suppression's loss of Play's inline destruction under production flags.
// No arm meets the prerequisite of absent out-of-line node destructor plus
// exact0x39c Play, so next-TU instantiation/link experiments are unwarranted
// for this source family. No forceinline declaration, generated probe source,
// COFF correction, padding, or output edit is retained in reconstructed code.
