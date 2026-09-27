// VC6 SP5 static-member ownership matrices, measured 2026-09-27.
// Worktree matcher-4, branch matcher/native-link-recovery, HEAD c4e8f6bfd.
// cwd, branch and HOMM2_DIR verified in one persistent Nix shell. Current
// source is the verified 115-byte native parent. Disposable source products
// only: no production source, vendor header, object, or executable changed.
//
// Hypothesis: ownership of the real std::ctype<unsigned short>::id might
// change when the existing guard/registration pair is emitted relative to N.
// This tests the initializer side of S A N G R; retail requires S A G R N.
// Older ownership-forms.cpp mentions related tests but their original sources
// are no longer present. This is fresh complete coverage on the new parent.
//
// FIRST COMPLETE PRODUCT: three forms below x three source boundaries,
// plus a freshly compiled unchanged control: 10/10 successful compiles.
// Boundaries: before Purge; after all public functions before primary node
// destructor definition; EOF after that definition. No timeout/truncation.
//
// 1. template std::locale::id std::ctype<unsigned short>::id;
// 2. template<> std::locale::id std::ctype<unsigned short>::id;
// 3. template<> std::locale::id std::ctype<unsigned short>::id
//        = std::locale::id();
//
// All nine variants preserve all12 public function sizes, raw bytes and
// ordered semantic relocation sites/types/targets/addends. Both Play EH
// sections also preserve bytes and ordered semantic relocation identities;
// compiler-counter labels are resolved by their actual section role/offset.
// All preserve the raw44-byte, zero-relocation node destructor.
//
// Form1 keeps N/G/R order and the39/18/5 helper bodies. Critically, the emitted
// definition is GLOBAL `?id@@3V0locale@std@@A`, not ctype's static member.
// Therefore this accepted VC6 syntax is NOT proof of correct member-specific
// explicit instantiation. It grows .bss28->36, moving actual EffectsState0->8.
// That growth includes the new id allocation and alignment; do not call it
// merely a four-byte section growth. All three boundaries behave identically.
//
// Form2 emits the real ctype member but removes the one-byte common guard,
// .CRT$XCU contribution, and all39/18/5 helpers. .bss likewise becomes36 and
// state moves to8. All three boundaries are identical. This is incompatible
// with the independently measured retail initializer and storage topology.
//
// Form3 replaces the required guarded39-byte initializer/18-byte wrapper
// with ordinary10/16-byte helpers. The common guard disappears, .bss becomes
//36 and state moves to8. Its helper bodies follow source position (early for
// before-Purge; after public bodies for the other two), but this is the wrong
// initializer family. No fabricated guard or replacement initializer retained.
//
// SECOND COMPLETE DIALECT PRODUCT: after Form1's wrong owner was discovered,
// the following three real ownership forms x the same three boundaries,
// plus a fresh control: all10 compiler invocations completed,7 successes.
//
// 4. namespace std { template locale::id ctype<unsigned short>::id; }
// 5. template class std::ctype<unsigned short>;
// 6. namespace std { template class ctype<unsigned short>; }
//
// Form4 triggers VC6 C1001 at all three boundaries. These are compiler
// rejections, not successful body/order evidence. Form5/6 all preserve12/12
// public body records and keep N before G/R, now sections60/61/62. They emit
//33 extra function records and change other data/EH topology; no link is
// needed to reject them as this native-layout explanation. This narrows the
// tested ownership dialects, not every possible legal static-member syntax.
//
// Artifacts (all relative to matcher-4):
// build/link/audiere-ctype-ownership-115/
//   probe.py, dialect.py, audit.py, probe.log, dialect.log,
//   results.json, audit.json, provenance.json, every source/object/compile.log.
// build/link/audiere-ctype-ownership-dialect-115/
//   results.json, audit.json, provenance.json, every source/log and successful
//   raw object. Reproduce both scripts inside the verified Nix shell; run
//   audit.py with each artifact-directory argument. Flags come unchanged from
//   unit_flags(BASE/AudiereEffects): /Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX /Gy.
//
// Disposition: no source change selected. The verified production executable
// remains SHA29064b1aa592911b31345d3f71535f802876f9e974cf35a360530da958dffde2
// with115 different bytes. These matrices reject the measured G/R-owner
// alternatives and avoid mistaking a silently wrong VC6 symbol for recovery.
