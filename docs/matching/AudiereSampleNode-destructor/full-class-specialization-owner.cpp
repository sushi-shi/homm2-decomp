/*
VC6 SP5 full-class specialization ownership product, 2026-09-27.
Worktree matcher-3, branch matcher/bss-dimmer; cwd, branch and HOMM2_DIR
verified before compilation. Disposable overlays only; no production edits.

Question: can an explicit specialization of the entire real resource-node
class, rather than just its destructor, keep Purge's out-of-line N suppressed
while Music supplies N before its real static-global initialization helpers?
Parent: music-provider-split-parent.cpp's exact three-owner source split.

Prior coverage reviewed first:
 - generic-list-ownership.cpp (matcher-2): nested generic state owner with
   concrete nested SampleNode. Direct nested-class instantiation rejects C2946;
   outer generic class instantiation does not instantiate nested destructor.
   Ordinary/template<> concrete member definitions recover the body but emit
   N before the required helpers. These are not new untried request forms.
 - extern-template-ownership.cpp and member-template-id.cpp (matcher-2):
   extern-template destructor declarations with bare and full template-id
   spellings both reject C2631; full primary destructor spellings were crossed.
 - No full node-class specialization product was present in those dossiers.

Complete new product, three identical-definition owners per cell:
   full-class specialization destructor definition:
     class body / shared header out-of-class / identical inline definition
     appended at EOF in each of Purge, Effects and Music
   x Music whole-class explicit-instantiation definition:
     absent / before real globals / EOF after any member definition.
Purge has the existing extern-template whole-class declaration in all cells.
The generic primary retains actual Resource* and next-node roles. The complete
specialization has the same concrete stream/sample/next fields and the same
constructor/destructor declarations in all owners. Its ordinary inline
constructor definition remains Effects-owned. Each destructor body remains
visible in every using TU; EOF placement changes declaration order, not access.
The primary destructor definition also remains available in the shared header.

27 invocations complete this 3 x 3 x 3-owner product:
 - All nine Purge objects fail C2908 at the extern-template class declaration.
 - All six requested Music objects fail C2908 at the explicit class request.
 - All nine Effects objects compile successfully.
 - All three Music objects without requests compile successfully, with no N.
Diagnostic (raw logs retained):
   explicit specialization; 'AudiereSampleListNode<class sample>' has already
   been instantiated from the primary template
This is a measured VC6 rejection, not an assertion that the request spelling is
universally invalid in other implementations. Requesting before declaring the
specialization would instead instantiate the primary before that specialization;
that is not a legitimate ownership repair.

Three additional diagnostic Purge controls remove the suppression request,
covering each definition position. All compile and emit N in section4 directly
after Purge (section3). Every N equals the exact N-only provider's 44 raw bytes,
has no relocations, characteristics 0x60501020, and COMDAT selection ANY (2).
This establishes the ordinary behavior behind the rejected suppression requests.

Complete audits of all 15 successful objects:
 - Effects: all 17 original public/helper functions exact in bytes and ordered
   relocations in all nine cells; Play remains 924 bytes; N absent.
 - Music: all 23 original public/helper functions exact in bytes and ordered
   relocations in the three no-request cells; N absent.
 - Purge: exact 354-byte function and ordered relocations in all three controls.
 - After excluding added N, every complete code/initialized-data/uninitialized-
   data section sequence matches its reference: bytes, flags, symbol offsets,
   and ordered relocations. This includes Music static storage, guards, CRT
   contributions, helpers and Effects EH data. No silent section regression.
References are raw preserved three-owner Purge/Effects and unchanged-source
Music objects from the prior experiment. Reviewed node semantic identity and
compiler-local identities use corresponding section/offset; raw symbol names
and every section/relocation are also retained independently.

Artifacts: build/audiere-full-class-owner/
 run.py, control.py, audit.py, results.json, audit.json, run.log, control.log,
 audit.log, references/, and all per-arm sources/headers/logs/raw objects.
30 compiler invocations total: 15 successes, 15 C2908 rejections, no timeouts.
The product was completed, not truncated. No native link was warranted because
no compiling arm supplies Music N or suppresses the early Purge copy.

Disposition: bounded negative for full-class specialization under these genuine
ownership requests and definition boundaries. Retain the earlier member-only
specialization scheduling result as a distinct active clue. Do not hide the
inline definition from Purge or create inconsistent owner definitions. The
review found no additional evidence-backed member-instantiation declaration
syntax to try; class/struct keyword substitutions and storage-class mixtures
would not introduce a new semantic ownership mechanism. No such spelling sweep
was performed. No universal impossibility claim about all source ownership.
*/

#if 0
// Shared primary: Resource represents actual stored resource identity.
template<class Resource>
struct AudiereSampleListNode {
    audiere::OutputStreamPtr stream;
    Resource* sampleResource;
    AudiereSampleListNode<Resource>* next;
    inline AudiereSampleListNode(Resource*, AudiereSampleListNode<Resource>*);
    inline ~AudiereSampleListNode();
};

// Shared explicit specialization: ordinary members, no template<> on definitions.
template<>
struct AudiereSampleListNode<sample> {
    audiere::OutputStreamPtr stream;
    sample* sampleResource;
    AudiereSampleListNode<sample>* next;
    inline AudiereSampleListNode(sample*, AudiereSampleListNode<sample>*);
    inline ~AudiereSampleListNode(); // class-body arm substitutes: ~AudiereSampleListNode() {}
};

template<class Resource>
inline AudiereSampleListNode<Resource>::~AudiereSampleListNode() {}

// Header arm, or identical definition at EOF in all three owners:
inline AudiereSampleListNode<sample>::~AudiereSampleListNode() {}

// Purge request (all definition-position arms reject C2908):
extern template struct AudiereSampleListNode<sample>;
// Music request at pre-global or EOF boundary (both reject C2908):
template struct AudiereSampleListNode<sample>;

// Effects only, ordinary member of full specialization:
inline AudiereSampleListNode<sample>::AudiereSampleListNode(
    sample* resource, AudiereSampleListNode<sample>* nextNode) {
    stream = NULL;
    sampleResource = resource;
    next = nextNode;
}
#endif
