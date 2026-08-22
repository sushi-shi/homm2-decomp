// VC6-measured complete relational-order matrices for the modem timer loop.
// The two operands are semantically interchangeable under !=, but their source
// order controls the stime/oldsec relocation order in both retail functions.
//
// Connect, RVA 0x0047326c:
//   artifacts: build/modem-connect-reloc-manifest.json
//              build/source-variant-batch/modem-connect-relocs/results.json
//   oldsec first: 99.905660%, size 0x19c, relocs 47/47
//   stime first:  100.000000%, size 0x19c, relocs 47/47, 16/16 blocks
//
// WaitForDirectConnect, RVA 0x00473408:
//   artifacts: build/modem-wait-reloc-manifest.json
//              build/source-variant-batch/modem-wait-relocs/results.json
//   oldsec first: 99.920630%, size 0x1e4, relocs 51/51
//   stime first:  100.000000%, size 0x1e4, relocs 51/51, 22/22 blocks
//
// Both products were complete two-arm axes with no AST or TU-state dimension.
// Retained in both functions:
//   stime / MILLISECONDS_PER_SECOND != oldsec / MILLISECONDS_PER_SECOND
