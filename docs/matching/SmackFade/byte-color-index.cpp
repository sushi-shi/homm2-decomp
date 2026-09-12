// C31: build/c31-{axes,manifest}.json, build/c31-batch/results.json.
// Complete 3/3, 5.75 seconds, no truncation, source restored, no state probes.
// All arms 566 bytes / 13 relocs / 19 exact blocks, strict exact flags true.
// Full native KB and independent genuine-retail function proof pass.
// a is provably selected from colors10..35 before the assignment (difference
// <=255 < sentinel999); it remains a real signed index for pointer arithmetic.
g[f] = (u8)a;               // baseline
g[f] = static_cast<u8>(a);  // measured
g[f] = a;                  // retained
