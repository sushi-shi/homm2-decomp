// Broad structural owner exploration, measured 2026-09-27 with VC6 SP5.
// Worktree .claude/worktrees/matcher-2, branch matcher/bss-audiere; cwd,
// branch and HOMM2_DIR verified in one persistent Nix shell. Disposable
// overlays only; no production files, fake storage, or cleanup calls added.
// The parameterized roles below hold actual Resource*/StreamPtr state.
//
// First complete structural product (six arms plus fresh plain control):
//   {nested generic state owner, stream base plus link-metadata base}
//   x {primary destructor definition at EOF, class body, implicit destructor}.
// All seven compile. Every state is retained, including the regressed branch.
//
// Order projection below omits AudioDevice's destructor (whose selected copy
// is elsewhere in the linked image) and the final five-byte ctype cleanup.
// S = OutputStream RefPtr destructor; A = its assignment; G/R = ctype
// guard/atexit-registration helpers; N = node destructor. Retail: S A G R N.
// arm                         projection    public exact vs control
// plain control               N S A G R     12/12
// nested generic EOF          A S G R       11/12 (N undefined)
// nested generic class body   N A S G R     12/12
// nested generic implicit     N S A G R     12/12 (N emits early, section5)
// two bases EOF               A S N G R     12/12
// two bases class body        A N S G R     12/12
// two bases implicit          N S A G R     12/12 (N emits early, section5)
//
// All exact claims here compare each public function's size, every masked
// byte, and complete ordered relocation sites/types/semantic identities/raw
// addends to the freshly compiled control; they are not a linked-image claim.
// Anonymous EH identities map by section role and value; Find/node names map
// only to the corresponding source owners. Both EH sections also compare in
// raw bytes and complete ordered relocations for every successful arm.
//
// The regressed nested-EOF parent has Play 0x385, 41 relocations (control
// 0x39c,40). First masked-byte difference is +0x58 (a branch displacement);
// first ordered-relocation difference is record32. Its cleanup is a call
// instead of the required inline body. Purge and the other ten public methods
// remain exact. This branch was explored further, not discarded by score.
//
// Descendant family 1: primary definition position {EOF, header before first
// concrete typedef} x explicit owner instantiation {none, outer, nested}.
// Reuses already measured EOF/none parent; five additional compiles complete
// the six-arm product. The three other compiling arms all retain undefined N
// and the same Play regression. Both nested-class instantiations fail C2946:
//   'EffectsState<...>::SampleNode' is not a template-class specialization.
// Explicit outer instantiation does not instantiate this nested destructor.
//
// Descendant family 2 follows that measured nested-definition limitation:
// concrete member qualification {full concrete EffectsState type, typedef}
// x {ordinary concrete definition, template<> specialization definition}.
// All four compile, restore all twelve exact public functions and EH, and
// emit N in section18 before S/A/G/R. Thus concrete nested-member ownership
// recovers the definition/inlining lost by the primary nested definition,
// but returns to the existing wrong order. The lower-scoring branch's
// descendants were measured before rejecting it as a solution.
//
// Total: 16 invocations, 14 success, two explicit-instantiation syntax
// rejections, no timeouts/truncation. Nine non-control arms preserve all12;
// four preserve11 with the same Play regression. Every successful explicit
// node body remains 0x2c. No arm places N after G/R. No full link warranted.
//
// Actual topology rank retains missing-role count separately from inversion
// count against S A G R N. Two-bases EOF has the closest complete projection
// (3 inversions); implicit/control projections have4, nested class body5.
// Undefined-node branches have1 inversion among present roles but one missing
// role, so they are never mistaken for a better complete solution. These
// ranking components are diagnostic, not semantic correctness predicates.
//
// Reproducible artifacts: build/audiere-structural-list/
// probe.py, descendant.py, specialization.py, audit.py, rank.py;
// provenance.json, results.json, audit.json, audit.log, topology-rank.json;
// each arm preserves full source/header, compiler log and object on success.
// Run these five scripts in the listed order in the verified Nix build shell.
// Compile flags from unit_flags(BASE/AudiereEffects), /Od /Ob1 /GX /Gy /MT.
// probe/audit use the reviewed COFF inspector, not normalized link objects.
// Disposition: preserve every measured branch as evidence; no retained code
// change and no universal claim about all generic-list implementations.

// First source family. Class-body and implicit variants replace/remove only
// the explicit destructor declaration and omit its EOF definition.
template<class Resource, class StreamPtr>
struct EffectsState {
    struct SampleNode {
        StreamPtr stream;
        Resource* sampleResource;
        SampleNode* next;
        SampleNode(Resource* resource, SampleNode* nextNode) {
            stream = NULL;
            sampleResource = resource;
            next = nextNode;
        }
        H2_RETAIL_INLINE ~SampleNode();
    };
    void* buffer;
    i32 frameCount;
    i32 channelCount;
    i32 sampleRate;
    audiere::SampleFormat sampleFormat;
    SampleNode* sampleList;
    i32 sampleIterationDepth;
};
typedef EffectsState<sample, audiere::OutputStreamPtr> AudiereEffectsState;
typedef AudiereEffectsState::SampleNode AudiereSampleNode;

// EOF primary definition (or before concrete typedef in header descendant):
template<class Resource, class StreamPtr>
H2_RETAIL_INLINE EffectsState<Resource, StreamPtr>::SampleNode::~SampleNode() {}

// Explicit owner instantiation alternatives (not simultaneously present):
// template struct EffectsState<sample, audiere::OutputStreamPtr>;
// template struct EffectsState<sample, audiere::OutputStreamPtr>::SampleNode;

// Concrete member-definition descendants replace the primary definition:
// [template<>] H2_RETAIL_INLINE
// EffectsState<sample, audiere::OutputStreamPtr>::SampleNode::~SampleNode() {}
// [template<>] H2_RETAIL_INLINE
// AudiereEffectsState::SampleNode::~SampleNode() {}

// Second independent source family. The stream base supplies offset0 storage;
// the nonvirtual metadata base supplies the real resource/next fields.
template<class Resource, class Node>
struct SampleLink {
    Resource* sampleResource;
    Node* next;
};
template<class Resource, class StreamPtr>
struct AudiereListNode : StreamPtr,
                        SampleLink<Resource, AudiereListNode<Resource, StreamPtr> > {
    AudiereListNode(Resource* resource, AudiereListNode<Resource, StreamPtr>* nextNode) {
        static_cast<StreamPtr&>(*this) = NULL;
        sampleResource = resource;
        next = nextNode;
    }
    H2_RETAIL_INLINE ~AudiereListNode();
};
// typedef AudiereListNode<sample, audiere::OutputStreamPtr> AudiereSampleNode;
template<class Resource, class StreamPtr>
H2_RETAIL_INLINE AudiereListNode<Resource, StreamPtr>::~AudiereListNode() {}

// Call-site mechanical transformations for second family:
// node->stream->method(...) -> (*node)->method(...), likewise other real nodes.
// !gAudiereEffects.sampleList->stream -> !*gAudiereEffects.sampleList
// gAudiereEffects.sampleList->stream = device->openBuffer(...)
//   -> static_cast<audiere::OutputStreamPtr&>(*gAudiereEffects.sampleList)
//        = device->openBuffer(...)
// Allocation, deallocation, traversal and resource use remain unchanged.
