// Reviewed comparison-order matrices for wsnet_rcv and dpnet_rcv.
// PoL: receive-buffer tail == head
// Buka: receive-buffer head == tail
//
// Complete matrices:
//   build/wsnet-rcv-compare-axes.json
//   build/source-variant-batch/wsnet-rcv-compare/results.json
//   build/dpnet-rcv-compare-axes.json
//   build/source-variant-batch/dpnet-rcv-compare/results.json
//   102/102 compiled per TU (204 total); no truncation; sources restored.
//
// Every arm was exact at 167 bytes, 4 blocks, and 15/15 relocations. Within
// all 51 paired states in both TUs, text and raw/normalized relocation hashes
// were identical.
//
// Disposition: port the byte/relocation-invariant Buka head-first order.
