from pathlib import Path
import sys
root=Path.cwd();base=root/'build/developer-first-draft';base.mkdir(parents=True, exist_ok=True)
a=(root/'src/BASE/AudiereEffects.cpp').read_text();label=sys.argv[1] if len(sys.argv)>1 else 'draft'
b=(root/'docs/matching/developer-first'/label/'src/BASE/AudiereEffects.cpp').read_text()
def body(s,name):
 s=s[s.index('VA(0x004cc740'):s.index('VA(0x004cc8b0')];s=s[s.index('\n')+1:];return s.replace('PurgeFinishedAudiereSamples',name)
s='''#include <vector>
#include <cassert>
#include <cstdio>
#include <cstddef>
struct AudiereSampleNode;
struct State { AudiereSampleNode* sampleList; } gAudiereEffects;
std::vector<int> released;
std::vector<int> observedHeads;
std::vector<int> queries;
struct Stream {
    int id;
    bool playing;
    bool isPlaying() { queries.push_back(id); return playing; }
};
struct AudiereSampleNode {
    Stream* stream;
    AudiereSampleNode* next;
    ~AudiereSampleNode() {
        released.push_back(stream->id);
        observedHeads.push_back(gAudiereEffects.sampleList ? gAudiereEffects.sampleList->stream->id : -1);
        delete stream;
    }
};
'''+body(a,'OriginalPurge')+'\n'+body(b,'DraftPurge')+'''
struct Result {
    std::vector<int> kept, deleted, heads, calls;
};
Result run(int length, unsigned mask, void (*purge)()) {
    gAudiereEffects.sampleList = NULL;
    for (int i = length - 1; i >= 0; --i) {
        Stream* stream = new Stream;
        stream->id = i;
        stream->playing = (mask & (1u << i)) != 0;
        AudiereSampleNode* node = new AudiereSampleNode;
        node->stream = stream;
        node->next = gAudiereEffects.sampleList;
        gAudiereEffects.sampleList = node;
    }
    released.clear(); observedHeads.clear(); queries.clear();
    purge();
    Result r;
    for (AudiereSampleNode* n = gAudiereEffects.sampleList; n; n = n->next)
        r.kept.push_back(n->stream->id);
    r.deleted = released; r.heads = observedHeads; r.calls = queries;
    while (gAudiereEffects.sampleList) {
        AudiereSampleNode* n = gAudiereEffects.sampleList;
        gAudiereEffects.sampleList = n->next;
        delete n;
    }
    return r;
}
int main() {
    int cases = 0, changedObservations = 0;
    for (int length = 0; length <= 8; ++length) {
        for (unsigned mask = 0; mask < (1u << length); ++mask) {
            Result original = run(length, mask, OriginalPurge);
            Result draft = run(length, mask, DraftPurge);
            std::vector<int> kept, dead;
            for (int i = 0; i < length; ++i)
                ((mask & (1u << i)) ? kept : dead).push_back(i);
            assert(original.kept == kept && draft.kept == kept);
            assert(original.deleted == dead && draft.deleted == dead);
            assert(original.calls == draft.calls);
            if (original.heads != draft.heads) ++changedObservations;
            ++cases;
        }
    }
    std::printf("%d list cases preserve retained nodes, release order, and query order.\\n", cases);
    std::printf("%d cases expose different head state during release.\\n", changedObservations);
}
'''
(base/('semantics.cpp' if label=='draft' else 'semantics-release-order.cpp')).write_text(s)
