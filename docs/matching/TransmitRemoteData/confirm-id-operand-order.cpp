// Function: TransmitRemoteData, RVA 0x0008dcc0, SOURCE/REMOTE
// Matrix artifacts:
//   build/transmit-remote-confirm-order-axes.json
//   build/transmit-remote-confirm-order-manifest.json
//   build/source-variant-batch/transmit-remote-confirm-order/results.json
//
// COMPLETE: 102/102 variants, no wall-time truncation, source restored.
// The two reviewed equality spellings were crossed with the clean TU and 50
// declaration-forest states. All 51 sent-ID-first arms produced 99.914530%,
// size 466, and 16 relocations. All 51 confirmed-ID-first arms produced
// 100.000000%, size 466, and 16 relocations. Both arms retained the exact
// 29-block CFG.
//
// Rejected arm:
//     if (iIDCtr == giLastConfirm)
//
// Retained arm:
//     if (giLastConfirm == iIDCtr)
//
// The batch runner marked the 100% arms non-exact at the raw relocation layer:
// raw site +0x194 names $SG58778 in the candidate and the reviewed
// sendRemoteMessageRetryPrompt DATA_COMPGEN owner in the paired target. The
// repository's required data-symbol normalization resolves that independent
// identity. The retained clean build is byte-exact for all 0x1d2 bytes, has
// 29/29 exact blocks, and has a complete equal ordered relocation stream at
// 16/16 sites. Disposition: retain confirmed-ID-first.
