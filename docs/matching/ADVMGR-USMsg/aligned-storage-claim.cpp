// Native BSS completion audit, 2026-09-26. Parent: 624fac59.
//
// The native link's data-symbol audit reported one displaced public symbol:
// USMsg was claimed at VA 0x00523fac but emitted at 0x00523fb0. This is a
// metadata error, not a compiler-layout mismatch requiring source padding.
//
// Current raw ADVMGR.obj defines the 28-byte tag_message at ordinary .bss
// offset 0x1d0. The section is anchored at VA 0x00523de0 by its independently
// referenced neighbors. All 54 other enrolled definitions in this section
// have the same retail-minus-candidate-offset base. Only the old USMsg claim
// implied base 0x00523ddc. Native MAP independently puts USMsg at 0x00523fb0.
//
// The immediately preceding s_drawPlayerColor occupies 0x00523fa8..0x523fac;
// the following cPanel is at 0x00523fcc. The 32-byte intervening range fits
// four bytes of alignment padding followed by the 28-byte message. VC6 gives
// this aggregate eight-byte allocation alignment. Only 0x00523fb0 satisfies
// that alignment while fitting the complete definition between the neighbors.
// Padding is not an object and must not be included in the USMsg claim.
//
// Read-only evidence:
//   build/bss-completion-audit/usmsg.py
//   build/bss-completion-audit/usmsg.json
// All raw project objects have zero relocations to USMsg. The reviewed retail
// DIR32 manifest has zero targets anywhere in 0x00523fac..0x00523fcc. The
// entire interval is zero. Thus no use-site evidence contradicts the native
// topology, and the old unaligned address had no independent reference proof.
// The identity USMsg remains reconstructed; stripped retail does not name it.
//
// Retained change (annotation only, byte-neutral in the production compiler):
//   DATA(0x00523fb0) struct tag_message USMsg = H2_ZERO_INIT;
// No declaration, initializer, layout, executable byte, or build option changes.
//
// Complete canonical BSS topology audit:
//   build/bss-completion-audit/topology.py
//   build/bss-completion-audit/topology.json
// The script reads all source-generated BSS definitions and raw object section
// coordinates. Native section bases are obtained independently from LINK MAP
// symbols and actual linked DIR32 operands, subtracting the raw symbol offset
// and relocation addend with 32-bit arithmetic. It checks every definition's
// claimed RVA against that base plus the untouched compiler offset.
// Before this annotation fix: 1086 definitions in 66 sections, zero unresolved
// sections, one mismatch (USMsg). All six REQUEST cells already agreed.
//
// Validation artifacts after regeneration/build/native historical relink:
//   build/bss-completion-audit/{redelink,build,link,link-audit}.log
// The remaining whole-executable Audiere/DIMMER code-placement differences are
// independent of this BSS annotation and must not be called an exact image.
//
// Final results: all 1086/1086 BSS definitions in 66/66 sections have exact
// native placement; zero unresolved sections or disagreeing definitions.
// The ordinary MAP public-data audit now has zero displaced symbols among
// its 1212 unique matches. It does not name all private data; the independent
// raw-object/relocation topology audit above covers those BSS definitions.
// Build passes: 1727/1727 exact functions, 291995/291995 data bytes.
// The regenerated native executable remains byte-identical to the pre-edit
// executable, SHA256:
// c0eacb837c0d8bdec918962df5bce11270d05247908f4be34badd4015255a1b8.
// Both images retain the same 94248-byte loader-zero writable tail and section
// geometry. The entire image still differs from retail by 500 bytes. This
// annotation change removes a false data-placement diagnostic, not those
// independent Audiere/DIMMER destructor-placement residuals.
