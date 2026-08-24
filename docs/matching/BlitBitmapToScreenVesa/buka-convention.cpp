// BlitBitmapToScreenVesa calling convention
//   PoL RVA  0x000d8540: @BlitBitmapToScreenVesa@28 (__fastcall)
//   Buka RVA 0x000d4610: _BlitBitmapToScreenVesa    (__cdecl)
//
// Both current reconstructions are exact against their retail functions.
// PoL: 482 bytes, 32/32 blocks, ordered 18/18 relocations.
// Buka: 486 bytes, 32/32 blocks, ordered 19/19 relocations.
//
// The decorated COFF identities, parameter placement, epilogues, and every
// caller relocation pin different external ABIs. Replacing PoL's fastcall
// declaration/definition with cdecl would change `_Blit...` ownership and all
// call sites; it cannot preserve PoL retail's `@Blit...@28` relocation stream.
//
// Disposition: version/compiler ABI change; no port. This is not gameplay
// logic. The separate row-copy/body-structure difference remains independent.
