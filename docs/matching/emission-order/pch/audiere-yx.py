import json
from pathlib import Path
eff=Path('src/BASE/AudiereEffects.cpp').read_text(encoding='latin-1')
hdr=Path('include/BASE/soundBackends.h').read_text(encoding='latin-1')
eff=eff.replace('H2_RETAIL_INLINE AudiereSampleNode::~AudiereSampleNode() {}\n','@@effdef@@',1)
eff=eff.replace('#include <va.h>\n','@@effpre@@#include <va.h>\n',1)
hdr=hdr.replace('    H2_RETAIL_INLINE ~AudiereSampleNode();\n','@@hdecl@@',1)
hdr=hdr.replace('#include <audiere.h>\n','@@hpre@@#include <audiere.h>\n',1)
hdr=hdr.replace('// Retail keeps AudiereMusic::stream','@@hdef@@// Retail keeps AudiereMusic::stream',1)
m={"schema":1,"files":{"AudiereEffects.cpp":eff,"BASE/soundBackends.h":hdr},"units":["AudiereEffects.cpp"],
 "flags":["/nologo","/c","/Od","/MT","/Gr","/G5","/Ob1","/Gf","/Gi-","/GX","/DNO_STRICT","/Gy","/I{work}","/YX","/Fp{work}\\\\vc60.pch"],
 "defaults":{"effpre":"","hpre":""},
 "reference":{"AudiereEffects.cpp":"build/objdiff/base/BASE/AudiereEffects.obj"},
 "axes":[
  {"name":"dtor","options":[
    {"name":"inclass","slots":{"hdecl":"    ~AudiereSampleNode() {}\n","hdef":"","effdef":""}},
    {"name":"hdr_inline","slots":{"hdecl":"    inline ~AudiereSampleNode();\n","hdef":"inline AudiereSampleNode::~AudiereSampleNode() {}\n\n","effdef":""}},
    {"name":"eof_inline","slots":{"hdecl":"    H2_RETAIL_INLINE ~AudiereSampleNode();\n","hdef":"","effdef":"H2_RETAIL_INLINE AudiereSampleNode::~AudiereSampleNode() {}\n"}}]},
  {"name":"order","options":[
    {"name":"current"},
    {"name":"hdr_windows","slots":{"hpre":"#include <windows.h>\n"}},
    {"name":"hdr_mss","slots":{"hpre":"#include <mss.h>\n"}},
    {"name":"eff_kb_first","slots":{"effpre":"#include <SOURCE/KB.h>\n"}},
    {"name":"eff_windows_first","slots":{"effpre":"#include <windows.h>\n"}}]},
  {"name":"pch","options":[{"name":"yx"},{"name":"nopch","flags_remove":["/YX","/Fp{work}\\\\vc60.pch"]}]}],
 "aliases":[["^\\?PurgeFinishedAudiereSamples","U"],["^\\?FindAudiereSample","F"],["^\\?PlayAudiereSample","P"],["^\\?AudiereSampleIterationActive","I"],["^\\?\\?1AudiereSampleNode","N"],
   ["^\\?\\?1\\?\\$RefPtr@VOutputStream","S"],["^\\?\\?4\\?\\$RefPtr@VOutputStream","A"],["^\\?\\?1\\?\\$RefPtr@VAudioDevice","D"],["^_\\$E\\d+$","E"],["^\\?id@\\?\\$ctype@G@std@@\\$E","C"]],
 "track":["P","U"],"expect":{"AudiereEffects.cpp":"U F P I S A D E E C N"}}
Path('build/probe/audiere-yx.json').write_text(json.dumps(m,indent=1))
