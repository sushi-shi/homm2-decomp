import json
from pathlib import Path
src=Path('src/BASE/DIMMER.cpp').read_text(encoding='latin-1')
hdr=Path('include/BASE/dimmerWidget.h').read_text(encoding='latin-1').replace("#include \"widget.h\"","#include <BASE/widget.h>")
src=src.replace('VA(0x004d3470, 0x1c)\ndimmerWidget::~dimmerWidget() {}\n','@@srcdef@@',1)
src=src.replace('#include <va.h>\n','#include <va.h>\n@@srcpre@@',1)
hdr=hdr.replace('    virtual ~dimmerWidget(void) OVERRIDE;\n','@@hdecl@@',1)
hdr=hdr.replace('#pragma pack(pop)\n','#pragma pack(pop)\n@@hdef@@',1)
assert '@@srcdef@@' in src and '@@hdecl@@' in hdr
m={"schema":1,"files":{"DIMMER.cpp":src,"BASE/dimmerWidget.h":hdr},"units":["DIMMER.cpp"],
 "flags":["/nologo","/c","/Od","/MT","/Gr","/G5","/Ob1","/Gf","/Gi-","/GX","/DNO_STRICT","/Gy","/I{work}","/YX","/Fp{work}\\\\vc60.pch"],
 "defaults":{"srcpre":""},
 "reference":{"DIMMER.cpp":"build/objdiff/base/BASE/DIMMER.obj"},
 "axes":[
  {"name":"dtor","options":[
    {"name":"ool_src","slots":{"hdecl":"    virtual ~dimmerWidget(void) OVERRIDE;\n","hdef":"","srcdef":"VA(0x004d3470, 0x1c)\ndimmerWidget::~dimmerWidget() {}\n"}},
    {"name":"inline_src","slots":{"hdecl":"    inline virtual ~dimmerWidget(void) OVERRIDE;\n","hdef":"","srcdef":"inline dimmerWidget::~dimmerWidget() {}\n"}},
    {"name":"inline_hdr","slots":{"hdecl":"    inline virtual ~dimmerWidget(void) OVERRIDE;\n","hdef":"inline dimmerWidget::~dimmerWidget() {}\n","srcdef":""}},
    {"name":"inclass","slots":{"hdecl":"    virtual ~dimmerWidget(void) OVERRIDE {}\n","hdef":"","srcdef":""}}]},
  {"name":"order","options":[{"name":"current"},{"name":"kb_first","slots":{"srcpre":"#include <SOURCE/KB.h>\n"}},{"name":"windows_first","slots":{"srcpre":"#include <windows.h>\n"}}]},
  {"name":"pch","options":[{"name":"yx"},{"name":"nopch","flags_remove":["/YX","/Fp{work}\\\\vc60.pch"]}]}],
 "aliases":[["^\\?\\?0dimmerWidget@@QAE@XZ","c0"],["^\\?\\?0dimmerWidget@@QAE@FFFFFF@Z","cA"],["^\\?Read@dimmerWidget","R"],["^\\?Main@dimmerWidget","M"],["^\\?Draw@dimmerWidget","D"],["^\\?\\?_GdimmerWidget","G"],["^\\?\\?1dimmerWidget","X"],["^_\\$E\\d+$","E"],["^\\?id@\\?\\$ctype@G@std@@\\$E","C"]],
 "expect":{"DIMMER.cpp":"c0 cA R M D G X E E C"}}
Path('build/probe/dimmer-yx.json').write_text(json.dumps(m,indent=1))
