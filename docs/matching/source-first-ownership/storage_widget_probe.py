"""Measured source products for whole-module-follow-through.cpp.

Run from a worktree with source AND raw objects at commit 23515eb2, inside
nix develop .#build. Writes disposable build artifacts only. See the dossier
for scope, completeness, startup corrections, and interpretation limits.
"""

from pathlib import Path
import json,re,hashlib,struct,time
from homm2.build.cc_wrap import run_compile
from homm2.core.manifest import load,units,unit_flags
from homm2.core.coff import CoffObject,MEM_EXECUTE
from homm2.core import wine
from homm2.build import native_link
root=Path.cwd();base=root/'build/holistic-storage-widgets';manifest=load();entries={u['unit']:u for u in units(manifest)}
request=(root/'src/SOURCE/REQUEST.cpp').read_text();rh=(root/'include/SOURCE/fileRequester.h').read_text();request_original=request
for name in ['emptyFileName','emptyExtension','emptyLastFilename','emptyCycleName','emptyFilteredName']:
 request,n=re.subn(r'[ \t]*DATA\(0x[0-9a-fA-F]+\) static char '+name+r'\[1\] = "";\n','',request);assert n==1
 request=re.sub(r'\b'+name+r'\b','""',request)
pointer='DATA(0x00516ae0) H2_CONST char* cFRDummy = "";'
for owner in ['global','member','local','literal']:
 s=request;h=rh
 if owner!='global':
  s=s.replace(pointer,'');h=h.replace('extern H2_CONST char* cFRDummy;\n','')
 if owner=='member':
  h=h.replace('    H2_CONST char* GetFilename(void);','    H2_CONST char* GetFilename(void);\n    static H2_CONST char* emptyFilename;')
  s=s.replace('return cFRDummy;','return emptyFilename;')+'\nH2_CONST char* fileRequester::emptyFilename = "";\n'
 elif owner=='local':s=s.replace('H2_CONST char* fileRequester::GetFilename(void) {','H2_CONST char* fileRequester::GetFilename(void) {\n    static H2_CONST char* emptyFilename = "";').replace('return cFRDummy;','return emptyFilename;')
 elif owner=='literal':s=s.replace('return cFRDummy;','return "";')
 d=base/('request-'+owner);(d/'include/SOURCE').mkdir(parents=True,exist_ok=True);(d/'REQUEST.cpp').write_text(s.replace('#include <SOURCE/fileRequester.h>', '#include "include/SOURCE/fileRequester.h"'));(d/'include/SOURCE/fileRequester.h').write_text(h)
# Completely authored class layouts, with no inserted fields or forced uses.
header='''#ifndef HOMM2_BASE_DIMMERWIDGET_H
#define HOMM2_BASE_DIMMERWIDGET_H
#include <BASE/widget.h>
EXTRA_INCLUDES
#pragma pack(push, 1)
class dimmerWidget : public widget {
public:
    DEFAULT_CTOR
    PARAM_CTOR
    READ
    MAIN
    DRAW
    DTOR
};
#pragma pack(pop)
SIZE(dimmerWidget, 0x20);
#endif
'''
fragments={
 'DEFAULT_CTOR':('dimmerWidget(void);','dimmerWidget(void) : widget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {}'),
 'PARAM_CTOR':('dimmerWidget(i16 x, i16 y, i16 width, i16 height, i16 id, H2_ENUM_PARAM(WidgetKind, i16) kind);','dimmerWidget(i16 x, i16 y, i16 width, i16 height, i16 id, H2_ENUM_PARAM(WidgetKind, i16) kind) : widget(x, y, width, height, id, kind) {}'),
 'READ':('void Read(void);','void Read(void) { READ_WIDGET_GEOMETRY(*this, gpResourceManager); m_id = gpResourceManager->ReadWord(); m_kind = gpResourceManager->ReadWord(); }'),
 'MAIN':('virtual MessageDispatchResult Main(tag_message& message) OVERRIDE;','virtual MessageDispatchResult Main(tag_message& message) OVERRIDE { return widget::Main(message); }'),
 'DRAW':('virtual void Draw(void) OVERRIDE;','virtual void Draw(void) OVERRIDE { widget::Dim(); }'),
 'DTOR':('virtual ~dimmerWidget(void) OVERRIDE;','virtual ~dimmerWidget(void) OVERRIDE {}')}
source=(root/'src/BASE/DIMMER.cpp').read_text();window=(root/'src/BASE/WINDOW.cpp').read_text();forms=[]
for layout in ['tu-narrow','tu-broad','virtual-header','all-header']:
 for destructor in ['explicit','implicit']:
  name=layout+'-'+destructor;d=base/name;h=header;s=source
  inline_keys={'MAIN','DRAW','DTOR'} if layout=='virtual-header' else set(fragments) if layout=='all-header' else set()
  for key,(decl,body) in fragments.items():h=h.replace(key, '' if key=='DTOR' and destructor=='implicit' else body if key in inline_keys else decl)
  h=h.replace('EXTRA_INCLUDES','#include <BASE/resourceManager.h>\n#include <SOURCE/KBDeclarations.h>' if layout=='all-header' else '')
  chunks=re.split(r'(?=VA\()',source)
  for chunk in chunks[1:]:
   fn='DEFAULT_CTOR' if 'dimmerWidget::dimmerWidget(void)' in chunk else 'PARAM_CTOR' if 'dimmerWidget::dimmerWidget(' in chunk else 'DTOR' if '::~' in chunk else 'READ' if '::Read(' in chunk else 'MAIN' if '::Main(' in chunk else 'DRAW'
   if fn in inline_keys or (fn=='DTOR' and destructor=='implicit'):s=s.replace(chunk,'')
  if layout=='tu-broad':s=s.replace('<SOURCE/KBDeclarations.h>','<SOURCE/KB.h>')
  (d/'include/BASE').mkdir(parents=True,exist_ok=True);(d/'include/BASE/dimmerWidget.h').write_text(h);(d/'DIMMER.cpp').write_text(s.replace('#include <BASE/dimmerWidget.h>', '#include "include/BASE/dimmerWidget.h"'));(d/'WINDOW.cpp').write_text(window.replace('#include <BASE/dimmerWidget.h>', '#include "include/BASE/dimmerWidget.h"'));forms.append(dict(name=name,layout=layout,destructor=destructor))
