// bitmap::DrawToBuffer / GrabBitmap linkage
//   PoL RVAs: DrawToBuffer 0x000d0260, GrabScreen 0x000d02e0,
//             GrabBitmap 0x000d0310
//   Buka RVAs: 0x000c6090, 0x000c6120, 0x000c6150
//
// Clean source evidence:
//   PoL declares/defines DrawToBuffer and GrabBitmap as exported inline
//   methods. All three PoL targets are exact. GrabScreen has no GrabBitmap
//   call relocation; VC4.2 inlines it and directly relocates BlitBitmap.
//   Buka declares/defines both methods as plain out-of-line methods. All
//   three Buka targets are exact, and GrabScreen relocates GrabBitmap.
//
// Reviewed structural probe under VC4.2:
//   removed `__declspec(dllexport) inline` from both declarations and
//   `inline` from both definitions, then rebuilt every header consumer.
//   DrawToBuffer remained exact (60 bytes, ordered 4/4 relocations), but
//   GrabScreen changed from the retail direct BlitBitmap body to a call of
//   GrabBitmap and lost exactness. Repository live exact count fell by one;
//   ordered relocation inventory changed 37123 -> 37120. Source was restored,
//   rebuilt, and all three targets returned to exact with 4/4, 2/2, and 1/1
//   ordered relocations respectively.
//
// Disposition: compiler/source-specific linkage; no port. PoL requires its
// inline/export form, while Buka requires plain out-of-line methods. No
// gameplay difference.
