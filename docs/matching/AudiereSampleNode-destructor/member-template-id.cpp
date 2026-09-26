// VC6 SP5 member-instantiation dialect matrix, measured 2026-09-27.
// Worktree: .claude/worktrees/matcher-2, matcher/bss-audiere; cwd/branch/
// HOMM2_DIR reverified in the persistent Nix shell before compilation.
// Root replay on decomp-gold-2.1-buka repeated all nine compiles and the full
// public-function/EH ordered-relocation audit with the same results; logs are
// root-probe.log and root-audit.log in the artifact directory below.
// No production source/header changes and no object/output corrections.
//
// Motivation: extern-template-ownership.cpp measured C2631 for only the bare
// destructor spelling in a member-specific extern-instantiation declaration.
// That did not establish whether VC6 accepts a full template-id spelling.
// Member-specific suppression is distinct from whole-class suppression, which
// removes Play's required inline destruction. The real Resource* template
// parameter and all caller bodies remain those of the reviewed parent.
//
// Complete 2 x 4 product, plus freshly compiled plain-node control:
//   primary destructor declaration AND EOF definition:
//       ~AudiereSampleListNode() / ~AudiereSampleListNode<Resource>()
//   ownership after typedef in header:
//       local / extern class / extern bare member / extern full-id member
// All nine compiler invocations completed, no truncation or timeout.
// Five succeed; the four member-specific arms fail with the SAME diagnostic:
//   error C2631: '__dtor' : destructors not allowed a return type
// The errors identify the extern member declaration, not the primary body.
// Thus VC6 accepts the full primary spelling in this product but rejects both
// tested member-only extern spellings under either primary spelling.
//
// Both local template arms versus fresh plain-node control:
// - All twelve public functions have equal size, relocation-masked bytes and
//   complete ordered relocation sites/types/semantic identities/raw addends.
// - Purge remains 0x162, Play 0x39c with 40 relocations.
// - .text$x/.xdata$x raw bytes and ordered relocations also match.
// - Node destructor is defined, section21 size0x2c; ctype E helpers follow
//   in sections22/23 (sizes0x27/0x12), cleanup section24 size5.
// - The preceding helper order is assignment section18, AudioDevice destructor
//   section19, OutputStream destructor section20: no retail tail recovery.
//
// Both extern-class template arms versus fresh plain-node control:
// - Eleven public functions remain equal by the same complete predicate.
// - Play changes to 0x385, 41 relocations, and loses inline node destruction.
// - Node destructor has COFF section0 (undefined); E helpers move to21/22.
// - The EH sections still agree in bytes and ordered semantic relocations.
// These independently reproduce the parent's whole-class contradiction.
// Compiler-counter identities are resolved by section role and symbol value;
// reviewed node/Find template names correspond to their actual source owners.
//
// Disposition: reject these member-only dialect spellings; retain no source
// change. This extends the measured syntax boundary, not a universal claim
// that every possible VC6 member-instantiation syntax is unsupported. No full
// link is justified: the member-only arms do not compile, local forms have
// known wrong helper order, and whole-class forms contradict Play's body.
//
// Artifacts/reproducer: build/audiere-member-template-id/
//   probe.py, audit.py, provenance.json, results.json, audit.json;
//   every arm retains full source/header, compile.log, and object if successful.
// probe.py reuses the setup/inspector from the retained
// build/link/audiere-extern-template/probe.py without executing its matrix.
// Flags are unit_flags(BASE/AudiereEffects), including /Od /Ob1 /GX /Gy /MT.
// Commands in the verified worktree build shell:
//   python3 build/audiere-member-template-id/probe.py
//   python3 build/audiere-member-template-id/audit.py

// Full template-id primary arm, compared independently with its bare spelling:
template<class Resource>
struct AudiereSampleListNode {
    audiere::OutputStreamPtr stream;
    Resource* sampleResource;
    AudiereSampleListNode<Resource>* next;
    AudiereSampleListNode(Resource* resource,
                         AudiereSampleListNode<Resource>* nextNode) {
        stream = NULL;
        sampleResource = resource;
        next = nextNode;
    }
    H2_RETAIL_INLINE ~AudiereSampleListNode<Resource>();
};
typedef AudiereSampleListNode<sample> AudiereSampleNode;

// Ownership arms are mutually exclusive; local has no additional declaration:
// extern template struct AudiereSampleListNode<sample>;
// extern template AudiereSampleListNode<sample>::~AudiereSampleListNode();
// extern template AudiereSampleListNode<sample>::~AudiereSampleListNode<sample>();

// The definition is at EOF after the unchanged twelve real methods:
template<class Resource>
H2_RETAIL_INLINE
AudiereSampleListNode<Resource>::~AudiereSampleListNode<Resource>() {}
