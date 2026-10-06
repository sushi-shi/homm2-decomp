import json
from pathlib import Path
ref='''template<class T> struct Ref { T* p; Ref() { p = 0; } ~Ref() { if (p) { p->unref(); p = 0; } }
  Ref& operator=(T* q) { if (p) p->unref(); p = q; return *this; } T* operator->() { return p; } };
struct Stream { virtual void unref() = 0; virtual int playing() = 0; };
'''
plain='''struct Node { Ref<Stream> s; int* r; Node* next; Node(int* res, Node* n) { s = 0; r = res; next = n; } @@ndecl@@ };
'''
tmpl='''template<class R> struct NodeT { Ref<Stream> s; R* r; NodeT* next; NodeT(R* res, NodeT* n) { s = 0; r = res; next = n; } @@ndecl@@ };
typedef NodeT<int> Node;
'''
hdr='@@strtop@@'+ref+'@@node@@@@nhdef@@'
eff='''@@pchinc@@#include "node.h"
Node* gList;
void Purge() { while (gList && !gList->s->playing()) { Node* n = gList->next; delete gList; gList = n; } }
void Play(int* r, Stream* st) { gList = new Node(r, gList); gList->s = st; if (!gList->s.p) { Node* d = gList; gList = gList->next; delete d; } Purge(); }
@@eofdef@@
@@strend@@'''
pch='#include "node.h"\n'
axes=[
 {"name":"shape","options":[
   {"name":"plain_inclass","slots":{"node":plain,"ndecl":"~Node() {}","nhdef":"","eofdef":""}},
   {"name":"plain_implicit","slots":{"node":plain,"ndecl":"","nhdef":"","eofdef":""}},
   {"name":"plain_eof","slots":{"node":plain,"ndecl":"inline ~Node();","nhdef":"","eofdef":"inline Node::~Node() {}"}},
   {"name":"plain_hdr","slots":{"node":plain,"ndecl":"inline ~Node();","nhdef":"inline Node::~Node() {}\n","eofdef":""}},
   {"name":"tmpl_inclass","slots":{"node":tmpl,"ndecl":"~NodeT() {}","nhdef":"","eofdef":""}},
   {"name":"tmpl_implicit","slots":{"node":tmpl,"ndecl":"","nhdef":"","eofdef":""}},
   {"name":"tmpl_eof","slots":{"node":tmpl,"ndecl":"~NodeT();","nhdef":"","eofdef":"template<class R> NodeT<R>::~NodeT() {}"}}]},
 {"name":"string","options":[
   {"name":"str_top","slots":{"strtop":"#include <string>\n","strend":""}},
   {"name":"str_end","slots":{"strtop":"","strend":"#include <string>\n"}}]},
 {"name":"pch","options":[
   {"name":"nopch","slots":{"pchinc":""}},
   {"name":"pch","units":["pch.cpp","eff.cpp"],"slots":{"pchinc":""},
    "unit_flags":{"pch.cpp":["/Ycnode.h","/Fp{work}\\\\n.pch"],"eff.cpp":["/Yunode.h","/Fp{work}\\\\n.pch"]}}]},
 {"name":"gy","options":[{"name":"gy"},{"name":"nogy","flags_remove":["/Gy"]}]}]
m={"schema":1,"files":{"node.h":hdr,"eff.cpp":eff,"pch.cpp":pch},"units":["eff.cpp"],
 "flags":["/nologo","/c","/Od","/MT","/Gr","/G5","/Ob1","/Gf","/Gi-","/GX","/DNO_STRICT","/Gy","/I{work}"],
 "defaults":{},"axes":axes,
 "aliases":[["^\\?Purge","U"],["^\\?Play","P"],["^\\?\\?1(\\?\\$)?Node","N"],["^\\?\\?1\\?\\$Ref","S"],["^\\?\\?4\\?\\$Ref","A"],["^_\\$E\\d+$","E"],["^\\?id@\\?\\$ctype@G@std@@\\$E","C"]],
 "track":["P","U"],"expect":{"eff.cpp":"U P S A E E N C"}}
Path('build/probe/post-init.json').write_text(json.dumps(m,indent=1))
