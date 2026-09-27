// Typed Purge owner: complete structural product, measured2026-09-27.
// Lane matcher-2 / matcher/bss-audiere; cwd and HOMM2_DIR verified in the
// persistent Nix shell. Actual115 constructor-specialization parent.
// Disposable source/header overlays only; no production or matcher-4 writes.
//
// Prior generic-state, nested-node, stream/link-base and destructor ownership
// matrices left Purge as an ordinary free function. This new family changes
// the actual cleanup owner: a real Resource-parameterized state type, storing
// the existing Node<Resource>* list, owns a static zero-argument Purge method
// and one instance of the existing seven-field state. There are no added
// wrapper calls, aliases, initialization triggers, fields or template tags.
// The Resource parameter determines real list/node/resource pointer types.
//
// All callers directly call AudiereEffectsState<sample>::PurgeFinishedSamples;
// all state references directly use AudiereEffectsState<sample>::state.
// The existing function ABI is preserved by a static member: no this or new
// argument. Under /Gr its mangling has SIXXZ versus the free function YIXXZ.
// Node's concrete ctor specialization and primary inline EOF dtor stay intact.
//
// COMPLETE first product: cleanup body {primary member, concrete sample
// specialization} x instantiation {implicit, explicit member, explicit class}.
// Body and explicit instantiation occupy original Purge position. Six arms
// plus freshly compiled original control. All seven compile without warnings.
//
// Results:
// - Concrete-specialized body: all three arms retain all12 exact public sizes,
//   non-relocation bytes, ordered sites/types/identities/addends and Play EH.
//   Purge is first text body, section4,0x162. Play remains0x39c. Actual state
//   and Purge names map to their corresponding real original semantic owners.
//   Node stays section21, before ctype sections22/23: S A N G R, not closure.
// - Primary member, implicit or explicit-member instantiation: VC6 expands
//   Purge into its four callers. No out-of-line Purge definition;7/12 public
//   bodies remain exact. Play grows0x39c->0x6a7 (60 relocs), Stop0x70->0x1ca,
//   Wait0x62->0x1bc, StopAll0x77->0x1cd. Play EH remains exact. A real46-byte
//   scalar deleting dtor is emitted in section8. Node remains section21,
//   before ctype. Explicit member instantiation does not force a body here.
// - Primary member, explicit class instantiation: same four caller changes,
//   but also emits an exact0x162 Purge as section22, after Node and before
//   ctype. Thus8/12 bodies exact, but Purge loses required first placement.
//
// COMPLETE descendant product: primary-body position {original, EOF} x
// instantiation {implicit, explicit member, explicit class}. The three original
// states above are reused and three EOF states compiled. Explicit instantiation
// stays at original Purge position; only its real definition moves after the
// primary Node dtor at EOF. All three compile without warnings and reproduce
// their corresponding original-position outcomes, including lower states.
// Total10/10 compiler invocations, no truncation or timeout.
//
// The four changed callers are retained with complete ordered-relocation and
// first-divergence evidence, not rejected merely by score. Play first differs
// at+0x18 (frame), ordered relocation record3; other changed callers first
// differ at+5, ordered relocation record1. Missing Purge is recorded separately
// from inexact bodies. Every successful arm keeps the actual44-byte Node dtor
// before G/R; no alternate late-helper orbit appears in this measured family.
//
// Storage audit, all arms: exactly one state object,28 uninitialized bytes,
// section3 .bss, offset0, original section flags0xc0400080. All seven field
// accesses retain original addends in exact arms. No padding or data added.
// IMPORTANT ownership difference: free state has COFF STATIC class3; the
// class static member has EXTERNAL class2. This is explicitly a linkage change,
// not a claim of full storage-class equality or native image equivalence.
// .bss sections and every state-target relocation are preserved in storage.json.
// No native link is warranted by these states: their helper order is wrong,
// and the ordinary member-state linkage would need independent layout review.
//
// Reviewed identity correspondences for comparison only (no COFF rewriting):
// ?PurgeFinishedSamples@?$AudiereEffectsState@Vsample@@@@SIXXZ
//      -> ?PurgeFinishedAudiereSamples@@YIXXZ
// ?state@?$AudiereEffectsState@Vsample@@@@2U1@A -> _gAudiereEffects
// Anonymous Play EH labels map by its actual handler/xdata owner and offset.
//
// Artifacts: build/audiere-purge-owner/
// probe.py, descendant.py, audit.py, storage.py, provenance.json, results.json,
// audit.json, summary.json, storage.json, and all source/header/object/log arms.
// Replay probe.py, descendant.py, audit.py, storage.py in verified Nix shell.
// Production unit flags remain /Od /Ob1 /GX /Gy /MT and unchanged other flags.
//
// Disposition: preserve this real cleanup-owner family and its regressed
// descendants as evidence. Concrete specialization preserves the public prefix
// but not the tail; primary instantiation moves/inlines the prefix but not N.
// Neither result proves all possible generic cleanup owners impossible. No
// generated code, pragma, fake reference or new compiler input is retained.

template<class Resource>
struct AudiereEffectsState {
    typedef AudiereSampleListNode<Resource> SampleNode;
    void* buffer;
    i32 frameCount;
    i32 channelCount;
    i32 sampleRate;
    audiere::SampleFormat sampleFormat;
    SampleNode* sampleList;
    i32 sampleIterationDepth;
    static AudiereEffectsState<Resource> state;
    static void PurgeFinishedSamples(void);
};

template<>
AudiereEffectsState<sample> AudiereEffectsState<sample>::state = H2_ZERO_INIT;

// Specialized arm replaces template<class Resource> with template<> and
// AudiereEffectsState<Resource> with AudiereEffectsState<sample> below.
template<class Resource>
void AudiereEffectsState<Resource>::PurgeFinishedSamples(void) {
    if (state.sampleList == NULL)
        return;
    SampleNode* head = NULL;
    for (;;) {
        if (!state.sampleList->stream->isPlaying()) {
            head = state.sampleList->next;
            delete state.sampleList;
            state.sampleList = head;
            if (state.sampleList == NULL)
                return;
        } else {
            break;
        }
    }
    SampleNode* current = state.sampleList->next;
    SampleNode* cursor = state.sampleList;
    while (current != NULL) {
        if (!current->stream->isPlaying()) {
            cursor->next = current->next;
            delete current;
            current = cursor->next;
        } else {
            cursor = current;
            current = current->next;
        }
    }
}

// Mutually exclusive explicit-instantiation arms, or neither:
// template void AudiereEffectsState<sample>::PurgeFinishedSamples(void);
// template struct AudiereEffectsState<sample>;
