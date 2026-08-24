// VC6 SP3 /Od cross-version source/state matrix.
// Matrix: build/source-variant-batch/dimension-door-fields-names/results.json
// Manifest: build/dimension-door-fields-names-manifest.json
// Complete: 204/204 variants; generator and wall-time truncation false.
//
// Buka candidate:
//   message.payload.mouse.screenX / screenY
//   i32 result
//
// Retail PoL loads the mouse coordinates from tag_message offsets +0x10/+0x14,
// proving screenX/screenY rather than x/y (+0x4/+0x8).
//
// Result:
//   - screenX/screenY improved every paired arm to 96.592590%.
//   - handled was better than result in both field families; result topped out
//     at 96.539680% with the corrected fields.
//   - All arms retained the same 52-vs-54-block structural residual and 28/28
//     relocations.
//
// Disposition: port the retail-proven Buka mouse fields; retain the
// slot-sensitive PoL name handled. The switch-CFG residual is separate.
