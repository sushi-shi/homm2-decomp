/*
VC6 measured experiment: existing Music TU owns the deferred node destructor.
Date: 2026-09-27. Diagnostic only; no production source changed.
Worktree matcher-3, branch matcher/bss-dimmer; verified cwd and HOMM2_DIR.

Parent: the exact three-owner Audiere split (Purge, Effects, N-only provider).
Purge has the narrow Audiere declarations; Effects owns the inline constructor
specialization and full Audiere SDK declarations. Music source is byte-identical
to matcher-4/src/BASE/AudiereMusic.cpp at this experiment's snapshot.

Artifacts: build/audiere-music-node-owner/{run.py,results.json,run.log,
 audit.py,audit.json,audit.log,references/}. Each arm preserves all three source
 inputs, include overlays, compile logs and raw .obj files. results.json records
 every section byte, symbol and ordered relocation before comparison.

Complete product: seven arms, three owners each, 21 successful VC6 compiles.
  primary-before             primary inline dtor, Music class request before globals
  primary-eof                primary inline dtor, Music class request at EOF
  specialized-inline-none    shared explicit inline dtor specialization; no request
  specialized-inline-before  same; Music class request before globals
  specialized-inline-eof     same; Music class request at EOF
  concrete-inline-none       real concrete node, shared out-of-class inline dtor
  concrete-inclass-none      real concrete node, shared in-class inline dtor
The first two are previously measured control forms, not novel discoveries.

Results (N is the exact 44-byte destructor, zero relocations):
  Arm                         Purge N      Effects N     Music N
  primary-before              absent       absent        section 30, late
  primary-eof                 absent       absent        section 30, late
  specialized-inline-none     section 4    absent        absent
  specialized-inline-before   section 4    absent        section 5, first function
  specialized-inline-eof      section 4    absent        section 5, first function
  concrete-inline-none        section 4    absent        absent
  concrete-inclass-none       section 4    absent        absent

Every emitted N has COFF characteristics 0x60501020 and selection ANY (2).
All are byte-identical to the exact N-only provider; none has relocations.
The new specialization/request product genuinely schedules N before Music's
stream/source initialization helpers. Music's first real helper _$E20 moves
from section 5 to section 6; all 23 original functions retain their order.

Across every arm, excluding the added N section:
  Purge: 1/1 functions exact bytes and ordered relocations;
  Effects: 17/17 public/helper functions exact bytes and ordered relocations;
  Music: 23/23 public/helper functions exact bytes and ordered relocations.
All complete code/initialized-data/uninitialized-data section sequences match
those references, including bytes, characteristics, symbol offsets and ordered
relocations. Thus Music static data, guards, helper bodies and CRT contributions
are unchanged. Play remains exactly 924 bytes, Purge exactly 354 bytes.
Comparison recognizes template/concrete node identity; compiler-local labels
use corresponding section and offset. Raw names/sections/relocations remain in
results.json. References are copied raw three-owner Purge/Effects objects and
previous unchanged-source music-control object; current Music source equality
was separately checked. No normalized object was linked.

ODR/ownership: each arm uses one identical node class/member definition in all
three owners. The specialized destructor is declared/defined before any use or
class instantiation in every owner. Its constructor specialization remains
Effects-owned. Only Purge uses the established narrow SDK view and extern-template
request. VC6's extern-template class suppression does not suppress the explicit
inline destructor specialization. Concrete arms preserve the actual three fields
and constructor semantics; they do not add references or fake roots.

Disposition: preserve specialized-inline-before and specialized-inline-eof as
new scheduling evidence, not an accepted ownership solution. Purge is loaded
before Music in the successful split ordering, and now supplies the first ANY
copy of N immediately after Purge. Ordinary first-provider selection therefore
keeps the early Purge contribution, defeating the desired Music placement. This
experiment did not replay the full native link; the duplicate and its selection
flags are raw object evidence, while the placement consequence follows the
established LINK selection behavior. Removing the inline definition only from
Purge would evade required inline-definition visibility and is not a solution.
No further forms were tested beyond this bounded product.
*/

// Generic control, shared by all owners:
#if 0
template<class Resource>
struct AudiereSampleListNode {
    audiere::OutputStreamPtr stream;
    Resource* sampleResource;
    AudiereSampleListNode<Resource>* next;
    inline AudiereSampleListNode(Resource*, AudiereSampleListNode<Resource>*);
    inline ~AudiereSampleListNode();
};
template<> inline AudiereSampleListNode<sample>::AudiereSampleListNode(
    sample*, AudiereSampleListNode<sample>*);

// Primary arms use this definition:
template<class Resource>
inline AudiereSampleListNode<Resource>::~AudiereSampleListNode() {}

// Specialized arms replace that primary definition with this shared definition:
template<> inline AudiereSampleListNode<sample>::~AudiereSampleListNode() {}

// Only Purge, following the definition:
extern template struct AudiereSampleListNode<sample>;

// Music request arms: before the real global definitions, or at source EOF.
template struct AudiereSampleListNode<sample>;

// Concrete arms replace the generic node with this actual concrete layout.
struct AudiereSampleNode {
    audiere::OutputStreamPtr stream;
    sample* sampleResource;
    AudiereSampleNode* next;
    inline AudiereSampleNode(sample*, AudiereSampleNode*);
    inline ~AudiereSampleNode(); // in-class arm instead uses: ~AudiereSampleNode() {}
};
inline AudiereSampleNode::~AudiereSampleNode() {}
// Concrete constructor retains the identical Effects-owned inline body.
#endif
