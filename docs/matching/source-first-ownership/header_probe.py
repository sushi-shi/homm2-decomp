"""Reproduce the measured controls in header-first-use.cpp.

Run from the repository root inside nix develop .#build. Outputs are disposable
and live under build/holistic-source-first; production source is never edited.
Run header_probe.py before archive_probe.py.
"""

from pathlib import Path
import json,re,hashlib
from homm2.build.cc_wrap import run_compile
from homm2.core.manifest import load,units,unit_flags
from homm2.core.coff import CoffObject,MEM_EXECUTE
from homm2.core import wine
root=Path.cwd();base=root/'build/holistic-source-first/header-probe';base.mkdir(parents=True, exist_ok=True)
manifest=load();entry=next(x for x in units(manifest) if x['unit']=='BASE/AudiereEffects');flags=unit_flags(entry,manifest)
header='''#ifndef PROBE_NODE_H
#define PROBE_NODE_H
#include <audiere.h>
#include <string>
class sample;
struct Node {
    audiere::OutputStreamPtr stream;
    sample* resource;
    Node* next;
    Node(sample* r, Node* n) { stream = NULL; resource = r; next = n; }
    DTOR_DECL
};
DTOR_HEADER_BODY
#endif
'''
effects='''INCLUDE
DTOR_TU_BODY
Node* effectsHead = NULL;
int EffectsBefore() { return 1; }
EFFECTS_USE
int EffectsAfter() { return 2; }
CREATE_EFFECT
'''
purge='''void PurgeEffects() {
    if (effectsHead == NULL) return;
    Node* head = NULL;
    for (;;) {
        if (!effectsHead->stream->isPlaying()) {
            head = effectsHead->next;
            delete effectsHead;
            effectsHead = head;
            if (effectsHead == NULL) return;
        } else break;
    }
    Node* current = effectsHead->next;
    Node* cursor = effectsHead;
    while (current != NULL) {
        if (!current->stream->isPlaying()) {
            cursor->next = current->next;
            delete current;
            current = cursor->next;
        } else { cursor = current; current = current->next; }
    }
}
'''
music='''INCLUDE
DTOR_TU_BODY
int MusicBefore() { return 3; }
void PurgeMusic(Node* head) {
    while (head != NULL) { Node* next = head->next; delete head; head = next; }
}
int MusicAfter() { return 4; }
int main() { return 0; }
'''
shapes=[('implicit-header','','','','',True),('empty-header','~Node() {}','','','',True),('inline-header','inline ~Node();','inline Node::~Node() {}','','',True),('inline-effects-only','inline ~Node();','','inline Node::~Node() {}','',True),('ordinary-music','~Node();','','','Node::~Node() {}',True),('ordinary-effects','~Node();','','Node::~Node() {}','',True),('inline-music-only','inline ~Node();','','','inline Node::~Node() {}',True),('music-first-use','','','','',False),('pasted-definition','','','','',True)]
# Complete all source products before compilation.
for name,decl,hbody,ebody,mbody,use in shapes:
 h=header.replace('DTOR_DECL',decl).replace('DTOR_HEADER_BODY',hbody)
 inc=h if name=='pasted-definition' else '#include "Node.h"'
 for ob in [0,1]:
  d=base/(name+'-Ob'+str(ob));d.mkdir(exist_ok=True);(d/'Node.h').write_text(h)
  (d/'Effects.cpp').write_text(effects.replace('INCLUDE',inc).replace('DTOR_TU_BODY',ebody).replace('EFFECTS_USE',purge if use else '').replace('CREATE_EFFECT', '''Node* CreateEffect(sample* resource, Node* next, audiere::OutputStream* stream) {
    Node* node = new Node(resource, next);
    node->stream = stream;
    if (node->stream == NULL) { delete node; return NULL; }
    return node;
}
''' if use else 'Node* CreateEffect(sample* resource, Node* next) { return new Node(resource, next); }'))
  (d/'Music.cpp').write_text(music.replace('INCLUDE',inc).replace('DTOR_TU_BODY',mbody))
def audit(p):
 c=CoffObject(p.read_bytes());rows=[]
 for s in c.sections:
  if not s.characteristics & MEM_EXECUTE:continue
  rows.append(dict(section=s.index,size=s.raw_size,names=[x.name for x in c.symbols.values() if x.section==s.index and x.typ==32],bytes=c.section_bytes(s).hex(),relocations=[dict(site=r.site,type=r.typ,target=c.symbols[r.symbol_index].name) for r in c.relocations if r.section==s.index]))
 return rows
rows=[]
for name,*_ in shapes:
 for ob in [0,1]:
  d=base/(name+'-Ob'+str(ob));row=dict(name=name,ob=ob,objects={},links={})
  f=[x for x in flags if not x.startswith('/Ob')]+['/Ob'+str(ob)]
  for unit in ['Effects','Music']:
   rc,log,timed=run_compile(d/(unit+'.cpp'),d/(unit+'.obj'),f,depfile=False,cl_timeout=60);(d/(unit+'.log')).write_text(log)
   if rc or timed:raise RuntimeError(f'{d.name} {unit}: compile {rc}, timeout={timed}')
   row['objects'][unit]=audit(d/(unit+'.obj'))
  for order in [('Effects','Music'),('Music','Effects')]:
   tag='-'.join(order);out=d/(tag+'.exe');mp=d/(tag+'.map');rsp=d/(tag+'.rsp')
   args=['/NOLOGO','/SUBSYSTEM:CONSOLE','/MACHINE:IX86','/INCREMENTAL:NO','/OPT:NOREF','/OUT:'+wine.winepath_w(out),'/MAP:'+wine.winepath_w(mp),'/LIBPATH:'+wine.winepath_w(root/'build/toolchain/msvc/lib'),*[wine.winepath_w(d/(u+'.obj')) for u in order],'MSVCPRT.LIB','LIBCMT.LIB','KERNEL32.LIB']
   rsp.write_text(' '.join(args)+'\n');wine.run(root/'build/toolchain/msvc/bin/LINK.EXE','@'+wine.winepath_w(rsp),cwd=root,log=d/(tag+'.log'))
   lines=[line.strip() for line in mp.read_text(errors='replace').splitlines() if any(s in line for s in ['??1Node@','PurgeEffects','PurgeMusic','_$E','EffectsBefore','EffectsAfter','MusicBefore','MusicAfter'])]
   row['links'][tag]=lines
  rows.append(row);(base/'results.json').write_text(json.dumps(rows,indent=2)+'\n')
  print(d.name, {u:[(s['section'],s['names']) for s in r if any('??1Node@' in n for n in s['names'])] for u,r in row['objects'].items()},flush=True)
print(f'Complete: {len(shapes)*2} source/build arms, {len(shapes)*4} compilations, {len(shapes)*4} native links.',flush=True)
