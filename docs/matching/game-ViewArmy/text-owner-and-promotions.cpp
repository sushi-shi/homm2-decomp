// C38 complete2/2 in4.294s: build/c38-{axes,manifest}.json and -batch/results.json.
// Structural old: u8 armyName0[32]; strcpy(reinterpret_cast<char*>(armyName0), name);
//                message.payload.widget.data.text = reinterpret_cast<char*>(armyName0);
// Applied owner: char armyName0[32]; strcpy(armyName0, name);
//                message.payload.widget.data.text = armyName0;
// Both retain armyName0[0] = ToUpperCp1251(armyName0[0]); unsigned-byte parameter.
// C39 under text parent: five independent cast/implicit axes, full32/32 in38.173s.
// static_cast<i32>(monster->attack/defense/damageMin/damageMax) -> same fields.
// Standalone static_cast<u32>(monster->hitPoints) -> monster->hitPoints.
// Unsigned hit-point subtraction is outside this family and retained.
// All3336bytes/182relocs;105blocks/805instructions; all textsha5d0dde5aae5f6a53.
// No AST/state probes; no truncation; source restored. All strict verdicts false
// due byte/relocation diagnostic residual; independent ordinary bytes TRUE but
// EH+6,+c,+14,+cfd and DIR32 sites unresolved. No whole-retail closure claim.
// Complete102-function/all-section/ordered-relocation native equivalence PASS.
// Detailed proof and complete text bounds: docs/reconstruction/C38-C41-S41-S44.md.
