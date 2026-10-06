// Target: ??1AudiereSampleNode at retail 0x004cd050, after the AudiereEffects
// ctype initializer pair. Retail order of the TU tail is S A G R N:
// S = RefPtr<OutputStream> destructor, A = its assignment, G/R = guarded ctype
// id initializer and its atexit registration, N = the node destructor.
// The RefPtr<AudioDevice> destructor copy is selected from an earlier owner.
//
// Measured 2026-10-06 on branch work/exact-hash-from-homm1 with the pinned VC6
// SP5 compiler and unit_flags(BASE/AudiereEffects)
// (/Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX /DNO_STRICT /Gy). Objects are disposable;
// section order was read from the COFF section table. No source is retained.
//
// Retail facts used:
// - Purge calls N out of line (two REL32 calls); Play expands N, and through
//   it the RefPtr destructor, inline at its own `delete`; no other caller.
// - The Rich header already matches with the current object count, so N is
//   not supplied by an additional object.
//
// Arm 1: class template node with an in-class destructor.
//   template<class Resource> struct AudiereNode {
//       audiere::OutputStreamPtr stream; Resource* sampleResource;
//       AudiereNode* next;
//       AudiereNode(Resource* resource, AudiereNode* nextNode) { ... }
//       ~AudiereNode() {}
//   };
//   typedef AudiereNode<class sample> AudiereSampleNode;
// Section tail: A D N S G R (D = RefPtr<AudioDevice> destructor). Template
// members move together to the end-of-parse phase, but N stays before the
// ctype pair. Not linked.
//
// Arm 2: N defined inline between FindAudiereSample and PlayAudiereSample.
// N is emitted at its definition (section 6, before Play); Play keeps 0x39c.
// An inline function already referenced out of line is emitted where its body
// appears, so no ordinary definition position reaches the post-initializer
// slot. Not linked.
//
// Arm 3: N declared out of line in soundBackends.h and defined first in
// AudiereMusic.cpp, before AudiereMusic's static members.
//   VA(0x004cd050, 0x2c)
//   AudiereSampleNode::~AudiereSampleNode() {}
// Object order becomes Effects: ... S A D G R, Music: N, $E..., i.e. the
// retail cross-object order. But Effects can no longer expand N in Play:
// Play calls ??1AudiereSampleNode instead of the inline RefPtr release and
// shrinks by 16 bytes, displacing everything after it. Full historical link
// (redelink + build): 116,738 differing bytes, 147 displaced project
// functions, against the 500-byte parent. Rejected.
//
// Disposition: the retail tail needs N emitted after the initializer pair
// while Play still sees its body. None of these arms does both. The earlier
// ownership matrices in this directory bound the remaining forms.
