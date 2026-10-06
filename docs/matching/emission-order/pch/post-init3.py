import json
from pathlib import Path
base=json.loads(Path('build/probe/post-init2.json').read_text())
own='''#include <string>
template<class T> struct Ref { T* p; Ref() { p = 0; } ~Ref() { if (p) { p->unref(); p = 0; } }
  Ref& operator=(T* q); T* operator->() { return p; } };
template<class T> Ref<T>& Ref<T>::operator=(T* q) { if (p) p->unref(); p = q; return *this; }
struct Stream { virtual void unref() = 0; virtual int isPlaying() = 0; };
typedef Ref<Stream> StreamPtr;
'''
aud='''#include <audiere.h>
typedef audiere::OutputStreamPtr StreamPtr;
'''
node='''struct Node { StreamPtr s; int* r; Node* next; Node(int* res, Node* n) { s = 0; r = res; next = n; } @@dt@@ };
@@dtdef@@'''
eff='''#include "node.h"
Node* gList;
void Purge() { while (gList && !gList->s->isPlaying()) { Node* n = gList->next; delete gList; gList = n; } }
void Play(int* r) { gList = new Node(r, gList); if (!gList->s) { Node* d = gList; gList = gList->next; delete d; } Purge(); }
'''
m=dict(base)
m["files"]={"node.h":"@@lib@@"+node,"eff.cpp":eff,"pch.cpp":'#include "node.h"\n'}
m["flags"]=base["flags"]+["/I"+str(Path('vendor/audiere-1.9.2').resolve()).replace('/','\\\\')]
m["defaults"]={}
m["axes"]=[{"name":"lib","options":[{"name":"own_ref","slots":{"lib":own}},{"name":"audiere","slots":{"lib":aud}}]},
 {"name":"dt","options":[{"name":"inclass","slots":{"dt":"~Node() {}","dtdef":""}},{"name":"hdr_inline","slots":{"dt":"inline ~Node();","dtdef":"inline Node::~Node() {}\n"}}]},
 {"name":"pch","options":[{"name":"pch"},{"name":"nopch","units":["eff.cpp"],"unit_flags":{"eff.cpp":[]}}]}]
m["aliases"]=[["^\\?Purge","U"],["^\\?Play","P"],["^\\?\\?1Node","N"],["^\\?\\?1\\?\\$RefPtr@VOutputStream","S"],["^\\?\\?1\\?\\$Ref@","S"],["^\\?\\?4\\?\\$","A"],["^_\\$E\\d+$","E"],["^\\?id@\\?\\$ctype@G@std@@\\$E","C"]]
Path('build/probe/post-init3.json').write_text(json.dumps(m,indent=1))
