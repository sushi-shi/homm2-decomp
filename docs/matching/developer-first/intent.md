# Developer-written draft

Write one coherent C++98 implementation before comparing its output. This draft
is a source candidate, not a matrix of byte-selected expressions. It deliberately
keeps ordinary headers, real state and straightforward lifetime management.

The Effects module owns its private list and decoding state. The node initializes
its members in its constructor and uses the implicit smart-pointer destructor.
Purge and Find are private to the implementation. Purge traverses the list once
using the link that owns each node, so head and interior removal need the same
code. Playback setup names the sample's existing format record and uses ordinary
conditionals and assignments. The existing public API, serialized layouts and
shared globals are kept; this pass does not redesign the audio subsystem.

DIMMER's small constructors and virtual forwarding methods belong with the class
in its header. Its resource-reading implementation stays in its cpp file and uses
explicit field assignments. The class has no invented template owner or separate
destructor source file. REQUEST's empty-string copies are simple statements with
literal operands; its existing global default-filename pointer remains.

Freeze all these files and their hashes before the first compilation. Check
compilation, ordinary full-game linking, emitted bodies, relocation identities,
data, and placement afterward. A lower match is an observation, not a reason to
undo this draft or revert to source that was chosen only for layout.

One behavior question requires explicit review: the single-loop Purge unlinks
nodes before destroying them, including the head. This avoids leaving a dangling
head visible during destruction, but the old implementation kept the head linked
until its destructor returned. If stream destruction reenters Effects, this can
be observable. The draft records that difference rather than calling its
semantics proven solely from the final list contents.
