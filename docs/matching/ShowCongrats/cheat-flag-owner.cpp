// C32: original unsigned-owner matrix: build/c32-{manifest.json,batch/results.json}.
// 2/2 in4.33s. Removing i8 cast alone adds a zero-extension byte (946->947),
// 99.298250%; rejected despite equivalent truth values for all256 byte patterns.
// Source owner recovery u8->b8 is justified by BOTH signed retail consumers and
// all other named uses (byte writes/copies/I/O). See reconstruction/C25-C32.md.
// With b8 owner: c32-owner-{manifest.json,batch/results.json}, 2/2 in3.92s;
// both946bytes/65relocs/22exactblocks. Whole98-unit native comparison passes;
// independent whole-function ShowCongrats retail proof passes.

// Header parent alternatives, evaluated as structural owner recovery:
u8 m_cheated;  // previous owner; direct consumer did NOT match
b8 m_cheated;  // recovered signed-byte flag at the same packed offset0x639d

// Complete consumer family under each parent:
if (static_cast<i8>(gpGame->m_cheated))  // previous compensating cast
if (gpGame->m_cheated)                  // retained with b8 owner
