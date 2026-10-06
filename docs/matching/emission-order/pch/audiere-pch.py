import json
from pathlib import Path
eff=Path('src/BASE/AudiereEffects.cpp').read_text(encoding='latin-1')
hdr=Path('include/BASE/soundBackends.h').read_text(encoding='latin-1')
eff=eff.replace('H2_RETAIL_INLINE AudiereSampleNode::~AudiereSampleNode() {}\n','@@effdef@@',1)
hdr=hdr.replace('    H2_RETAIL_INLINE ~AudiereSampleNode();\n','@@hdecl@@',1)
hdr=hdr.replace('// Retail keeps AudiereMusic::stream','@@hdef@@// Retail keeps AudiereMusic::stream',1)
assert '@@hdef@@' in hdr
pch_sb='#include <va.h>\n#include <BASE/sample.h>\n#include <BASE/soundBackends.h>\n'
pch_kb=pch_sb+'#include <BASE/soundManager.h>\n#include <SOURCE/KB.h>\n'
FP='/Fp{work}\\\\h.pch'
shapes=[
 {"name":"eof_inline","slots":{"hdecl":"    H2_RETAIL_INLINE ~AudiereSampleNode();\n","hdef":"","effdef":"H2_RETAIL_INLINE AudiereSampleNode::~AudiereSampleNode() {}\n"}},
 {"name":"inclass","slots":{"hdecl":"    ~AudiereSampleNode() {}\n","hdef":"","effdef":""}},
 {"name":"hdr_inline","slots":{"hdecl":"    inline ~AudiereSampleNode();\n","hdef":"inline AudiereSampleNode::~AudiereSampleNode() {}\n\n","effdef":""}},
 {"name":"implicit","slots":{"hdecl":"","hdef":"","effdef":""}},
]
pchs=[{"name":"nopch"},
 {"name":"pch_sb_sep","units":["pch.cpp","AudiereEffects.cpp"],"slots":{"pch":pch_sb},
  "unit_flags":{"pch.cpp":["/YcBASE/soundBackends.h",FP],"AudiereEffects.cpp":["/YuBASE/soundBackends.h",FP]}},
 {"name":"pch_kb_sep","units":["pch.cpp","AudiereEffects.cpp"],"slots":{"pch":pch_kb},
  "unit_flags":{"pch.cpp":["/YcSOURCE/KB.h",FP],"AudiereEffects.cpp":["/YuSOURCE/KB.h",FP]}},
 {"name":"pch_sb_self","unit_flags":{"AudiereEffects.cpp":["/YcBASE/soundBackends.h",FP]}},
 {"name":"pch_kb_self","unit_flags":{"AudiereEffects.cpp":["/YcSOURCE/KB.h",FP]}},
 {"name":"yx","flags_add":["/YX",FP]},
]
m={"schema":1,"files":{"AudiereEffects.cpp":eff,"BASE/soundBackends.h":hdr,"pch.cpp":"@@pch@@"},
 "units":["AudiereEffects.cpp"],
 "flags":["/nologo","/c","/Od","/MT","/Gr","/G5","/Ob1","/Gf","/Gi-","/GX","/DNO_STRICT","/Gy","/I{work}"],
 "defaults":{"pch":""},"axes":[{"name":"node","options":shapes},{"name":"pch","options":pchs}],
 "aliases":[["^\\?PurgeFinishedAudiereSamples","U"],["^\\?FindAudiereSample","F"],["^\\?PlayAudiereSample","P"],["^\\?AudiereSampleIterationActive","I"],["^\\?\\?1AudiereSampleNode","N"],
   ["^\\?\\?1\\?\\$RefPtr@VOutputStream","S"],["^\\?\\?4\\?\\$RefPtr@VOutputStream","A"],["^\\?\\?1\\?\\$RefPtr@VAudioDevice","D"],["^_\\$E\\d+$","E"],["^\\?id@\\?\\$ctype@G@std@@\\$E","C"]],
 "track":["P","U"],"expect":{"AudiereEffects.cpp":"U F P I S A D E E C N"}}
Path('build/probe/audiere-pch.json').write_text(json.dumps(m,indent=1))