(base/'manifest.json').write_text(json.dumps(dict(request=['global','member','local','literal'],widgets=forms),indent=2)+'\n')
def audit(p):
 c=CoffObject(p.read_bytes());return dict(sections=[dict(index=s.index,name=s.name,size=s.raw_size,flags=s.characteristics,bytes=c.section_bytes(s).hex()) for s in c.sections],symbols=[dict(index=s.index,name=s.name,section=s.section,value=s.value,type=s.typ,storage=s.storage_class) for s in c.symbols.values()],relocations=[dict(section=r.section,site=r.site,type=r.typ,target=c.symbols[r.symbol_index].name) for r in c.relocations])
def compile(d,unit,name):
 flags=unit_flags(entries[unit],manifest)+['/I'+wine.winepath_w(d/'include')];out=d/(name+'.obj');rc,log,timed=run_compile(d/(name+'.cpp'),out,flags,depfile=False,cl_timeout=60);out.with_suffix('.log').write_text(log)
 if rc or timed:raise RuntimeError((d.name,name,rc,timed,log))
 out.with_suffix('.json').write_text(json.dumps(audit(out),indent=2)+'\n')
for owner in ['global','member','local','literal']:compile(base/('request-'+owner),'SOURCE/REQUEST','REQUEST')
for v in forms:
 d=base/v['name'];compile(d,'BASE/WINDOW','WINDOW')
 if v['layout']!='all-header':compile(d,'BASE/DIMMER','DIMMER')
def sections(blob):
 pe=struct.unpack_from('<I',blob,60)[0];n=struct.unpack_from('<H',blob,pe+6)[0];opt=struct.unpack_from('<H',blob,pe+20)[0];rows={}
 for i in range(n):
  p=pe+24+opt+i*40;name=blob[p:p+8].split(b'\0')[0].decode();vs,rva,sz,off=struct.unpack_from('<IIII',blob,p+8);rows[name]=(blob[off:off+sz],dict(rva=rva,virtual_size=vs,size=sz,offset=off))
 return rows
retail=sections((root/'build/orig/HMM2PL.exe').read_bytes());ninja=(root/'build.ninja').read_text().replace('$\n',' ');inputs0=native_link.final_inputs(native_link.ninja_link_args(),include_resources=True);rows=[]
for v in forms:
 d=base/v['name'];libs={};replacements={'build/objdiff/base/BASE/WINDOW.obj':str((d/'WINDOW.obj').relative_to(root)),'build/objdiff/base/BASE/DIMMER.obj':None if v['layout']=='all-header' else str((d/'DIMMER.obj').relative_to(root))}
 for item in ['build/link/BASE-prefix.lib','build/link/BASE-suffix.lib']:
  line=next(x for x in ninja.splitlines() if x.startswith('build '+item+': archive '));members=line.split(': archive ',1)[1].split(' |',1)[0].split();members=[replacements.get(m,m) for m in members];members=[m for m in members if m]
  lib=d/Path(item).name;rsp=lib.with_suffix('.rsp');rsp.write_text(' '.join(['/NOLOGO','/OUT:'+str(lib.relative_to(root)),*members])+'\n');wine.run(root/'build/toolchain/msvc/bin/LIB.EXE','@'+str(rsp.relative_to(root)),cwd=root,log=lib.with_suffix('.log'));libs[item]=str(lib.relative_to(root))
 for owner in ['global','member','local','literal']:
  arm=d/owner;arm.mkdir(exist_ok=True);out=arm/'HMM2PL.exe';mp=out.with_suffix('.map');rsp=out.with_suffix('.rsp');inputs=[str((base/('request-'+owner)/'REQUEST.obj').relative_to(root)) if p=='build/objdiff/base/SOURCE/REQUEST.obj' else libs.get(p,p) for p in inputs0]
  rsp.write_text(' '.join(native_link.link_prefix(out,mp,str((arm/'HMM2PL.pdb').relative_to(root)))+inputs)+'\n');wine.run(native_link.LINK_EXE,'@'+str(rsp.relative_to(root)),cwd=root,log=arm/'link.log');blob=out.read_bytes();ss=sections(blob)
  row=dict(v,request=owner,sha256=hashlib.sha256(blob).hexdigest(),differences={k:sum(a!=b for a,b in zip(s[0],ss[k][0]))+abs(len(s[0])-len(ss[k][0])) for k,s in retail.items()},sections={k:s[1] for k,s in ss.items()});rows.append(row);(base/'results.json').write_text(json.dumps(rows,indent=2)+'\n');print(row['name'],owner,row['differences'],flush=True)
