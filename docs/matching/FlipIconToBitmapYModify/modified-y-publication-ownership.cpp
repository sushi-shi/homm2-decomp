/*
FlipIconToBitmapYModify, retail RVA 0x000d9ce0, size 0x58d.
Modified-Y publication and setup ownership, 2026-07-27.

Fresh full block comparison localized the first divergence to setup B0.
Retail pins shear in ESI and clipW in EBP before publishing gFYY; the clean
candidate pins shear in EBP and later reloads clipW. The earlier setup matrix
varied the shear value itself, and direct clipW ownership was also complete,
but no prior matrix varied who owns the modified Y coordinate.

Six reviewed setup structures were tested:

  - the retained direct global publication;
  - a setup-local modifiedY used by the shear read and row calculation;
  - mutation and reuse of the formal y parameter;
  - a local modifiedY published only after the row calculation;
  - a narrow inline modified-X calculation using global Y;
  - the same inline boundary using setup-local Y.

The required top-of-TU declaration-forest matrix completed all
6 * (clean + 50) = 306/306 builds in 225.67 seconds:

  build/iconf2by-modified-y-owner-axes-20260727.json
  build/iconf2by-modified-y-owner-top-states-20260727.json
  build/match-variants/iconf2by-modified-y-owner-top-states-20260727/results.json

Best top-state results:

  direct global             93.605896%, 1420 bytes, 144/144 relocations
  global inline X           93.605896%, 1420 bytes, 144/144 relocations
  formal y                  91.120640%, 1404 bytes, 141/144 relocations
  setup-local y             90.975870%, 1405 bytes, 141/144 relocations
  local y plus inline X     90.975870%, 1405 bytes, 141/144 relocations
  delayed publication       90.372660%, 1405 bytes, 141/144 relocations

The direct arm reproduced durable MAX at trials 10, 12, 19, 24, 31, and 47.
The global inline arm reached the identical 6c176f78ade4bf3c target text at
trials 13, 19, 25, 31, and 47, so its helper does not survive optimization as
a distinct boundary. Local/formal ownership removes three retail relocations,
shrinks the function by 15-16 bytes versus the best island, and reduces the
block census to 51 exact plus eight size-only blocks. This contradicts
retail's global-publication shape despite opening genuine lower byte islands.

For completeness, the same 306 forms were first compiled with the runner's
default target-adjacent state insertion. That diagnostic matrix completed in
204.44 seconds and topped out at 90.120640%, 1407 bytes, 142/144 relocations:

  build/iconf2by-modified-y-owner-states-20260727.json
  build/match-variants/iconf2by-modified-y-owner-states-20260727/results.json

It is not the historical-state census for this TU; the top-of-TU matrix above
is authoritative and reproduces MAX. No arm reaches retail size 1421, exact
ordered relocations, or exact bytes.

Disposition: retain the direct global publication and durable MAX. Preserve
the local/formal objects as lower structural evidence, but retain no generated
helper, declaration-forest probe, configuration change, or source change.
*/

#if 0
// Rejected setup-local owner.
i32 modifiedY = gFYEntry->y + y;
gFYY = modifiedY;
gFYX = gFYXEnd - shear[modifiedY];
gFYRow = dest->m_pixels + dest->m_width * modifiedY;

// Byte-neutral at the strongest states.
static inline i32 FlipModifiedX(i32 xEnd, i8* shear, i32 modifiedY) {
    return xEnd - shear[modifiedY];
}
#endif
