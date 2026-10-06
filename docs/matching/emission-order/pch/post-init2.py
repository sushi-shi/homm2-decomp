import json
from pathlib import Path
base=json.loads(Path('build/probe/post-init.json').read_text())
hdr='''@@pre@@#include <string>
template<class T> struct Ref { T* p; Ref() { p = 0; } ~Ref() { if (p) { p->unref(); p = 0; } }
  Ref& operator=(T* q); T* operator->() { return p; } };
template<class T> Ref<T>& Ref<T>::operator=(T* q) { if (p) p->unref(); p = q; return *this; }
struct Stream { virtual void unref() = 0; virtual int playing() = 0; };
struct Device { virtual void unref() = 0; virtual Stream* open() = 0; };
struct Node { Ref<Stream> s; int* r; Node* next; Node(int* res, Node* n) { s = 0; r = res; next = n; } ~Node() {} };
'''
eff='''#include "node.h"
@@state@@
Node* gList;
void Purge() { while (gList && !gList->s->playing()) { Node* n = gList->next; delete gList; gList = n; } }
void Play(int* r, @@dev@@) { gList = new Node(r, gList); gList->s = @@open@@; if (!gList->s.p) { Node* d = gList; gList = gList->next; delete d; } Purge(); }
'''
FP='/Fp{work}\\\\n.pch'
m={"schema":1,"files":{"node.h":hdr,"eff.cpp":eff,"pch.cpp":'#include "node.h"\n'},"units":["pch.cpp","eff.cpp"],
 "unit_flags":{"pch.cpp":["/Ycnode.h",FP],"eff.cpp":["/Yunode.h",FP]},
 "flags":base["flags"],"defaults":{"pre":"","state":""},
 "axes":[
  {"name":"pre","options":[{"name":"pre_none"},{"name":"pre_stdio","slots":{"pre":"#include <stdio.h>\n#include <stdlib.h>\n"}}]},
  {"name":"state","options":[{"name":"st_none"},{"name":"st_static","slots":{"state":"struct State { Node* list; int n; };\nstatic State gState = { 0 };\n"}}]},
  {"name":"dev","options":[{"name":"dev_ptr","slots":{"dev":"Stream* st","open":"st"}},{"name":"dev_ref","slots":{"dev":"Ref<Device> dev","open":"dev->open()"}}]}],
 "aliases":base["aliases"]+[["^\\?\\?1\\?\\$Ref@UDevice","D"]],"track":["P","U"]}
m["aliases"]=[["^\\?\\?1\\?\\$Ref@UDevice","D"]]+base["aliases"]
Path('build/probe/post-init2.json').write_text(json.dumps(m,indent=1))
