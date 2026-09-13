// VC6 SP5 source product s72-events-generic-ai; RVA 0x47989.
// Parent: 3a80a103 plus measured C79 human flag simplification.
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 8 arms, 7.803362s; no compile failure or timeout; source restored.
// build/source-variant-batch/s72-events-generic-ai/results.json and build/s72-events-generic-ai-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'siteLevel6': 'keep', 'unusedPair8': 'keep', 'unusedTriple26': 'keep', 'candidate': 'baseline'} / 849 / 96a2ced660456cf4 / 7fa5b65d569be490 / 48:48 / 47,1,0,0,0 / True
// 1 / {'siteLevel6': 'keep', 'unusedPair8': 'keep', 'unusedTriple26': 'recover', 'candidate': 'baseline'} / 849 / 1f3d415ac7ad8c1f / 90bcf3d7bd127160 / 48:48 / 47,1,0,0,0 / True
// 2 / {'siteLevel6': 'keep', 'unusedPair8': 'recover', 'unusedTriple26': 'keep', 'candidate': 'baseline'} / 849 / e3b26f3b9e671622 / 90bcf3d7bd127160 / 48:48 / 47,1,0,0,0 / True
// 3 / {'siteLevel6': 'keep', 'unusedPair8': 'recover', 'unusedTriple26': 'recover', 'candidate': 'baseline'} / 849 / 2a7f18870427096b / bc9565bebbec6d5a / 48:48 / 47,1,0,0,0 / True
// 4 / {'siteLevel6': 'recover', 'unusedPair8': 'keep', 'unusedTriple26': 'keep', 'candidate': 'baseline'} / 815 / bbe93f4e394f19d4 / 50bca3c376e0c4e0 / 48:48 / 46,2,0,0,0 / True
// 5 / {'siteLevel6': 'recover', 'unusedPair8': 'keep', 'unusedTriple26': 'recover', 'candidate': 'baseline'} / 815 / dafbfebef0795bcd / 5f1fa15b37a64917 / 48:48 / 46,2,0,0,0 / True
// 6 / {'siteLevel6': 'recover', 'unusedPair8': 'recover', 'unusedTriple26': 'keep', 'candidate': 'baseline'} / 815 / 2e2db4b31616ab95 / 5f1fa15b37a64917 / 48:48 / 46,2,0,0,0 / True
// 7 / {'siteLevel6': 'recover', 'unusedPair8': 'recover', 'unusedTriple26': 'recover', 'candidate': 'baseline'} / 815 / aacb61a096ede893 / 84c2b0ea4e4e95c5 / 48:48 / 46,2,0,0,0 / True

// AXIS siteLevel6
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(siteLevel6);
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(siteLevel6);
// EXTRA BEFORE:
// |     siteLevel6 = cell->m_objectMetadata;
// EXTRA REPLACEMENT:
// EXTRA BEFORE:
// |     siteLevel6 >>= GENERIC_SITE_LEVEL_SHIFT;
// EXTRA REPLACEMENT:

// AXIS unusedPair8
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(unusedPair8)[2];
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(unusedPair8)[2];

// AXIS unusedTriple26
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(unusedTriple26)[3];
// OPTION keep
// OPTION recover
// | --- before
// | +++ recover
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(unusedTriple26)[3];
