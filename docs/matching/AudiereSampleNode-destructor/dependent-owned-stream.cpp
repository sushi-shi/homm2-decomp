/*
 * VC6 measured structural matrix: dependent owned-stream member, 2026-09-26.
 *
 * Hypothesis: the EOF queue-C specialization work described in
 * docs/compiler-re-allocation-order.md might defer a node destructor when its
 * owned smart pointer itself depends on a template parameter. Earlier local
 * Resource* node probes left OutputStreamPtr nondependent. This parameter
 * models the real owned stream type; it is not an unused template tag.
 *
 * The older template-real source/object artifacts referenced by compiler docs
 * were unavailable in both this worktree and root. Therefore this result is
 * distinguished from the preserved Resource* matrix, not claimed to be new
 * relative to every historical undocumented template spelling.
 *
 * Complete product: {interface, pointer} x {header, EOF} = 4 arms, plus a
 * freshly compiled unchanged-source control. All 5 compiles succeeded.
 * The definitions below show the EOF forms; header arms move only the same
 * destructor definition immediately after the typedef in soundBackends.h.
 * Source uses of AudiereSampleNode remain unchanged in every arm.
 *
 * Verified worktree/branch/HOMM2_DIR:
 * /home/sheep/Projects/homm2/homm2-buka/.claude/worktrees/matcher-2
 * matcher/bss-audiere
 * Compiled in one persistent nix develop .#build shell with unit_flags:
 * /nologo /c /Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX /DNO_STRICT /Gy
 *
 * Artifacts under build/audiere-owned-stream/:
 * probe.py, audit.py, provenance.json, results.json, audit.json;
 * each arm contains generated .cpp/header, raw .obj and compile.log.
 * Reproduce in the worktree's Nix shell:
 *   python3 build/audiere-owned-stream/probe.py
 *   python3 build/audiere-owned-stream/audit.py
 * probe.py reuses inspect/run_compile setup from the retained
 * build/link/audiere-extern-template/probe.py; no production file is changed.
 *
 * Result in ALL FOUR arms:
 *   public functions sections 4,5,6,9..17: identical to fresh control in
 *     sizes, every relocation-masked byte, and all ordered relocation sites,
 *     types, semantic target identities, and raw addends.
 *   Purge size 0x162, node calls at +0x65/+0x114, both addend zero.
 *   Play size 0x39c, 40 relocations, retains inline node destruction.
 *   Node/Find symbols change naturally with the template type. The only
 *     other public relocation spelling change is Play+6's EH label
 *     $L55809 -> $L55821. Both identify section 7 +0x1d. Entire .text$x
 *     section 7 and .xdata$x section 8 bytes and ordered relocations match
 *     after resolving compiler-counter names by section/value identity.
 *
 * Raw helper topology (section, size), ALL FOUR arms:
 *   18 0x51 RefPtr<OutputStream>::operator=
 *   19 0x2c RefPtr<AudioDevice>::~RefPtr
 *   20 0x2c RefPtr<OutputStream>::~RefPtr
 *   21 0x2c AudiereSampleListNode<...>::~AudiereSampleListNode
 *   22 0x27 _$E21
 *   23 0x12 _$E20
 *   24 0x05 std::ctype<unsigned short>::id cleanup
 * Fresh plain-node control instead emits node/stream-dtor/assignment/device-
 * dtor in sections 18/19/20/21, followed by the same E helpers 22/23.
 *
 * This family changes template helper scheduling but does NOT enqueue the
 * node destructor after the ctype E helpers. It also reverses retail's proven
 * OutputStream destructor -> assignment order (retail ccf70 -> ccfa0).
 * Retail requires node destructor at cd050, after E helpers cd000/cd030.
 *
 * Disposition: rejected for native layout recovery; no production changes,
 * no linker proof claimed. The comparison establishes preservation versus
 * the current plain-source control, not a new retail-exact linked binary.
 * No TU-state spelling census is justified by this unchanged wrong emission
 * phase. This narrows dependent owned-member templates, not all imaginable
 * semantic ownership or late-instantiation paths.
 */

// Interface-parameter arm: the smart pointer's pointee is dependent.
template<class Stream>
struct AudiereSampleListNode {
    audiere::RefPtr<Stream> stream;
    class sample* sampleResource;
    AudiereSampleListNode* next;

    AudiereSampleListNode(class sample* resource, AudiereSampleListNode* nextNode) {
        stream = NULL;
        sampleResource = resource;
        next = nextNode;
    }

    H2_RETAIL_INLINE ~AudiereSampleListNode();
};

typedef AudiereSampleListNode<audiere::OutputStream> AudiereSampleNode;

template<class Stream>
H2_RETAIL_INLINE AudiereSampleListNode<Stream>::~AudiereSampleListNode() {}

/*
 * Pointer-parameter arm substitutes these exact spans:
 *   template<class Stream> -> template<class StreamPtr>
 *   audiere::RefPtr<Stream> stream -> StreamPtr stream
 *   AudiereSampleListNode<audiere::OutputStream>
 *     -> AudiereSampleListNode<audiere::OutputStreamPtr>
 *   AudiereSampleListNode<Stream>:: -> AudiereSampleListNode<StreamPtr>::
 * Both parameter choices therefore model the actual owned stream field.
 */
