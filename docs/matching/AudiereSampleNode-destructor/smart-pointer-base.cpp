/*
 * VC6 measured semantic ownership matrix: smart-pointer base, 2026-09-27.
 *
 * Evidence supporting this hypothesis:
 * - vendor/audiere-1.9.2/audiere.h RefPtr<T> owns one pointer, has no virtual
 *   functions, and exposes pointer, bool, and assignment operations.
 * - The node's remaining fields are sampleResource and next. Nothing in the
 *   audited call sites requires the stream storage to be a member rather than
 *   a nonvirtual base at zero offset.
 * - Retail Play's construction-unwind funclet at e9434 passes its saved node
 *   pointer directly to the OutputStream RefPtr destructor at ccf70. Either a
 *   first member or this base model explains that actual pointer/cleanup role.
 *   See typed-unwind-ownership.cpp for the corrected semantic identity; older
 *   delink objects in this worktree may still misname the identical dtor body.
 *
 * Complete structural product, not a spelling sweep:
 * {concrete OutputStreamPtr base, generic RefPtr<Stream> base}
 * x {empty inline destructor defined in header, defined at TU EOF}.
 * Four compiling arms plus a freshly compiled original composition control.
 * Header definitions are out-of-class immediately following the class/typedef.
 * No generated helpers, data, aliases, padding, or directives enter production.
 *
 * Worktree/branch/HOMM2_DIR verified:
 * /home/sheep/Projects/homm2/homm2-buka/.claude/worktrees/matcher-2
 * matcher/bss-audiere
 * All compiles in the same persistent nix develop .#build shell, unit_flags:
 * /nologo /c /Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX /DNO_STRICT /Gy
 *
 * Reproducible artifacts in build/audiere-smartptr-base/:
 * probe.py, audit.py, provenance.json, results.json, audit.json;
 * clean-eof/, concrete-header/, concrete-eof/, generic-header/, generic-eof/
 * contain generated source/header, compile.log, and raw AudiereEffects.obj.
 * sema-rva.txt, sema-callees.txt, sema-strings.txt capture preliminary audit.
 * Run in verified shell:
 *   python3 build/audiere-smartptr-base/probe.py
 *   python3 build/audiere-smartptr-base/audit.py
 * probe.py reuses the retained inspection/compile setup from
 * build/link/audiere-extern-template/probe.py.
 * An initial qualified base::operator= call was rejected by VC6 (C2352).
 * The complete product above uses a standard base-reference conversion for
 * assignment and compiles successfully. No semantic inference uses failures.
 *
 * Results, every arm:
 * - All 12 public functions preserve size, every relocation-masked byte,
 *   ordered relocation site/type/semantic identity/raw addend versus control.
 * - Purge 0x162; its node-dtor calls remain +0x65 and +0x114, addend zero.
 * - Play 0x39c, 40 relocations, preserves its inlined destruction.
 * - Node dtor remains the exact 0x2c control payload without relocations.
 * - Whole .text$x and .xdata$x bytes and ordered relocations/addends match.
 *   Audit maps compiler-counter names by section role and symbol value;
 *   template node/Find names map only to their corresponding semantic owners.
 *   Header concrete arm shifts EH sections from 7/8 to 8/9 because it emits
 *   the node dtor early, immediately after Purge.
 *
 * Raw helper section order (numbers are COFF section indices):
 * arm             node  stream-dtor assignment device-dtor E21 E20 id-cleanup
 * clean-eof        18       19          20          21      22  23     24
 * concrete-header   5       19          20          21      22  23     24
 * concrete-eof     18       19          20          21      22  23     24
 * generic-header   21       20          18          19      22  23     24
 * generic-eof      21       20          18          19      22  23     24
 * Sizes: destructors 0x2c; assignment 0x51; E21 0x27; E20 0x12; id 5.
 * Retail requires OutputStream dtor ccf70 -> assignment ccfa0 -> E helpers
 * cd000/cd030 -> node dtor cd050. No arm gives the required post-E node dtor.
 * Generic base additionally reverses the required stream-dtor/assignment order.
 *
 * Disposition: ownership model is semantically plausible and code-compatible,
 * but does not recover native helper scheduling. No production changes, no
 * full-link claim. This rejects the measured base/destructor product only.
 */

// Concrete-base arm; source has no stream data member in this version.
struct AudiereSampleNode : audiere::OutputStreamPtr {
    class sample* sampleResource;
    AudiereSampleNode* next;

    AudiereSampleNode(class sample* resource, AudiereSampleNode* nextNode) {
        static_cast<audiere::OutputStreamPtr&>(*this) = NULL;
        sampleResource = resource;
        next = nextNode;
    }

    H2_RETAIL_INLINE ~AudiereSampleNode();
};

H2_RETAIL_INLINE AudiereSampleNode::~AudiereSampleNode() {}

// Alternative generic-base arm (mutually exclusive with the concrete arm).
template<class Stream>
struct AudiereSampleListNode : audiere::RefPtr<Stream> {
    class sample* sampleResource;
    AudiereSampleListNode* next;

    AudiereSampleListNode(class sample* resource, AudiereSampleListNode* nextNode) {
        static_cast<audiere::RefPtr<Stream>&>(*this) = NULL;
        sampleResource = resource;
        next = nextNode;
    }

    H2_RETAIL_INLINE ~AudiereSampleListNode();
};

typedef AudiereSampleListNode<audiere::OutputStream> AudiereSampleNode;

template<class Stream>
H2_RETAIL_INLINE AudiereSampleListNode<Stream>::~AudiereSampleListNode() {}

/*
 * Exact call-site transformation for both ownership arms:
 * node->stream->method(...)       -> (*node)->method(...)
 * !gAudiereEffects.sampleList->stream -> !*gAudiereEffects.sampleList
 * gAudiereEffects.sampleList->stream = device->openBuffer(...)
 *   -> static_cast<audiere::OutputStreamPtr&>(*gAudiereEffects.sampleList)
 *        = device->openBuffer(...)
 * The first rule applies also to current, sampleNode and sampleList.
 * All next/sampleResource uses, allocation and deletion remain unchanged.
 */
